/*
 * TMXC_OS - Kernel Main Entry Point (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file kernel_main_new.c
 * @brief Main kernel entry point and initialization sequence for TMXC OS
 * 
 * This file contains the primary kernel entry point that is called from the
 * boot assembly code. It orchestrates the initialization of all kernel
 * subsystems in the correct order.
 * 
 * Architecture: ARMv8-A (AArch64)
 * Exception Level: EL1 (Kernel Mode)
 * Memory Model: Virtual Memory (after MMU initialization)
 * 
 * Zero-Dependency: This code does not use any standard C library (libc).
 * All functionality is implemented from scratch.
 */

#ifndef TMXC_KERNEL_MAIN_NEW_C
#define TMXC_KERNEL_MAIN_NEW_C

#include <stdint.h>

/*
 * ============================================================================
 * License System
 * ============================================================================
 */

/**
 * @brief Check license at boot
 * 
 * Performs complete license check at boot time.
 * Refuses to boot if license is invalid.
 * 
 * @return 0 on success (license valid), negative error code on failure
 */
extern int tmxc_license_boot_check(void);

/*
 * ============================================================================
 * Kernel Constants and Configuration
 * ============================================================================
 */

/**
 * @brief Kernel version information
 */
#define TMXC_VERSION_MAJOR  1
#define TMXC_VERSION_MINOR  0
#define TMXC_VERSION_PATCH  0

/**
 * @brief Memory configuration
 * 
 * ARMv8-A supports 4KB, 16KB, and 64KB page sizes.
 * We use 4KB pages for compatibility and fine-grained memory control.
 */
#define TMXC_PAGE_SIZE           4096    /* 4KB pages */
#define TMXC_KERNEL_STACK_SIZE   65536   /* 64KB kernel stack */
#define TMXC_MEMORY_SIZE         (512 * 1024 * 1024) /* 512MB RAM */

/**
 * @brief Physical memory layout
 * 
 * These addresses are platform-specific and should be configured
 * for the target hardware (e.g., Raspberry Pi, QEMU virt machine).
 */
#define TMXC_KERNEL_BASE         0x40080000  /* Kernel load address */
#define TMXC_PHYS_MEMORY_BASE    0x40000000  /* Physical RAM base */

/*
 * ============================================================================
 * Forward Declarations (Zero-Dependency)
 * ============================================================================
 */

/**
 * @brief Initialize MMIO framework for hardware register access
 * 
 * This sets up the memory-mapped I/O subsystem that allows safe
 * access to hardware registers with proper volatile semantics.
 * 
 * @return 0 on success, negative error code on failure
 */
extern int tmxc_mmio_init(void);

/**
 * @brief Initialize UART driver for console output
 * 
 * Sets up the UART hardware for serial console communication.
 * This is the primary debugging output mechanism.
 * 
 * @return 0 on success, negative error code on failure
 */
extern int tmxc_uart_init(void);

/**
 * @brief Output a single character to UART
 * 
 * @param c Character to output
 */
extern void tmxc_uart_putc(char c);

/**
 * @brief Output a null-terminated string to UART
 * 
 * @param str String to output
 */
extern void tmxc_uart_puts(const char* str);

/**
 * @brief Initialize Memory Management Unit (MMU)
 * 
 * Sets up virtual memory with 4KB page tables.
 * Enforces NX (No-Execute) and Read-Only permissions on kernel text.
 * 
 * @return 0 on success, negative error code on failure
 */
extern int tmxc_mmu_init(void);

/**
 * @brief Configure Exception Vector Table
 * 
 * Sets up the exception vectors for EL1 to handle:
 * - Synchronous exceptions (e.g., data abort, instruction abort)
 * - IRQ (interrupt requests)
 * - FIQ (fast interrupt requests)
 * - SError (system errors)
 * 
 * @return 0 on success, negative error code on failure
 */
extern int tmxc_exceptions_init(void);

/**
 * @brief Initialize Hardware Watchdog Timer
 * 
 * Sets up the watchdog timer to automatically reset the system
 * if the kernel freezes or panics.
 * 
 * @return 0 on success, negative error code on failure
 */
extern int tmxc_watchdog_init(void);

/**
 * @brief Kick the watchdog timer
 * 
 * Resets the watchdog countdown to prevent system reset.
 * Must be called periodically during normal operation.
 */
extern void tmxc_watchdog_kick(void);

/**
 * @brief Initialize preemptive scheduler
 * 
 * Sets up the timer-driven preemptive scheduler with IRQ-based
 * task switching. Eliminates busy-waiting loops.
 * 
 * @return 0 on success, negative error code on failure
 */
