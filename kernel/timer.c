/*
 * TMXC OS - Timer Driver Implementation
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Generic Timer driver implementation for ARM64 architecture.
 */

#include "timer.h"
#include "uart.h"

/*
 * ============================================================================
 * Timer Implementation
 * ============================================================================
 */

/**
 * @brief Initialize timer
 * 
 * Initializes the generic timer.
 */
int tmxc_timer_init(void) {
    /*
     * Step 1: Disable timer interrupt
     */
    tmxc_timer_disable_interrupt();
    
    /*
     * Step 2: Set initial compare value
     * Set compare to current time + tick interval
     */
    uint64_t current = tmxc_timer_get_value();
    uint64_t freq = tmxc_timer_get_freq();
    uint64_t tick_interval = (freq * TMXC_TICK_INTERVAL_US) / 1000000;
    tmxc_timer_set_compare(current + tick_interval);
    
    tmxc_uart_puts("[TIMER] Timer initialized\r\n");
    
    return 0;
}

/**
 * @brief Get current timer value
 * 
 * Returns the current value of the system counter.
 */
uint64_t tmxc_timer_get_value(void) {
    uint64_t value;
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(value));
    return value;
}

/**
 * @brief Get timer frequency
 * 
 * Returns the frequency of the system counter.
 */
uint64_t tmxc_timer_get_freq(void) {
    uint64_t freq;
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;
}

/**
 * @brief Set timer compare value
 * 
 * Sets the compare value for the timer.
 */
void tmxc_timer_set_compare(uint64_t compare) {
    __asm__ volatile("msr cntv_cval_el0, %0" : : "r"(compare));
}

/**
 * @brief Enable timer interrupt
 * 
 * Enables the timer interrupt.
 */
void tmxc_timer_enable_interrupt(void) {
    /*
     * Enable timer interrupt by setting the enable bit
     */
    __asm__ volatile("msr cntv_ctl_el0, %0" : : "r"(1));
}

/**
 * @brief Disable timer interrupt
 * 
 * Disables the timer interrupt.
 */
void tmxc_timer_disable_interrupt(void) {
    /*
     * Disable timer interrupt by clearing the enable bit
     */
    __asm__ volatile("msr cntv_ctl_el0, %0" : : "r"(0));
}

/**
 * @brief Timer interrupt handler
 * 
 * Called when a timer interrupt occurs.
 */
void tmxc_timer_handler(void) {
    /*
     * TODO: Call scheduler
     */
    
    /*
     * Set next compare value
     */
    uint64_t current = tmxc_timer_get_value();
    uint64_t freq = tmxc_timer_get_freq();
    uint64_t tick_interval = (freq * TMXC_TICK_INTERVAL_US) / 1000000;
    tmxc_timer_set_compare(current + tick_interval);
}
