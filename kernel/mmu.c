/*
 * TMXC OS - MMU Implementation
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Memory Management Unit (MMU) implementation for ARM64 architecture.
 * Provides virtual memory management with 4KB page tables.
 */

#include "mmu.h"
#include "uart.h"

/*
 * ============================================================================
 * Page Table Definitions
 * ============================================================================
 */

/**
 * @brief Page table entry structure
 */
typedef uint64_t tmxc_pte_t;

/**
 * @brief Page table structure
 */
typedef struct {
    tmxc_pte_t entries[TMXC_PT_ENTRIES];
} tmxc_page_table_t;

/*
 * ============================================================================
 * Global Page Tables
 * ============================================================================
 */

/**
 * @brief Level 0 page table (PGD)
 * Allocated at TMXC_PT_BASE
 */
static tmxc_page_table_t* tmxc_pgd;

/**
 * @brief Level 1 page table (PUD)
 * Allocated after PGD
 */
static tmxc_page_table_t* tmxc_pud;

/**
 * @brief Level 2 page table (PD)
 * Allocated after PUD
 */
static tmxc_page_table_t* tmxc_pd;

/**
 * @brief Level 3 page table (PT)
 * Allocated after PD
 */
static tmxc_page_table_t* tmxc_pt;

/*
 * ============================================================================
 * Utility Functions
 * ============================================================================
 */

/**
 * @brief Memory set
 */
static void tmxc_memset(void* ptr, uint8_t value, uint32_t len) {
    uint8_t* p = (uint8_t*)ptr;
    while (len--) {
        *p++ = value;
    }
}

/**
 * @brief Convert a 64-bit value to hexadecimal string
 */
static void tmxc_print_hex(uint64_t value) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[17];
    buffer[16] = '\0';
    
    for (int i = 15; i >= 0; i--) {
        buffer[i] = hex_chars[value & 0xF];
        value >>= 4;
    }
    
    tmxc_uart_puts(buffer);
}

/*
 * ============================================================================
 * MMU Implementation
 * ============================================================================
 */

/**
 * @brief Initialize MMU
 * 
 * Initializes the MMU by setting up page tables and enabling virtual memory.
 */
