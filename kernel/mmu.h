/*
 * TMXC OS - MMU Header
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Memory Management Unit (MMU) for ARM64 architecture.
 * Provides virtual memory management with 4KB page tables.
 */

#ifndef TMXC_MMU_H
#define TMXC_MMU_H

#include <stdint.h>

/*
 * ============================================================================
 * Page Table Configuration
 * ============================================================================
 */

/**
 * @brief Page size (4KB)
 */
#define TMXC_PAGE_SIZE 4096

/**
 * @brief Page shift (log2 of page size)
 */
#define TMXC_PAGE_SHIFT 12

/**
 * @brief Page table entries per table (512)
 */
#define TMXC_PT_ENTRIES 512

/**
 * @brief Page table entry size (8 bytes)
 */
#define TMXC_PTE_SIZE 8

/*
 * ============================================================================
 * Page Table Entry Bits
 * ============================================================================
 */

/**
 * @brief Valid bit
 */
#define TMXC_PTE_VALID (1UL << 0)

/**
 * @brief Table bit (indicates next level table)
 */
#define TMXC_PTE_TABLE (1UL << 1)

/**
 * @brief Block bit (indicates block mapping)
 */
#define TMXC_PTE_BLOCK (1UL << 1)

/**
 * @brief Access flag
 */
#define TMXC_PTE_AF (1UL << 10)

/**
 * @brief Shareable bit
 */
#define TMXC_PTE_SH (3UL << 8)

/**
 * @brief Access permission bits
 */
#define TMXC_PTE_AP (3UL << 6)

/**
 * @brief Non-secure bit
 */
#define TMXC_PTE_NS (1UL << 5)

/**
 * @brief Execute never bit
 */
#define TMXC_PTE_XN (1UL << 54)

/*
 * ============================================================================
 * Memory Attributes
 * ============================================================================
 */

/**
 * @brief Device memory attributes
 */
#define TMXC_ATTR_DEVICE (0x0)

/**
 * @brief Normal memory attributes
 */
#define TMXC_ATTR_NORMAL (0x2)

/**
 * @brief Normal non-cacheable memory
 */
#define TMXC_ATTR_NORMAL_NC (0x1)

/*
 * ============================================================================
 * Access Permissions
 * ============================================================================
 */

/**
 * @brief Read-only at EL1, no access at EL0
 */
#define TMXC_AP_EL1_RO (0b10 << 6)

/**
 * @brief Read-write at EL1, no access at EL0
 */
#define TMXC_AP_EL1_RW (0b00 << 6)

/**
 * @brief Read-only at both EL1 and EL0
 */
#define TMXC_AP_RO (0b11 << 6)

/**
 * @brief Read-write at both EL1 and EL0
 */
#define TMXC_AP_RW (0b01 << 6)

/*
 * ============================================================================
 * Memory Layout
 * ============================================================================
 */

/**
 * @brief Kernel base address
 */
#define TMXC_KERNEL_BASE 0x80000000

/**
 * @brief Kernel size (4MB)
 */
#define TMXC_KERNEL_SIZE 0x400000

/**
 * @brief Page table base address
 */
#define TMXC_PT_BASE 0x80400000

/**
 * @brief Page table size (4MB)
 */
#define TMXC_PT_SIZE 0x400000

/*
 * ============================================================================
 * MMU Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize MMU
 * 
 * Initializes the MMU by setting up page tables and enabling virtual memory.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_mmu_init(void);

/**
 * @brief Enable MMU
 * 
 * Enables the MMU and data cache.
 */
void tmxc_mmu_enable(void);

/**
 * @brief Disable MMU
 * 
 * Disables the MMU and data cache.
 */
void tmxc_mmu_disable(void);

/**
 * @brief Map a physical page to a virtual page
 * 
 * Maps a physical page to a virtual page with specified attributes.
 * 
 * @param virt Virtual address
 * @param phys Physical address
 * @param attrs Page attributes
 * @return 0 on success, negative error code on failure
 */
int tmxc_mmu_map_page(uint64_t virt, uint64_t phys, uint64_t attrs);

/**
 * @brief Unmap a virtual page
 * 
 * Unmaps a virtual page.
 * 
 * @param virt Virtual address
 * @return 0 on success, negative error code on failure
 */
int tmxc_mmu_unmap_page(uint64_t virt);

/**
 * @brief Get physical address from virtual address
 * 
 * Returns the physical address corresponding to a virtual address.
 * 
 * @param virt Virtual address
 * @return uint64_t Physical address, or 0 if not mapped
 */
uint64_t tmxc_mmu_virt_to_phys(uint64_t virt);

/**
 * @brief Invalidate TLB entry
 * 
 * Invalidates a specific TLB entry.
 * 
 * @param virt Virtual address
 */
void tmxc_mmu_invalidate_tlb_entry(uint64_t virt);

/**
 * @brief Invalidate entire TLB
 * 
 * Invalidates all TLB entries.
 */
void tmxc_mmu_invalidate_tlb_all(void);

#endif /* TMXC_MMU_H */
