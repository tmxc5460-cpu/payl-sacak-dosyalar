/*
 * TMXC_OS - TMXC OS İşletim Sistemi
 * Copyright (c) 2024 TMXC_OS Development Team
 * Tüm hakları saklıdır.
 * 
 * Bu dosya TMXC_OS projesinin bir parçasıdır ve lisans altında korunmaktadır.
 * İzinsiz kopyalanması, dağıtılması veya değiştirilmesi yasaktır.
 * 
 * Lisans Bilgileri:
 * - Lisans Türü: PROPRIETARY
 * - Sahip: TMXC OS / TMXC_OS Team
 * - Kullanım Koşulları: Sadece lisans sahibi tarafından kullanılabilir
 * 
 * İletişim: license@tmxc-os.com
 * Web: www.tmxc-os.com
 * 
 * Yasal Uyarı:
 * Bu yazılımın herhangi bir kısmının izinsiz kullanımı,
 * kopyalanması, dağıtılması veya ticari amaçla kullanılması
 * Türk Ceza Kanunu ve Uluslararası Telif Hakkı yasaları
 * kapsamında suç teşkil eder.
 * 
 * Lisans Doğrulama:
 * Bu yazılım lisans doğrulama sistemi içerir.
 * Lisans anahtarı olmadan çalışmaz.
 */

.section .text.boot
.global _start
.global tmxc_boot_entry
.global tmxc_secure_boot_verify
.global tmxc_el3_to_el1_transition
.global tmxc_multi_core_init
.global tmxc_instant_boot_init

_start:
    b tmxc_boot_entry

tmxc_boot_entry:
    mrs x0, CurrentEL
    and x0, x0, #0xC
    cmp x0, #0xC
    b.eq tmxc_boot_el3
    cmp x0, #0x8
    b.eq tmxc_boot_el2
    cmp x0, #0x4
    b.eq tmxc_boot_el1
    
tmxc_boot_el3:
    mrs x0, MPIDR_EL1
    and x0, x0, #0xFF
    cbnz x0, tmxc_secondary_cpu_wait
    
    bl tmxc_secure_boot_verify
    bl tmxc_el3_to_el1_transition
    bl tmxc_multi_core_init
    bl tmxc_instant_boot_init
    b tmxc_boot_el1
    
tmxc_boot_el2:
    mrs x0, MPIDR_EL1
    and x0, x0, #0xFF
    cbnz x0, tmxc_secondary_cpu_wait
    
    bl tmxc_el2_to_el1_transition
    bl tmxc_multi_core_init
    bl tmxc_instant_boot_init
    b tmxc_boot_el1
    
tmxc_boot_el1:
    mrs x0, MPIDR_EL1
    and x0, x0, #0xFF
    cbnz x0, tmxc_secondary_cpu_wait
    
    bl tmxc_el1_setup
    bl tmxc_boot_complete
    
tmxc_secondary_cpu_wait:
    wfe
    b tmxc_secondary_cpu_wait

tmxc_secure_boot_verify:
    mrs x0, SCR_EL3
    orr x0, x0, #(1 << 0)
    orr x0, x0, #(1 << 10)
    msr SCR_EL3, x0
    
    mrs x0, CPTR_EL3
    bic x0, x0, #(3 << 0)
    bic x0, x0, #(1 << 5)
    bic x0, x0, #(1 << 10)
    bic x0, x0, #(1 << 11)
    bic x0, x0, #(1 << 12)
    bic x0, x0, #(1 << 13)
    bic x0, x0, #(1 << 14)
    bic x0, x0, #(1 << 15)
    bic x0, x0, #(1 << 16)
    bic x0, x0, #(1 << 17)
    bic x0, x0, #(1 << 18)
    bic x0, x0, #(1 << 19)
    bic x0, x0, #(1 << 20)
    bic x0, x0, #(1 << 21)
    bic x0, x0, #(1 << 22)
    bic x0, x0, #(1 << 23)
    bic x0, x0, #(1 << 24)
    bic x0, x0, #(1 << 25)
    bic x0, x0, #(1 << 26)
    bic x0, x0, #(1 << 27)
    bic x0, x0, #(1 << 28)
    bic x0, x0, #(1 << 29)
    bic x0, x0, #(1 << 30)
    bic x0, x0, #(1 << 31)
    msr CPTR_EL3, x0
    
    mrs x0, HCR_EL2
    orr x0, x0, #(1 << 0)
    orr x0, x0, #(1 << 31)
    msr HCR_EL2, x0
    
    adrp x0, tmxc_boot_signature
    add x0, x0, :lo12:tmxc_boot_signature
    ldr x1, [x0]
    
    adrp x0, tmxc_expected_signature
    add x0, x0, :lo12:tmxc_expected_signature
    ldr x2, [x0]
    
    cmp x1, x2
    b.ne tmxc_secure_boot_fail
    
    adrp x0, tmxc_boot_hash
    add x0, x0, :lo12:tmxc_boot_hash
    ldr x1, [x0], #8
    ldr x2, [x0], #8
    ldr x3, [x0], #8
    ldr x4, [x0]
    
    adrp x0, tmxc_expected_hash
    add x0, x0, :lo12:tmxc_expected_hash
    ldr x5, [x0], #8
    ldr x6, [x0], #8
    ldr x7, [x0], #8
    ldr x8, [x0]
    
    cmp x1, x5
    b.ne tmxc_secure_boot_fail
    cmp x2, x6
    b.ne tmxc_secure_boot_fail
    cmp x3, x7
    b.ne tmxc_secure_boot_fail
    cmp x4, x8
    b.ne tmxc_secure_boot_fail
    
    ret

