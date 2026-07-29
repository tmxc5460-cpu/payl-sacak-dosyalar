/*
 * TMXC_OS - Stack Canary Protection Implementation (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file stack_canary.c
 * @brief Kernel-space stack canaries for overflow protection
 * 
 * This module implements stack canaries to detect and prevent buffer overflows.
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#include "stack_canary.h"
#include "uart.h"

/*
 * ============================================================================
 * Stack Canary State
 * ============================================================================
 */

/**
 * @brief Global stack canary value
 * 
 * This value is placed at stack boundaries and validated on function return.
 */
uint64_t tmxc_stack_canary = 0;

/**
 * @brief Stack canary enabled flag
 */
static int tmxc_stack_canary_enabled_flag = 1;

/*
 * ============================================================================
 * Stack Canary Initialization
 * ============================================================================
 */

/**
 * @brief Initialize stack canary system
 * 
 * Generates a random stack canary value.
 * The canary should be different on each boot for security.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_stack_canary_init(void) {
    /*
     * Generate a random canary value
     */
    tmxc_stack_canary = tmxc_stack_canary_generate();
    
    tmxc_uart_puts("[CANARY] Stack canary initialized: 0x");
    tmxc_print_hex(tmxc_stack_canary);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/*
 * ============================================================================
 * Stack Canary Generation
 * ============================================================================
 */

/**
 * @brief Generate a random canary value
 * 
 * Uses system timer and CPU ID to generate a pseudo-random value.
 * 
 * @return uint64_t Random canary value
 */
uint64_t tmxc_stack_canary_generate(void) {
    uint64_t canary;
    uint64_t timer;
    uint64_t mpidr;
    uint64_t midr;
    
    /*
     * Read system timer
     */
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(timer));
    
    /*
     * Read CPU ID (MPIDR)
     */
    __asm__ volatile("mrs %0, mpidr_el1" : "=r"(mpidr));
    
    /*
     * Read CPU MIDR
     */
    __asm__ volatile("mrs %0, midr_el1" : "=r"(midr));
    
    /*
     * Combine values to create a pseudo-random canary
     * Use XOR and bit rotation to mix the values
     */
    canary = TMXC_CANARY_MAGIC;
    canary ^= timer;
    canary ^= (mpidr << 32) | (mpidr >> 32);
    canary ^= (midr << 16) | (midr >> 16);
    
    /*
     * Add some bit mixing
     */
    canary = ((canary >> 32) ^ canary) * 0x5851F42D4C957F2D;
    canary = ((canary >> 32) ^ canary) * 0xBF58476D1CE4E5B9;
    canary = (canary >> 32) ^ canary;
    
    /*
     * Ensure canary is non-zero
     */
    if (canary == 0) {
        canary = TMXC_CANARY_MAGIC;
    }
    
    return canary;
}

/*
 * ============================================================================
 * Stack Canary Access Functions
 * ============================================================================
 */

/**
 * @brief Get the current stack canary value
 * 
 * @return uint64_t Current stack canary value
 */
uint64_t tmxc_stack_canary_get(void) {
    return tmxc_stack_canary;
}

/**
 * @brief Set the stack canary value
 * 
 * @param canary New canary value
 */
void tmxc_stack_canary_set(uint64_t canary) {
    tmxc_stack_canary = canary;
}

/*
 * ============================================================================
 * Stack Canary Validation
 * ============================================================================
 */

/**
 * @brief Validate stack canary
 * 
 * Checks if the stack canary has been corrupted.
 * Called on function return to detect buffer overflows.
 * 
 * @param canary Pointer to canary value on stack
 * @return int 1 if valid, 0 if corrupted
 */
int tmxc_stack_canary_validate(uint64_t* canary) {
    if (!tmxc_stack_canary_enabled_flag) {
        return 1;  /* Disabled, always valid */
    }
    
    return (*canary == tmxc_stack_canary);
}

/**
 * @brief Handle stack canary failure
 * 
 * Called when stack canary corruption is detected.
 * This indicates a buffer overflow has occurred.
 * 
 * @param canary Pointer to corrupted canary value
 */
void tmxc_stack_canary_fail(uint64_t* canary) {
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("       STACK CANARY CORRUPTION\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("CANARY: Buffer overflow detected!\r\n");
    
    tmxc_uart_puts("Expected: 0x");
    tmxc_print_hex(tmxc_stack_canary);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("Actual: 0x");
    tmxc_print_hex(*canary);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("System Halted\r\n");
    tmxc_uart_puts("========================================\r\n");
    
    /*
     * Halt the system
     * In a full implementation, this would:
     * - Log the error to persistent storage
     * - Trigger a kernel panic
     * - Attempt to recover or reset
     */
    while (1) {
        __asm__ volatile("wfi");
    }
}

/*
 * ============================================================================
 * Stack Canary Control Functions
 * ============================================================================
 */

/**
 * @brief Check if stack canary is enabled
 * 
 * @return int 1 if enabled, 0 if disabled
 */
int tmxc_stack_canary_enabled(void) {
    return tmxc_stack_canary_enabled_flag;
}

/**
 * @brief Enable stack canary protection
 */
void tmxc_stack_canary_enable(void) {
    tmxc_stack_canary_enabled_flag = 1;
    tmxc_uart_puts("[CANARY] Stack canary protection enabled\r\n");
}

/**
 * @brief Disable stack canary protection
 * 
 * WARNING: Disabling stack canaries reduces security.
 * Only use for debugging purposes.
 */
void tmxc_stack_canary_disable(void) {
    tmxc_stack_canary_enabled_flag = 0;
    tmxc_uart_puts("[CANARY] Stack canary protection disabled\r\n");
}