extern int tmxc_scheduler_init(void);

/**
 * @brief Start the scheduler
 * 
 * Begins task scheduling. Does not return.
 */
extern void tmxc_scheduler_start(void) __attribute__((noreturn));

/**
 * @brief Initialize stack canary protection
 * 
 * Sets up stack canaries for kernel-space stack overflow detection.
 * Places random values at stack boundaries and validates them on context switch.
 * 
 * @return 0 on success, negative error code on failure
 */
extern int tmxc_stack_canary_init(void);

/**
 * @brief Kernel panic handler
 * 
 * Called when a fatal error occurs. Dumps CPU registers to UART,
 * logs stack trace, and triggers a clean reboot.
 * 
 * @param message Panic message describing the error
 */
extern void tmxc_kernel_panic(const char* message) __attribute__((noreturn));

/*
 * ============================================================================
 * ARMv8-A System Register Access Functions
 * ============================================================================
 */

/**
 * @brief Read the current CPU cycle count
 * 
 * Uses the CNTVCT_EL0 register (Counter-timer Virtual Count Register).
 * This provides a monotonically increasing counter for timing.
 * 
 * @return Current cycle count
 */
static inline uint64_t tmxc_get_cycle_count(void) {
    uint64_t count;
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(count));
    return count;
}

/**
 * @brief Read the counter frequency
 * 
 * Uses the CNTFRQ_EL0 register (Counter-timer Frequency Register).
 * Returns the frequency of the system counter in Hz.
 * 
 * @return Counter frequency in Hz
 */
static inline uint64_t tmxc_get_frequency(void) {
    uint64_t freq;
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;
}

/**
 * @brief Read the current exception level
 * 
 * Uses the CurrentEL register to determine the current privilege level.
 * 
 * @return Current exception level (0-3)
 */
static inline uint64_t tmxc_get_current_el(void) {
    uint64_t el;
    __asm__ volatile("mrs %0, CurrentEL" : "=r"(el));
    return (el >> 2) & 0x3;
}

/**
 * @brief Wait For Event instruction
 * 
 * Puts the CPU into a low-power state until an event occurs.
 * Used in idle loops to reduce power consumption.
 */
static inline void tmxc_wfi(void) {
    __asm__ volatile("wfi");
}

/**
 * @brief Data Synchronization Barrier
 * 
 * Ensures all memory accesses before this point complete before
 * any memory accesses after this point.
 */
static inline void tmxc_dsb_sy(void) {
    __asm__ volatile("dsb sy");
}

/**
 * @brief Instruction Synchronization Barrier
 * 
 * Ensures all instructions before this point complete before
 * any instructions after this point are fetched or executed.
 */
static inline void tmxc_isb(void) {
    __asm__ volatile("isb");
}

/*
 * ============================================================================
 * Utility Functions (Zero-Dependency)
 * ============================================================================
 */

/**
 * @brief Convert a 64-bit value to hexadecimal string (OPTIMIZED)
 * 
 * @param value Value to convert
 * @param buffer Output buffer (must be at least 17 bytes)
 */
static void tmxc_print_hex(uint64_t value) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[17];
    buffer[16] = '\0';
    
    /* Optimized: Process nibbles in pairs */
    for (int i = 15; i >= 0; i -= 2) {
        buffer[i] = hex_chars[value & 0xF];
        value >>= 4;
        buffer[i - 1] = hex_chars[value & 0xF];
        value >>= 4;
    }
    
    tmxc_uart_puts(buffer);
}

/**
 * @brief Convert a 64-bit value to decimal string
 * 
 * @param value Value to convert
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
 * Kernel Banner and Information
 * ============================================================================
 */

/**
 * @brief Print kernel boot banner
 * 
 * Displays kernel version, architecture, and platform information.
 */
