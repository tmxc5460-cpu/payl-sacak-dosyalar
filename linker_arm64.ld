/*
 * TMXC_OS - General Mobile İşletim Sistemi
 * Copyright (c) 2024 TMXC_OS Development Team
 * Tüm hakları saklıdır.
 * 
 * Bu dosya TMXC_OS projesinin bir parçasıdır ve lisans altında korunmaktadır.
 * İzinsiz kopyalanması, dağıtılması veya değiştirilmesi yasaktır.
 * 
 * Lisans Bilgileri:
 * - Lisans Türü: PROPRIETARY
 * - Sahip: General Mobile / TMXC_OS Team
 * - Kullanım Koşulları: Sadece lisans sahibi tarafından kullanılabilir
 * 
 * İletişim: license@tmxc-os.com
 * Web: www.tmxc-os.com
 */

/* TMXC OS Linker Script for ARM64 */
/* This script defines the memory layout for the TMXC OS kernel */

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
        *(.rodata)
        *(.rodata.*)
    }
    
    /* Data section */
    .data : {
        *(.data)
        *(.data.*)
    }
    
    /* BSS section (uninitialized data) */
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    
    /* Align to page boundary */
    . = ALIGN(4096);
    
    /* End of kernel */
    _kernel_end = .;
    
    /* Stack space */
    . = ALIGN(16384);
    _stack_bottom = .;
    . = . + 16384 * TMXC_MAX_CPUS;
    _stack_top = .;
}