int tmxc_mmu_init(void) {
    /*
     * Step 1: Allocate page tables
     * Place page tables at known physical addresses
     */
    tmxc_pgd = (tmxc_page_table_t*)TMXC_PT_BASE;
    tmxc_pud = (tmxc_page_table_t*)(TMXC_PT_BASE + TMXC_PAGE_SIZE);
    tmxc_pd = (tmxc_page_table_t*)(TMXC_PT_BASE + TMXC_PAGE_SIZE * 2);
    tmxc_pt = (tmxc_page_table_t*)(TMXC_PT_BASE + TMXC_PAGE_SIZE * 3);
    
    /*
     * Step 2: Clear page tables
     */
    tmxc_memset(tmxc_pgd, 0, sizeof(tmxc_page_table_t));
    tmxc_memset(tmxc_pud, 0, sizeof(tmxc_page_table_t));
    tmxc_memset(tmxc_pd, 0, sizeof(tmxc_page_table_t));
    tmxc_memset(tmxc_pt, 0, sizeof(tmxc_page_table_t));
    
    tmxc_uart_puts("[MMU] Page tables allocated and cleared\r\n");
    
    /*
     * Step 3: Set up identity mapping for kernel
     * Map kernel physical memory to same virtual address
     * This allows kernel to run with MMU enabled
     */
    uint64_t kernel_pages = TMXC_KERNEL_SIZE / TMXC_PAGE_SIZE;
    for (uint64_t i = 0; i < kernel_pages; i++) {
        uint64_t virt = TMXC_KERNEL_BASE + (i * TMXC_PAGE_SIZE);
        uint64_t phys = TMXC_KERNEL_BASE + (i * TMXC_PAGE_SIZE);
        
        /*
         * Map with normal memory attributes, read-write
         */
        tmxc_mmu_map_page(virt, phys, TMXC_ATTR_NORMAL | TMXC_AP_RW);
    }
    
    tmxc_uart_puts("[MMU] Kernel identity mapped\r\n");
    
    /*
     * Step 4: Set up device memory mapping
     * Map device memory (MMIO) with device attributes
     */
    /*
     * Map UART (0x09000000)
     */
    tmxc_mmu_map_page(0x09000000, 0x09000000, TMXC_ATTR_DEVICE | TMXC_AP_RW);
    
    tmxc_uart_puts("[MMU] Device memory mapped\r\n");
    
    /*
     * Step 5: Configure MAIR_EL1 (Memory Attribute Indirection Register)
     * Index 0: Device memory
     * Index 1: Normal non-cacheable
     * Index 2: Normal memory
     */
    uint64_t mair = (TMXC_ATTR_DEVICE << (0 * 8)) |
                    (TMXC_ATTR_NORMAL_NC << (1 * 8)) |
                    (TMXC_ATTR_NORMAL << (2 * 8));
    __asm__ volatile("msr mair_el1, %0" : : "r"(mair));
    
    tmxc_uart_puts("[MMU] MAIR_EL1 configured\r\n");
    
    /*
     * Step 6: Configure TCR_EL1 (Translation Control Register)
     * - Use 48-bit virtual address space
     * - Page table walk uses TTBR0_EL1
     * - Page table base address
     */
    uint64_t tcr = (16 << 0) |    /* T0SZ = 16 (48-bit VA) */
                    (0 << 6) |     /* IRGN0 = Normal memory */
                    (0 << 8) |     /* ORGN0 = Normal memory */
                    (3 << 12) |    /* SH0 = Inner shareable */
                    (0 << 14) |    /* EPD0 = Enable TTBR0 walks */
                    (3 << 22) |    /* IPS = 48-bit PA */
                    (1 << 23);     /* A = Enable TTBR0 walks */
    __asm__ volatile("msr tcr_el1, %0" : : "r"(tcr));
    
    tmxc_uart_puts("[MMU] TCR_EL1 configured\r\n");
    
    /*
     * Step 7: Set TTBR0_EL1 (Translation Table Base Register 0)
     * Point to level 0 page table
     */
    __asm__ volatile("msr ttbr0_el1, %0" : : "r"((uint64_t)tmxc_pgd));
    
    tmxc_uart_puts("[MMU] TTBR0_EL1 configured\r\n");
    
    /*
     * Step 8: Enable MMU
     */
    tmxc_mmu_enable();
    
    tmxc_uart_puts("[MMU] MMU enabled\r\n");
    
    return 0;
}

/**
 * @brief Enable MMU
 * 
 * Enables the MMU and data cache.
 */
void tmxc_mmu_enable(void) {
    /*
     * Read SCTLR_EL1
     */
    uint64_t sctlr;
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    
    /*
     * Enable MMU (M bit)
     * Enable data cache (C bit)
     * Enable instruction cache (I bit)
     */
    sctlr |= (1 << 0);   /* M bit */
    sctlr |= (1 << 2);   /* C bit */
    sctlr |= (1 << 12);  /* I bit */
    
    /*
     * Write SCTLR_EL1
     */
    __asm__ volatile("msr sctlr_el1, %0" : : "r"(sctlr));
    
    /*
     * Ensure changes take effect
     */
    __asm__ volatile("isb");
}

/**
 * @brief Disable MMU
 * 
 * Disables the MMU and data cache.
 */
void tmxc_mmu_disable(void) {
    /*
     * Read SCTLR_EL1
     */
    uint64_t sctlr;
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    
    /*
     * Disable MMU (M bit)
     * Disable data cache (C bit)
     * Disable instruction cache (I bit)
     */
    sctlr &= ~(1 << 0);  /* M bit */
    sctlr &= ~(1 << 2);  /* C bit */
    sctlr &= ~(1 << 12); /* I bit */
    
    /*
     * Write SCTLR_EL1
     */
    __asm__ volatile("msr sctlr_el1, %0" : : "r"(sctlr));
    
    /*
     * Ensure changes take effect
     */
    __asm__ volatile("isb");
}

/**
 * @brief Map a physical page to a virtual page
 * 
 * Maps a physical page to a virtual page with specified attributes.
 */
