/*
 * TMXC OS - UART Driver Header
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * PL011 UART driver for ARM64 architecture.
 * Provides serial I/O for debug output.
 */

#ifndef TMXC_UART_H
#define TMXC_UART_H

#include <stdint.h>

/*
 * ============================================================================
 * UART Configuration
 * ============================================================================
 */

/**
 * @brief UART base address
 * 
 * This is the base address for the PL011 UART.
 * For QEMU virt machine, UART0 is at 0x09000000.
 * For Raspberry Pi 3, UART0 is at 0x3F201000.
 */
#define TMXC_UART_BASE 0x09000000

/**
 * @brief UART clock frequency in Hz
 * 
 * For QEMU virt machine, UART clock is 24MHz.
 * For Raspberry Pi 3, UART clock is 48MHz.
 */
#define TMXC_UART_CLOCK 24000000

/**
 * @brief UART baud rate
 */
#define TMXC_UART_BAUD 115200

/*
 * ============================================================================
 * UART Register Offsets
 * ============================================================================
 */

/**
 * @brief UART Data Register
 */
#define UART_DR_OFFSET 0x000

/**
 * @brief UART Receive Status Register / Error Clear Register
 */
#define UART_RSR_OFFSET 0x004

/**
 * @brief UART Flag Register
 */
#define UART_FR_OFFSET 0x018

/**
 * @brief UART Integer Baud Rate Register
 */
#define UART_IBRD_OFFSET 0x024

/**
 * @brief UART Fractional Baud Rate Register
 */
#define UART_FBRD_OFFSET 0x028

/**
 * @brief UART Line Control Register
 */
#define UART_LCRH_OFFSET 0x02C

/**
 * @brief UART Control Register
 */
#define UART_CR_OFFSET 0x030

/**
 * @brief UART Interrupt FIFO Level Select Register
 */
#define UART_IFLS_OFFSET 0x034

/**
 * @brief UART Interrupt Mask Set/Clear Register
 */
#define UART_IMSC_OFFSET 0x038

/**
 * @brief UART Raw Interrupt Status Register
 */
#define UART_RIS_OFFSET 0x03C

/**
 * @brief UART Masked Interrupt Status Register
 */
#define UART_MIS_OFFSET 0x040

/**
 * @brief UART Interrupt Clear Register
 */
#define UART_ICR_OFFSET 0x044

/*
 * ============================================================================
 * UART Flag Register Bits
 * ============================================================================
 */

/**
 * @brief Transmit FIFO Full flag
 */
#define UART_FR_TXFF (1 << 5)

/**
 * @brief Transmit FIFO Empty flag
 */
#define UART_FR_TXFE (1 << 7)

/**
 * @brief Receive FIFO Full flag
 */
#define UART_FR_RXFF (1 << 6)

/**
 * @brief Receive FIFO Empty flag
 */
#define UART_FR_RXFE (1 << 4)

/*
 * ============================================================================
 * UART Line Control Register Bits
 * ============================================================================
 */

/**
 * @brief Word length (8 bits)
 */
#define UART_LCRH_WLEN_8BIT (0b11 << 5)

/**
 * @brief Enable FIFOs
 */
#define UART_LCRH_FEN (1 << 4)

/**
 * @brief No parity
 */
#define UART_LCRH_PEN (0 << 1)

/**
 * @brief 1 stop bit
 */
#define UART_LCRH_STP2 (0 << 3)

/*
 * ============================================================================
 * UART Control Register Bits
 * ============================================================================
 */

/**
 * @brief Enable UART
 */
#define UART_CR_UARTEN (1 << 0)

/**
 * @brief Enable transmitter
 */
#define UART_CR_TXE (1 << 8)

/**
 * @brief Enable receiver
 */
#define UART_CR_RXE (1 << 9)

/*
 * ============================================================================
 * UART Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize UART
 * 
 * Initializes the PL011 UART with the configured baud rate.
 * Sets up 8N1 (8 data bits, no parity, 1 stop bit) with FIFO enabled.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_uart_init(void);

/**
 * @brief Write a character to UART
 * 
 * Writes a single character to the UART transmit FIFO.
 * Blocks until the FIFO has space.
 * Handles newline conversion (\n -> \r\n).
 * 
 * @param c Character to write
 */
void tmxc_uart_putc(char c);

/**
 * @brief Write a string to UART
 * 
 * Writes a null-terminated string to the UART.
 * 
 * @param str String to write
 */
void tmxc_uart_puts(const char* str);

/**
 * @brief Read a character from UART
 * 
 * Reads a single character from the UART receive FIFO.
 * Blocks until data is available.
 * 
 * @return char Character read
 */
char tmxc_uart_getc(void);

/**
 * @brief Check if data is available
 * 
 * Checks if there is data available in the receive FIFO.
 * 
 * @return int 1 if data available, 0 if not
 */
int tmxc_uart_has_data(void);

/**
 * @brief Flush UART
 * 
 * Flushes the UART FIFOs.
 */
void tmxc_uart_flush(void);

#endif /* TMXC_UART_H */
