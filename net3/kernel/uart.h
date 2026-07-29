/*
 * TMXC_OS - UART Driver (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file uart.h
 * @brief UART driver for console output
 * 
 * This driver provides serial console communication via UART.
 * It supports the PL011 UART (used in Raspberry Pi and QEMU virt).
 * 
 * ARMv8-A: UART is accessed via memory-mapped I/O registers.
 * The driver uses the MMIO framework for safe register access.
 * 
 * Zero-Dependency: This code does not use any standard C library.
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
 * @brief UART base addresses for different platforms
 * 
 * QEMU virt machine: 0x09000000
 * Raspberry Pi 3/4: 0x3F201000 (BCM2837/BCM2711)
 * Raspberry Pi 5: 0x1F215000 (BCM2712)
 */
#define TMXC_UART_BASE_QEMU      0x09000000
#define TMXC_UART_BASE_RPI3      0x3F201000
#define TMXC_UART_BASE_RPI4      0xFE215000
#define TMXC_UART_BASE_RPI5      0x1F215000

/**
 * @brief Default UART base address
 * 
 * Configurable for different platforms.
 * Default is QEMU virt machine.
 */
#ifndef TMXC_UART_BASE
#define TMXC_UART_BASE          TMXC_UART_BASE_QEMU
#endif

/**
 * @brief UART clock frequency
 * 
 * QEMU virt: 24000000 Hz (24 MHz)
 * Raspberry Pi: 48000000 Hz (48 MHz)
 */
#ifndef TMXC_UART_CLOCK
#define TMXC_UART_CLOCK         24000000
#endif

/**
 * @brief UART baud rate
 * 
 * Standard baud rate for console: 115200 baud
 */
#ifndef TMXC_UART_BAUD
#define TMXC_UART_BAUD          115200
#endif

/*
 * ============================================================================
 * PL011 UART Register Offsets
 * ============================================================================
 */

/**
 * @brief UART Data Register
 * 
 * Write: Transmit data register
 * Read: Receive data register
 */
#define UART_DR_OFFSET          0x00

/**
 * @brief UART Receive Status Register / Error Clear Register
 */
#define UART_RSR_OFFSET         0x04

/**
 * @brief UART Flag Register
 * 
 * Bit 0: TXFF - Transmit FIFO full
 * Bit 1: RXFF - Receive FIFO full
 * Bit 4: TXFE - Transmit FIFO empty
 * Bit 5: RXFE - Receive FIFO empty
 * Bit 6: BUSY - UART busy
 */
#define UART_FR_OFFSET          0x18

/**
 * @brief UART Integer Baud Rate Divisor
 */
#define UART_IBRD_OFFSET        0x24

/**
 * @brief UART Fractional Baud Rate Divisor
 */
#define UART_FBRD_OFFSET        0x28

/**
 * @brief UART Line Control Register
 * 
 * Bit 0: WLEN - Word length (0b11 = 8 bits)
 * Bit 1: STP2 - Two stop bits
 * Bit 2: EPS - Even parity select
 * Bit 3: PEN - Parity enable
 * Bit 4: BRK - Send break
 * Bit 5: SPS - Stick parity select
 * Bit 6: FEN - FIFO enable
 */
#define UART_LCRH_OFFSET        0x2C

/**
 * @brief UART Control Register
 * 
 * Bit 0: UARTEN - UART enable
 * Bit 8: TXE - Transmit enable
 * Bit 9: RXE - Receive enable
 * Bit 14: RTSEN - RTS enable
 * Bit 15: CTSEN - CTS enable
 */
#define UART_CR_OFFSET          0x30

/**
 * @brief UART Interrupt FIFO Level Select Register
 */
#define UART_IFLS_OFFSET        0x34

/**
 * @brief UART Interrupt Mask Set/Clear Register
 */
#define UART_IMSC_OFFSET        0x38

/**
 * @brief UART Raw Interrupt Status Register
 */
#define UART_RIS_OFFSET         0x3C

/**
 * @brief UART Masked Interrupt Status Register
 */
#define UART_MIS_OFFSET         0x40

/**
 * @brief UART Interrupt Clear Register
 */
#define UART_ICR_OFFSET         0x44

/*
 * ============================================================================
 * UART Flag Register Bits
 * ============================================================================
 */

/**
 * @brief Transmit FIFO full flag
 */
#define UART_FR_TXFF            (1 << 0)

/**
 * @brief Receive FIFO full flag
 */
#define UART_FR_RXFF            (1 << 1)

/**
 * @brief Transmit FIFO empty flag
 */
#define UART_FR_TXFE            (1 << 4)

/**
 * @brief Receive FIFO empty flag
 */
#define UART_FR_RXFE            (1 << 5)

/**
 * @brief UART busy flag
 */
#define UART_FR_BUSY            (1 << 6)

/*
 * ============================================================================
 * UART Line Control Register Bits
 * ============================================================================
 */

/**
 * @brief Word length: 8 bits
 */
#define UART_LCRH_WLEN_8BIT     (0b11 << 0)

/**
 * @brief FIFO enable
 */
#define UART_LCRH_FEN           (1 << 6)

/*
 * ============================================================================
 * UART Control Register Bits
 * ============================================================================
 */

/**
 * @brief UART enable
 */
#define UART_CR_UARTEN          (1 << 0)

/**
 * @brief Transmit enable
 */
#define UART_CR_TXE             (1 << 8)

/**
 * @brief Receive enable
 */
#define UART_CR_RXE             (1 << 9)

/*
 * ============================================================================
 * UART Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize UART driver
 * 
 * Configures the UART for 8N1 (8 data bits, no parity, 1 stop bit)
 * at the specified baud rate. Enables FIFOs.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_uart_init(void);

/**
 * @brief Output a single character to UART
 * 
 * Blocks until the transmit FIFO has space.
 * 
 * @param c Character to output
 */
void tmxc_uart_putc(char c);

/**
 * @brief Output a null-terminated string to UART
 * 
 * @param str String to output
 */
void tmxc_uart_puts(const char* str);

/**
 * @brief Read a single character from UART
 * 
 * Blocks until a character is available.
 * 
 * @return char Character read from UART
 */
char tmxc_uart_getc(void);

/**
 * @brief Check if UART has data available
 * 
 * @return int 1 if data available, 0 otherwise
 */
int tmxc_uart_has_data(void);

/**
 * @brief Flush UART transmit FIFO
 * 
 * Blocks until all data in transmit FIFO is sent.
 */
void tmxc_uart_flush(void);

#endif /* TMXC_UART_H */
