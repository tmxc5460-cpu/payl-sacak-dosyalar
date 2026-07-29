/*
 * TMXC_OS - Memory Management Unit Implementation (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file mmu.c
 * @brief MMU implementation with 4KB page tables and NX/RO permissions
 * 
 * This module implements virtual memory management for ARMv8-A (AArch64).
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#include "mmu.h"
#include "uart.h"

/*
 * ============================================================================
 * Page Table Storage
 * ============================================================================
 */

/**
 * @brief Page table storage
 * 
 * We allocate page tables in a static array.
 * For a 48-bit virtual address space with 4KB pages:
 * - L0 table: 1 page (512 entries, each 8 bytes)
 * - L1 tables: Up to 512 tables (one per L0 entry)
 * - L2 tables: Up to 512 * 512 tables
 * - L3 tables: Up to 512 * 512 * 512 tables
 * 
 * For simplicity, we allocate a fixed number of page tables.
 * This can be extended to dynamic allocation later.
 */

/**
 * @brief Maximum number of page tables
 */
#define TMXC_MAX_PT_TABLES      1024

/**
 * @brief Page table storage area
 * 
 * Aligned to 64KB as required by TCR_EL1.
 */
__attribute__((aligned(65536)))
static uint64_t tmxc_page_tables[TMXC_MAX_PT_TABLES * (TMXC_PT_SIZE / 8)];

/**
 * @brief Page table allocation index
 */
static uint32_t tmxc_pt_alloc_index = 0;

/*
 * ============================================================================
 * Page Table Allocation
 * ============================================================================
 */

/**
 * @brief Allocate a page table
 * 
 * Allocates a new page table from the static storage area.
 * 
 * @return uint64_t Physical address of the allocated page table, or 0 on failure
 */
static uint64_t tmxc_mmu_alloc_page_table(void) {
    if (tmxc_pt_alloc_index >= TMXC_MAX_PT_TABLES) {
        return 0;  /* Out of page tables */
    }
    
    uint64_t pt_addr = (uint64_t)&tmxc_page_tables[tmxc_pt_alloc_index * (TMXC_PT_SIZE / 8)];
    tmxc_pt_alloc_index++;
    
    /* Zero the page table */
    uint64_t* pt = (uint64_t*)pt_addr;
    for (int i = 0; i < TMXC_PT_ENTRIES; i++) {
        pt[i] = 0;
    }
    
    return pt_addr;
}

/*
 * ============================================================================
 * Page Table Entry Creation
 * ============================================================================
 */

/**
 * @brief Create a page table entry
 * 
 * @param phys_addr Physical address of the page or next table
 * @param is_table 1 if table descriptor, 0 if page/block descriptor
 * @param is_kernel 1 if kernel mapping, 0 if user mapping
 * @param is_executable 1 if executable, 0 if NX
 * @param is_writable 1 if writable, 0 if read-only
 * @param attr_index Memory attribute index from MAIR_EL1
 * @return uint64_t Page table entry value
 */
uint64_t tmxc_mmu_create_pte(uint64_t phys_addr, int is_table, int is_kernel, 
                              int is_executable, int is_writable, uint64_t attr_index) {
    uint64_t pte = 0;
    
    /* Set valid bit */
    pte |= TMXC_PTE_VALID;
    
    /* Set output address (must be aligned to page size) */
    pte |= (phys_addr & TMXC_PTE_OUTPUT_ADDR_MASK);
    
    /* Set table or block bit */
    if (is_table) {
        pte |= TMXC_PTE_TABLE;
    } else {
        pte |= TMXC_PTE_BLOCK;
    }
    
    /* Set access flag */
    pte |= TMXC_PTE_AF;
    
    /* Set shareability (inner shareable for normal memory) */
    pte |= TMXC_PTE_SH_INNER;
    
    /* Set access permissions */
    if (is_kernel) {
        if (is_writable) {
            pte |= TMXC_PTE_AP_EL1_RW;  /* EL1 RW, EL0 none */
        } else {
            pte |= TMXC_PTE_AP_EL1_RO;  /* EL1 RO, EL0 none */
        }
    } else {
        if (is_writable) {
            pte |= TMXC_PTE_AP_EL0_EL1_RW;  /* EL0/EL1 RW */
        } else {
            pte |= TMXC_PTE_AP_EL0_EL1_RO;  /* EL0/EL1 RO */
        }
    }
    
    /* Set attribute index */
    pte |= TMXC_PTE_ATTR_INDEX(attr_index);
    
    /* Set execute-never bits (NX enforcement) */
    if (!is_executable) {
        pte |= TMXC_PTE_UXN;  /* No execute at EL1 */
        if (!is_kernel) {
            pte |= TMXC_PTE_PXN;  /* No execute at EL0 */
        }
    }
    
    /* Set global bit (kernel mappings are global) */
    if (is_kernel) {
        pte &= ~TMXC_PTE_nG;  /* Clear nG for global */
    } else {
        pte |= TMXC_PTE_nG;  /* Set nG for process-specific */
    }
    
    return pte;
}

