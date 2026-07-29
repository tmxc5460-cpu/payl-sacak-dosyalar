/*
 * TMXC_OS - TMXC OS Boot Sequence (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * This file contains the bare-metal boot sequence for ARM64 (AArch64) architecture.
 * It handles the initial CPU state, sets up the stack, and jumps to the C kernel.
 * 
 * ARMv8-A Architecture Reference Manual: 
 * - Exception Levels: EL3 (Secure Monitor), EL2 (Hypervisor), EL1 (Kernel), EL0 (User)
 * - This boot sequence assumes we boot at EL2 and drop to EL1 for kernel operation
 * 
 * Memory Layout:
 * - Kernel base: 0x40080000 (defined in linker script)
 * - Stack top: 0x40080000 - 0x10000 (64KB stack)
 */

.section .text.boot
.global _start
.global tmxc_boot_entry

/**
 * @brief Boot entry point for TMXC OS kernel
 * 
 * This is the first code executed after bootloader passes control.
 * ARM64 boot requirements:
 * 1. CPU must be in AAarch64 state
 * 2. Exception level: EL2 (we'll drop to EL1)
 * 3. MMU: Disabled
 * 4. Caches: Disabled
 * 5. Interrupts: Masked (DAIF bits set)
 */
_start:
tmxc_boot_entry:
    /* 
     * ARMv8-A: Check current exception level
     * CurrentEL register: [3:2] bits indicate EL
     * We expect EL2 (0b10) from bootloader
     */
    mrs     x0, CurrentEL
    and     x0, x0, #0xC          /* Mask to get EL bits [3:2] */
    cmp     x0, #0x8              /* Check if EL2 (0b10 << 2 = 0x8) */
    b.eq    boot_el2              /* Branch if at EL2 */
    
    /* If not at EL2, we have a problem */
    b       boot_error

boot_el2:
    /*
     * ARMv8-A: Disable interrupts at EL2
     * DAIF register bits:
     * - D (Debug): bit 9
     * - A (SError): bit 8  
     * - I (IRQ): bit 7
     * - F (FIQ): bit 6
     * Setting these bits masks the corresponding exceptions
     */
    msr     DAIFSet, #0xF         /* Mask all interrupts (D, A, I, F) */
    
    /*
     * ARMv8-A: Invalidate instruction and data caches
     * This ensures no stale data from previous execution
     */
    /* Invalidate instruction caches to PoU (Point of Unification) */
    ic      iallu                  /* Invalidate all instruction caches */
    dsb     sy                     /* Data synchronization barrier */
    isb                             /* Instruction synchronization barrier */
    
    /* Invalidate data caches to PoC (Point of Coherency) */
    /* Note: Full cache invalidation requires iterating sets/ways */
    /* For simplicity, we assume clean cache state from bootloader */
    
    /*
     * ARMv8-A: Set up stack pointer for EL2
     * Stack grows downward, so we set SP to the top of stack region
     * Stack size: 64KB (0x10000 bytes)
     */
    ldr     x0, =__stack_top_el2   /* Load stack top address from linker */
    mov     sp, x0                 /* Set stack pointer */
    
    /*
     * ARMv8-A: Configure system control registers at EL2
     * HCR_EL2 (Hypervisor Configuration Register):
     * - RW bit: 0 = EL1 is AArch32, 1 = EL1 is AArch64
     * We want EL1 to be AArch64 for 64-bit kernel
     */
    mrs     x0, HCR_EL2
    orr     x0, x0, #(1 << 31)     /* Set RW bit (bit 31) for AArch64 EL1 */
    msr     HCR_EL2, x0
    isb                             /* Ensure configuration takes effect */
    
    /*
     * ARMv8-A: Configure timer at EL2
     * CNTHCTL_EL2 (Counter-timer Hypervisor Control Register):
     * - EL1PCTEN: bit 0 = Allow EL1 to access physical timer
     * - EL1PCEN: bit 1 = Allow EL1 to access physical counter
     */
    mrs     x0, CNTHCTL_EL2
    orr     x0, x0, #3             /* Enable EL1 access to physical timer/counter */
    msr     CNTHCTL_EL2, x0
    isb
    
    /*
     * ARMv8-A: Drop from EL2 to EL1
     * We execute an ERET (Exception Return) to drop privilege level
     * Before ERET, we must set up ELR_EL1 (Exception Link Register)
     * to point to our kernel entry point
     */
    adr     x0, kernel_entry_el1   /* Load address of EL1 entry point */
    msr     ELR_EL2, x0           /* Set ELR_EL2 to jump to kernel_entry_el1 */
    
    /*
     * ARMv8-A: Configure SPSR_EL2 (Saved Program Status Register)
     * This determines the processor state after ERET
     * - M[3:0]: Target exception level (0b0101 = EL1h, AArch64)
     * - D: Debug mask bit
     * - A: SError mask bit
     * - I: IRQ mask bit
     * - F: FIQ mask bit
     */
    mov     x0, #(0b0101 << 0)     /* EL1h, AArch64 */
    orr     x0, x0, #(1 << 6)      /* Mask FIQ */
    orr     x0, x0, #(1 << 7)      /* Mask IRQ */
    orr     x0, x0, #(1 << 8)      /* Mask SError */
    orr     x0, x0, #(1 << 9)      /* Mask Debug */
    msr     SPSR_EL2, x0
    
    /*
     * ARMv8-A: Execute exception return to drop to EL1
     * ERET restores ELR_EL1 to PC and SPSR_EL1 to PSTATE
     * This effectively transitions us from EL2 to EL1
     */
    eret

boot_error:
    /* 
     * Boot error handler - infinite loop
     * In production, this would trigger a watchdog reset
     */
    wfe                             /* Wait for event */
    b       boot_error

/**
 * @brief Kernel entry point at EL1
 * 
 * At this point:
 * - We are at EL1 (kernel mode)
 * - AArch64 execution state
 * - Interrupts masked
 * - MMU disabled
 * - Caches disabled
 * - Stack pointer set
 */
kernel_entry_el1:
    /*
     * ARMv8-A: Disable interrupts at EL1
     */
    msr     DAIFSet, #0xF
    
    /*
     * ARMv8-A: Set up stack pointer for EL1
     * Stack size: 64KB (0x10000 bytes)
     */
    ldr     x0, =__stack_top_el1
    mov     sp, x0
    
    /*
     * ARMv8-A: Configure system control registers at EL1
     * SCTLR_EL1 (System Control Register):
     * - M bit: Enable MMU (we'll enable this later in C)
     * - C bit: Enable data cache (we'll enable this later)
     * - I bit: Enable instruction cache (we'll enable this later)
     * - A bit: Enable alignment checking
     * - SA bit: Enable stack alignment checking
     */
    mrs     x0, SCTLR_EL1
    bic     x0, x0, #(1 << 0)      /* Clear M bit (MMU disabled for now) */
    bic     x0, x0, #(1 << 2)      /* Clear C bit (data cache disabled) */
    bic     x0, x0, #(1 << 12)     /* Clear I bit (instruction cache disabled) */
    orr     x0, x0, #(1 << 1)      /* Set A bit (alignment check enabled) */
    orr     x0, x0, #(1 << 3)      /* Set SA bit (stack alignment check) */
    msr     SCTLR_EL1, x0
    isb
    
    /*
     * ARMv8-A: Clear BSS section (OPTIMIZED)
     * BSS contains uninitialized global variables
     * We zero them out before use
     * Optimized: Use 64-bit stores with loop unrolling
     */
    ldr     x0, =__bss_start
    ldr     x1, =__bss_end
clear_bss:
    cmp     x0, x1
    b.ge    bss_done
    /* Store 4 x 64-bit zeros per iteration (loop unrolling) */
    stp     xzr, xzr, [x0], #16
    stp     xzr, xzr, [x0], #16
    cmp     x0, x1
    b.lt    clear_bss
bss_done:
    
    /*
     * ARMv8-A: Perform integrity check before kernel initialization
     * This verifies the kernel binary and copyright string
     * If integrity check fails, system will halt
     */
    bl      tmxc_integrity_verify_all    /* Verify kernel integrity */
    
    /*
     * ARMv8-A: Jump to C kernel main
     * Pass boot parameters if needed (currently none)
     */
    mov     x0, #0                 /* No arguments */
    mov     x1, #0                 /* No argument pointer */
    bl      tmxc_kernel_main       /* Branch to C kernel entry point */
    
    /*
     * If tmxc_kernel_main returns, halt the system
     * This should never happen in normal operation
     */
kernel_halt:
    wfe                             /* Wait for event */
    b       kernel_halt

/*
 * Stack definitions (must match linker script)
 * These are symbols that the linker will resolve
 */
.global __stack_top_el2
.global __stack_top_el1
.global __bss_start
.global __bss_end

/*
 * Exception Vector Table placeholder
 * This will be filled in by the exception handler module
 */
.section .text.exception_vectors
.global tmxc_exception_vectors
.align 11                          /* 2KB alignment required by ARMv8-A */
tmxc_exception_vectors:
    /* Current EL with SP0 */
    .space 0x80                    /* Synchronous */
    .space 0x80                    /* IRQ */
    .space 0x80                    /* FIQ */
    .space 0x80                    /* SError */
    
    /* Current EL with SPx */
    .space 0x80                    /* Synchronous */
    .space 0x80                    /* IRQ */
    .space 0x80                    /* FIQ */
    .space 0x80                    /* SError */
    
    /* Lower EL using AArch64 */
    .space 0x80                    /* Synchronous from EL0 */
    .space 0x80                    /* IRQ from EL0 */
    .space 0x80                    /* FIQ from EL0 */
    .space 0x80                    /* SError from EL0 */
    
    /* Lower EL using AArch32 */
    .space 0x80                    /* Synchronous from EL0 */
    .space 0x80                    /* IRQ from EL0 */
    .space 0x80                    /* FIQ from EL0 */
    .space 0x80                    /* SError from EL0 */
