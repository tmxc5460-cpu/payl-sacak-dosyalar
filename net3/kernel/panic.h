/*
 * TMXC_OS - Kernel Panic Handler (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file panic.h
 * @brief Fail-safe panic handler with register dump and reboot
 * 
 * This module implements the kernel panic handler that is called when a fatal
 * error occurs. It dumps CPU registers to UART, logs the stack trace, and
 * triggers a clean reboot.
 * 
 * ARMv8-A Architecture Reference Manual:
 * - Panic handler saves all general-purpose registers
 * - Reads system registers (ELR, ESR, FAR, etc.)
 * - Dumps stack trace by walking back through frames
 * - Triggers system reset via watchdog or PSCI
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#ifndef TMXC_PANIC_H
#define TMXC_PANIC_H

#include <stdint.h>

/*
 * ============================================================================
 * Panic Context Structure
 * ============================================================================
 */

/**
 * @brief Saved processor state during panic
 * 
 * This structure holds the complete processor state when a panic occurs.
 * It includes all general-purpose registers and system registers.
 */
typedef struct {
    /* General purpose registers */
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint64_t x11;
    uint64_t x12;
    uint64_t x13;
    uint64_t x14;
    uint64_t x15;
    uint64_t x16;
    uint64_t x17;
    uint64_t x18;
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29;     /* Frame pointer */
    uint64_t x30;     /* Link register */
    
    /* Special registers */
    uint64_t sp;      /* Stack pointer */
    uint64_t pc;      /* Program counter (ELR_EL1) */
    uint64_t cpsr;    /* Current program status (SPSR_EL1) */
    
    /* Exception-related registers */
    uint64_t esr;     /* Exception syndrome (ESR_EL1) */
    uint64_t far;     /* Fault address (FAR_EL1) */
    
    /* System registers */
    uint64_t ttbr0;   /* Translation table base register 0 */
    uint64_t ttbr1;   /* Translation table base register 1 */
    uint64_t tcr;     /* Translation control register */
    uint64_t mair;    /* Memory attribute indirection register */
    
    /* Timer registers */
    uint64_t cntvct;  /* Counter timer virtual count */
    uint64_t cntfrq;  /* Counter timer frequency */
    
    /* Current exception level */
    uint64_t current_el;
    
    /* MIDR (CPU ID) */
    uint64_t midr;
    
    /* MPIDR (Multiprocessor affinity) */
    uint64_t mpidr;
} tmxc_panic_context_t;

/*
 * ============================================================================
 * Panic Function Declarations
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
void tmxc_kernel_panic(const char* message) __attribute__((noreturn));

/**
 * @brief Panic with context
 * 
 * Called when a panic occurs with a specific context (e.g., from exception handler).
 * 
 * @param message Panic message describing the error
 * @param ctx Panic context (saved processor state)
 */
void tmxc_kernel_panic_context(const char* message, tmxc_panic_context_t* ctx) __attribute__((noreturn));

/**
 * @brief Save current context
 * 
 * Saves the current processor state to a panic context structure.
 * 
 * @param ctx Pointer to panic context structure to fill
 */
void tmxc_panic_save_context(tmxc_panic_context_t* ctx);

/**
 * @brief Dump panic context to UART
 * 
 * Outputs the saved processor state to UART for debugging.
 * 
 * @param ctx Panic context (saved processor state)
 */
void tmxc_panic_dump_context(tmxc_panic_context_t* ctx);

/**
 * @brief Dump stack trace to UART
 * 
 * Walks the stack and outputs the return addresses.
 * 
 * @param sp Current stack pointer
 * @param fp Current frame pointer
 */
void tmxc_panic_dump_stack_trace(uint64_t sp, uint64_t fp);

/**
 * @brief Trigger system reboot
 * 
 * Attempts to reboot the system using various methods:
 * 1. Watchdog timer reset
 * 2. PSCI system reset (if available)
 * 3. CPU reset via system control register
 */
void tmxc_panic_reboot(void) __attribute__((noreturn));

/*
 * ============================================================================
 * Panic Utility Functions
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
void tmxc_assert_fail(const char* expr, const char* file, uint32_t line) __attribute__((noreturn));

/**
 * @brief Panic assertion macro
 * 
 * Checks a condition and panics if it fails.
 */
#define TMXC_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            tmxc_assert_fail(#expr, __FILE__, __LINE__); \
        } \
    } while (0)

/**
 * @brief Panic if null macro
 * 
 * Panics if the pointer is null.
 */
#define TMXC_ASSERT_NOT_NULL(ptr) \
    TMXC_ASSERT((ptr) != NULL)

/**
 * @brief Panic if out of range macro
 * 
 * Panics if value is outside the specified range.
 */
#define TMXC_ASSERT_IN_RANGE(value, min, max) \
    TMXC_ASSERT((value) >= (min) && (value) <= (max))

/*
 * ============================================================================
 * Panic Reboot Methods
 * ============================================================================
 */

/**
 * @brief Reboot via watchdog timer
 * 
 * Triggers a watchdog reset by stopping the watchdog kick.
 * The watchdog will reset the system when it times out.
 */
void tmxc_panic_reboot_watchdog(void) __attribute__((noreturn));

/**
 * @brief Reboot via PSCI
 * 
 * Uses the Power State Coordination Interface (PSCI) to reset the system.
 * Only available if PSCI is implemented.
 */
void tmxc_panic_reboot_psci(void) __attribute__((noreturn));

/**
 * @brief Reboot via system reset register
 * 
 * Uses the system reset register to trigger a reset.
 * Platform-specific implementation.
 */
void tmxc_panic_reboot_system_reset(void) __attribute__((noreturn));

#endif /* TMXC_PANIC_H */
