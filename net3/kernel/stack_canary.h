/*
 * TMXC_OS - Stack Canary Protection (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file stack_canary.h
 * @brief Kernel-space stack canaries for overflow protection
 * 
 * This module implements stack canaries to detect and prevent buffer overflows.
 * Stack canaries are random values placed at stack boundaries that are validated
 * on function return to detect stack corruption.
 * 
 * ARMv8-A Architecture Reference Manual:
 * - Stack canaries are placed before the saved frame pointer
 * - Validated on function return
 * - Use the SP_ELx register for kernel stack pointer
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#ifndef TMXC_STACK_CANARY_H
#define TMXC_STACK_CANARY_H

#include <stdint.h>

/*
 * ============================================================================
 * Stack Canary Configuration
 * ============================================================================
 */

/**
 * @brief Stack canary value
 * 
 * A random value used to detect stack corruption.
 * This should be different for each boot.
 */
extern uint64_t tmxc_stack_canary;

/**
 * @brief Stack canary magic pattern
 * 
 * A known pattern used to identify canary corruption.
 * This is combined with a random value.
 */
#define TMXC_CANARY_MAGIC        0xCAFEBABEDEADBEEF

/*
 * ============================================================================
 * Stack Canary Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize stack canary system
 * 
 * Generates a random stack canary value.
 * The canary should be different on each boot for security.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_stack_canary_init(void);

/**
 * @brief Get the current stack canary value
 * 
 * @return uint64_t Current stack canary value
 */
uint64_t tmxc_stack_canary_get(void);

/**
 * @brief Set the stack canary value
 * 
 * @param canary New canary value
 */
void tmxc_stack_canary_set(uint64_t canary);

/**
 * @brief Validate stack canary
 * 
 * Checks if the stack canary has been corrupted.
 * Called on function return to detect buffer overflows.
 * 
 * @param canary Pointer to canary value on stack
 * @return int 1 if valid, 0 if corrupted
 */
int tmxc_stack_canary_validate(uint64_t* canary);

/**
 * @brief Handle stack canary failure
 * 
 * Called when stack canary corruption is detected.
 * This indicates a buffer overflow has occurred.
 * 
 * @param canary Pointer to corrupted canary value
 */
void tmxc_stack_canary_fail(uint64_t* canary) __attribute__((noreturn));

/*
 * ============================================================================
 * Compiler Attributes for Stack Canaries
 * ============================================================================
 */

/**
 * @brief Function prologue to save stack canary
 * 
 * This macro should be inserted at the beginning of functions
 * that need stack canary protection.
 */
#define TMXC_STACK_CANARY_SAVE() \
    __asm__ volatile("sub sp, sp, #16"); \
    __asm__ volatile("str x0, [sp, #8]"); \
    __asm__ volatile("ldr x0, =tmxc_stack_canary"); \
    __asm__ volatile("ldr x0, [x0]"); \
    __asm__ volatile("str x0, [sp]"); \
    __asm__ volatile("ldr x0, [sp, #8]")

/**
 * @brief Function epilogue to validate stack canary
 * 
 * This macro should be inserted before function return
 * to validate the stack canary.
 */
#define TMXC_STACK_CANARY_CHECK() \
    __asm__ volatile("sub sp, sp, #16"); \
    __asm__ volatile("str x0, [sp, #8]"); \
    __asm__ volatile("ldr x0, [sp, #16]"); \
    __asm__ volatile("ldr x1, =tmxc_stack_canary"); \
    __asm__ volatile("ldr x1, [x1]"); \
    __asm__ volatile("cmp x0, x1"); \
    __asm__ volatile("b.ne stack_canary_fail"); \
    __asm__ volatile("ldr x0, [sp, #8]"); \
    __asm__ volatile("add sp, sp, #16")

/*
 * ============================================================================
 * Stack Canary Utility Functions
 * ============================================================================
 */

/**
 * @brief Generate a random canary value
 * 
 * Uses system timer and CPU ID to generate a pseudo-random value.
 * 
 * @return uint64_t Random canary value
 */
uint64_t tmxc_stack_canary_generate(void);

/**
 * @brief Check if stack canary is enabled
 * 
 * @return int 1 if enabled, 0 if disabled
 */
int tmxc_stack_canary_enabled(void);

/**
 * @brief Enable stack canary protection
 */
void tmxc_stack_canary_enable(void);

/**
 * @brief Disable stack canary protection
 * 
 * WARNING: Disabling stack canaries reduces security.
 * Only use for debugging purposes.
 */
void tmxc_stack_canary_disable(void);

#endif /* TMXC_STACK_CANARY_H */