int tmxc_mmu_map_page(uint64_t virt, uint64_t phys, uint64_t attrs) {
    /*
     * Extract page table indices
     */
    uint64_t l0_index = (virt >> 39) & 0x1FF;
    uint64_t l1_index = (virt >> 30) & 0x1FF;
    uint64_t l2_index = (virt >> 21) & 0x1FF;
    uint64_t l3_index = (virt >> 12) & 0x1FF;
    
    /*
     * Step 1: Set up L0 entry
     * Point to L1 table
     */
    if (!(tmxc_pgd->entries[l0_index] & TMXC_PTE_VALID)) {
        tmxc_pgd->entries[l0_index] = (uint64_t)tmxc_pud | TMXC_PTE_VALID | TMXC_PTE_TABLE;
    }
    
    /*
     * Step 2: Set up L1 entry
     * Point to L2 table
     */
    if (!(tmxc_pud->entries[l1_index] & TMXC_PTE_VALID)) {
        tmxc_pud->entries[l1_index] = (uint64_t)tmxc_pd | TMXC_PTE_VALID | TMXC_PTE_TABLE;
    }
    
    /*
     * Step 3: Set up L2 entry
     * Point to L3 table
     */
    if (!(tmxc_pd->entries[l2_index] & TMXC_PTE_VALID)) {
        tmxc_pd->entries[l2_index] = (uint64_t)tmxc_pt | TMXC_PTE_VALID | TMXC_PTE_TABLE;
    }
    
    /*
     * Step 4: Set up L3 entry
     * Map physical page to virtual page
     */
    tmxc_pt->entries[l3_index] = phys | TMXC_PTE_VALID | TMXC_PTE_AF | TMXC_PTE_SH | attrs;
    
    /*
     * Invalidate TLB entry for this virtual address
     */
    tmxc_mmu_invalidate_tlb_entry(virt);
    
    return 0;
}

/**
 * @brief Unmap a virtual page
 * 
 * Unmaps a virtual page.
 */
int tmxc_mmu_unmap_page(uint64_t virt) {
    /*
     * Extract page table indices
     */
    uint64_t l0_index = (virt >> 39) & 0x1FF;
    uint64_t l1_index = (virt >> 30) & 0x1FF;
    uint64_t l2_index = (virt >> 21) & 0x1FF;
    uint64_t l3_index = (virt >> 12) & 0x1FF;
    
    /*
     * Clear L3 entry
     */
    tmxc_pt->entries[l3_index] = 0;
    
    /*
     * Invalidate TLB entry for this virtual address
     */
    tmxc_mmu_invalidate_tlb_entry(virt);
    
    return 0;
}

/**
 * @brief Get physical address from virtual address
 * 
 * Returns the physical address corresponding to a virtual address.
 */
uint64_t tmxc_mmu_virt_to_phys(uint64_t virt) {
    /*
     * Extract page table indices
     */
    uint64_t l0_index = (virt >> 39) & 0x1FF;
    uint64_t l1_index = (virt >> 30) & 0x1FF;
    uint64_t l2_index = (virt >> 21) & 0x1FF;
    uint64_t l3_index = (virt >> 12) & 0x1FF;
    
    /*
     * Walk page tables
     */
    if (!(tmxc_pgd->entries[l0_index] & TMXC_PTE_VALID)) {
        return 0;
    }
    
    if (!(tmxc_pud->entries[l1_index] & TMXC_PTE_VALID)) {
        return 0;
    }
    
    if (!(tmxc_pd->entries[l2_index] & TMXC_PTE_VALID)) {
        return 0;
    }
    
    if (!(tmxc_pt->entries[l3_index] & TMXC_PTE_VALID)) {
        return 0;
    }
    
    /*
     * Get physical address from L3 entry
     */
    uint64_t phys = tmxc_pt->entries[l3_index] & ~0xFFF;
    phys += (virt & 0xFFF);
    
    return phys;
}

/**
 * @brief Invalidate TLB entry
 * 
 * Invalidates a specific TLB entry.
 */
void tmxc_mmu_invalidate_tlb_entry(uint64_t virt) {
    /*
     * Invalidate TLB entry for this virtual address
     */
    __asm__ volatile("tlbi vaae1is, %0" : : "r"(virt));
    
    /*
     * Ensure invalidation completes
     */
    __asm__ volatile("dsb ish");
    __asm__ volatile("isb");
}

/**
 * @brief Invalidate entire TLB
 * 
 * Invalidates all TLB entries.
 */
void tmxc_mmu_invalidate_tlb_all(void) {
    /*
     * Invalidate all TLB entries
     */
    __asm__ volatile("tlbi vmalle1is");
    
    /*
     * Ensure invalidation completes
     */
    __asm__ volatile("dsb ish");
    __asm__ volatile("isb");
}
