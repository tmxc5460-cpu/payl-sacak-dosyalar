/*
 * TMXC_OS - Hardware Watchdog Timer Implementation (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file watchdog.c
 * @brief Hardware Watchdog Timer driver implementation
 * 
 * This driver implements the SP805 watchdog timer for fail-safe reset.
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#include "watchdog.h"
#include "mmio.h"
#include "uart.h"

/*
 * ============================================================================
 * Watchdog Register Access Macros
 * ============================================================================
 */

/**
 * @brief Read watchdog register
 */
#define WDOG_READ(offset) \
    TMXC_MMIO_READ32(TMXC_WATCHDOG_BASE + (offset))

/**
 * @brief Write watchdog register
 */
#define WDOG_WRITE(offset, value) \
    TMXC_MMIO_WRITE32(TMXC_WATCHDOG_BASE + (offset), (value))

/*
 * ============================================================================
 * Watchdog Initialization
 * ============================================================================
 */

/**
 * @brief Initialize hardware watchdog timer
 * 
 * Configures the watchdog timer with the specified timeout.
 * The watchdog is initially disabled and must be explicitly enabled.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_watchdog_init(void) {
    /*
     * Step 1: Unlock the watchdog (if locked)
     * Write the magic value to unlock configuration
     */
    WDOG_WRITE(WDOG_LOCK_OFFSET, WDOG_LOCK_VALUE);
    
    /*
     * Step 2: Disable the watchdog
     * Clear enable and reset bits
     */
    WDOG_WRITE(WDOG_CONTROL_OFFSET, 0);
    
    /*
     * Step 3: Set the load value (timeout)
     * Load value = timeout_seconds * clock_frequency
     */
    WDOG_WRITE(WDOG_LOAD_OFFSET, TMXC_WATCHDOG_LOAD_VALUE);
    
    /*
     * Step 4: Clear any pending interrupts
     */
    WDOG_WRITE(WDOG_INTCLR_OFFSET, 0xFFFFFFFF);
    
    tmxc_uart_puts("[WATCHDOG] Watchdog initialized\r\n");
    tmxc_uart_puts("[WATCHDOG] Timeout: ");
    tmxc_print_dec(TMXC_WATCHDOG_TIMEOUT);
    tmxc_uart_puts(" seconds\r\n");
    
    return 0;
}

/*
 * ============================================================================
 * Watchdog Control Functions
 * ============================================================================
 */

/**
 * @brief Enable the watchdog timer
 * 
 * Starts the watchdog countdown.
 * The system will reset if the watchdog is not kicked within the timeout.
 */
void tmxc_watchdog_enable(void) {
    /*
     * Unlock the watchdog (if locked)
     */
    WDOG_WRITE(WDOG_LOCK_OFFSET, WDOG_LOCK_VALUE);
    
    /*
     * Enable watchdog with reset
     * Set enable bit and reset bit
     */
    WDOG_WRITE(WDOG_CONTROL_OFFSET, WDOG_CONTROL_ENABLE | WDOG_CONTROL_RESET);
    
    /*
     * Lock the watchdog to prevent accidental changes
     */
    WDOG_WRITE(WDOG_LOCK_OFFSET, WDOG_LOCK_VALUE);
    
    tmxc_uart_puts("[WATCHDOG] Watchdog enabled\r\n");
}

/**
 * @brief Disable the watchdog timer
 * 
 * Stops the watchdog countdown.
 * The system will not reset even if the watchdog is not kicked.
 */
void tmxc_watchdog_disable(void) {
    /*
     * Unlock the watchdog (if locked)
     */
    WDOG_WRITE(WDOG_LOCK_OFFSET, WDOG_LOCK_VALUE);
    
    /*
     * Disable watchdog
     * Clear enable and reset bits
     */
    WDOG_WRITE(WDOG_CONTROL_OFFSET, 0);
    
    /*
     * Lock the watchdog
     */
    WDOG_WRITE(WDOG_LOCK_OFFSET, WDOG_LOCK_VALUE);
    
    tmxc_uart_puts("[WATCHDOG] Watchdog disabled\r\n");
}

/**
 * @brief Kick the watchdog timer
 * 
 * Resets the watchdog countdown to the initial value.
 * Must be called periodically during normal operation to prevent reset.
 */
void tmxc_watchdog_kick(void) {
    /*
     * Write to the load register to reset the countdown
     * This is the standard way to "kick" the SP805 watchdog
     */
    WDOG_WRITE(WDOG_LOAD_OFFSET, TMXC_WATCHDOG_LOAD_VALUE);
}

/*
 * ============================================================================
 * Watchdog Status Functions
 * ============================================================================
 */

/**
 * @brief Get current watchdog value
 * 
 * Returns the current countdown value.
 * 
 * @return uint32_t Current countdown value
 */
uint32_t tmxc_watchdog_get_value(void) {
    return WDOG_READ(WDOG_VALUE_OFFSET);
}

/**
 * @brief Check if watchdog interrupt is pending
 * 
 * @return int 1 if interrupt pending, 0 otherwise
 */
int tmxc_watchdog_interrupt_pending(void) {
    return (WDOG_READ(WDOG_RIS_OFFSET) & 0x1) != 0;
}

/**
 * @brief Clear watchdog interrupt
 * 
 * Clears the watchdog interrupt flag.
 */
void tmxc_watchdog_clear_interrupt(void) {
    WDOG_WRITE(WDOG_INTCLR_OFFSET, 0xFFFFFFFF);
}

/*
 * ============================================================================
 * Watchdog Lock Functions
 * ============================================================================
 */

/**
 * @brief Lock watchdog configuration
 * 
 * Locks the watchdog configuration to prevent accidental changes.
 * Once locked, the configuration cannot be changed until system reset.
 */
void tmxc_watchdog_lock(void) {
    WDOG_WRITE(WDOG_LOCK_OFFSET, WDOG_LOCK_VALUE);
}

/**
 * @brief Unlock watchdog configuration
 * 
 * Unlocks the watchdog configuration to allow changes.
 */
void tmxc_watchdog_unlock(void) {
    WDOG_WRITE(WDOG_LOCK_OFFSET, WDOG_LOCK_VALUE);
}