static void tmxc_kernel_banner(void) {
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("       TMXC OS - Revolutionary Mobile\r\n");
    tmxc_uart_puts("       Bare-Metal Kernel Architecture\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("Version: ");
    tmxc_print_dec(TMXC_VERSION_MAJOR);
    tmxc_uart_putc('.');
    tmxc_print_dec(TMXC_VERSION_MINOR);
    tmxc_uart_putc('.');
    tmxc_print_dec(TMXC_VERSION_PATCH);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("Architecture: ARM64 (AArch64)\r\n");
    tmxc_uart_puts("Exception Level: EL1\r\n");
    tmxc_uart_puts("Platform: Mobile Devices\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("\r\n");
}

/**
 * @brief Detect and report CPU information
 * 
 * Reads CPU identification registers and reports:
 * - CPU implementer, variant, architecture, part number, revision
 * - CPU affinity (core ID)
 * - Timer frequency
 * - Memory model
 * - Execution state support
 */
static void tmxc_hardware_detect(void) {
    uint64_t midr;
    __asm__ volatile("mrs %0, midr_el1" : "=r"(midr));
    
    uint64_t mpidr;
    __asm__ volatile("mrs %0, mpidr_el1" : "=r"(mpidr));
    
    uint32_t affinity = (mpidr >> 8) & 0xFF;
    
    tmxc_uart_puts("[HW] CPU Detected: Implementer ");
    tmxc_print_hex((midr >> 24) & 0xFF);
    tmxc_uart_puts(", Variant ");
    tmxc_print_hex((midr >> 20) & 0xF);
    tmxc_uart_puts(", Architecture ");
    tmxc_print_hex((midr >> 16) & 0xF);
    tmxc_uart_puts(", Part ");
    tmxc_print_hex((midr >> 4) & 0xFFF);
    tmxc_uart_puts(", Revision ");
    tmxc_print_hex(midr & 0xF);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("[HW] CPU Affinity: ");
    tmxc_print_dec(affinity);
    tmxc_uart_puts("\r\n");
    
    uint64_t freq = tmxc_get_frequency();
    tmxc_uart_puts("[HW] Timer Frequency: ");
    tmxc_print_dec(freq);
    tmxc_uart_puts(" Hz\r\n");
    
    uint64_t current_el = tmxc_get_current_el();
    tmxc_uart_puts("[HW] Current Exception Level: EL");
    tmxc_print_dec(current_el);
    tmxc_uart_puts("\r\n");
    
    uint64_t id_aa64mmfr0;
    __asm__ volatile("mrs %0, id_aa64mmfr0_el1" : "=r"(id_aa64mmfr0));
    tmxc_uart_puts("[HW] Memory Model: ");
    tmxc_print_hex(id_aa64mmfr0 & 0xF);
    tmxc_uart_puts("\r\n");
    
    uint64_t id_aa64pfr0;
    __asm__ volatile("mrs %0, id_aa64pfr0_el1" : "=r"(id_aa64pfr0));
    tmxc_uart_puts("[HW] EL0 Execution: ");
    tmxc_print_hex((id_aa64pfr0 >> 0) & 0xF);
    tmxc_uart_puts("\r\n");
}

/**
 * @brief Report memory configuration
 * 
 * Displays total memory size, page size, and memory layout.
 */
static void tmxc_memory_info(void) {
    tmxc_uart_puts("[MEM] Total Memory: ");
    tmxc_print_dec(TMXC_MEMORY_SIZE / (1024 * 1024));
    tmxc_uart_puts(" MB\r\n");
    
    tmxc_uart_puts("[MEM] Page Size: ");
    tmxc_print_dec(TMXC_PAGE_SIZE);
    tmxc_uart_puts(" bytes\r\n");
    
    tmxc_uart_puts("[MEM] Kernel Base: 0x");
    tmxc_print_hex(TMXC_KERNEL_BASE);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("[MEM] Physical Base: 0x");
    tmxc_print_hex(TMXC_PHYS_MEMORY_BASE);
    tmxc_uart_puts("\r\n");
}

/*
 * ============================================================================
 * Kernel Main Entry Point
 * ============================================================================
 */

/**
 * @brief Kernel main entry point
 * 
 * This is the primary C entry point called from boot.S.
 * It initializes all kernel subsystems in a specific order:
 * 
 * 1. MMIO Framework (for hardware access)
 * 2. UART (for console output)
 * 3. Hardware detection (CPU/memory info)
 * 4. MMU (virtual memory with NX/RO)
 * 5. Exception vectors (for interrupt handling)
 * 6. Watchdog (for fail-safe reset)
 * 7. Stack canaries (for overflow protection)
 * 8. Scheduler (for preemptive multitasking)
 * 
 * Each initialization step is checked for errors. If any step fails,
 * the kernel panics with a descriptive error message.
 * 
 * @param argc Argument count (always 0 for bare-metal)
 * @param argv Argument vector (always NULL for bare-metal)
 * 
 * @note This function does not return under normal operation.
 *       It either calls the scheduler (which never returns) or panics.
 */
void tmxc_kernel_main(uint64_t argc, uint64_t argv) {
    (void)argc;  /* Unused in bare-metal */
    (void)argv;  /* Unused in bare-metal */
    
    uint64_t boot_time = tmxc_get_cycle_count();
    
    /*
     * Step 1: Initialize MMIO Framework
     * This must be first as all hardware access depends on it.
     */
    if (tmxc_mmio_init() != 0) {
        /* Cannot report error yet (UART not initialized) */
        while (1) {
            tmxc_wfi();
        }
    }
    
    /*
     * Step 2: Initialize UART for console output
     * This enables us to report errors and debug information.
     */
    if (tmxc_uart_init() != 0) {
        /* Cannot report error (UART failed) */
        while (1) {
            tmxc_wfi();
        }
    }
    
    /* Now we can use UART for output */
    tmxc_kernel_banner();
    tmxc_uart_puts("[INIT] TMXC OS Kernel Starting...\r\n");
    
    /*
     * Step 3: License Check (CRITICAL)
     * Verify license before proceeding with kernel initialization.
     * System refuses to boot if license is invalid.
     */
    tmxc_uart_puts("[INIT] Checking License...\r\n");
    if (tmxc_license_boot_check() != 0) {
        tmxc_kernel_panic("License validation failed - System halted");
    }
    tmxc_uart_puts("[INIT] License validated successfully\r\n");
    
    /*
     * Step 4: Detect and report hardware
     */
    tmxc_hardware_detect();
    tmxc_memory_info();
    
    /*
     * Step 5: Initialize MMU
     * Sets up virtual memory with 4KB page tables.
     * Enforces NX (No-Execute) on data pages.
     * Enforces Read-Only on kernel text pages.
     */
    tmxc_uart_puts("[INIT] Initializing MMU...\r\n");
    if (tmxc_mmu_init() != 0) {
        tmxc_kernel_panic("MMU initialization failed");
    }
    tmxc_uart_puts("[INIT] MMU initialized successfully\r\n");
    
    /*
     * Step 6: Initialize Exception Vector Table
     * Sets up handlers for synchronous exceptions, IRQ, FIQ, SError.
     */
    tmxc_uart_puts("[INIT] Initializing Exception Vectors...\r\n");
    if (tmxc_exceptions_init() != 0) {
        tmxc_kernel_panic("Exception vector initialization failed");
    }
    tmxc_uart_puts("[INIT] Exception vectors configured\r\n");
    
    /*
     * Step 7: Initialize Hardware Watchdog
     * Provides fail-safe reset capability if kernel freezes.
     */
    tmxc_uart_puts("[INIT] Initializing Hardware Watchdog...\r\n");
    if (tmxc_watchdog_init() != 0) {
        tmxc_kernel_panic("Watchdog initialization failed");
    }
    tmxc_uart_puts("[INIT] Hardware watchdog initialized\r\n");
    
    /*
     * Step 8: Initialize Stack Canaries
     * Provides stack overflow detection for kernel tasks.
     */
    tmxc_uart_puts("[INIT] Initializing Stack Canaries...\r\n");
    if (tmxc_stack_canary_init() != 0) {
        tmxc_kernel_panic("Stack canary initialization failed");
    }
    tmxc_uart_puts("[INIT] Stack canaries initialized\r\n");
    
    /*
     * Step 9: Initialize Preemptive Scheduler
     * Sets up timer-driven task switching.
     */
    tmxc_uart_puts("[INIT] Initializing Scheduler...\r\n");
    if (tmxc_scheduler_init() != 0) {
        tmxc_kernel_panic("Scheduler initialization failed");
    }
    tmxc_uart_puts("[INIT] Scheduler initialized\r\n");
    
    /*
     * Kernel initialization complete
     * Report boot time and start scheduler.
     */
    uint64_t boot_cycles = tmxc_get_cycle_count() - boot_time;
    uint64_t boot_ms = (boot_cycles * 1000) / tmxc_get_frequency();
    
    tmxc_uart_puts("[INIT] Kernel Initialization Complete\r\n");
    tmxc_uart_puts("[INIT] Boot Time: ");
    tmxc_print_dec(boot_ms);
    tmxc_uart_puts(" ms\r\n");
    
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("       TMXC OS Ready\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("\r\n");
    
    /*
     * Kick the watchdog before starting scheduler
     */
    tmxc_watchdog_kick();
    
    /*
     * Start the scheduler
     * This function never returns under normal operation.
     */
    tmxc_uart_puts("[SCHED] Starting Scheduler...\r\n");
    tmxc_scheduler_start();
    
    /*
     * Should never reach here
     */
    tmxc_kernel_panic("Scheduler returned unexpectedly");
}

#endif /* TMXC_KERNEL_MAIN_NEW_C */
