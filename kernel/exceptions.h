/*
 * TMXC OS - Exception Handler Header
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Exception handling for ARM64 architecture.
 * Provides interrupt and exception handling.
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
 * Each exception vector is 128 bytes (32 instructions)
 */
#define TMXC_EXCEPTION_VECTOR_SIZE 128

/**
 * @brief Exception vector table alignment (2KB)
 */
#define TMXC_EXCEPTION_TABLE_ALIGN 2048

/*
 * ============================================================================
 * Exception Types
 * ============================================================================
 */

typedef enum {
    TMXC_EXCEPTION_SYNC = 0,
    TMXC_EXCEPTION_IRQ = 1,
    TMXC_EXCEPTION_FIQ = 2,
    TMXC_EXCEPTION_SERROR = 3
} tmxc_exception_type_t;

/*
 * ============================================================================
 * Exception Context Structure
 * ============================================================================
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
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_exceptions_init(void);

/**
 * @brief Synchronous exception handler
 * 
 * Handles synchronous exceptions.
 * 
 * @param ctx Exception context
 */
void tmxc_handle_sync(tmxc_exception_context_t* ctx);

/**
 * @brief IRQ handler
 * 
 * Handles normal interrupt requests.
 * 
 * @param ctx Exception context
 */
void tmxc_handle_irq(tmxc_exception_context_t* ctx);

/**
 * @brief FIQ handler
 * 
 * Handles fast interrupt requests.
 * 
 * @param ctx Exception context
 */
void tmxc_handle_fiq(tmxc_exception_context_t* ctx);

/**
 * @brief SError handler
 * 
 * Handles system errors.
 * 
 * @param ctx Exception context
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

#endif /* TMXC_EXCEPTIONS_H */