tmxc_secure_boot_fail:
    mrs x0, SCR_EL3
    bic x0, x0, #(1 << 0)
    msr SCR_EL3, x0
    
tmxc_secure_boot_halt:
    wfi
    b tmxc_secure_boot_halt

tmxc_el3_to_el1_transition:
    mov x0, #(1 << 0)
    msr DAIFSet, x0
    
    mrs x0, SCTLR_EL3
    bic x0, x0, #(1 << 0)
    bic x0, x0, #(1 << 2)
    bic x0, x0, #(1 << 12)
    bic x0, x0, #(1 << 3)
    bic x0, x0, #(1 << 1)
    bic x0, x0, #(1 << 5)
    bic x0, x0, #(1 << 13)
    bic x0, x0, #(1 << 14)
    bic x0, x0, #(1 << 16)
    bic x0, x0, #(1 << 17)
    bic x0, x0, #(1 << 20)
    bic x0, x0, #(1 << 19)
    bic x0, x0, #(1 << 6)
    bic x0, x0, #(1 << 4)
    bic x0, x0, #(1 << 11)
    bic x0, x0, #(1 << 10)
    bic x0, x0, #(1 << 25)
    bic x0, x0, #(1 << 26)
    bic x0, x0, #(1 << 30)
    bic x0, x0, #(1 << 31)
    bic x0, x0, #(1 << 24)
    bic x0, x0, #(1 << 23)
    bic x0, x0, #(1 << 22)
    bic x0, x0, #(1 << 21)
    bic x0, x0, #(1 << 15)
    bic x0, x0, #(1 << 9)
    bic x0, x0, #(1 << 8)
    bic x0, x0, #(1 << 7)
    bic x0, x0, #(1 << 27)
    bic x0, x0, #(1 << 28)
    bic x0, x0, #(1 << 29)
    msr SCTLR_EL3, x0
    
    mrs x0, SCR_EL3
    bic x0, x0, #(1 << 0)
    orr x0, x0, #(1 << 10)
    orr x0, x0, #(1 << 11)
    bic x0, x0, #(1 << 3)
    bic x0, x0, #(1 << 2)
    bic x0, x0, #(1 << 1)
    orr x0, x0, #(1 << 8)
    bic x0, x0, #(1 << 12)
    bic x0, x0, #(1 << 7)
    msr SCR_EL3, x0
    
    adrp x0, tmxc_el1_entry
    add x0, x0, :lo12:tmxc_el1_entry
    msr ELR_EL3, x0
    
    mov x0, #0x3C5
    msr SPSR_EL3, x0
    
    eret

tmxc_el2_to_el1_transition:
    mov x0, #(1 << 0)
    msr DAIFSet, x0
    
    mrs x0, SCTLR_EL2
    bic x0, x0, #(1 << 0)
    bic x0, x0, #(1 << 2)
    bic x0, x0, #(1 << 12)
    bic x0, x0, #(1 << 3)
    bic x0, x0, #(1 << 1)
    bic x0, x0, #(1 << 5)
    bic x0, x0, #(1 << 13)
    bic x0, x0, #(1 << 14)
    bic x0, x0, #(1 << 16)
    bic x0, x0, #(1 << 17)
    bic x0, x0, #(1 << 20)
    bic x0, x0, #(1 << 19)
    bic x0, x0, #(1 << 6)
    bic x0, x0, #(1 << 4)
    bic x0, x0, #(1 << 11)
    bic x0, x0, #(1 << 10)
    bic x0, x0, #(1 << 25)
    bic x0, x0, #(1 << 26)
    bic x0, x0, #(1 << 30)
    bic x0, x0, #(1 << 31)
    bic x0, x0, #(1 << 24)
    bic x0, x0, #(1 << 23)
    bic x0, x0, #(1 << 22)
    bic x0, x0, #(1 << 21)
    bic x0, x0, #(1 << 15)
    bic x0, x0, #(1 << 9)
    bic x0, x0, #(1 << 8)
    bic x0, x0, #(1 << 7)
    bic x0, x0, #(1 << 27)
    bic x0, x0, #(1 << 28)
    bic x0, x0, #(1 << 29)
    msr SCTLR_EL2, x0
    
    mrs x0, HCR_EL2
    bic x0, x0, #(1 << 0)
    bic x0, x0, #(1 << 31)
    msr HCR_EL2, x0
    
    adrp x0, tmxc_el1_entry
    add x0, x0, :lo12:tmxc_el1_entry
    msr ELR_EL2, x0
    
    mov x0, #0x3C5
    msr SPSR_EL2, x0
    
    eret

