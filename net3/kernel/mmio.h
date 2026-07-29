/*
 * TMXC_OS - Memory-Mapped I/O Framework (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file mmio.h
 * @brief Memory-Mapped I/O framework for safe hardware register access
 * 
 * This framework provides volatile-safe access to hardware registers.
 * It ensures proper memory ordering and prevents compiler optimizations
 * that could break hardware interactions.
 * 
 * ARMv8-A Architecture Reference Manual:
 * - Memory-mapped registers are accessed via load/store instructions
 * - Volatile semantics are required to prevent reordering
 * - Memory barriers ensure ordering of MMIO operations
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#ifndef TMXC_MMIO_H
#define TMXC_MMIO_H

#include <stdint.h>

/*
 * ============================================================================
 * MMIO Access Macros
 * ============================================================================
 */

/**
 * @brief Read 8-bit value from MMIO register
 * 
 * @param addr Physical address of the register
 * @return uint8_t Value read from the register
 * 
 * @note Uses volatile to prevent compiler optimization and reordering
 */
#define TMXC_MMIO_READ8(addr) \
    (*((volatile uint8_t*)(addr)))

/**
 * @brief Write 8-bit value to MMIO register
 * 
 * @param addr Physical address of the register
 * @param value Value to write
 * 
 * @note Uses volatile to ensure write is not optimized away
 */
#define TMXC_MMIO_WRITE8(addr, value) \
    (*((volatile uint8_t*)(addr)) = (value))

/**
 * @brief Read 16-bit value from MMIO register
 * 
 * @param addr Physical address of the register
 * @return uint16_t Value read from the register
 */
#define TMXC_MMIO_READ16(addr) \
    (*((volatile uint16_t*)(addr)))

/**
 * @brief Write 16-bit value to MMIO register
 * 
 * @param addr Physical address of the register
 * @param value Value to write
 */
#define TMXC_MMIO_WRITE16(addr, value) \
    (*((volatile uint16_t*)(addr)) = (value))

/**
 * @brief Read 32-bit value from MMIO register
 * 
 * @param addr Physical address of the register
 * @return uint32_t Value read from the register
 */
#define TMXC_MMIO_READ32(addr) \
    (*((volatile uint32_t*)(addr)))

/**
 * @brief Write 32-bit value to MMIO register
 * 
 * @param addr Physical address of the register
 * @param value Value to write
 */
#define TMXC_MMIO_WRITE32(addr, value) \
    (*((volatile uint32_t*)(addr)) = (value))

/**
 * @brief Read 64-bit value from MMIO register
 * 
 * @param addr Physical address of the register
 * @return uint64_t Value read from the register
 */
#define TMXC_MMIO_READ64(addr) \
    (*((volatile uint64_t*)(addr)))

/**
 * @brief Write 64-bit value to MMIO register
 * 
 * @param addr Physical address of the register
 * @param value Value to write
 */
#define TMXC_MMIO_WRITE64(addr, value) \
    (*((volatile uint64_t*)(addr)) = (value))

/*
 * ============================================================================
 * MMIO Bit Manipulation Macros
 * ============================================================================
 */

/**
 * @brief Set bits in MMIO register
 * 
 * Reads the register, ORs with the mask, and writes back.
 * 
 * @param addr Physical address of the register
 * @param mask Bit mask of bits to set
 */
#define TMXC_MMIO_SET_BITS(addr, mask) \
    TMXC_MMIO_WRITE32(addr, TMXC_MMIO_READ32(addr) | (mask))

/**
 * @brief Clear bits in MMIO register
 * 
 * Reads the register, ANDs with inverted mask, and writes back.
 * 
 * @param addr Physical address of the register
 * @param mask Bit mask of bits to clear
 */
#define TMXC_MMIO_CLEAR_BITS(addr, mask) \
    TMXC_MMIO_WRITE32(addr, TMXC_MMIO_READ32(addr) & ~(mask))

