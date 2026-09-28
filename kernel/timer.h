/*
 * TMXC OS - Timer Driver Header
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Generic Timer driver for ARM64 architecture.
 * Provides system timer for scheduling.
 */

#ifndef TMXC_TIMER_H
#define TMXC_TIMER_H

#include <stdint.h>

/*
 * ============================================================================
 * Timer Configuration
 * ============================================================================
 */

/**
 * @brief Timer frequency in Hz
 * 
 * The generic timer frequency is typically 1-100MHz depending on hardware.
 * For QEMU virt machine, timer frequency is 62.5MHz.
 */
#define TMXC_TIMER_FREQ 62500000

/**
 * @brief Timer tick interval in microseconds
 */
#define TMXC_TICK_INTERVAL_US 10000  /* 10ms */

/*
 * ============================================================================
 * Timer Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize timer
 * 
 * Initializes the generic timer.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_timer_init(void);

/**
 * @brief Get current timer value
 * 
 * Returns the current value of the system counter.
 * 
 * @return uint64_t Current timer value
 */
uint64_t tmxc_timer_get_value(void);

/**
 * @brief Get timer frequency
 * 
 * Returns the frequency of the system counter.
 * 
 * @return uint64_t Timer frequency in Hz
 */
uint64_t tmxc_timer_get_freq(void);

/**
 * @brief Set timer compare value
 * 
 * Sets the compare value for the timer.
 * When the counter reaches this value, an interrupt is generated.
 * 
 * @param compare Compare value
 */
void tmxc_timer_set_compare(uint64_t compare);

/**
 * @brief Enable timer interrupt
 * 
 * Enables the timer interrupt.
 */
void tmxc_timer_enable_interrupt(void);

/**
 * @brief Disable timer interrupt
 * 
 * Disables the timer interrupt.
 */
void tmxc_timer_disable_interrupt(void);

/**
 * @brief Timer interrupt handler
 * 
 * Called when a timer interrupt occurs.
 */
void tmxc_timer_handler(void);

#endif /* TMXC_TIMER_H */