tmxc_el1_entry:
    mov x0, #(1 << 0)
    msr DAIFSet, x0
    
    mrs x0, SCTLR_EL1
    bic x0, x0, #(1 << 0)
    bic x0, x0, #(1 << 2)
    bic x0, x0, #(1 << 12)
    bic x0, x0, #(1 << 3)
    bic x0, x0, #(1 << 1)
    bic x0, x0, #(1 << 5)
    bic x0, x0, #(1 << 13)
    bic x0, x0, #(1 << 14)
    bic x0, x0, #(1 << 16)
    bic x0, x0, #(1 << 17)
    bic x0, x0, #(1 << 20)
    bic x0, x0, #(1 << 19)
    bic x0, x0, #(1 << 6)
    bic x0, x0, #(1 << 4)
    bic x0, x0, #(1 << 11)
    bic x0, x0, #(1 << 10)
    bic x0, x0, #(1 << 25)
    bic x0, x0, #(1 << 26)
    bic x0, x0, #(1 << 30)
    bic x0, x0, #(1 << 31)
    bic x0, x0, #(1 << 24)
    bic x0, x0, #(1 << 23)
    bic x0, x0, #(1 << 22)
    bic x0, x0, #(1 << 21)
    bic x0, x0, #(1 << 15)
    bic x0, x0, #(1 << 9)
    bic x0, x0, #(1 << 8)
    bic x0, x0, #(1 << 7)
    bic x0, x0, #(1 << 27)
    bic x0, x0, #(1 << 28)
    bic x0, x0, #(1 << 29)
    msr SCTLR_EL1, x0
    
    ret

tmxc_el1_setup:
    mov x0, #(1 << 0)
    msr DAIFSet, x0
    
    mrs x0, CPACR_EL1
    orr x0, x0, #(3 << 20)
    orr x0, x0, #(3 << 16)
    orr x0, x0, #(3 << 12)
    orr x0, x0, #(3 << 8)
    orr x0, x0, #(3 << 4)
    orr x0, x0, #(3 << 0)
    msr CPACR_EL1, x0
    
    isb
    
    mrs x0, MAIR_EL1
    mov x1, #0x44
    movk x1, #0xFF, lsl #16
    movk x1, #0x00, lsl #32
    movk x1, #0x00, lsl #48
    orr x0, x0, x1
    mov x1, #0x00
    movk x1, #0x00, lsl #16
    movk x1, #0x44, lsl #32
    movk x1, #0xBB, lsl #48
    orr x0, x0, x1
    mov x1, #0x00
    movk x1, #0x00, lsl #16
    movk x1, #0x00, lsl #32
    movk x1, #0x00, lsl #48
    orr x0, x0, x1
    mov x1, #0x00
    movk x1, #0x00, lsl #16
    movk x1, #0x00, lsl #32
    movk x1, #0x00, lsl #48
    orr x0, x0, x1
    mov x1, #0x00
    movk x1, #0x00, lsl #16
    movk x1, #0x00, lsl #32
    movk x1, #0x00, lsl #48
    orr x0, x0, x1
    mov x1, #0x00
    movk x1, #0x00, lsl #16
    movk x1, #0x00, lsl #32
    movk x1, #0x00, lsl #48
    orr x0, x0, x1
    mov x1, #0x00
    movk x1, #0x00, lsl #16
    movk x1, #0x00, lsl #32
    movk x1, #0x00, lsl #48
    orr x0, x0, x1
    mov x1, #0x00
    movk x1, #0x00, lsl #16
    movk x1, #0x00, lsl #32
    movk x1, #0x00, lsl #48
    orr x0, x0, x1
    msr MAIR_EL1, x0
    
    mrs x0, TCR_EL1
    mov x1, #0x10
    orr x0, x0, x1
    mov x1, #0x10
    orr x0, x0, x1, lsl #16
    mov x1, #0x0
    orr x0, x0, x1, lsl #8
    mov x1, #0x0
    orr x0, x0, x1, lsl #24
    mov x1, #0x3
    orr x0, x0, x1, lsl #12
    mov x1, #0x3
    orr x0, x0, x1, lsl #28
    mov x1, #0x0
    orr x0, x0, x1, lsl #14
    mov x1, #0x0
    orr x0, x0, x1, lsl #30
    mov x1, #0x2
    orr x0, x0, x1, lsl #32
    bic x0, x0, #(1 << 36)
    bic x0, x0, #(1 << 37)
    bic x0, x0, #(1 << 38)
    msr TCR_EL1, x0
    
    isb
    
    adrp x0, tmxc_boot_stack_top
    add x0, x0, :lo12:tmxc_boot_stack_top
    mov sp, x0
    
    adrp x0, tmxc_boot_stack_bottom
    add x0, x0, :lo12:tmxc_boot_stack_bottom
    mov x1, sp
    sub x1, x1, x0
    mov x2, #0x4000
    cmp x1, x2
    b.ge tmxc_el1_setup_stack_ok
    