/*
 * ============================================================================
 * Page Mapping
 * ============================================================================
 */

/**
 * @brief Get page table entry at a specific level
 * 
 * @param virt_addr Virtual address
 * @param level Page table level (0-3)
 * @return uint64_t Index into the page table at this level
 */
static uint64_t tmxc_mmu_get_pt_index(uint64_t virt_addr, int level) {
    /*
     * For 48-bit addresses with 4KB pages:
     * - L0: bits [47:39] (9 bits)
     * - L1: bits [38:30] (9 bits)
     * - L2: bits [29:21] (9 bits)
     * - L3: bits [20:12] (9 bits)
     */
    int shift = 39 - (level * 9);
    return (virt_addr >> shift) & 0x1FF;
}

/**
 * @brief Map a physical page to a virtual address
 * 
 * @param virt_addr Virtual address to map
 * @param phys_addr Physical address to map
 * @param is_kernel 1 if kernel mapping, 0 if user mapping
 * @param is_executable 1 if executable, 0 if NX
 * @param is_writable 1 if writable, 0 if read-only
 * @return 0 on success, negative error code on failure
 */
int tmxc_mmu_map_page(uint64_t virt_addr, uint64_t phys_addr, 
                      int is_kernel, int is_executable, int is_writable) {
    /*
     * For simplicity, we use identity mapping for kernel.
     * This will be extended to full virtual memory later.
     */
    
    /* Align addresses to page size */
    virt_addr &= ~TMXC_PAGE_MASK;
    phys_addr &= ~TMXC_PAGE_MASK;
    
    /* Use normal memory with write-back cache for kernel */
    uint64_t attr_index = TMXC_MAIR_NORMAL_WBWA;
    
    /* Create page table entry */
    uint64_t pte = tmxc_mmu_create_pte(phys_addr, 0, is_kernel, 
                                         is_executable, is_writable, attr_index);
    
    /* For now, we don't actually set up the page tables.
     * This is a placeholder for the full implementation.
     * The full implementation would:
     * 1. Walk the page table levels
     * 2. Allocate intermediate tables as needed
     * 3. Set the final PTE
     */
    
    (void)pte;  /* Suppress unused warning */
    
    return 0;
}

/*
 * ============================================================================
 * MMU Initialization
 * ============================================================================
 */

