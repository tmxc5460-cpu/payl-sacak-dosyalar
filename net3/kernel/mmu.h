/*
 * TMXC_OS - Memory Management Unit (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file mmu.h
 * @brief Memory Management Unit with 4KB page tables and NX/RO permissions
 * 
 * This module implements virtual memory management for ARMv8-A (AArch64).
 * It provides:
 * - 4KB page table translation
 * - NX (No-Execute) enforcement on data pages
 * - Read-Only enforcement on kernel text pages
 * - Kernel/user space privilege separation
 * 
 * ARMv8-A Architecture Reference Manual:
 * - 4-level page table structure (L0, L1, L2, L3)
 * - 48-bit virtual address space (256 TB)
 * - 4KB granule page size
 * - Page table entries with AF (Access Flag), AP (Access Permissions), UXN (Unprivileged Execute-Never)
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#ifndef TMXC_MMU_H
#define TMXC_MMU_H

#include <stdint.h>

/*
 * ============================================================================
 * MMU Configuration
 * ============================================================================
 */

/**
 * @brief Page size configuration
 * 
 * ARMv8-A supports 4KB, 16KB, and 64KB granules.
 * We use 4KB for fine-grained memory control.
 */
#define TMXC_PAGE_SIZE           4096    /* 4KB pages */
#define TMXC_PAGE_SHIFT          12      /* log2(4096) = 12 */
#define TMXC_PAGE_MASK           (TMXC_PAGE_SIZE - 1)  /* 0xFFF */

/**
 * @brief Virtual address space configuration
 * 
 * ARMv8-A with 4KB granule supports:
 * - 48-bit virtual addresses (256 TB)
 * - 4-level page tables
 * - 39-bit addresses for LPAE (Large Physical Address Extension)
 */
#define TMXC_VA_BITS             48      /* 48-bit virtual addresses */
#define TMXC_PA_BITS             48      /* 48-bit physical addresses */

/**
 * @brief Page table levels
 * 
 * 4-level page table structure for 48-bit addresses:
 * - L0: Table descriptor (512 entries, each covering 512GB)
 * - L1: Table descriptor (512 entries, each covering 1GB)
 * - L2: Table descriptor (512 entries, each covering 2MB)
 * - L3: Page descriptor (512 entries, each covering 4KB)
 */
#define TMXC_PT_LEVELS           4
#define TMXC_PT_ENTRIES          512     /* Entries per table */

/**
 * @brief Page table entry size
 * 
 * Each PTE is 8 bytes (64 bits)
 */
#define TMXC_PTE_SIZE            8

/**
 * @brief Page table size
 * 
 * 512 entries * 8 bytes = 4096 bytes (1 page)
 */
#define TMXC_PT_SIZE             (TMXC_PT_ENTRIES * TMXC_PTE_SIZE)

/*
 * ============================================================================
 * Memory Attributes (MAIR_EL1)
 * ============================================================================
 */

/**
 * @brief Memory Attribute Indirection Register indices
 * 
 * MAIR_EL1 has 8 attribute fields (Attr0-Attr7), each 8 bits.
 * We define the most common memory types.
 */

/**
 * @brief Device-nGnRnE memory
 * 
 * Device memory, non-Gathering, non-Reordering, no Early write acknowledgement.
 * Used for MMIO registers.
 * 
 * Attr encoding: 0b0000_0000
 */
#define TMXC_MAIR_DEVICE_nGnRnE  0x00

/**
 * @brief Normal memory, Non-Cacheable
 * 
 * Normal memory without caching.
 * 
 * Attr encoding: 0b0100_0100
 */
#define TMXC_MAIR_NORMAL_NC      0x44

/**
 * @brief Normal memory, Write-Back, Read-Allocate, Write-Allocate
 * 
 * Normal memory with full caching.
 * 
 * Attr encoding: 0b1111_1111
 */
#define TMXC_MAIR_NORMAL_WBWA    0xFF

/**
 * @brief Normal memory, Write-Through, Read-Allocate
 * 
 * Normal memory with write-through cache.
 * 
 * Attr encoding: 0b1010_1010
 */
#define TMXC_MAIR_NORMAL_WT      0xBB

/*
 * ============================================================================
 * Page Table Entry Bits
 * ============================================================================
 */

/**
 * @brief Valid bit
 * 
 * Indicates that the entry is valid.
 * If clear, a translation fault occurs.
 */
#define TMXC_PTE_VALID           (1UL << 0)

/**
 * @brief Table bit
 * 
 * For L0-L2: Indicates this is a table descriptor (points to next level).
 * For L3: Must be 0 (page descriptor).
 */
#define TMXC_PTE_TABLE           (1UL << 1)

/**
 * @brief Block bit
 * 
 * For L1-L2: Indicates this is a block descriptor (maps a block of memory).
 * For L0 and L3: Must be 0.
 */
#define TMXC_PTE_BLOCK           (1UL << 1)

/**
 * @brief Output address
 * 
 * Bits [47:12] contain the physical address of the next table or page.
 * Must be aligned to page size.
 */
#define TMXC_PTE_OUTPUT_ADDR_MASK (0x0000FFFFFFFFF000UL)

/**
 * @brief Access Flag
 * 
 * Indicates whether the page has been accessed.
 * Hardware sets this on first access.
 * If clear, access fault occurs.
 */
#define TMXC_PTE_AF              (1UL << 10)

/**
 * @brief Shareable field
 * 
 * Bits [9:8] indicate shareability:
 * - 00: Non-shareable
 * - 10: Outer shareable
 * - 11: Inner shareable
 */
