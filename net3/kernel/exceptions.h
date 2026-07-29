/*
 * TMXC_OS - Exception Vector Table (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file exceptions.h
 * @brief Exception Vector Table and exception handlers for EL1/EL2
 * 
 * This module configures the exception vector table and provides handlers for:
 * - Synchronous exceptions (e.g., data abort, instruction abort)
 * - IRQ (interrupt requests)
 * - FIQ (fast interrupt requests)
 * - SError (system errors)
 * 
 * ARMv8-A Architecture Reference Manual:
 * - Exception Vector Table must be aligned to 2KB (2048 bytes)
 * - 16 exception vectors total (4 exception types × 4 exception sources)
 * - VBAR_EL1 holds the base address of the exception vector table
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#ifndef TMXC_EXCEPTIONS_H
#define TMXC_EXCEPTIONS_H

#include <stdint.h>

/*
 * ============================================================================
 * Exception Vector Table Layout
 * ============================================================================
 */

/**
 * @brief Exception vector table offset
 * 
 * Each exception vector is 128 bytes (32 instructions).
 * The table must be aligned to 2KB.
 */
#define TMXC_EXCEPTION_VECTOR_SIZE  128
#define TMXC_EXCEPTION_TABLE_ALIGN  2048

/**
 * @brief Exception types
 * 
 * The exception vector table has 4 exception types:
 * - Synchronous: Instruction faults, data aborts, etc.
 * - IRQ: Normal interrupt requests
 * - FIQ: Fast interrupt requests
 * - SError: System errors (external aborts)
 */
typedef enum {
    TMXC_EXCEPTION_SYNC = 0,
    TMXC_EXCEPTION_IRQ = 1,
    TMXC_EXCEPTION_FIQ = 2,
    TMXC_EXCEPTION_SERROR = 3
} tmxc_exception_type_t;

/**
 * @brief Exception sources
 * 
 * Exceptions can originate from:
 * - Current EL with SP0 (using SP_EL0)
 * - Current EL with SPx (using SP_ELx)
 * - Lower EL using AArch64
 * - Lower EL using AArch32
 */
typedef enum {
    TMXC_SOURCE_CURRENT_SP0 = 0,
    TMXC_SOURCE_CURRENT_SPX = 1,
    TMXC_SOURCE_LOWER_AARCH64 = 2,
    TMXC_SOURCE_LOWER_AARCH32 = 3
} tmxc_exception_source_t;

/*
 * ============================================================================
 * Exception Syndrome Register (ESR_EL1) Fields
 * ============================================================================
 */

/**
 * @brief Exception Class (EC) field
 * 
 * Bits [31:26] of ESR_EL1 indicate the exception class.
 * Common values:
 * - 0b000000: Unknown reason
 * - 0b000001: Trapped WFI/WFE
 * - 0b000011: Trapped MRS/MSR
 * - 0b000101: Trapped system register access
 * - 0b001000: Instruction abort from lower level
 * - 0b001001: Instruction abort from same level
 * - 0b001100: Data abort from lower level
 * - 0b001101: Data abort from same level
 * - 0b010000: Trapped FP/NEON
 * - 0b010001: Trapped FP/NEON from AArch32
 * - 0b011000: Trapped execution of AArch32
 * - 0b011100: Trapped branch to AArch32
 * - 0b100000: Trapped system instruction from AArch32
 * - 0b100101: Illegal execution state
 * - 0b110000: Breakpoint from lower level
 * - 0b110001: Breakpoint from same level
 * - 0b111000: Software step from lower level
 * - 0b111001: Software step from same level
 * - 0b111100: Watchpoint from lower level
 * - 0b111101: Watchpoint from same level
 */
#define ESR_EC_SHIFT            26
#define ESR_EC_MASK             (0x3F << ESR_EC_SHIFT)