/**
 * @brief Modify bits in MMIO register
 * 
 * Clears bits in clear_mask, then sets bits in set_mask.
 * 
 * @param addr Physical address of the register
 * @param clear_mask Bit mask of bits to clear
 * @param set_mask Bit mask of bits to set
 */
#define TMXC_MMIO_MODIFY_BITS(addr, clear_mask, set_mask) \
    TMXC_MMIO_WRITE32(addr, (TMXC_MMIO_READ32(addr) & ~(clear_mask)) | (set_mask))

/**
 * @brief Wait for bit to be set in MMIO register
 * 
 * Polls the register until the specified bit is set.
 * 
 * @param addr Physical address of the register
 * @param mask Bit mask of bit(s) to wait for
 * 
 * @note This is a busy-wait loop. Use with caution.
 */
#define TMXC_MMIO_WAIT_FOR_BIT_SET(addr, mask) \
    do { \
        while (!(TMXC_MMIO_READ32(addr) & (mask))) { \
            __asm__ volatile("nop"); \
        } \
    } while (0)

/**
 * @brief Wait for bit to be cleared in MMIO register
 * 
 * Polls the register until the specified bit is cleared.
 * 
 * @param addr Physical address of the register
 * @param mask Bit mask of bit(s) to wait for
 * 
 * @note This is a busy-wait loop. Use with caution.
 */
#define TMXC_MMIO_WAIT_FOR_BIT_CLEAR(addr, mask) \
    do { \
        while (TMXC_MMIO_READ32(addr) & (mask)) { \
            __asm__ volatile("nop"); \
        } \
    } while (0)

/*
 * ============================================================================
 * Memory Barrier Macros
 * ============================================================================
 */

/**
 * @brief Data Synchronization Barrier
 * 
 * Ensures all memory accesses before this point complete before
 * any memory accesses after this point.
 * 
 * ARMv8-A: DSB SY is the strongest barrier, affecting all observers.
 */
#define TMXC_DSB_SY() \
    __asm__ volatile("dsb sy" ::: "memory")

/**
 * @brief Data Synchronization Barrier (Light)
 * 
 * Lighter barrier that only affects the current observer.
 */
#define TMXC_DSB_LD() \
    __asm__ volatile("dsb ld" ::: "memory")

/**
 * @brief Data Synchronization Barrier (Store)
 * 
 * Barrier that ensures all stores complete before subsequent operations.
 */
#define TMXC_DSB_ST() \
    __asm__ volatile("dsb st" ::: "memory")

/**
 * @brief Instruction Synchronization Barrier
 * 
 * Ensures all instructions before this point complete before
 * any instructions after this point are fetched or executed.
 * 
 * ARMv8-A: ISB flushes the pipeline and ensures context changes take effect.
 */
#define TMXC_ISB() \
    __asm__ volatile("isb" ::: "memory")

/**
 * @brief Data Memory Barrier
 * 
 * Ensures ordering of data memory accesses.
 * Lighter than DSB, suitable for most MMIO operations.
 */
#define TMXC_DMB_SY() \
    __asm__ volatile("dmb sy" ::: "memory")

/*
 * ============================================================================
 * MMIO Initialization
 * ============================================================================
 */

/**
 * @brief Initialize MMIO framework
 * 
 * Sets up the memory-mapped I/O subsystem.
 * Currently a no-op but provides a hook for future initialization
 * (e.g., setting up device memory regions in MMU).
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_mmio_init(void);

/*
 * ============================================================================
 * MMIO Address Translation
 * ============================================================================
 */

/**
 * @brief Convert physical address to virtual address
 * 
 * Before MMU is enabled, physical = virtual.
 * After MMU is enabled, this performs the appropriate translation.
 * 
 * @param phys Physical address
 * @return uint64_t Virtual address
 */
uint64_t tmxc_phys_to_virt(uint64_t phys);

/**
 * @brief Convert virtual address to physical address
 * 
 * @param virt Virtual address
 * @return uint64_t Physical address
 */
uint64_t tmxc_virt_to_phys(uint64_t virt);

#endif /* TMXC_MMIO_H */
