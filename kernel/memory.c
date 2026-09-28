/*
 * TMXC OS - Memory Manager Implementation
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Physical memory manager implementation for ARM64 architecture.
 */

#include "memory.h"
#include "uart.h"

/*
 * ============================================================================
 * Memory Manager State
 * ============================================================================
 */

/**
 * @brief Memory bitmap
 * Each bit represents a physical page (0 = free, 1 = allocated)
 */
static uint8_t tmxc_memory_bitmap[TMXC_NUM_PAGES / 8];

/**
 * @brief Number of free pages
 */
static uint64_t tmxc_free_pages;

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
 * @brief Convert a 64-bit value to decimal string
 */
static void tmxc_print_dec(uint64_t value) {
    if (value == 0) {
        tmxc_uart_putc('0');
        return;
    }
    
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    
    while (value > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (value % 10);
        value /= 10;
    }
    
    tmxc_uart_puts(&buffer[pos]);
}

/*
 * ============================================================================
 * Memory Manager Implementation
 * ============================================================================
 */

/**
 * @brief Initialize memory manager
 * 
 * Initializes the physical memory allocator.
 */
int tmxc_memory_init(void) {
    /*
     * Step 1: Clear memory bitmap
     */
    tmxc_memset(tmxc_memory_bitmap, 0, sizeof(tmxc_memory_bitmap));
    
    /*
     * Step 2: Mark kernel pages as allocated
     * Kernel occupies first 4MB (1024 pages)
     */
    uint64_t kernel_pages = TMXC_KERNEL_SIZE / TMXC_PAGE_SIZE;
    for (uint64_t i = 0; i < kernel_pages; i++) {
        uint64_t byte_index = i / 8;
        uint64_t bit_index = i % 8;
        tmxc_memory_bitmap[byte_index] |= (1 << bit_index);
    }
    
    /*
     * Step 3: Mark page table pages as allocated
     * Page tables occupy next 4MB (1024 pages)
     */
    uint64_t pt_pages = TMXC_PT_SIZE / TMXC_PAGE_SIZE;
    for (uint64_t i = kernel_pages; i < kernel_pages + pt_pages; i++) {
        uint64_t byte_index = i / 8;
        uint64_t bit_index = i % 8;
        tmxc_memory_bitmap[byte_index] |= (1 << bit_index);
    }
    
    /*
     * Step 4: Calculate free pages
     */
    tmxc_free_pages = TMXC_NUM_PAGES - kernel_pages - pt_pages;
    
    tmxc_uart_puts("[MEMORY] Memory manager initialized\r\n");
    tmxc_uart_puts("[MEMORY] Total pages: ");
    tmxc_print_dec(TMXC_NUM_PAGES);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("[MEMORY] Free pages: ");
    tmxc_print_dec(tmxc_free_pages);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Allocate a physical page
 * 
 * Allocates a single physical page.
 */
uint64_t tmxc_memory_alloc_page(void) {
    /*
     * Step 1: Find free page
     */
    for (uint64_t i = 0; i < TMXC_NUM_PAGES; i++) {
        uint64_t byte_index = i / 8;
        uint64_t bit_index = i % 8;
        
        if (!(tmxc_memory_bitmap[byte_index] & (1 << bit_index))) {
            /*
             * Page is free, mark as allocated
             */
            tmxc_memory_bitmap[byte_index] |= (1 << bit_index);
            tmxc_free_pages--;
            
            /*
             * Return physical address
             */
            return TMXC_PHYS_BASE + (i * TMXC_PAGE_SIZE);
        }
    }
    
    /*
     * No free pages
     */
    return 0;
}

/**
 * @brief Free a physical page
 * 
 * Frees a previously allocated physical page.
 */
int tmxc_memory_free_page(uint64_t phys) {
    /*
     * Step 1: Check if address is valid
     */
    if (phys < TMXC_PHYS_BASE || phys >= TMXC_PHYS_BASE + TMXC_PHYS_SIZE) {
        return -1;
    }
    
    /*
     * Step 2: Calculate page index
     */
    uint64_t page_index = (phys - TMXC_PHYS_BASE) / TMXC_PAGE_SIZE;
    
    /*
     * Step 3: Mark page as free
     */
    uint64_t byte_index = page_index / 8;
    uint64_t bit_index = page_index % 8;
    tmxc_memory_bitmap[byte_index] &= ~(1 << bit_index);
    tmxc_free_pages++;
    
    return 0;
}

/**
 * @brief Get free page count
 * 
 * Returns the number of free physical pages.
 */
uint64_t tmxc_memory_get_free_pages(void) {
    return tmxc_free_pages;
}

/**
 * @brief Get total page count
 * 
 * Returns the total number of physical pages.
 */
uint64_t tmxc_memory_get_total_pages(void) {
    return TMXC_NUM_PAGES;
}
