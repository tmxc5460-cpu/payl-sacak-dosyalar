/*
 * TMXC OS - Kernel Main Entry Point
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Kernel initialization and main loop for ARM64 (AArch64) architecture.
 * This is the C entry point called from the bootloader.
 * 
 * Initialization sequence:
 * 1. UART initialization (for debug output)
 * 2. MMU initialization (virtual memory)
 * 3. Exception handler initialization (interrupts)
 * 4. Timer initialization (system timer)
 * 5. Memory manager initialization (physical allocator)
 * 6. Process manager initialization (scheduler)
 * 7. Jump to userspace (init process)
 */

#include "kernel_main.h"
#include "uart.h"
#include "mmu.h"
#include "exceptions.h"
#include "timer.h"
#include "memory.h"
#include "process.h"

/*
 * Kernel version information
 */
#define TMXC_VERSION_MAJOR 1
#define TMXC_VERSION_MINOR 0
#define TMXC_VERSION_PATCH 0

/*
 * ============================================================================
 * Utility Functions
 * ============================================================================
 */

/**
 * @brief String length calculation
 */
static uint32_t tmxc_strlen(const char* str) {
    uint32_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/**
 * @brief String copy
 */
static void tmxc_strcpy(char* dst, const char* src) {
    while (*src != '\0') {
        *dst++ = *src++;
    }
    *dst = '\0';
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
 * Kernel Initialization
 * ============================================================================
 */

/**
 * @brief Print kernel banner
 */
static void tmxc_print_banner(void) {
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("TMXC OS Microkernel\r\n");
    tmxc_uart_puts("Version: ");
    tmxc_print_dec(TMXC_VERSION_MAJOR);
    tmxc_uart_putc('.');
    tmxc_print_dec(TMXC_VERSION_MINOR);
    tmxc_uart_putc('.');
    tmxc_print_dec(TMXC_VERSION_PATCH);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("Architecture: ARM64 (AArch64)\r\n");
    tmxc_uart_puts("Copyright (c) 2026 Ödül Ensar Yılmaz\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("\r\n");
}

/**
 * @brief Kernel panic handler
 * 
 * Called when a fatal error occurs.
 * Prints error message and halts the system.
 * 
 * @param message Error message
 */
void tmxc_kernel_panic(const char* message) {
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("KERNEL PANIC\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("Error: ");
    tmxc_uart_puts(message);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("System halted.\r\n");
    tmxc_uart_puts("========================================\r\n");
    
    /*
     * Halt the system
     */
    while (1) {
        __asm__ volatile("wfe");
    }
}

/**
 * @brief Assert macro implementation
 */
void tmxc_assert_fail(const char* expr, const char* file, uint32_t line) {
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("ASSERTION FAILED\r\n");
    tmxc_uart_puts("Expression: ");
    tmxc_uart_puts(expr);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("File: ");
    tmxc_uart_puts(file);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("Line: ");
    tmxc_print_dec(line);
    tmxc_uart_puts("\r\n");
    
    tmxc_kernel_panic("Assertion failed");
}

/*
 * ============================================================================
 * Kernel Main Entry Point
 * ============================================================================
 */

/**
 * @brief Kernel main entry point
 * 
 * This is the C entry point called from the bootloader.
 * Initializes all kernel subsystems and jumps to userspace.
 * 
 * @param arg0 First argument (unused)
 * @param arg1 Second argument (unused)
 * @return int Never returns (jumps to userspace)
 */
int tmxc_kernel_main(uint64_t arg0, uint64_t arg1) {
    /*
     * Suppress unused parameter warnings
     */
    (void)arg0;
    (void)arg1;
    
    /*
     * Step 1: Initialize UART (for debug output)
     * This must be done first so we can print debug messages
     */
    if (tmxc_uart_init() != 0) {
        /*
         * If UART init fails, we cannot print error messages
         * Halt the system
         */
        while (1) {
            __asm__ volatile("wfe");
        }
    }
    
    /*
     * Print kernel banner
     */
    tmxc_print_banner();
    
    tmxc_uart_puts("[KERNEL] Boot sequence started\r\n");
    
    /*
     * Step 2: Initialize MMU (virtual memory)
     * Sets up page tables and enables MMU
     */
    tmxc_uart_puts("[KERNEL] Initializing MMU...\r\n");
    if (tmxc_mmu_init() != 0) {
        tmxc_kernel_panic("MMU initialization failed");
    }
    tmxc_uart_puts("[KERNEL] MMU initialized\r\n");
    
    /*
     * Step 3: Initialize exception handlers (interrupts)
     * Sets up exception vector table
     */
    tmxc_uart_puts("[KERNEL] Initializing exception handlers...\r\n");
    if (tmxc_exceptions_init() != 0) {
        tmxc_kernel_panic("Exception handler initialization failed");
    }
    tmxc_uart_puts("[KERNEL] Exception handlers initialized\r\n");
    
    /*
     * Step 4: Initialize timer (system timer)
     * Sets up generic timer for scheduling
     */
    tmxc_uart_puts("[KERNEL] Initializing timer...\r\n");
    if (tmxc_timer_init() != 0) {
        tmxc_kernel_panic("Timer initialization failed");
    }
    tmxc_uart_puts("[KERNEL] Timer initialized\r\n");
    
    /*
     * Step 5: Initialize memory manager (physical allocator)
     * Sets up physical memory allocation
     */
    tmxc_uart_puts("[KERNEL] Initializing memory manager...\r\n");
    if (tmxc_memory_init() != 0) {
        tmxc_kernel_panic("Memory manager initialization failed");
    }
    tmxc_uart_puts("[KERNEL] Memory manager initialized\r\n");
    
    /*
     * Step 6: Initialize process manager (scheduler)
     * Sets up process management and scheduling
     */
    tmxc_uart_puts("[KERNEL] Initializing process manager...\r\n");
    if (tmxc_process_init() != 0) {
        tmxc_kernel_panic("Process manager initialization failed");
    }
    tmxc_uart_puts("[KERNEL] Process manager initialized\r\n");
    
    /*
     * Step 7: Enable interrupts
     * Allow interrupts to be processed
     */
    tmxc_uart_puts("[KERNEL] Enabling interrupts...\r\n");
    tmxc_enable_irqs();
    tmxc_uart_puts("[KERNEL] Interrupts enabled\r\n");
    
    /*
     * Step 8: Create init process (PID 0)
     * This is the first userspace process
     */
    tmxc_uart_puts("[KERNEL] Creating init process...\r\n");
    uint64_t init_pid = tmxc_process_create("init", NULL, NULL);
    if (init_pid == 0) {
        tmxc_kernel_panic("Init process creation failed");
    }
    tmxc_uart_puts("[KERNEL] Init process created (PID: ");
    tmxc_print_dec(init_pid);
    tmxc_uart_puts(")\r\n");
    
    /*
     * Step 9: Jump to userspace
     * This never returns
     */
    tmxc_uart_puts("[KERNEL] Jumping to userspace...\r\n");
    tmxc_uart_puts("[KERNEL] Boot sequence complete\r\n");
    tmxc_uart_puts("\r\n");
    
    /*
     * Schedule the first process
     * This will jump to userspace and never return
     */
    tmxc_schedule();
    
    /*
     * This should never be reached
     */
    tmxc_kernel_panic("Scheduler returned unexpectedly");
    
    return 0;
}