/**
 * @brief Instruction Set Specific (ISS) field
 * 
 * Bits [24:0] of ESR_EL1 contain instruction-set specific information.
 * The meaning depends on the exception class.
 */
#define ESR_ISS_SHIFT           0
#define ESR_ISS_MASK            (0x1FFFFFF << ESR_ISS_SHIFT)

/**
 * @brief Instruction Abort specific fields
 * 
 * For instruction aborts (EC = 0b001000 or 0b001001):
 * - IFSC: Bits [5:0] - Instruction fault status code
 * - PTW: Bit 6 - Page table walk fault
 * - FSC: Bits [5:0] - Fault status code
 */
#define ESR_IFSC_SHIFT          0
#define ESR_IFSC_MASK           (0x3F << ESR_IFSC_SHIFT)
#define ESR_PTW                 (1 << 6)
#define ESR_FSC_SHIFT           0
#define ESR_FSC_MASK            (0x3F << ESR_FSC_SHIFT)

/**
 * @brief Data Abort specific fields
 * 
 * For data aborts (EC = 0b001100 or 0b001101):
 * - ISV: Bit 24 - Instruction syndrome valid
 * - SAS: Bits [23:22] - Access size
 * - SSE: Bit 21 - Sign extend
 * - SRT: Bits [20:16] - Source register
 * - SF: Bit 15 - Sixty-four bit
 * - AR: Bit 14 - Acquire/release
 * - VNCR: Bit 13 - Virtual not cacheable
 * - SET: Bit 12 - SVE context
 * - FnV: Bit 11 - First not done
 * - EA: Bit 9 - External abort
 * - CM: Bit 8 - Cache maintenance
 * - S1PTW: Bit 7 - Stage 1 page table walk
 * - WnR: Bit 6 - Write not read
 * - FSC: Bits [5:0] - Fault status code
 */
#define ESR_ISV                 (1 << 24)
#define ESR_SAS_SHIFT           22
#define ESR_SAS_MASK            (0x3 << ESR_SAS_SHIFT)
#define ESR_SSE                 (1 << 21)
#define ESR_SRT_SHIFT           16
#define ESR_SRT_MASK            (0x1F << ESR_SRT_SHIFT)
#define ESR_SF                  (1 << 15)
#define ESR_AR                  (1 << 14)
#define ESR_VNCR                (1 << 13)
#define ESR_SET                 (1 << 12)
#define ESR_FnV                 (1 << 11)
#define ESR_EA                  (1 << 9)
#define ESR_CM                  (1 << 8)
#define ESR_S1PTW               (1 << 7)
#define ESR_WnR                 (1 << 6)
#define ESR_DFSC_SHIFT          0
#define ESR_DFSC_MASK           (0x3F << ESR_DFSC_SHIFT)

/*
 * ============================================================================
 * Exception Context Structure
 * ============================================================================
 */

/**
 * @brief Saved processor state during exception
 * 
 * This structure holds the processor state when an exception occurs.
 * It is used by exception handlers to restore state after handling.
 */
typedef struct {
    uint64_t x0;      /* General purpose registers */
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
    uint64_t sp;      /* Stack pointer */
    uint64_t pc;      /* Program counter (ELR_EL1) */
    uint64_t cpsr;    /* Current program status (SPSR_EL1) */
    uint64_t esr;     /* Exception syndrome (ESR_EL1) */
    uint64_t far;     /* Fault address (FAR_EL1) */
} tmxc_exception_context_t;

/*
 * ============================================================================
 * Exception Handler Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize exception vector table
 * 
 * Configures the exception vector table for EL1.
 * Sets VBAR_EL1 to point to the exception vector table.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_exceptions_init(void);

/**
 * @brief Synchronous exception handler
 * 
 * Handles synchronous exceptions such as:
 * - Instruction aborts
 * - Data aborts
 * - Trapped system instructions
 * - Illegal execution state
 * 
 * @param ctx Exception context (saved processor state)
 */
