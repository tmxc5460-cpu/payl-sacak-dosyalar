/*
 * TMXC OS - Memory Manager Header
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Physical memory manager for ARM64 architecture.
 * Provides physical page allocation.
 */

#ifndef TMXC_MEMORY_H
#define TMXC_MEMORY_H

#include <stdint.h>

/*
 * ============================================================================
 * Memory Configuration
 * ============================================================================
 */

/**
 * @brief Page size (4KB)
 */
#define TMXC_PAGE_SIZE 4096

/**
 * @brief Physical memory base address
 */
#define TMXC_PHYS_BASE 0x80000000

/**
 * @brief Physical memory size (128MB)
 */
#define TMXC_PHYS_SIZE 0x8000000

/**
 * @brief Number of physical pages
 */
#define TMXC_NUM_PAGES (TMXC_PHYS_SIZE / TMXC_PAGE_SIZE)

/*
 * ============================================================================
 * Memory Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize memory manager
 * 
 * Initializes the physical memory allocator.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_memory_init(void);

/**
 * @brief Allocate a physical page
 * 
 * Allocates a single physical page.
 * 
 * @return uint64_t Physical address of allocated page, or 0 on failure
 */
uint64_t tmxc_memory_alloc_page(void);

/**
 * @brief Free a physical page
 * 
 * Frees a previously allocated physical page.
 * 
 * @param phys Physical address of page to free
 * @return 0 on success, negative error code on failure
 */
int tmxc_memory_free_page(uint64_t phys);

/**
 * @brief Get free page count
 * 
 * Returns the number of free physical pages.
 * 
 * @return uint64_t Number of free pages
 */
uint64_t tmxc_memory_get_free_pages(void);

/**
 * @brief Get total page count
 * 
 * Returns the total number of physical pages.
 * 
 * @return uint64_t Total number of pages
 */
uint64_t tmxc_memory_get_total_pages(void);

#endif /* TMXC_MEMORY_H */
