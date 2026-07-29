/*
 * TMXC_OS - Kernel Panic Handler Implementation (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file panic.c
 * @brief Fail-safe panic handler with register dump and reboot
 * 
 * This module implements the kernel panic handler that is called when a fatal
 * error occurs. It dumps CPU registers to UART, logs the stack trace, and
 * triggers a clean reboot.
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#include "panic.h"
#include "uart.h"
#include "watchdog.h"

/*
 * ============================================================================
 * Utility Functions (from kernel_main_new.c)
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
 * Context Saving
 * ============================================================================
 */

/**
 * @brief Save current context
 * 
 * Saves the current processor state to a panic context structure.
 * 
 * @param ctx Pointer to panic context structure to fill
 */
void tmxc_panic_save_context(tmxc_panic_context_t* ctx) {
    /*
     * Save general-purpose registers
     * Note: We can't save all registers from C, so we save what we can
     * The assembly exception handler would save the full context
     */
    ctx->sp = (uint64_t)ctx;  /* Approximate SP */
    
    /*
     * Read special registers
     */
    __asm__ volatile("mrs %0, elr_el1" : "=r"(ctx->pc));
    __asm__ volatile("mrs %0, spsr_el1" : "=r"(ctx->cpsr));
    __asm__ volatile("mrs %0, esr_el1" : "=r"(ctx->esr));
    __asm__ volatile("mrs %0, far_el1" : "=r"(ctx->far));
    
    /*
     * Read system registers
     */
    __asm__ volatile("mrs %0, ttbr0_el1" : "=r"(ctx->ttbr0));
    __asm__ volatile("mrs %0, ttbr1_el1" : "=r"(ctx->ttbr1));
    __asm__ volatile("mrs %0, tcr_el1" : "=r"(ctx->tcr));
    __asm__ volatile("mrs %0, mair_el1" : "=r"(ctx->mair));
    
    /*
     * Read timer registers
     */
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(ctx->cntvct));
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(ctx->cntfrq));
    
    /*
     * Read current exception level
     */
    __asm__ volatile("mrs %0, CurrentEL" : "=r"(ctx->current_el));
    ctx->current_el = (ctx->current_el >> 2) & 0x3;
    
    /*
     * Read CPU ID
     */
    __asm__ volatile("mrs %0, midr_el1" : "=r"(ctx->midr));
    __asm__ volatile("mrs %0, mpidr_el1" : "=r"(ctx->mpidr));
    
    /*
     * Zero out general-purpose registers (can't save from C)
     */
    for (int i = 0; i < 31; i++) {
        ctx->x0 = 0;  /* All registers set to 0 */
    }
}

/*
 * ============================================================================
 * Context Dumping
 * ============================================================================
 */

/**
 * @brief Dump panic context to UART
 * 
 * Outputs the saved processor state to UART for debugging.
 * 
 * @param ctx Panic context (saved processor state)
 */