void tmxc_handle_sync(tmxc_exception_context_t* ctx);

/**
 * @brief IRQ handler
 * 
 * Handles normal interrupt requests.
 * Dispatches to the appropriate device driver.
 * 
 * @param ctx Exception context (saved processor state)
 */
void tmxc_handle_irq(tmxc_exception_context_t* ctx);

/**
 * @brief FIQ handler
 * 
 * Handles fast interrupt requests.
 * Used for high-priority interrupts (e.g., watchdog).
 * 
 * @param ctx Exception context (saved processor state)
 */
void tmxc_handle_fiq(tmxc_exception_context_t* ctx);

/**
 * @brief SError handler
 * 
 * Handles system errors such as:
 * - External aborts
 * - Parity errors
 * - Asynchronous data aborts
 * 
 * @param ctx Exception context (saved processor state)
 */
void tmxc_handle_serror(tmxc_exception_context_t* ctx);

/**
 * @brief Enable IRQs
 * 
 * Unmasks IRQ exceptions at the current exception level.
 */
void tmxc_enable_irqs(void);

/**
 * @brief Disable IRQs
 * 
 * Masks IRQ exceptions at the current exception level.
 */
void tmxc_disable_irqs(void);

/**
 * @brief Enable FIQs
 * 
 * Unmasks FIQ exceptions at the current exception level.
 */
void tmxc_enable_fiqs(void);

/**
 * @brief Disable FIQs
 * 
 * Masks FIQ exceptions at the current exception level.
 */
void tmxc_disable_fiqs(void);

/**
 * @brief Print exception information
 * 
 * Outputs exception details to UART for debugging.
 * 
 * @param ctx Exception context (saved processor state)
 * @param type Exception type
 */
void tmxc_print_exception(tmxc_exception_context_t* ctx, tmxc_exception_type_t type);

/*
 * ============================================================================
 * System Call Interface
 * ============================================================================
 */

/**
 * @brief System call numbers
 */
typedef enum {
    TMXC_SYSCALL_READ_SENSOR = 0x100,
    TMXC_SYSCALL_QUERY_STATUS = 0x101,
    TMXC_SYSCALL_EXEC_APP = 0x102,
    TMXC_SYSCALL_EXIT = 0x103,
    TMXC_SYSCALL_YIELD = 0x104
} tmxc_syscall_number_t;

/**
 * @brief System call handler
 * 
 * Handles system calls from userspace.
 * Dispatches to the appropriate system call implementation.
 * 
 * @param ctx Exception context (saved processor state)
 */
void tmxc_handle_syscall(tmxc_exception_context_t* ctx);

/**
 * @brief Read sensor system call
 * 
 * System call to read sensor data from userspace.
 * 
 * @param sensor_type Sensor type
 * @param data_buffer Pointer to data buffer in userspace
 * @param buffer_size Size of data buffer
 * @return int Number of bytes read, or negative error code
 */
int tmxc_syscall_read_sensor(uint64_t sensor_type, uint64_t data_buffer, uint64_t buffer_size);

/**
 * @brief Query status system call
 * 
 * System call to query kernel/system status from userspace.
 * 
 * @param query_type Type of query
 * @param result_buffer Pointer to result buffer in userspace
 * @param buffer_size Size of result buffer
 * @return int Status code, or negative error code
 */
int tmxc_syscall_query_status(uint64_t query_type, uint64_t result_buffer, uint64_t buffer_size);

/**
 * @brief Execute application system call
 * 
 * System call to execute an application in isolated memory space.
 * 
 * @param app_path Pointer to application path in userspace
 * @param argv Pointer to argument vector in userspace
 * @param envp Pointer to environment vector in userspace
 * @return int Process ID, or negative error code
 */
int tmxc_syscall_exec_app(uint64_t app_path, uint64_t argv, uint64_t envp);

#endif /* TMXC_EXCEPTIONS_H */