#define TMXC_PTE_SH_NONE         (0UL << 8)
#define TMXC_PTE_SH_OUTER        (2UL << 8)
#define TMXC_PTE_SH_INNER        (3UL << 8)

/**
 * @brief Access Permission field
 * 
 * Bits [7:6] indicate access permissions:
 * - 00: EL0 no access, EL1 read/write
 * - 01: EL0 read/write, EL1 read/write
 * - 10: EL0 no access, EL1 read-only
 * - 11: EL0 read-only, EL1 read-only
 */
#define TMXC_PTE_AP_EL1_RW       (0UL << 6)  /* EL1 RW, EL0 none */
#define TMXC_PTE_AP_EL0_EL1_RW   (1UL << 6)  /* EL0/EL1 RW */
#define TMXC_PTE_AP_EL1_RO       (2UL << 6)  /* EL1 RO, EL0 none */
#define TMXC_PTE_AP_EL0_EL1_RO   (3UL << 6)  /* EL0/EL1 RO */

/**
 * @brief Non-secure bit
 * 
 * Indicates whether the translation is to secure or non-secure memory.
 * Only relevant when EL3 is implemented.
 */
#define TMXC_PTE_NS              (1UL << 5)

/**
 * @brief Attribute Index
 * 
 * Bits [4:2] select the memory attribute from MAIR_EL1.
 */
#define TMXC_PTE_ATTR_INDEX(x)    ((x) << 2)

/**
 * @brief Execute-Never bit (privileged)
 * 
 * If set, instruction fetches from this region cause a fault at EL1.
 * Used to enforce NX on kernel data pages.
 */
#define TMXC_PTE_UXN             (1UL << 54)

/**
 * @brief Execute-Never bit (unprivileged)
 * 
 * If set, instruction fetches from this region cause a fault at EL0.
 * Used to enforce NX on user data pages.
 */
#define TMXC_PTE_PXN             (1UL << 53)

/**
 * @brief Contiguous hint
 * 
 * Hint to hardware that entries are contiguous.
 * Can improve TLB performance.
 */
#define TMXC_PTE_CONTIGUOUS      (1UL << 52)

/**
 * @brief Dirty bit
 * 
 * Indicates whether the page has been written to.
 * Only used with FEAT_HAFDBS (Hardware Access Flag and Dirty Bit State).
 */
#define TMXC_PTE_DBM             (1UL << 51)

/**
 * @brief Global bit
 * 
 * If clear, entry is process-specific (ASID).
 * If set, entry is global (all processes).
 */
#define TMXC_PTE_nG              (1UL << 11)

/*
 * ============================================================================
 * Translation Control Register (TCR_EL1) Bits
 * ============================================================================
 */

/**
 * @brief Translation table walk size
 * 
 * Bits [6:5] select the granule size:
 * - 00: 4KB granule
 * - 01: 64KB granule
 * - 10: 16KB granule
 */
#define TCR_TG0_4KB              (0UL << 5)

/**
 * @brief Shareability for translation table walks
 * 
 * Bits [9:8] indicate shareability.
 */
#define TCR_SH0_INNER            (3UL << 8)

/**
 * @brief Outer cacheability for translation table walks
 * 
 * Bits [11:10] indicate outer cacheability.
 */
#define TCR_ORGN0_WBWA           (1UL << 10)

/**
 * @brief Inner cacheability for translation table walks
 * 
 * Bits [13:12] indicate inner cacheability.
 */
#define TCR_IRGN0_WBWA           (1UL << 12)

/**
 * @brief Translation table base address
 * 
 * Bits [47:16] contain the physical address of the L0 table.
 * Must be aligned to 64KB.
 */
#define TCR_T0SZ_SHIFT           0
#define TCR_T0SZ(x)              ((x) << TCR_T0SZ_SHIFT)

/*
 * ============================================================================
 * System Control Register (SCTLR_EL1) Bits
 * ============================================================================
 */

/**
 * @brief MMU enable bit
 * 
 * If set, MMU translations are enabled for EL1/EL0.
 */
#define SCTLR_M                  (1UL << 0)

/**
 * @brief Alignment check enable
 */
#define SCTLR_A                  (1UL << 1)

/**
 * @brief Data cache enable
 */
#define SCTLR_C                  (1UL << 2)

/**
 * @brief Stack alignment check enable
 */
#define SCTLR_SA                  (1UL << 3)

/**
 * @brief Instruction cache enable
 */
#define SCTLR_I                  (1UL << 12)

/*
 * ============================================================================
 * MMU Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize MMU
 * 
 * Sets up virtual memory with 4KB page tables.
 * Enforces NX (No-Execute) on data pages.
 * Enforces Read-Only on kernel text pages.
 * 
 * Steps:
 * 1. Allocate and zero page tables
 * 2. Configure MAIR_EL1 (memory attributes)
 * 3. Configure TCR_EL1 (translation control)
 * 4. Set up page table mappings
 * 5. Set TTBR0_EL1 (translation table base register)
 * 6. Enable MMU in SCTLR_EL1
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_mmu_init(void);

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
                              int is_executable, int is_writable, uint64_t attr_index);

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
                      int is_kernel, int is_executable, int is_writable);

/**
 * @brief Enable MMU
 * 
 * Enables the MMU by setting the M bit in SCTLR_EL1.
 * Must be called after page tables are set up.
 */
void tmxc_mmu_enable(void);

/**
 * @brief Disable MMU
 * 
 * Disables the MMU by clearing the M bit in SCTLR_EL1.
 */
void tmxc_mmu_disable(void);

/**
 * @brief Invalidate TLB entries
 * 
 * Invalidates all TLB entries for the current ASID.
 */
void tmxc_mmu_invalidate_tlb(void);

#endif /* TMXC_MMU_H */