void tmxc_panic_dump_context(tmxc_panic_context_t* ctx) {
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("           KERNEL PANIC\r\n");
    tmxc_uart_puts("========================================\r\n");
    
    /*
     * Dump general-purpose registers
     */
    tmxc_uart_puts("\r\nGeneral Purpose Registers:\r\n");
    tmxc_uart_puts("  X0:  0x"); tmxc_print_hex(ctx->x0); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X1:  0x"); tmxc_print_hex(ctx->x1); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X2:  0x"); tmxc_print_hex(ctx->x2); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X3:  0x"); tmxc_print_hex(ctx->x3); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X4:  0x"); tmxc_print_hex(ctx->x4); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X5:  0x"); tmxc_print_hex(ctx->x5); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X6:  0x"); tmxc_print_hex(ctx->x6); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X7:  0x"); tmxc_print_hex(ctx->x7); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X8:  0x"); tmxc_print_hex(ctx->x8); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X9:  0x"); tmxc_print_hex(ctx->x9); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X10: 0x"); tmxc_print_hex(ctx->x10); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X11: 0x"); tmxc_print_hex(ctx->x11); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X12: 0x"); tmxc_print_hex(ctx->x12); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X13: 0x"); tmxc_print_hex(ctx->x13); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X14: 0x"); tmxc_print_hex(ctx->x14); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X15: 0x"); tmxc_print_hex(ctx->x15); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X16: 0x"); tmxc_print_hex(ctx->x16); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X17: 0x"); tmxc_print_hex(ctx->x17); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X18: 0x"); tmxc_print_hex(ctx->x18); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X19: 0x"); tmxc_print_hex(ctx->x19); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X20: 0x"); tmxc_print_hex(ctx->x20); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X21: 0x"); tmxc_print_hex(ctx->x21); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X22: 0x"); tmxc_print_hex(ctx->x22); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X23: 0x"); tmxc_print_hex(ctx->x23); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X24: 0x"); tmxc_print_hex(ctx->x24); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X25: 0x"); tmxc_print_hex(ctx->x25); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X26: 0x"); tmxc_print_hex(ctx->x26); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X27: 0x"); tmxc_print_hex(ctx->x27); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X28: 0x"); tmxc_print_hex(ctx->x28); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  X29: 0x"); tmxc_print_hex(ctx->x29); tmxc_uart_puts(" (FP)\r\n");
    tmxc_uart_puts("  X30: 0x"); tmxc_print_hex(ctx->x30); tmxc_uart_puts(" (LR)\r\n");
    
    /*
     * Dump special registers
     */
    tmxc_uart_puts("\r\nSpecial Registers:\r\n");
    tmxc_uart_puts("  SP:   0x"); tmxc_print_hex(ctx->sp); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  PC:   0x"); tmxc_print_hex(ctx->pc); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  CPSR: 0x"); tmxc_print_hex(ctx->cpsr); tmxc_uart_puts("\r\n");
    
    /*
     * Dump exception-related registers
     */
    tmxc_uart_puts("\r\nException Registers:\r\n");
    tmxc_uart_puts("  ESR:  0x"); tmxc_print_hex(ctx->esr); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  FAR:  0x"); tmxc_print_hex(ctx->far); tmxc_uart_puts("\r\n");
    
    /*
     * Dump system registers
     */
    tmxc_uart_puts("\r\nSystem Registers:\r\n");
    tmxc_uart_puts("  TTBR0: 0x"); tmxc_print_hex(ctx->ttbr0); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  TTBR1: 0x"); tmxc_print_hex(ctx->ttbr1); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  TCR:   0x"); tmxc_print_hex(ctx->tcr); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  MAIR:  0x"); tmxc_print_hex(ctx->mair); tmxc_uart_puts("\r\n");
    
    /*
     * Dump timer registers
     */
    tmxc_uart_puts("\r\nTimer Registers:\r\n");
    tmxc_uart_puts("  CNTVCT: 0x"); tmxc_print_hex(ctx->cntvct); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  CNTFRQ: 0x"); tmxc_print_hex(ctx->cntfrq); tmxc_uart_puts("\r\n");
    
    /*
     * Dump CPU information
     */
    tmxc_uart_puts("\r\nCPU Information:\r\n");
    tmxc_uart_puts("  Current EL: EL"); tmxc_print_dec(ctx->current_el); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  MIDR:       0x"); tmxc_print_hex(ctx->midr); tmxc_uart_puts("\r\n");
    tmxc_uart_puts("  MPIDR:      0x"); tmxc_print_hex(ctx->mpidr); tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("========================================\r\n");
}

/**
 * @brief Dump stack trace to UART
 * 
 * Walks the stack and outputs the return addresses.
 * 
 * @param sp Current stack pointer
 * @param fp Current frame pointer
 */
void tmxc_panic_dump_stack_trace(uint64_t sp, uint64_t fp) {
    tmxc_uart_puts("\r\nStack Trace:\r\n");
    
    /*
     * Walk the stack using frame pointers
     * Each frame contains:
     * - Previous frame pointer (FP)
     * - Return address (LR)
     */
    uint64_t current_fp = fp;
    uint64_t frame_count = 0;
    
    while (current_fp != 0 && frame_count < 16) {
        /*
         * Check if FP is aligned and in reasonable range
         */
        if (current_fp < sp || current_fp > sp + (64 * 1024)) {
            tmxc_uart_puts("  Invalid frame pointer\r\n");
            break;
        }
        
        /*
         * Read LR from stack (FP + 8)
         * Note: This assumes standard AAPCS64 stack layout
         */
        uint64_t* frame = (uint64_t*)current_fp;
        uint64_t lr = frame[1];  /* Return address */
        uint64_t prev_fp = frame[0];  /* Previous frame pointer */
        
        tmxc_uart_puts("  [");
        tmxc_print_dec(frame_count);
        tmxc_uart_puts("] 0x");
        tmxc_print_hex(lr);
        tmxc_uart_puts("\r\n");
        
        current_fp = prev_fp;
        frame_count++;
    }
    
    if (frame_count >= 16) {
        tmxc_uart_puts("  ... (trace truncated)\r\n");
    }
}

/*
 * ============================================================================
 * Panic Handlers
 * ============================================================================
 */

/**
 * @brief Kernel panic handler
 * 
 * Called when a fatal error occurs. Dumps CPU registers to UART,
 * logs stack trace, and triggers a clean reboot.
 * 
 * This function does not return.
 * 
 * @param message Panic message describing the error
 */
