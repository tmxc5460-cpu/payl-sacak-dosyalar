/*
 * TMXC OS - Exception Handler Implementation
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Exception handling implementation for ARM64 architecture.
 */

#include "exceptions.h"
#include "uart.h"

/*
 * ============================================================================
 * Exception Vector Table (Assembly)
 * ============================================================================
 */

/**
 * @brief Exception vector table
 * The table is defined in boot.S
 */
extern void tmxc_exception_vectors(void);

/*
 * ============================================================================
 * Utility Functions
 * ============================================================================
 */

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
 * Exception Handler Implementation
 * ============================================================================
 */

/**
 * @brief Initialize exception vector table
 * 
 * Configures the exception vector table for EL1.
 */
int tmxc_exceptions_init(void) {
    /*
     * Set VBAR_EL1 to point to the exception vector table
     */
    __asm__ volatile("msr vbar_el1, %0" : : "r"((uint64_t)&tmxc_exception_vectors));
    
    /*
     * Ensure the change takes effect
     */
    __asm__ volatile("isb");
    
    tmxc_uart_puts("[EXCEPTION] Exception vector table configured\r\n");
    
    return 0;
}

/**
 * @brief Synchronous exception handler
 * 
 * Handles synchronous exceptions.
 */
void tmxc_handle_sync(tmxc_exception_context_t* ctx) {
    tmxc_uart_puts("\r\n[EXCEPTION] Synchronous Exception\r\n");
    tmxc_uart_puts("[EXCEPTION] PC: 0x");
    tmxc_print_hex(ctx->pc);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("[EXCEPTION] ESR: 0x");
    tmxc_print_hex(ctx->esr);
    tmxc_uart_puts("\r\n");
    
    /*
     * Halt the system
     */
    while (1) {
        __asm__ volatile("wfe");
    }
}

/**
 * @brief IRQ handler
 * 
 * Handles normal interrupt requests.
 */
void tmxc_handle_irq(tmxc_exception_context_t* ctx) {
    /*
     * Suppress unused parameter warning
     */
    (void)ctx;
    
    tmxc_uart_puts("[EXCEPTION] IRQ received\r\n");
    
    /*
     * TODO: Dispatch to appropriate interrupt handler
     */
}

/**
 * @brief FIQ handler
 * 
 * Handles fast interrupt requests.
 */
void tmxc_handle_fiq(tmxc_exception_context_t* ctx) {
    /*
     * Suppress unused parameter warning
     */
    (void)ctx;
    
    tmxc_uart_puts("[EXCEPTION] FIQ received\r\n");
    
    /*
     * TODO: Dispatch to appropriate interrupt handler
     */
}

/**
 * @brief SError handler
 * 
 * Handles system errors.
 */
void tmxc_handle_serror(tmxc_exception_context_t* ctx) {
    tmxc_uart_puts("\r\n[EXCEPTION] SError\r\n");
    tmxc_uart_puts("[EXCEPTION] PC: 0x");
    tmxc_print_hex(ctx->pc);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("[EXCEPTION] ESR: 0x");
    tmxc_print_hex(ctx->esr);
    tmxc_uart_puts("\r\n");
    
    /*
     * Halt the system
     */
    while (1) {
        __asm__ volatile("wfe");
    }
}

/**
 * @brief Enable IRQs
 * 
 * Unmasks IRQ exceptions at the current exception level.
 */
void tmxc_enable_irqs(void) {
    /*
     * Clear DAIF.I bit to enable IRQs
     */
    __asm__ volatile("msr daifclr, #2");
    
    tmxc_uart_puts("[EXCEPTION] IRQs enabled\r\n");
}

/**
 * @brief Disable IRQs
 * 
 * Masks IRQ exceptions at the current exception level.
 */
void tmxc_disable_irqs(void) {
    /*
     * Set DAIF.I bit to disable IRQs
     */
    __asm__ volatile("msr daifset, #2");
    
    tmxc_uart_puts("[EXCEPTION] IRQs disabled\r\n");
}

/**
 * @brief Enable FIQs
 * 
 * Unmasks FIQ exceptions at the current exception level.
 */
void tmxc_enable_fiqs(void) {
    /*
     * Clear DAIF.F bit to enable FIQs
     */
    __asm__ volatile("msr daifclr, #4");
    
    tmxc_uart_puts("[EXCEPTION] FIQs enabled\r\n");
}

/**
 * @brief Disable FIQs
 * 
 * Masks FIQ exceptions at the current exception level.
 */
void tmxc_disable_fiqs(void) {
    /*
     * Set DAIF.F bit to disable FIQs
     */
    __asm__ volatile("msr daifset, #4");
    
    tmxc_uart_puts("[EXCEPTION] FIQs disabled\r\n");
}
