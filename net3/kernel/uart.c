/*
 * TMXC_OS - UART Driver Implementation (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file uart.c
 * @brief UART driver implementation for console output
 * 
 * This driver implements the PL011 UART for serial console communication.
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#include "uart.h"
#include "mmio.h"

/*
 * ============================================================================
 * UART Register Access Macros
 * ============================================================================
 */

/**
 * @brief Read UART register
 */
#define UART_READ(offset) \
    TMXC_MMIO_READ32(TMXC_UART_BASE + (offset))

/**
 * @brief Write UART register
 */
#define UART_WRITE(offset, value) \
    TMXC_MMIO_WRITE32(TMXC_UART_BASE + (offset), (value))

/*
 * ============================================================================
 * UART Initialization
 * ============================================================================
 */

/**
 * @brief Initialize UART driver
 * 
 * Configures the PL011 UART for 8N1 communication at the specified baud rate.
 * 
 * Steps:
 * 1. Disable UART
 * 2. Clear all interrupts
 * 3. Set baud rate divisor
 * 4. Configure line control (8N1, FIFO enable)
 * 5. Enable UART, TX, and RX
 * 
 * Baud rate calculation:
 * - Baud rate divisor = UART_CLOCK / (16 * BAUD_RATE)
 * - Integer part goes in IBRD
 * - Fractional part goes in FBRD
 * - Fractional part = (remainder * 64) + 0.5
 * 
 * @return 0 on success, negative error code on failure
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

/*
 * ============================================================================
 * UART Output Functions
 * ============================================================================
 */

/**
 * @brief Output a single character to UART
 * 
 * Blocks until the transmit FIFO has space.
 * 
 * ARMv8-A: We poll the TXFF flag in the FR register.
 * When TXFF is 0, the FIFO has space for more data.
 * 
 * @param c Character to output
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
 * @brief Output a null-terminated string to UART
 * 
 * @param str String to output
 */
void tmxc_uart_puts(const char* str) {
    /*
     * Iterate through string until null terminator
     */
    while (*str != '\0') {
        tmxc_uart_putc(*str);
        str++;
    }
}

/*
 * ============================================================================
 * UART Input Functions
 * ============================================================================
 */

/**
 * @brief Read a single character from UART
 * 
 * Blocks until a character is available.
 * 
 * ARMv8-A: We poll the RXFE flag in the FR register.
 * When RXFE is 0, the FIFO has data available.
 * 
 * @return char Character read from UART
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
 * @brief Check if UART has data available
 * 
 * @return int 1 if data available, 0 otherwise
 */
int tmxc_uart_has_data(void) {
    /*
     * Check RXFE flag
     * If RXFE is 0, data is available
     */
    return !(UART_READ(UART_FR_OFFSET) & UART_FR_RXFE);
}

/*
 * ============================================================================
 * UART Utility Functions
 * ============================================================================
 */

/**
 * @brief Flush UART transmit FIFO
 * 
 * Blocks until all data in transmit FIFO is sent.
 * 
 * ARMv8-A: We poll the BUSY flag in the FR register.
 * When BUSY is 0, the UART is idle.
 */
void tmxc_uart_flush(void) {
    /*
     * Wait for UART to complete transmission
     * Poll BUSY flag until it is 0
     */
    while (UART_READ(UART_FR_OFFSET) & UART_FR_BUSY) {
        __asm__ volatile("nop");
    }
    
    /*
     * Additional wait for FIFO to empty
     */
    while (!(UART_READ(UART_FR_OFFSET) & UART_FR_TXFE)) {
        __asm__ volatile("nop");
    }
}