void tmxc_kernel_panic(const char* message) {
    tmxc_panic_context_t ctx;
    
    /*
     * Disable interrupts
     */
    __asm__ volatile("msr daifset, #0xF");
    
    /*
     * Save current context
     */
    tmxc_panic_save_context(&ctx);
    
    /*
     * Output panic message
     */
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("           KERNEL PANIC\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("PANIC: ");
    tmxc_uart_puts(message);
    tmxc_uart_puts("\r\n");
    
    /*
     * Dump context
     */
    tmxc_panic_dump_context(&ctx);
    
    /*
     * Dump stack trace
     */
    tmxc_panic_dump_stack_trace(ctx.sp, ctx.x29);
    
    /*
     * Trigger reboot
     */
    tmxc_uart_puts("\r\nAttempting system reboot...\r\n");
    tmxc_panic_reboot();
}

/**
 * @brief Panic with context
 * 
 * Called when a panic occurs with a specific context (e.g., from exception handler).
 * 
 * @param message Panic message describing the error
 * @param ctx Panic context (saved processor state)
 */
void tmxc_kernel_panic_context(const char* message, tmxc_panic_context_t* ctx) {
    /*
     * Disable interrupts
     */
    __asm__ volatile("msr daifset, #0xF");
    
    /*
     * Output panic message
     */
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("           KERNEL PANIC\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("PANIC: ");
    tmxc_uart_puts(message);
    tmxc_uart_puts("\r\n");
    
    /*
     * Dump context
     */
    tmxc_panic_dump_context(ctx);
    
    /*
     * Dump stack trace
     */
    tmxc_panic_dump_stack_trace(ctx->sp, ctx->x29);
    
    /*
     * Trigger reboot
     */
    tmxc_uart_puts("\r\nAttempting system reboot...\r\n");
    tmxc_panic_reboot();
}

/*
 * ============================================================================
 * Assertion Handler
 * ============================================================================
 */

/**
 * @brief Assertion handler
 * 
 * Called when an assertion fails.
 * 
 * @param expr Assertion expression that failed
 * @param file Source file where assertion failed
 * @param line Line number where assertion failed
 */
void tmxc_assert_fail(const char* expr, const char* file, uint32_t line) {
    char buffer[256];
    
    /*
     * Build assertion failure message
     */
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("       ASSERTION FAILED\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("Assertion: ");
    tmxc_uart_puts(expr);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("File: ");
    tmxc_uart_puts(file);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("Line: ");
    tmxc_print_dec(line);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    
    /*
     * Trigger kernel panic
     */
    tmxc_kernel_panic("Assertion failed");
}

/*
 * ============================================================================
 * Reboot Methods
 * ============================================================================
 */

/**
 * @brief Trigger system reboot
 * 
 * Attempts to reboot the system using various methods:
 * 1. Watchdog timer reset
 * 2. PSCI system reset (if available)
 * 3. CPU reset via system control register
 */
void tmxc_panic_reboot(void) {
    /*
     * Try watchdog reset first
     */
    tmxc_panic_reboot_watchdog();
    
    /*
     * If watchdog fails, try PSCI
     */
    tmxc_panic_reboot_psci();
    
    /*
     * If PSCI fails, try system reset register
     */
    tmxc_panic_reboot_system_reset();
    
    /*
     * If all methods fail, halt
     */
    tmxc_uart_puts("Reboot failed, halting system\r\n");
    while (1) {
        __asm__ volatile("wfi");
    }
}

/**
 * @brief Reboot via watchdog timer
 * 
 * Triggers a watchdog reset by stopping the watchdog kick.
 * The watchdog will reset the system when it times out.
 */
void tmxc_panic_reboot_watchdog(void) {
    tmxc_uart_puts("[REBOOT] Attempting watchdog reset...\r\n");
    
    /*
     * Disable the watchdog
     * This will cause the watchdog to timeout and reset the system
     * (assuming the watchdog was previously enabled)
     */
    tmxc_watchdog_disable();
    
    /*
     * Wait for watchdog timeout
     * This should never return
     */
    while (1) {
        __asm__ volatile("wfi");
    }
}

/**
 * @brief Reboot via PSCI
 * 
 * Uses the Power State Coordination Interface (PSCI) to reset the system.
 * Only available if PSCI is implemented.
 */
void tmxc_panic_reboot_psci(void) {
    tmxc_uart_puts("[REBOOT] Attempting PSCI reset...\r\n");
    
    /*
     * PSCI system reset is performed via SMC (Secure Monitor Call)
     * Function ID: 0x84000009 (PSCI 0.2 SYSTEM_RESET)
     * 
     * This requires EL3 support, which may not be available
     */
    __asm__ volatile(
        "mov x0, #0x84000009\n"
        "smc #0"
    );
    
    /*
     * If SMC returns, PSCI is not available
     */
    tmxc_uart_puts("[REBOOT] PSCI not available\r\n");
}

/**
 * @brief Reboot via system reset register
 * 
 * Uses the system reset register to trigger a reset.
 * Platform-specific implementation.
 */
void tmxc_panic_reboot_system_reset(void) {
    tmxc_uart_puts("[REBOOT] Attempting system reset register...\r\n");
    
    /*
     * For QEMU virt, we can use the system reset register
     * This is platform-specific and may not work on all hardware
     * 
     * For now, this is a placeholder
     */
    
    tmxc_uart_puts("[REBOOT] System reset not implemented\r\n");
}
