/*
 * TMXC_OS - Hardware Watchdog Timer (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file watchdog.h
 * @brief Hardware Watchdog Timer driver for fail-safe reset
 * 
 * This driver implements a hardware watchdog timer that automatically resets
 * the system if the kernel freezes or panics. The watchdog must be
 * periodically "kicked" to prevent a reset.
 * 
 * ARMv8-A: Watchdog timers are platform-specific.
 * This driver uses the SP805 watchdog (common in ARM platforms).
 * For QEMU virt, we use a simplified implementation.
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#ifndef TMXC_WATCHDOG_H
#define TMXC_WATCHDOG_H

#include <stdint.h>

/*
 * ============================================================================
 * Watchdog Configuration
 * ============================================================================
 */

/**
 * @brief Watchdog base addresses for different platforms
 * 
 * QEMU virt: 0x09010000 (SP805 watchdog)
 * Raspberry Pi: 0x3F100000 (BCM watchdog)
 */
#define TMXC_WATCHDOG_BASE_QEMU   0x09010000
#define TMXC_WATCHDOG_BASE_RPI    0x3F100000

/**
 * @brief Default watchdog base address
 */
#ifndef TMXC_WATCHDOG_BASE
#define TMXC_WATCHDOG_BASE       TMXC_WATCHDOG_BASE_QEMU
#endif

/**
 * @brief Watchdog clock frequency
 * 
 * QEMU virt: 24000000 Hz (24 MHz)
 * Raspberry Pi: 1000000 Hz (1 MHz)
 */
#ifndef TMXC_WATCHDOG_CLOCK
#define TMXC_WATCHDOG_CLOCK      24000000
#endif

/**
 * @brief Watchdog timeout in seconds
 * 
 * Default: 5 seconds
 * The kernel must kick the watchdog within this interval.
 */
#ifndef TMXC_WATCHDOG_TIMEOUT
#define TMXC_WATCHDOG_TIMEOUT    5
#endif

/**
 * @brief Watchdog load value
 * 
 * Calculated as: timeout_seconds * clock_frequency
 */
#define TMXC_WATCHDOG_LOAD_VALUE (TMXC_WATCHDOG_TIMEOUT * TMXC_WATCHDOG_CLOCK)

/*
 * ============================================================================
 * SP805 Watchdog Register Offsets
 * ============================================================================
 */

/**
 * @brief Watchdog Load Register
 * 
 * Write the initial countdown value here.
 */
#define WDOG_LOAD_OFFSET         0x00

/**
 * @brief Watchdog Value Register
 * 
 * Read the current countdown value.
 */
#define WDOG_VALUE_OFFSET        0x04

/**
 * @brief Watchdog Control Register
 * 
 * Bit 0: Watchdog enable
 * Bit 1: Watchdog reset enable
 */
#define WDOG_CONTROL_OFFSET      0x08

/**
 * @brief Watchdog Interrupt Clear Register
 * 
 * Write to clear watchdog interrupt.
 */
#define WDOG_INTCLR_OFFSET       0x0C

/**
 * @brief Watchdog Raw Interrupt Status Register
 */
#define WDOG_RIS_OFFSET          0x10

/**
 * @brief Watchdog Masked Interrupt Status Register
 */
#define WDOG_MIS_OFFSET          0x14

/**
 * @brief Watchdog Lock Register
 * 
 * Write to lock watchdog configuration.
 * Once locked, configuration cannot be changed until reset.
 */
#define WDOG_LOCK_OFFSET         0xC00

/**
 * @brief Watchdog Lock value
 * 
 * Magic value to lock the watchdog.
 */
#define WDOG_LOCK_VALUE          0x1ACCE551

/*
 * ============================================================================
 * Watchdog Control Register Bits
 * ============================================================================
 */

/**
 * @brief Watchdog enable bit
 */
#define WDOG_CONTROL_ENABLE      (1 << 0)

/**
 * @brief Watchdog reset enable bit
 */
#define WDOG_CONTROL_RESET       (1 << 1)

/*
 * ============================================================================
 * Watchdog Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize hardware watchdog timer
 * 
 * Configures the watchdog timer with the specified timeout.
 * The watchdog is initially disabled and must be explicitly enabled.
 * 
 * Steps:
 * 1. Unlock the watchdog (if locked)
 * 2. Disable the watchdog
 * 3. Set the load value (timeout)
 * 4. Enable the watchdog
 * 5. Lock the watchdog (optional)
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_watchdog_init(void);

/**
 * @brief Enable the watchdog timer
 * 
 * Starts the watchdog countdown.
 * The system will reset if the watchdog is not kicked within the timeout.
 */
void tmxc_watchdog_enable(void);

/**
 * @brief Disable the watchdog timer
 * 
 * Stops the watchdog countdown.
 * The system will not reset even if the watchdog is not kicked.
 */
void tmxc_watchdog_disable(void);

/**
 * @brief Kick the watchdog timer
 * 
 * Resets the watchdog countdown to the initial value.
 * Must be called periodically during normal operation to prevent reset.
 */
void tmxc_watchdog_kick(void);

/**
 * @brief Get current watchdog value
 * 
 * Returns the current countdown value.
 * 
 * @return uint32_t Current countdown value
 */
uint32_t tmxc_watchdog_get_value(void);

/**
 * @brief Check if watchdog interrupt is pending
 * 
 * @return int 1 if interrupt pending, 0 otherwise
 */
int tmxc_watchdog_interrupt_pending(void);

/**
 * @brief Clear watchdog interrupt
 * 
 * Clears the watchdog interrupt flag.
 */
void tmxc_watchdog_clear_interrupt(void);

/**
 * @brief Lock watchdog configuration
 * 
 * Locks the watchdog configuration to prevent accidental changes.
 * Once locked, the configuration cannot be changed until system reset.
 */
void tmxc_watchdog_lock(void);

/**
 * @brief Unlock watchdog configuration
 * 
 * Unlocks the watchdog configuration to allow changes.
 */
void tmxc_watchdog_unlock(void);

#endif /* TMXC_WATCHDOG_H */
