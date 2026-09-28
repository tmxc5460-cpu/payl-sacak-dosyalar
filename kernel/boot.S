/*
 * TMXC OS - ARM64 Bootloader
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Boot sequence for ARM64 (AArch64) architecture.
 * Handles transition from EL2/EL3 to EL1 and prepares for kernel execution.
 * 
 * ARMv8-A Architecture Reference Manual:
 * - Exception Levels: EL3 (Secure Monitor), EL2 (Hypervisor), EL1 (Kernel), EL0 (User)
 * - This bootloader assumes boot at EL2 or EL3 and drops to EL1
 * 
 * Memory Layout:
 * - Kernel base: 0x80000000 (defined in linker script)
 * - Stack top: 0x80000000 - 0x10000 (64KB stack)
 */

.section .text.boot
.global _start
.global tmxc_boot_entry

/*
 * Boot entry point
 * This is the first code executed after bootloader passes control
 */
_start:
    b tmxc_boot_entry

tmxc_boot_entry:
    /*
     * Step 1: Check current exception level
     * CurrentEL register: [3:2] bits indicate EL
     * We expect EL2 (0b10) or EL3 (0b11) from bootloader
     */
    mrs x0, CurrentEL
    and x0, x0, #0xC          /* Mask to get EL bits [3:2] */
    cmp x0, #0xC              /* Check if EL3 (0b11 << 2 = 0xC) */
    b.eq boot_el3
    cmp x0, #0x8              /* Check if EL2 (0b10 << 2 = 0x8) */
    b.eq boot_el2
    cmp x0, #0x4              /* Check if EL1 (0b01 << 2 = 0x4) */
    b.eq boot_el1
    
    /*
     * Unknown exception level - halt
     */
boot_error:
    wfe
    b boot_error

boot_el3:
    /*
     * Step 2: Configure EL3 system registers
     * Set SCR_EL3 to enable non-secure state
     */
    mrs x0, SCR_EL3
    orr x0, x0, #(1 << 0)      /* NS bit - Non-secure state */
    orr x0, x0, #(1 << 10)     /* RW bit - Lower levels are AArch64 */
    msr SCR_EL3, x0
    
    /*
     * Configure CPTR_EL3 to enable FP/SIMD access at lower ELs
     */
    mrs x0, CPTR_EL3
    bic x0, x0, #(3 << 0)      /* TCPAC - Enable FP/SIMD at EL2 */
    bic x0, x0, #(1 << 10)     /* TTA - Enable System register access at EL2 */
    msr CPTR_EL3, x0
    
    /*
     * Step 3: Drop to EL2
     */
    mrs x0, SPSR_EL3
    orr x0, x0, #(1 << 9)      /* Disable interrupts */
    orr x0, x0, #(1 << 7)      /* Disable FIQ */
    orr x0, x0, #(1 << 6)      /* Disable IRQ */
    orr x0, x0, #(1 << 8)      /* Disable SError */
    msr SPSR_EL3, x0
    
    /*
     * Set ELR_EL3 to boot_el2 entry
     */
    adr x0, boot_el2
    msr ELR_EL3, x0
    
    /*
     * Execute ERET to drop to EL2
     */
    eret

boot_el2:
    /*
     * Step 4: Configure EL2 system registers
     * Set HCR_EL2 to disable virtualization
     */
    mrs x0, HCR_EL2
    bic x0, x0, #(1 << 31)     /* VM bit - Disable stage 2 translation */
    msr HCR_EL2, x0
    
    /*
     * Configure SCTLR_EL2
     */
    mrs x0, SCTLR_EL2
    bic x0, x0, #(1 << 0)      /* M bit - Disable MMU */
    bic x0, x0, #(1 << 2)      /* C bit - Disable data cache */
    bic x0, x0, #(1 << 12)     /* I bit - Disable instruction cache */
    msr SCTLR_EL2, x0
    
    /*
     * Step 5: Drop to EL1
     */
    mrs x0, SPSR_EL2
    orr x0, x0, #(1 << 9)      /* Disable interrupts */
    orr x0, x0, #(1 << 7)      /* Disable FIQ */
    orr x0, x0, #(1 << 6)      /* Disable IRQ */
    orr x0, x0, #(1 << 8)      /* Disable SError */
    orr x0, x0, #(0 << 3)      /* EL1h mode - Kernel mode */
    orr x0, x0, #(0 << 0)      /* AArch64 execution state */
    msr SPSR_EL2, x0
    
    /*
     * Set ELR_EL2 to boot_el1 entry
     */
    adr x0, boot_el1
    msr ELR_EL2, x0
    
    /*
     * Execute ERET to drop to EL1
     */
    eret

boot_el1:
    /*
     * Step 6: Configure EL1 system registers
     * Configure SCTLR_EL1
     */
    mrs x0, SCTLR_EL1
    bic x0, x0, #(1 << 0)      /* M bit - Disable MMU */
    bic x0, x0, #(1 << 2)      /* C bit - Disable data cache */
    bic x0, x0, #(1 << 12)     /* I bit - Disable instruction cache */
    bic x0, x0, #(1 << 3)      /* A bit - Disable alignment check */
    bic x0, x0, #(1 << 11)     /* EOS bit - Disable exception endianness */
    msr SCTLR_EL1, x0
    
    /*
     * Step 7: Set up stack pointer
     * Stack top is defined in linker script
     */
    ldr x0, =_stack_top
    mov sp, x0
    
    /*
     * Step 8: Clear BSS segment
     * BSS start and end are defined in linker script
     */
    ldr x0, =_bss_start
    ldr x1, =_bss_end
    
clear_bss:
    cmp x0, x1
    b.ge bss_done
    str xzr, [x0], #8
    b clear_bss
    
bss_done:
    /*
     * Step 9: Jump to kernel main
     * Pass boot parameters if needed (currently none)
     */
    mov x0, #0                 /* No arguments */
    mov x1, #0                 /* No argument pointer */
    bl tmxc_kernel_main
    
    /*
     * If tmxc_kernel_main returns, halt the system
     * This should never happen in normal operation
     */
kernel_halt:
    wfe
    b kernel_halt

/*
 * Exception Vector Table placeholder
 * This will be filled in by the exception handler module
 * The vector table must be aligned to 2KB (2048 bytes)
 */
.section .text.exception_vectors
.global tmxc_exception_vectors
.align 11                      /* 2KB alignment required by ARMv8-A */
tmxc_exception_vectors:
    /* Current EL with SP0 */
    .space 0x80                /* Synchronous */
    .space 0x80                /* IRQ */
    .space 0x80                /* FIQ */
    .space 0x80                /* SError */
    
    /* Current EL with SPx */
    .space 0x80                /* Synchronous */
    .space 0x80                /* IRQ */
    .space 0x80                /* FIQ */
    .space 0x80                /* SError */
    
    /* Lower EL using AArch64 */
    .space 0x80                /* Synchronous from EL0 */
    .space 0x80                /* IRQ from EL0 */
    .space 0x80                /* FIQ from EL0 */
    .space 0x80                /* SError from EL0 */
    
    /* Lower EL using AArch32 */
    .space 0x80                /* Synchronous from EL0 */
    .space 0x80                /* IRQ from EL0 */
    .space 0x80                /* FIQ from EL0 */
    .space 0x80                /* SError from EL0 */

/*
 * Stack definitions (must match linker script)
 * These are symbols that the linker will resolve
 */
.global _stack_top
.global _bss_start
.global _bss_end
