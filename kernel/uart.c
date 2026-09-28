/*
 * TMXC OS - UART Driver Implementation
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * PL011 UART driver implementation for ARM64 architecture.
 * Provides serial I/O for debug output.
 */

#include "uart.h"

/*
 * ============================================================================
 * MMIO Access Macros
 * ============================================================================
 */

/**
 * @brief Read a 32-bit register from UART
 */
#define UART_READ(offset) \
    (*((volatile uint32_t*)(TMXC_UART_BASE + (offset))))

/**
 * @brief Write a 32-bit register to UART
 */
#define UART_WRITE(offset, value) \
    ((*((volatile uint32_t*)(TMXC_UART_BASE + (offset)))) = (value))

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

/*
 * ============================================================================
 * UART Implementation
 * ============================================================================
 */

/**
 * @brief Initialize UART
 * 
 * Initializes the PL011 UART with the configured baud rate.
 * Sets up 8N1 (8 data bits, no parity, 1 stop bit) with FIFO enabled.
 */
int tmxc_uart_init(void) {
    uint32_t divisor;
    uint32_t remainder;
    uint32_t fractional;
    
    /*
     * Step 1: Disable UART
     * Clear UARTEN bit to disable the UART during configuration
     */
    UART_WRITE(UART_CR_OFFSET, 0);
    
    /*
     * Step 2: Clear all interrupts
     * Write to ICR to clear any pending interrupts
     */
    UART_WRITE(UART_ICR_OFFSET, 0x7FF);
    
    /*
     * Step 3: Calculate baud rate divisor
     * Divisor = UART_CLOCK / (16 * BAUD_RATE)
     */
    divisor = TMXC_UART_CLOCK / (16 * TMXC_UART_BAUD);
    remainder = TMXC_UART_CLOCK % (16 * TMXC_UART_BAUD);
    
    /*
     * Calculate fractional part
     * Fractional = (remainder * 64 + 8) / 16
     * The +8 provides rounding (0.5 * 16)
     */
    fractional = ((remainder * 64) + 8) / 16;
    
    /*
     * Clamp fractional part to 6 bits (max value 63)
     */
    if (fractional > 63) {
        fractional = 63;
    }
    
    /*
     * Step 4: Set baud rate divisors
     * Write integer divisor to IBRD
     */
    UART_WRITE(UART_IBRD_OFFSET, divisor);
    
    /*
     * Write fractional divisor to FBRD
     */
    UART_WRITE(UART_FBRD_OFFSET, fractional);
    
    /*
     * Step 5: Configure line control
     * - 8 data bits (WLEN = 0b11)
     * - No parity (PEN = 0)
     * - 1 stop bit (STP2 = 0)
     * - Enable FIFOs (FEN = 1)
     */
    UART_WRITE(UART_LCRH_OFFSET, UART_LCRH_WLEN_8BIT | UART_LCRH_FEN);
    
    /*
     * Step 6: Enable UART
     * - Enable UART (UARTEN = 1)
     * - Enable transmitter (TXE = 1)
     * - Enable receiver (RXE = 1)
     */
    UART_WRITE(UART_CR_OFFSET, UART_CR_UARTEN | UART_CR_TXE | UART_CR_RXE);
    
    return 0;
}

/**
 * @brief Write a character to UART
 * 
 * Writes a single character to the UART transmit FIFO.
 * Blocks until the FIFO has space.
 * Handles newline conversion (\n -> \r\n).
 */
void tmxc_uart_putc(char c) {
    /*
     * Wait for transmit FIFO to have space
     * Poll TXFF flag until it is 0
     */
    while (UART_READ(UART_FR_OFFSET) & UART_FR_TXFF) {
        __asm__ volatile("nop");
    }
    
    /*
     * Write character to data register
     */
    UART_WRITE(UART_DR_OFFSET, (uint32_t)c);
    
    /*
     * Special handling for newline
     * Convert \n to \r\n for proper terminal display
     */
    if (c == '\n') {
        while (UART_READ(UART_FR_OFFSET) & UART_FR_TXFF) {
            __asm__ volatile("nop");
        }
        UART_WRITE(UART_DR_OFFSET, (uint32_t)'\r');
    }
}

/**
 * @brief Write a string to UART
 * 
 * Writes a null-terminated string to the UART.
 */
void tmxc_uart_puts(const char* str) {
    while (*str != '\0') {
        tmxc_uart_putc(*str++);
    }
}

/**
 * @brief Read a character from UART
 * 
 * Reads a single character from the UART receive FIFO.
 * Blocks until data is available.
 */
char tmxc_uart_getc(void) {
    /*
     * Wait for receive FIFO to have data
     * Poll RXFE flag until it is 0
     */
    while (UART_READ(UART_FR_OFFSET) & UART_FR_RXFE) {
        __asm__ volatile("nop");
    }
    
    /*
     * Read character from data register
     * Return as 8-bit value
     */
    return (char)(UART_READ(UART_DR_OFFSET) & 0xFF);
}

/**
 * @brief Check if data is available
 * 
 * Checks if there is data available in the receive FIFO.
 */
int tmxc_uart_has_data(void) {
    /*
     * Check RXFE flag
     * If 0, data is available
     */
    return !(UART_READ(UART_FR_OFFSET) & UART_FR_RXFE);
}

/**
 * @brief Flush UART
 * 
 * Flushes the UART FIFOs.
 */
void tmxc_uart_flush(void) {
    /*
     * Read from data register until FIFO is empty
     */
    while (!(UART_READ(UART_FR_OFFSET) & UART_FR_RXFE)) {
        (void)UART_READ(UART_DR_OFFSET);
    }
}
