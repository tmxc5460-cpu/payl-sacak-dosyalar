/*
 * TMXC OS - Kernel Main Header
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 */

#ifndef TMXC_KERNEL_MAIN_H
#define TMXC_KERNEL_MAIN_H

#include <stdint.h>

/*
 * ============================================================================
 * Kernel Version
 * ============================================================================
 */

#define TMXC_VERSION_MAJOR 1
#define TMXC_VERSION_MINOR 0
#define TMXC_VERSION_PATCH 0

/*
 * ============================================================================
 * Kernel Panic Handler
 * ============================================================================
 */

/**
 * @brief Kernel panic handler
 * 
 * Called when a fatal error occurs.
 * Prints error message and halts the system.
 * 
 * @param message Error message
 */
void tmxc_kernel_panic(const char* message);

/**
 * @brief Assert macro implementation
 * 
 * Called when an assertion fails.
 * 
 * @param expr Failed expression
 * @param file File name
 * @param line Line number
 */
void tmxc_assert_fail(const char* expr, const char* file, uint32_t line);

/*
 * ============================================================================
 * Assert Macro
 * ============================================================================
 */

#ifdef TMXC_DEBUG
#define TMXC_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            tmxc_assert_fail(#expr, __FILE__, __LINE__); \
        } \
    } while (0)
#else
#define TMXC_ASSERT(expr) ((void)0)
#endif

/*
 * ============================================================================
 * Kernel Main Entry Point
 * ============================================================================
 */

/**
 * @brief Kernel main entry point
 * 
 * This is the C entry point called from the bootloader.
 * 
 * @param arg0 First argument (unused)
 * @param arg1 Second argument (unused)
 * @return int Never returns (jumps to userspace)
 */
int tmxc_kernel_main(uint64_t arg0, uint64_t arg1);

#endif /* TMXC_KERNEL_MAIN_H */