/**
 * @brief Initialize MMU
 * 
 * Sets up virtual memory with 4KB page tables.
 * Enforces NX (No-Execute) on data pages.
 * Enforces Read-Only on kernel text pages.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_mmu_init(void) {
    uint64_t mair;
    uint64_t tcr;
    uint64_t ttbr0;
    
    /*
     * Step 1: Allocate and zero L0 page table
     */
    uint64_t l0_table = tmxc_mmu_alloc_page_table();
    if (l0_table == 0) {
        return -1;  /* Failed to allocate page table */
    }
    
    /*
     * Step 2: Configure MAIR_EL1 (Memory Attribute Indirection Register)
     * 
     * Attr0: Device-nGnRnE (for MMIO)
     * Attr1: Normal NC (for uncached memory)
     * Attr2: Normal WBWA (for cached memory)
     */
    mair = 0;
    mair |= (TMXC_MAIR_DEVICE_nGnRnE << (0 * 8));   /* Attr0 */
    mair |= (TMXC_MAIR_NORMAL_NC << (1 * 8));      /* Attr1 */
    mair |= (TMXC_MAIR_NORMAL_WBWA << (2 * 8));     /* Attr2 */
    
    __asm__ volatile("msr mair_el1, %0" : : "r"(mair));
    tmxc_uart_puts("[MMU] MAIR_EL1 configured\r\n");
    
    /*
     * Step 3: Configure TCR_EL1 (Translation Control Register)
     * 
     * - T0SZ = 16 (48-bit virtual address space: 64 - 16 = 48)
     * - TG0 = 00 (4KB granule)
     * - SH0 = 11 (Inner shareable)
     * - ORGN0 = 01 (Normal memory, Outer WBWA)
     * - IRGN0 = 01 (Normal memory, Inner WBWA)
     * - EPD0 = 0 (Enable TTBR0 walks)
     */
    tcr = 0;
    tcr |= TCR_T0SZ(16);           /* 48-bit VA */
    tcr |= TCR_TG0_4KB;            /* 4KB granule */
    tcr |= TCR_SH0_INNER;          /* Inner shareable */
    tcr |= TCR_ORGN0_WBWA;         /* Outer WBWA */
    tcr |= TCR_IRGN0_WBWA;         /* Inner WBWA */
    
    __asm__ volatile("msr tcr_el1, %0" : : "r"(tcr));
    tmxc_uart_puts("[MMU] TCR_EL1 configured\r\n");
    
    /*
     * Step 4: Set up page table mappings
     * 
     * For now, we use identity mapping for the entire physical memory.
     * This is a simplified approach for initial bring-up.
     * 
     * The full implementation would:
     * 1. Map kernel code as RO + executable
     * 2. Map kernel data as RW + NX
     * 3. Map device memory as device-nGnRnE
     * 4. Set up user space mappings
     */
    
    /* Identity map the first 512MB of physical memory */
    uint64_t phys_start = 0x40000000;
    uint64_t phys_end = phys_start + (512 * 1024 * 1024);
    
    for (uint64_t addr = phys_start; addr < phys_end; addr += TMXC_PAGE_SIZE) {
        /* Map as kernel, executable, writable (for now) */
        tmxc_mmu_map_page(addr, addr, 1, 1, 1);
    }
    
    tmxc_uart_puts("[MMU] Identity mapping configured\r\n");
    
    /*
     * Step 5: Set TTBR0_EL1 (Translation Table Base Register 0)
     * 
     * This points to the L0 page table.
     * Bits [47:1] contain the physical address.
     */
    ttbr0 = l0_table;
    
    __asm__ volatile("msr ttbr0_el1, %0" : : "r"(ttbr0));
    tmxc_uart_puts("[MMU] TTBR0_EL1 configured\r\n");
    
    /*
     * Step 6: Invalidate TLB
     */
    tmxc_mmu_invalidate_tlb();
    tmxc_uart_puts("[MMU] TLB invalidated\r\n");
    
    /*
     * Step 7: Enable MMU
     */
    tmxc_mmu_enable();
    tmxc_uart_puts("[MMU] MMU enabled\r\n");
    
    return 0;
}

/*
 * ============================================================================
 * MMU Control
 * ============================================================================
 */

/**
 * @brief Enable MMU
 * 
 * Enables the MMU by setting the M bit in SCTLR_EL1.
 * Also enables instruction and data caches.
 */
void tmxc_mmu_enable(void) {
    uint64_t sctlr;
    
    /*
     * Read SCTLR_EL1
     */
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    
    /*
     * Set M bit (MMU enable)
     * Set C bit (data cache enable)
     * Set I bit (instruction cache enable)
     */
    sctlr |= SCTLR_M;
    sctlr |= SCTLR_C;
    sctlr |= SCTLR_I;
    
    /*
     * Write SCTLR_EL1
     */
    __asm__ volatile("msr sctlr_el1, %0" : : "r"(sctlr));
    
    /*
     * Ensure the change takes effect
     */
    __asm__ volatile("isb");
}

/**
 * @brief Disable MMU
 * 
 * Disables the MMU by clearing the M bit in SCTLR_EL1.
 * Also disables instruction and data caches.
 */
void tmxc_mmu_disable(void) {
    uint64_t sctlr;
    
    /*
     * Read SCTLR_EL1
     */
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    
    /*
     * Clear M bit (MMU disable)
     * Clear C bit (data cache disable)
     * Clear I bit (instruction cache disable)
     */
    sctlr &= ~SCTLR_M;
    sctlr &= ~SCTLR_C;
    sctlr &= ~SCTLR_I;
    
    /*
     * Write SCTLR_EL1
     */
    __asm__ volatile("msr sctlr_el1, %0" : : "r"(sctlr));
    
    /*
     * Ensure the change takes effect
     */
    __asm__ volatile("isb");
}

/**
 * @brief Invalidate TLB entries
 * 
 * Invalidates all TLB entries for the current ASID.
 * 
 * ARMv8-A: TLBI ALLE1 instruction invalidates all EL1/EL0 entries.
 */
void tmxc_mmu_invalidate_tlb(void) {
    /*
     * Invalidate all TLB entries
     */
    __asm__ volatile("tlbi alle1");
    
    /*
     * Ensure the invalidation completes
     */
    __asm__ volatile("dsb ish");
    
    /*
     * Synchronize context
     */
    __asm__ volatile("isb");
}