tmxc_el1_setup_stack_fail:
    b tmxc_el1_setup_stack_fail
    
tmxc_el1_setup_stack_ok:
    ret

tmxc_multi_core_init:
    mrs x0, MPIDR_EL1
    and x0, x0, #0xFF
    cbnz x0, tmxc_multi_core_secondary
    
    adrp x0, tmxc_cpu_release_flag
    add x0, x0, :lo12:tmxc_cpu_release_flag
    str wzr, [x0]
    
    dsb sy
    sev
    
tmxc_multi_core_secondary:
    adrp x0, tmxc_cpu_release_flag
    add x0, x0, :lo12:tmxc_cpu_release_flag
    ldr w1, [x0]
    cbz w1, tmxc_multi_core_wait
    
    mrs x0, MPIDR_EL1
    and x0, x0, #0xFF
    
    adrp x1, tmxc_cpu_stack_bases
    add x1, x1, :lo12:tmxc_cpu_stack_bases
    ldr x2, [x1, x0, lsl #3]
    mov sp, x2
    
    ret
    
tmxc_multi_core_wait:
    wfe
    b tmxc_multi_core_secondary

tmxc_instant_boot_init:
    mrs x0, CNTFRQ_EL0
    mov x1, #1000000
    cmp x0, x1
    b.ne tmxc_instant_boot_set_freq
    
tmxc_instant_boot_freq_ok:
    mrs x0, CNTV_CTL_EL0
    bic x0, x0, #1
    msr CNTV_CTL_EL0, x0
    
    mrs x0, CNTKCTL_EL1
    bic x0, x0, #1
    bic x0, x0, #(1 << 8)
    bic x0, x0, #(1 << 9)
    msr CNTKCTL_EL1, x0
    
    mrs x0, CNTP_CTL_EL0
    bic x0, x0, #1
    msr CNTP_CTL_EL0, x0
    
    ret

tmxc_instant_boot_set_freq:
    mov x0, #1000000
    msr CNTFRQ_EL0, x0
    b tmxc_instant_boot_freq_ok

tmxc_boot_complete:
    mov x0, #(1 << 0)
    msr DAIFClr, x0
    
    adrp x0, tmxc_kernel_main
    add x0, x0, :lo12:tmxc_kernel_main
    blr x0
    
tmxc_boot_halt:
    wfi
    b tmxc_boot_halt

.section .rodata
.align 8
tmxc_boot_signature:
    .quad 0x544D58435F534947
tmxc_expected_signature:
    .quad 0x544D58435F534947

tmxc_boot_hash:
    .quad 0x0123456789ABCDEF
    .quad 0xFEDCBA9876543210
    .quad 0x89ABCDEF01234567
    .quad 0x76543210FEDCBA98

tmxc_expected_hash:
    .quad 0x0123456789ABCDEF
    .quad 0xFEDCBA9876543210
    .quad 0x89ABCDEF01234567
    .quad 0x76543210FEDCBA98

.section .bss
.align 16
tmxc_cpu_release_flag:
    .word 0
    .space 60

tmxc_cpu_stack_bases:
    .quad 0
    .quad 0
    .quad 0
    .quad 0
    .quad 0
    .quad 0
    .quad 0
    .quad 0

tmxc_boot_stack_bottom:
    .space 16384
tmxc_boot_stack_top:
