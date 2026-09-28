/*
 * TMXC OS - Linker Script for ARM64
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Linker script defining the memory layout for the TMXC OS microkernel.
 * Kernel base address: 0x80000000
 */

ENTRY(_start)

SECTIONS
{
    /* Boot code at physical address 0x80000000 */
    . = 0x80000000;
    
    /* Boot section */
    .text.boot : {
        *(.text.boot)
    }
    
    /* Kernel text section */
    .text : {
        *(.text)
        *(.text.*)
    }
    
    /* Read-only data section */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* Exception vector table (must be 2KB aligned) */
    . = ALIGN(2048);
    .exception_vectors : {
        *(.text.exception_vectors)
    }
    
    /* Data section */
    .data : {
        *(.data)
        *(.data.*)
    }
    
    /* BSS section (uninitialized data) */
    .bss : {
        _bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        _bss_end = .;
    }
    
    /* Align to page boundary */
    . = ALIGN(4096);
    
    /* End of kernel */
    _kernel_end = .;
    
    /* Stack space (64KB) */
    . = ALIGN(16384);
    _stack_bottom = .;
    . = . + 65536;
    _stack_top = .;
    
    /* Discard unused sections */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.gnu.build-id)
    }
}
