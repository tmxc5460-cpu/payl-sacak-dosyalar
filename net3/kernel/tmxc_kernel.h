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

/*
.
#ifndef TMXC_KERNEL_H
#define TMXC_KERNEL_H

#pragma once

#define TMXC_VERSION_MAJOR 1
#define TMXC_VERSION_MINOR 0
#define TMXC_VERSION_PATCH 0

#define TMXC_TRUE 1
#define TMXC_FALSE 0

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;
typedef unsigned long size_t;
typedef signed long ssize_t;
typedef unsigned long uintptr_t;
typedef unsigned long long phys_addr_t;
typedef unsigned long long virt_addr_t;
typedef uint64_t paddr_t;
typedef uint64_t vaddr_t;

typedef uint8_t bool_t;

#define NULL ((void*)0)
#define TMXC_PAGE_SIZE 4096
#define TMXC_PAGE_SHIFT 12
#define TMXC_PAGE_MASK (~(TMXC_PAGE_SIZE - 1))

#define TMXC_KERNEL_BASE 0xFFFF800000000000ULL
#define TMXC_KERNEL_OFFSET 0xFFFF800000000000ULL
#define TMXC_PHYS_MEMORY_BASE 0x80000000ULL
#define TMXC_MEMORY_SIZE (2ULL * 1024 * 1024 * 1024)

#define TMXC_STACK_SIZE 16384
#define TMXC_MAX_CPUS 8
#define TMXC_MAX_PROCESSES 256
#define TMXC_MAX_THREADS 1024
#define TMXC_MAX_OPEN_FILES 128
#define TMXC_MAX_SIGNALS 64

#define TMXC_IRQ_BASE 32
#define TMXC_IRQ_COUNT 1024

#define TMXC_GICD_BASE 0x08000000ULL
#define TMXC_GICC_BASE 0x08010000ULL
#define TMXC_GICH_BASE 0x08020000ULL
#define TMXC_GICV_BASE 0x08030000ULL

#define TMXC_UART0_BASE 0x09000000ULL
#define TMXC_UART1_BASE 0x09010000ULL
#define TMXC_UART2_BASE 0x09020000ULL

#define TMXC_TIMER_BASE 0x0A000000ULL
#define TMXC_WATCHDOG_BASE 0x0A100000ULL

#define TMXC_GPIO_BASE 0x0B000000ULL
#define TMXC_I2C0_BASE 0x0C000000ULL
#define TMXC_I2C1_BASE 0x0C100000ULL
#define TMXC_SPI0_BASE 0x0D000000ULL
#define TMXC_SPI1_BASE 0x0D100000ULL

#define TMXC_SDHCI_BASE 0x0E000000ULL
#define TMXC_EMMC_BASE 0x0E100000ULL

#define TMXC_USB_BASE 0x0F000000ULL
#define TMXC_PCIE_BASE 0x10000000ULL

#define TMXC_DISPLAY_BASE 0x11000000ULL
#define TMXC_GPU_BASE 0x12000000ULL
#define TMXC_VPU_BASE 0x13000000ULL

#define TMXC_CAMERA_BASE 0x14000000ULL
#define TMXC_AUDIO_BASE 0x15000000ULL
#define TMXC_CODEC_BASE 0x15100000ULL

#define TMXC_WIFI_BASE 0x16000000ULL
#define TMXC_BLUETOOTH_BASE 0x16100000ULL
#define TMXC_MODEM_BASE 0x17000000ULL

#define TMXC_PMU_BASE 0x18000000ULL
#define TMXC_RNG_BASE 0x19000000ULL
#define TMXC_TRNG_BASE 0x19010000ULL

#define TMXC_CRYPTO_BASE 0x1A000000ULL
#define TMXC_TRUSTZONE_BASE 0x1B000000ULL

#define TMXC_SENSOR_BASE 0x1C000000ULL
#define TMXC_TOUCH_BASE 0x1C100000ULL

#define TMXC_SYSCON_BASE 0x1D000000ULL
#define TMXC_CLOCK_BASE 0x1D100000ULL
#define TMXC_RESET_BASE 0x1D200000ULL
#define TMXC_POWER_BASE 0x1D300000ULL

#define TMXC_DDR_BASE 0x1E000000ULL
#define TMXC_DMC_BASE 0x1E100000ULL

#define TMXC_FIRMWARE_BASE 0x20000000ULL
#define TMXC_SECURE_BASE 0x30000000ULL

#define TMXC_FRAMEBUFFER_BASE 0x40000000ULL
#define TMXC_FRAMEBUFFER_SIZE (1920 * 1080 * 4)

#define TMXC_PERIPH_BASE 0x08000000ULL
#define TMXC_PERIPH_SIZE 0x78000000ULL

#define TMXC_ARM64_SYSREG_CNTFRQ_EL0       "mrs %0, cntfrq_el0"
#define TMXC_ARM64_SYSREG_CNTVCT_EL0       "mrs %0, cntvct_el0"
#define TMXC_ARM64_SYSREG_CNTV_CTL_EL0_READ  "mrs %0, cntv_ctl_el0"
#define TMXC_ARM64_SYSREG_CNTV_CTL_EL0_WRITE "msr cntv_ctl_el0, %0"
#define TMXC_ARM64_SYSREG_CNTKCTL_EL1_READ  "mrs %0, cntkctl_el1"
#define TMXC_ARM64_SYSREG_CNTKCTL_EL1_WRITE "msr cntkctl_el1, %0"
#define TMXC_ARM64_SYSREG_CNTPCT_EL0       "mrs %0, cntpct_el0"
#define TMXC_ARM64_SYSREG_CNTP_CTL_EL0_READ  "mrs %0, cntp_ctl_el0"
#define TMXC_ARM64_SYSREG_CNTP_CTL_EL0_WRITE "msr cntp_ctl_el0, %0"
#define TMXC_ARM64_SYSREG_CNTP_CVAL_EL0_READ  "mrs %0, cntp_cval_el0"
#define TMXC_ARM64_SYSREG_CNTP_CVAL_EL0_WRITE "msr cntp_cval_el0, %0"

#define TMXC_ARM64_SYSREG_DAIF_READ   "mrs %0, daif"
#define TMXC_ARM64_SYSREG_DAIF_WRITE  "msr daif, %0"
#define TMXC_ARM64_SYSREG_DAIFClr     "msr daifclr, %0"
#define TMXC_ARM64_SYSREG_DAIFSet     "msr daifset, %0"

#define TMXC_ARM64_SYSREG_CurrentEL   "mrs %0, CurrentEL"
#define TMXC_ARM64_SYSREG_SPSR_EL1_READ    "mrs %0, spsr_el1"
#define TMXC_ARM64_SYSREG_SPSR_EL1_WRITE   "msr spsr_el1, %0"
#define TMXC_ARM64_SYSREG_SPSR_EL2_READ    "mrs %0, spsr_el2"
#define TMXC_ARM64_SYSREG_SPSR_EL2_WRITE   "msr spsr_el2, %0"
#define TMXC_ARM64_SYSREG_SPSR_EL3_READ    "mrs %0, spsr_el3"
#define TMXC_ARM64_SYSREG_SPSR_EL3_WRITE   "msr spsr_el3, %0"
#define TMXC_ARM64_SYSREG_ELR_EL1_READ     "mrs %0, elr_el1"
#define TMXC_ARM64_SYSREG_ELR_EL1_WRITE    "msr elr_el1, %0"
#define TMXC_ARM64_SYSREG_ELR_EL2_READ     "mrs %0, elr_el2"
#define TMXC_ARM64_SYSREG_ELR_EL2_WRITE    "msr elr_el2, %0"
#define TMXC_ARM64_SYSREG_ELR_EL3_READ     "mrs %0, elr_el3"
#define TMXC_ARM64_SYSREG_ELR_EL3_WRITE    "msr elr_el3, %0"

#define TMXC_ARM64_SYSREG_SCTLR_EL1_READ   "mrs %0, sctlr_el1"
#define TMXC_ARM64_SYSREG_SCTLR_EL1_WRITE  "msr sctlr_el1, %0"
#define TMXC_ARM64_SYSREG_SCTLR_EL2_READ   "mrs %0, sctlr_el2"
#define TMXC_ARM64_SYSREG_SCTLR_EL2_WRITE  "msr sctlr_el2, %0"
#define TMXC_ARM64_SYSREG_SCTLR_EL3_READ   "mrs %0, sctlr_el3"
#define TMXC_ARM64_SYSREG_SCTLR_EL3_WRITE  "msr sctlr_el3, %0"

#define TMXC_ARM64_SYSREG_TCR_EL1_READ     "mrs %0, tcr_el1"
#define TMXC_ARM64_SYSREG_TCR_EL1_WRITE    "msr tcr_el1, %0"
#define TMXC_ARM64_SYSREG_TCR_EL2_READ     "mrs %0, tcr_el2"
#define TMXC_ARM64_SYSREG_TCR_EL2_WRITE    "msr tcr_el2, %0"
#define TMXC_ARM64_SYSREG_TCR_EL3_READ     "mrs %0, tcr_el3"
#define TMXC_ARM64_SYSREG_TCR_EL3_WRITE    "msr tcr_el3, %0"

#define TMXC_ARM64_SYSREG_TTBR0_EL1_READ   "mrs %0, ttbr0_el1"
#define TMXC_ARM64_SYSREG_TTBR0_EL1_WRITE  "msr ttbr0_el1, %0"
#define TMXC_ARM64_SYSREG_TTBR1_EL1_READ   "mrs %0, ttbr1_el1"
#define TMXC_ARM64_SYSREG_TTBR1_EL1_WRITE  "msr ttbr1_el1, %0"
#define TMXC_ARM64_SYSREG_TTBR0_EL2_READ   "mrs %0, ttbr0_el2"
#define TMXC_ARM64_SYSREG_TTBR0_EL2_WRITE  "msr ttbr0_el2, %0"
#define TMXC_ARM64_SYSREG_VTTBR_EL2_READ   "mrs %0, vttbr_el2"
#define TMXC_ARM64_SYSREG_VTTBR_EL2_WRITE  "msr vttbr_el2, %0"

#define TMXC_ARM64_SYSREG_MAIR_EL1_READ    "mrs %0, mair_el1"
#define TMXC_ARM64_SYSREG_MAIR_EL1_WRITE   "msr mair_el1, %0"
#define TMXC_ARM64_SYSREG_MAIR_EL2_READ    "mrs %0, mair_el2"
#define TMXC_ARM64_SYSREG_MAIR_EL2_WRITE   "msr mair_el2, %0"

#define TMXC_ARM64_SYSREG_ID_AA64ISAR0_EL1 "mrs %0, id_aa64isar0_el1"
#define TMXC_ARM64_SYSREG_ID_AA64ISAR1_EL1 "mrs %0, id_aa64isar1_el1"
#define TMXC_ARM64_SYSREG_ID_AA64MMFR0_EL1 "mrs %0, id_aa64mmfr0_el1"
#define TMXC_ARM64_SYSREG_ID_AA64MMFR1_EL1 "mrs %0, id_aa64mmfr1_el1"
#define TMXC_ARM64_SYSREG_ID_AA64MMFR2_EL1 "mrs %0, id_aa64mmfr2_el1"
#define TMXC_ARM64_SYSREG_ID_AA64PFR0_EL1  "mrs %0, id_aa64pfr0_el1"
#define TMXC_ARM64_SYSREG_ID_AA64PFR1_EL1  "mrs %0, id_aa64pfr1_el1"
#define TMXC_ARM64_SYSREG_MIDR_EL1        "mrs %0, midr_el1"
#define TMXC_ARM64_SYSREG_MPIDR_EL1       "mrs %0, mpidr_el1"
#define TMXC_ARM64_SYSREG_REVIDR_EL1      "mrs %0, revidr_el1"

#define TMXC_ARM64_SYSREG_CLIDR_EL1    "mrs %0, clidr_el1"
#define TMXC_ARM64_SYSREG_CSSELR_EL1_READ   "mrs %0, csselr_el1"
#define TMXC_ARM64_SYSREG_CSSELR_EL1_WRITE  "msr csselr_el1, %0"
#define TMXC_ARM64_SYSREG_CCSIDR_EL1   "mrs %0, ccsidr_el1"
#define TMXC_ARM64_SYSREG_CTR_EL0      "mrs %0, ctr_el0"

#define TMXC_ARM64_SYSREG_DC_ISW       "dc isw, %0"
#define TMXC_ARM64_SYSREG_DC_CSW       "dc csw, %0"
#define TMXC_ARM64_SYSREG_DC_CISW      "dc cisw, %0"
#define TMXC_ARM64_SYSREG_IC_IALLUIS   "ic ialluis"
#define TMXC_ARM64_SYSREG_IC_IALLU     "ic iallu"
#define TMXC_ARM64_SYSREG_IC_IVAU      "ic ivau, %0"
#define TMXC_ARM64_SYSREG_DSB_SY       "dsb sy"
#define TMXC_ARM64_SYSREG_DSB_ISH      "dsb ish"
#define TMXC_ARM64_SYSREG_DMB_SY       "dmb sy"
#define TMXC_ARM64_SYSREG_DMB_ISH      "dmb ish"
#define TMXC_ARM64_SYSREG_ISB          "isb"

#define TMXC_ARM64_CPACR_EL1_READ     "mrs %0, cpacr_el1"
#define TMXC_ARM64_CPACR_EL1_WRITE    "msr cpacr_el1, %0"

#define TMXC_ARM64_HCR_EL2_READ       "mrs %0, hcr_el2"
#define TMXC_ARM64_HCR_EL2_WRITE      "msr hcr_el2, %0"

#define TMXC_ARM64_SCR_EL3_READ       "mrs %0, scr_el3"
#define TMXC_ARM64_SCR_EL3_WRITE      "msr scr_el3, %0"

#define TMXC_ARM64_VBAR_EL1_READ      "mrs %0, vbar_el1"
#define TMXC_ARM64_VBAR_EL1_WRITE     "msr vbar_el1, %0"
#define TMXC_ARM64_VBAR_EL2_READ      "mrs %0, vbar_el2"
#define TMXC_ARM64_VBAR_EL2_WRITE     "msr vbar_el2, %0"
#define TMXC_ARM64_VBAR_EL3_READ      "mrs %0, vbar_el3"
#define TMXC_ARM64_VBAR_EL3_WRITE     "msr vbar_el3, %0"

#define TMXC_GICD_CTLR           (TMXC_GICD_BASE + 0x0000)
#define TMXC_GICD_TYPER          (TMXC_GICD_BASE + 0x0004)
#define TMXC_GICD_IIDR           (TMXC_GICD_BASE + 0x0008)
#define TMXC_GICD_IGROUPR        (TMXC_GICD_BASE + 0x0080)
#define TMXC_GICD_ISENABLER      (TMXC_GICD_BASE + 0x0100)
#define TMXC_GICD_ICENABLER      (TMXC_GICD_BASE + 0x0180)
#define TMXC_GICD_ISPENDR        (TMXC_GICD_BASE + 0x0200)
#define TMXC_GICD_ICPENDR        (TMXC_GICD_BASE + 0x0280)
#define TMXC_GICD_ISACTIVER      (TMXC_GICD_BASE + 0x0300)
#define TMXC_GICD_ICACTIVER      (TMXC_GICD_BASE + 0x0380)
#define TMXC_GICD_IPRIORITYR     (TMXC_GICD_BASE + 0x0400)
#define TMXC_GICD_ITARGETSR      (TMXC_GICD_BASE + 0x0800)
#define TMXC_GICD_ICFGR          (TMXC_GICD_BASE + 0x0C00)
#define TMXC_GICD_SGIR           (TMXC_GICD_BASE + 0x0F00)
#define TMXC_GICD_CPENDSGIR      (TMXC_GICD_BASE + 0x0F10)
#define TMXC_GICD_SPENDSGIR      (TMXC_GICD_BASE + 0x0F20)

#define TMXC_GICD_IGROUPR_MOD    32
#define TMXC_GICD_ISENABLER_MOD  32
#define TMXC_GICD_ICENABLER_MOD  32
#define TMXC_GICD_ISPENDR_MOD    32
#define TMXC_GICD_ICPENDR_MOD    32
#define TMXC_GICD_ISACTIVER_MOD  32
#define TMXC_GICD_ICACTIVER_MOD  32
#define TMXC_GICD_IPRIORITYR_MOD 4
#define TMXC_GICD_ITARGETSR_MOD  4
#define TMXC_GICD_ICFGR_MOD      16

#define TMXC_GICC_CTLR           (TMXC_GICC_BASE + 0x0000)
#define TMXC_GICC_PMR            (TMXC_GICC_BASE + 0x0004)
#define TMXC_GICC_BPR            (TMXC_GICC_BASE + 0x0008)
#define TMXC_GICC_IAR            (TMXC_GICC_BASE + 0x000C)
#define TMXC_GICC_EOIR           (TMXC_GICC_BASE + 0x0010)
#define TMXC_GICC_RPR            (TMXC_GICC_BASE + 0x0014)
#define TMXC_GICC_HPPIR          (TMXC_GICC_BASE + 0x0018)
#define TMXC_GICC_ABPR           (TMXC_GICC_BASE + 0x001C)
#define TMXC_GICC_AIAR           (TMXC_GICC_BASE + 0x0020)
#define TMXC_GICC_AEOIR          (TMXC_GICC_BASE + 0x0028)
#define TMXC_GICC_AHPPIR         (TMXC_GICC_BASE + 0x002C)
#define TMXC_GICC_APR            (TMXC_GICC_BASE + 0x00D0)
#define TMXC_GICC_NSAPR          (TMXC_GICC_BASE + 0x00E0)
#define TMXC_GICC_IIDR           (TMXC_GICC_BASE + 0x00FC)
#define TMXC_GICC_DIR            (TMXC_GICC_BASE + 0x1000)
#define TMXC_GICC_PRIORITY_MASK  0xF0
#define TMXC_GICC_PRIORITY_SHIFT 4

#define TMXC_UART_DR             (TMXC_UART0_BASE + 0x0000)
#define TMXC_UART_RSR            (TMXC_UART0_BASE + 0x0004)
#define TMXC_UART_FR             (TMXC_UART0_BASE + 0x0018)
#define TMXC_UART_ILPR           (TMXC_UART0_BASE + 0x0020)
#define TMXC_UART_IBRD           (TMXC_UART0_BASE + 0x0024)
#define TMXC_UART_FBRD           (TMXC_UART0_BASE + 0x0028)
#define TMXC_UART_LCR_H          (TMXC_UART0_BASE + 0x002C)
#define TMXC_UART_CR             (TMXC_UART0_BASE + 0x0030)
#define TMXC_UART_IFLS           (TMXC_UART0_BASE + 0x0034)
#define TMXC_UART_IMSC           (TMXC_UART0_BASE + 0x0038)
#define TMXC_UART_RIS            (TMXC_UART0_BASE + 0x003C)
#define TMXC_UART_MIS            (TMXC_UART0_BASE + 0x0040)
#define TMXC_UART_ICR            (TMXC_UART0_BASE + 0x0044)
#define TMXC_UART_DMACR          (TMXC_UART0_BASE + 0x0048)

#define TMXC_UART_FR_TXFF        (1 << 5)
#define TMXC_UART_FR_RXFE        (1 << 4)
#define TMXC_UART_FR_BUSY        (1 << 3)
#define TMXC_UART_FR_TXFF        (1 << 5)
#define TMXC_UART_FR_RXFF        (1 << 6)

#define TMXC_UART_LCR_H_SPS      (1 << 7)
#define TMXC_UART_LCR_H_WLEN_8   (3 << 5)
#define TMXC_UART_LCR_H_WLEN_7   (2 << 5)
#define TMXC_UART_LCR_H_WLEN_6   (1 << 5)
#define TMXC_UART_LCR_H_WLEN_5   (0 << 5)
#define TMXC_UART_LCR_H_FEN      (1 << 4)
#define TMXC_UART_LCR_H_STP2     (1 << 3)
#define TMXC_UART_LCR_H_EPS      (1 << 2)
#define TMXC_UART_LCR_H_PEN      (1 << 1)
#define TMXC_UART_LCR_H_BRK      (1 << 0)

#define TMXC_UART_CR_CTSEN       (1 << 15)
#define TMXC_UART_CR_RTSEN       (1 << 14)
#define TMXC_UART_CR_RTS         (1 << 11)
#define TMXC_UART_CR_RXE         (1 << 9)
#define TMXC_UART_CR_TXE         (1 << 8)
#define TMXC_UART_CR_LBE         (1 << 7)
#define TMXC_UART_CR_UARTEN      (1 << 0)

#define TMXC_UART_IMSC_CTSMIM    (1 << 1)
#define TMXC_UART_IMSC_RXIM      (1 << 4)
#define TMXC_UART_IMSC_TXIM      (1 << 5)
#define TMXC_UART_IMSC_RTIM      (1 << 6)
#define TMXC_UART_IMSC_FEIM      (1 << 7)
#define TMXC_UART_IMSC_PEIM      (1 << 8)
#define TMXC_UART_IMSC_BEIM      (1 << 9)
#define TMXC_UART_IMSC_OEIM      (1 << 10)

#define TMXC_TIMER_CS            (TMXC_TIMER_BASE + 0x0000)
#define TMXC_TIMER_CLO           (TMXC_TIMER_BASE + 0x0004)
#define TMXC_TIMER_CHI           (TMXC_TIMER_BASE + 0x0008)
#define TMXC_TIMER_C0            (TMXC_TIMER_BASE + 0x000C)
#define TMXC_TIMER_C1            (TMXC_TIMER_BASE + 0x0010)
#define TMXC_TIMER_C2            (TMXC_TIMER_BASE + 0x0014)
#define TMXC_TIMER_C3            (TMXC_TIMER_BASE + 0x0018)

#define TMXC_WATCHDOG_LOAD       (TMXC_WATCHDOG_BASE + 0x0000)
#define TMXC_WATCHDOG_VALUE      (TMXC_WATCHDOG_BASE + 0x0004)
#define TMXC_WATCHDOG_CONTROL    (TMXC_WATCHDOG_BASE + 0x0008)
#define TMXC_WATCHDOG_CLEAR      (TMXC_WATCHDOG_BASE + 0x000C)
#define TMXC_WATCHDOG_RAW_IIR     (TMXC_WATCHDOG_BASE + 0x0010)
#define TMXC_WATCHDOG_MIS_IIR    (TMXC_WATCHDOG_BASE + 0x0014)

#define TMXC_GPIO_GPFSEL0        (TMXC_GPIO_BASE + 0x0000)
#define TMXC_GPIO_GPFSEL1        (TMXC_GPIO_BASE + 0x0004)
#define TMXC_GPIO_GPFSEL2        (TMXC_GPIO_BASE + 0x0008)
#define TMXC_GPIO_GPFSEL3        (TMXC_GPIO_BASE + 0x000C)
#define TMXC_GPIO_GPFSEL4        (TMXC_GPIO_BASE + 0x0010)
#define TMXC_GPIO_GPFSEL5        (TMXC_GPIO_BASE + 0x0014)
#define TMXC_GPIO_GPSET0         (TMXC_GPIO_BASE + 0x001C)
#define TMXC_GPIO_GPSET1         (TMXC_GPIO_BASE + 0x0020)
#define TMXC_GPIO_GPCLR0         (TMXC_GPIO_BASE + 0x0028)
#define TMXC_GPIO_GPCLR1         (TMXC_GPIO_BASE + 0x002C)
#define TMXC_GPIO_GPLEV0         (TMXC_GPIO_BASE + 0x0034)
#define TMXC_GPIO_GPLEV1         (TMXC_GPIO_BASE + 0x0038)
#define TMXC_GPIO_GPEDS0         (TMXC_GPIO_BASE + 0x0040)
#define TMXC_GPIO_GPEDS1         (TMXC_GPIO_BASE + 0x0044)
#define TMXC_GPIO_GPREN0         (TMXC_GPIO_BASE + 0x004C)
#define TMXC_GPIO_GPREN1         (TMXC_GPIO_BASE + 0x0050)
#define TMXC_GPIO_GPFEN0         (TMXC_GPIO_BASE + 0x0058)
#define TMXC_GPIO_GPFEN1         (TMXC_GPIO_BASE + 0x005C)
#define TMXC_GPIO_GPHEN0         (TMXC_GPIO_BASE + 0x0064)
#define TMXC_GPIO_GPHEN1         (TMXC_GPIO_BASE + 0x0068)
#define TMXC_GPIO_GPLEN0         (TMXC_GPIO_BASE + 0x0070)
#define TMXC_GPIO_GPLEN1         (TMXC_GPIO_BASE + 0x0074)
#define TMXC_GPIO_GPAREN0        (TMXC_GPIO_BASE + 0x007C)
#define TMXC_GPIO_GPAREN1        (TMXC_GPIO_BASE + 0x0080)
#define TMXC_GPIO_GPAFEN0        (TMXC_GPIO_BASE + 0x0088)
#define TMXC_GPIO_GPAFEN1        (TMXC_GPIO_BASE + 0x008C)
#define TMXC_GPIO_GPPUD          (TMXC_GPIO_BASE + 0x0094)
#define TMXC_GPIO_GPPUDCLK0      (TMXC_GPIO_BASE + 0x0098)
#define TMXC_GPIO_GPPUDCLK1      (TMXC_GPIO_BASE + 0x009C)

#define TMXC_I2C_CON            (TMXC_I2C0_BASE + 0x0000)
#define TMXC_I2C_TAR             (TMXC_I2C0_BASE + 0x0004)
#define TMXC_I2C_DATA_CMD        (TMXC_I2C0_BASE + 0x0010)
#define TMXC_I2C_SS_SCL_HCNT     (TMXC_I2C0_BASE + 0x0014)
#define TMXC_I2C_SS_SCL_LCNT     (TMXC_I2C0_BASE + 0x0018)
#define TMXC_I2C_FS_SCL_HCNT     (TMXC_I2C0_BASE + 0x001C)
#define TMXC_I2C_FS_SCL_LCNT     (TMXC_I2C0_BASE + 0x0020)
#define TMXC_I2C_HS_SCL_HCNT     (TMXC_I2C0_BASE + 0x0024)
#define TMXC_I2C_HS_SCL_LCNT     (TMXC_I2C0_BASE + 0x0028)
#define TMXC_I2C_INTR_STAT       (TMXC_I2C0_BASE + 0x002C)
#define TMXC_I2C_INTR_MASK       (TMXC_I2C0_BASE + 0x0030)
#define TMXC_I2C_RAW_INTR_STAT   (TMXC_I2C0_BASE + 0x0034)
#define TMXC_I2C_RX_TL           (TMXC_I2C0_BASE + 0x0038)
#define TMXC_I2C_TX_TL           (TMXC_I2C0_BASE + 0x003C)
#define TMXC_I2C_CLR_INTR        (TMXC_I2C0_BASE + 0x0040)
#define TMXC_I2C_CLR_RX_UNDER    (TMXC_I2C0_BASE + 0x0044)
#define TMXC_I2C_CLR_RX_OVER     (TMXC_I2C0_BASE + 0x0048)
#define TMXC_I2C_CLR_TX_OVER     (TMXC_I2C0_BASE + 0x004C)
#define TMXC_I2C_CLR_RD_REQ      (TMXC_I2C0_BASE + 0x0050)
#define TMXC_I2C_CLR_TX_ABRT     (TMXC_I2C0_BASE + 0x0054)
#define TMXC_I2C_CLR_RX_DONE     (TMXC_I2C0_BASE + 0x0058)
#define TMXC_I2C_CLR_ACTIVITY    (TMXC_I2C0_BASE + 0x005C)
#define TMXC_I2C_CLR_STOP_DET    (TMXC_I2C0_BASE + 0x0060)
#define TMXC_I2C_CLR_START_DET   (TMXC_I2C0_BASE + 0x0064)
#define TMXC_I2C_CLR_GEN_CALL    (TMXC_I2C0_BASE + 0x0068)
#define TMXC_I2C_ENABLE          (TMXC_I2C0_BASE + 0x006C)
#define TMXC_I2C_STATUS          (TMXC_I2C0_BASE + 0x0070)
#define TMXC_I2C_TXFLR           (TMXC_I2C0_BASE + 0x0074)
#define TMXC_I2C_RXFLR           (TMXC_I2C0_BASE + 0x0078)
#define TMXC_I2C_SDA_HOLD        (TMXC_I2C0_BASE + 0x007C)
#define TMXC_I2C_TX_ABRT_SOURCE  (TMXC_I2C0_BASE + 0x0080)
#define TMXC_I2C_SLV_DATA_NACK   (TMXC_I2C0_BASE + 0x0084)
#define TMXC_I2C_DMA_CR          (TMXC_I2C0_BASE + 0x0088)
#define TMXC_I2C_DMA_TDLR        (TMXC_I2C0_BASE + 0x008C)
#define TMXC_I2C_DMA_RDLR        (TMXC_I2C0_BASE + 0x0090)
#define TMXC_I2C_SDA_SETUP       (TMXC_I2C0_BASE + 0x0094)
#define TMXC_I2C_ACK_GENERAL_CALL (TMXC_I2C0_BASE + 0x0098)
#define TMXC_I2C_ENABLE_STATUS   (TMXC_I2C0_BASE + 0x009C)
#define TMXC_I2C_FS_SPKLEN       (TMXC_I2C0_BASE + 0x00A0)

#define TMXC_SPI_SSIENR          (TMXC_SPI0_BASE + 0x0000)
#define TMXC_SPI_MWCR            (TMXC_SPI0_BASE + 0x0004)
#define TMXC_SPI_SER             (TMXC_SPI0_BASE + 0x0008)
#define TMXC_SPI_BAUDR           (TMXC_SPI0_BASE + 0x000C)
#define TMXC_SPI_TXFTLR          (TMXC_SPI0_BASE + 0x0010)
#define TMXC_SPI_RXFTLR          (TMXC_SPI0_BASE + 0x0014)
#define TMXC_SPI_TXFLR           (TMXC_SPI0_BASE + 0x0018)
#define TMXC_SPI_RXFLR           (TMXC_SPI0_BASE + 0x001C)
#define TMXC_SPI_SR              (TMXC_SPI0_BASE + 0x0020)
#define TMXC_SPI_IMR             (TMXC_SPI0_BASE + 0x0024)
#define TMXC_SPI_ISR             (TMXC_SPI0_BASE + 0x0028)
#define TMXC_SPI_RISR            (TMXC_SPI0_BASE + 0x002C)
#define TMXC_SPI_TXOICR          (TMXC_SPI0_BASE + 0x0030)
#define TMXC_SPI_RXOICR          (TMXC_SPI0_BASE + 0x0034)
#define TMXC_SPI_RXUICR          (TMXC_SPI0_BASE + 0x0038)
#define TMXC_SPI_MSTICR          (TMXC_SPI0_BASE + 0x003C)
#define TMXC_SPI_ICR             (TMXC_SPI0_BASE + 0x0040)
#define TMXC_SPI_DMACR           (TMXC_SPI0_BASE + 0x0044)
#define TMXC_SPI_DMATDLR         (TMXC_SPI0_BASE + 0x0048)
#define TMXC_SPI_DMARDLR         (TMXC_SPI0_BASE + 0x004C)
#define TMXC_SPI_IDR             (TMXC_SPI0_BASE + 0x0050)
#define TMXC_SPI_SSI_VERSION_ID  (TMXC_SPI0_BASE + 0x0054)
#define TMXC_SPI_DR              (TMXC_SPI0_BASE + 0x0060)
#define TMXC_SPI_RX_SAMPLE_DLY   (TMXC_SPI0_BASE + 0x0064)
#define TMXC_SPI_SPI_CTRLR0      (TMXC_SPI0_BASE + 0x0068)
#define TMXC_SPI_TXD_DRIVE_EDGE  (TMXC_SPI0_BASE + 0x006C)

#define TMXC_SDHCI_DMA_ADDR      (TMXC_SDHCI_BASE + 0x0000)
#define TMXC_SDHCI_BLOCK_SIZE    (TMXC_SDHCI_BASE + 0x0004)
#define TMXC_SDHCI_BLOCK_COUNT   (TMXC_SDHCI_BASE + 0x0006)
#define TMXC_SDHCI_ARGUMENT      (TMXC_SDHCI_BASE + 0x0008)
#define TMXC_SDHCI_TRANSFER_MODE (TMXC_SDHCI_BASE + 0x000C)
#define TMXC_SDHCI_COMMAND       (TMXC_SDHCI_BASE + 0x000E)
#define TMXC_SDHCI_RESPONSE      (TMXC_SDHCI_BASE + 0x0010)
#define TMXC_SDHCI_BUFFER        (TMXC_SDHCI_BASE + 0x0020)
#define TMXC_SDHCI_PRESENT_STATE (TMXC_SDHCI_BASE + 0x0024)
#define TMXC_SDHCI_HOST_CONTROL  (TMXC_SDHCI_BASE + 0x0028)
#define TMXC_SDHCI_POWER_CONTROL (TMXC_SDHCI_BASE + 0x0029)
#define TMXC_SDHCI_CLOCK_CONTROL (TMXC_SDHCI_BASE + 0x002C)
#define TMXC_SDHCI_TIMEOUT_CTRL  (TMXC_SDHCI_BASE + 0x002E)
#define TMXC_SDHCI_SOFTWARE_RESET (TMXC_SDHCI_BASE + 0x002F)
#define TMXC_SDHCI_INT_STATUS    (TMXC_SDHCI_BASE + 0x0030)
#define TMXC_SDHCI_INT_ENABLE    (TMXC_SDHCI_BASE + 0x0034)
#define TMXC_SDHCI_SIGNAL_ENABLE (TMXC_SDHCI_BASE + 0x0038)
#define TMXC_SDHCI_ACMD_STATUS   (TMXC_SDHCI_BASE + 0x003C)
#define TMXC_SDHCI_CAPABILITIES  (TMXC_SDHCI_BASE + 0x0040)
#define TMXC_SDHCI_MAX_CURRENT   (TMXC_SDHCI_BASE + 0x0048)
#define TMXC_SDHCI_FORCE_EVT     (TMXC_SDHCI_BASE + 0x0050)
#define TMXC_SDHCI_ADMA_ERR      (TMXC_SDHCI_BASE + 0x0054)
#define TMXC_SDHCI_ADMA_ADDR     (TMXC_SDHCI_BASE + 0x0058)
#define TMXC_SDHCI_SLOT_INT_STATUS (TMXC_SDHCI_BASE + 0x00FC)

#define TMXC_DISPLAY_HDMI_BASE   (TMXC_DISPLAY_BASE + 0x0000)
#define TMXC_DISPLAY_DSI_BASE    (TMXC_DISPLAY_BASE + 0x1000)
#define TMXC_DISPLAY_LCD_BASE    (TMXC_DISPLAY_BASE + 0x2000)

#define TMXC_DISPLAY_HDMI_PHY_CTRL (TMXC_DISPLAY_HDMI_BASE + 0x0000)
#define TMXC_DISPLAY_HDMI_PLL_CTRL (TMXC_DISPLAY_HDMI_BASE + 0x0004)
#define TMXC_DISPLAY_HDMI_CTRL    (TMXC_DISPLAY_HDMI_BASE + 0x0008)
#define TMXC_DISPLAY_HDMI_INT_STAT (TMXC_DISPLAY_HDMI_BASE + 0x000C)
#define TMXC_DISPLAY_HDMI_INT_MASK (TMXC_DISPLAY_HDMI_BASE + 0x0010)

#define TMXC_DISPLAY_DSI_HOST_CTRL (TMXC_DISPLAY_DSI_BASE + 0x0000)
#define TMXC_DISPLAY_DSI_PHY_CTRL (TMXC_DISPLAY_DSI_BASE + 0x0004)
#define TMXC_DISPLAY_DSI_PKT_HDR  (TMXC_DISPLAY_DSI_BASE + 0x0008)
#define TMXC_DISPLAY_DSI_PKT_PAYLOAD (TMXC_DISPLAY_DSI_BASE + 0x000C)
#define TMXC_DISPLAY_DSI_GEN_CTRL (TMXC_DISPLAY_DSI_BASE + 0x0010)

#define TMXC_DISPLAY_LCD_CTRL     (TMXC_DISPLAY_LCD_BASE + 0x0000)
#define TMXC_DISPLAY_LCD_TIMING0 (TMXC_DISPLAY_LCD_BASE + 0x0004)
#define TMXC_DISPLAY_LCD_TIMING1 (TMXC_DISPLAY_LCD_BASE + 0x0008)
#define TMXC_DISPLAY_LCD_TIMING2 (TMXC_DISPLAY_LCD_BASE + 0x000C)
#define TMXC_DISPLAY_LCD_VSYNC    (TMXC_DISPLAY_LCD_BASE + 0x0010)
#define TMXC_DISPLAY_LCD_HSYNC    (TMXC_DISPLAY_LCD_BASE + 0x0014)
#define TMXC_DISPLAY_LCD_POLARITY (TMXC_DISPLAY_LCD_BASE + 0x0018)
#define TMXC_DISPLAY_LCD_CTRL2    (TMXC_DISPLAY_LCD_BASE + 0x001C)
#define TMXC_DISPLAY_LCD_FIFO_CTRL (TMXC_DISPLAY_LCD_BASE + 0x0020)

#define TMXC_GPU_GP0_BASE        (TMXC_GPU_BASE + 0x0000)
#define TMXC_GPU_GP1_BASE        (TMXC_GPU_BASE + 0x1000)
#define TMXC_GPU_V3D_BASE        (TMXC_GPU_BASE + 0x2000)

#define TMXC_CAMERA_MIPI_CSI2_BASE (TMXC_CAMERA_BASE + 0x0000)
#define TMXC_CAMERA_ISP_BASE      (TMXC_CAMERA_BASE + 0x1000)
#define TMXC_CAMERA_CSI2_DPHY_CTRL (TMXC_CAMERA_MIPI_CSI2_BASE + 0x0000)
#define TMXC_CAMERA_CSI2_CTRL     (TMXC_CAMERA_MIPI_CSI2_BASE + 0x0004)
#define TMXC_CAMERA_CSI2_STAT     (TMXC_CAMERA_MIPI_CSI2_BASE + 0x0008)
#define TMXC_CAMERA_CSI2_INT_MASK (TMXC_CAMERA_MIPI_CSI2_BASE + 0x000C)
#define TMXC_CAMERA_CSI2_VC0_CTRL (TMXC_CAMERA_MIPI_CSI2_BASE + 0x0010)
#define TMXC_CAMERA_CSI2_VC1_CTRL (TMXC_CAMERA_MIPI_CSI2_BASE + 0x0014)
#define TMXC_CAMERA_CSI2_VC2_CTRL (TMXC_CAMERA_MIPI_CSI2_BASE + 0x0018)
#define TMXC_CAMERA_CSI2_VC3_CTRL (TMXC_CAMERA_MIPI_CSI2_BASE + 0x001C)

#define TMXC_AUDIO_I2S_BASE      (TMXC_AUDIO_BASE + 0x0000)
#define TMXC_AUDIO_PCM_BASE      (TMXC_AUDIO_BASE + 0x1000)
#define TMXC_AUDIO_SPDIF_BASE    (TMXC_AUDIO_BASE + 0x2000)
#define TMXC_AUDIO_I2S_CTRL      (TMXC_AUDIO_I2S_BASE + 0x0000)
#define TMXC_AUDIO_I2S_CLK_CTRL  (TMXC_AUDIO_I2S_BASE + 0x0004)
#define TMXC_AUDIO_I2S_TX_FIFO   (TMXC_AUDIO_I2S_BASE + 0x0008)
#define TMXC_AUDIO_I2S_RX_FIFO   (TMXC_AUDIO_I2S_BASE + 0x000C)
#define TMXC_AUDIO_I2S_INT_STAT  (TMXC_AUDIO_I2S_BASE + 0x0010)
#define TMXC_AUDIO_I2S_INT_MASK  (TMXC_AUDIO_I2S_BASE + 0x0014)

#define TMXC_TOUCH_I2C_ADDR       0x38
#define TMXC_TOUCH_REG_X         0x03
#define TMXC_TOUCH_REG_Y         0x05
#define TMXC_TOUCH_REG_PRESSURE  0x07
#define TMXC_TOUCH_REG_GESTURE   0x09
#define TMXC_TOUCH_REG_MODE      0x0A
#define TMXC_TOUCH_REG_INT_ENABLE 0x0E
#define TMXC_TOUCH_REG_INT_STATUS 0x0F

#define TMXC_SENSOR_ACCEL_X      0x28
#define TMXC_SENSOR_ACCEL_Y      0x2A
#define TMXC_SENSOR_ACCEL_Z      0x2C
#define TMXC_SENSOR_GYRO_X       0x22
#define TMXC_SENSOR_GYRO_Y       0x24
#define TMXC_SENSOR_GYRO_Z       0x26
#define TMXC_SENSOR_MAG_X        0x08
#define TMXC_SENSOR_MAG_Y        0x0A
#define TMXC_SENSOR_MAG_Z        0x0C
#define TMXC_SENSOR_TEMP         0x41
#define TMXC_SENSOR_LIGHT        0x0A
#define TMXC_SENSOR_PROXIMITY    0x08

#define TMXC_PMU_CPU_PWR         (TMXC_PMU_BASE + 0x0000)
#define TMXC_PMU_CORE_PWR        (TMXC_PMU_BASE + 0x0004)
#define TMXC_PMU_GPU_PWR         (TMXC_PMU_BASE + 0x0008)
#define TMXC_PMU_VPU_PWR         (TMXC_PMU_BASE + 0x000C)
#define TMXC_PMU_MEM_PWR         (TMXC_PMU_BASE + 0x0010)
#define TMXC_PMU_STATUS          (TMXC_PMU_BASE + 0x0014)
#define TMXC_PMU_INT_MASK        (TMXC_PMU_BASE + 0x0018)

#define TMXC_SYSCON_CHIP_ID      (TMXC_SYSCON_BASE + 0x0000)
#define TMXC_SYSCON_CHIP_REV     (TMXC_SYSCON_BASE + 0x0004)
#define TMXC_SYSCON_PLL_CTRL     (TMXC_SYSCON_BASE + 0x0008)
#define TMXC_SYSCON_CLK_SEL      (TMXC_SYSCON_BASE + 0x000C)
#define TMXC_SYSCON_CLK_DIV      (TMXC_SYSCON_BASE + 0x0010)
#define TMXC_SYSCON_RESET_CTRL   (TMXC_SYSCON_BASE + 0x0014)
#define TMXC_SYSCON_PWR_CTRL     (TMXC_SYSCON_BASE + 0x0018)

#define TMXC_CLOCK_CPU_CLK       (TMXC_CLOCK_BASE + 0x0000)
#define TMXC_CLOCK_CORE_CLK      (TMXC_CLOCK_BASE + 0x0004)
#define TMXC_CLOCK_BUS_CLK       (TMXC_CLOCK_BASE + 0x0008)
#define TMXC_CLOCK_PERIPH_CLK    (TMXC_CLOCK_BASE + 0x000C)
#define TMXC_CLOCK_EMMC_CLK      (TMXC_CLOCK_BASE + 0x0010)
#define TMXC_CLOCK_HDMI_CLK      (TMXC_CLOCK_BASE + 0x0014)
#define TMXC_CLOCK_PLL0          (TMXC_CLOCK_BASE + 0x0020)
#define TMXC_CLOCK_PLL1          (TMXC_CLOCK_BASE + 0x0024)
#define TMXC_CLOCK_PLL2          (TMXC_CLOCK_BASE + 0x0028)

#define TMXC_DDR_CTRL            (TMXC_DDR_BASE + 0x0000)
#define TMXC_DDR_TIMING0         (TMXC_DDR_BASE + 0x0004)
#define TMXC_DDR_TIMING1         (TMXC_DDR_BASE + 0x0008)
#define TMXC_DDR_TIMING2         (TMXC_DDR_BASE + 0x000C)
#define TMXC_DDR_TIMING3         (TMXC_DDR_BASE + 0x0010)
#define TMXC_DDR_MR0             (TMXC_DDR_BASE + 0x0014)
#define TMXC_DDR_MR1             (TMXC_DDR_BASE + 0x0018)
#define TMXC_DDR_MR2             (TMXC_DDR_BASE + 0x001C)
#define TMXC_DDR_MR3             (TMXC_DDR_BASE + 0x0020)
#define TMXC_DDR_STATUS          (TMXC_DDR_BASE + 0x0024)

#define TMXC_TRUSTZONE_NS_OFFSET 0x00000000ULL
#define TMXC_TRUSTZONE_S_OFFSET  0x10000000ULL

#define TMXC_TRUSTZONE_TZPC_CTRL (TMXC_TRUSTZONE_BASE + 0x0000)
#define TMXC_TRUSTZONE_TZPC_R0_SIZE (TMXC_TRUSTZONE_BASE + 0x0004)
#define TMXC_TRUSTZONE_TZPC_R1_SIZE (TMXC_TRUSTZONE_BASE + 0x0008)
#define TMXC_TRUSTZONE_TZPC_R2_SIZE (TMXC_TRUSTZONE_BASE + 0x000C)
#define TMXC_TRUSTZONE_TZPC_R3_SIZE (TMXC_TRUSTZONE_BASE + 0x0010)

#define TMXC_CRYPTO_AES_KEY0     (TMXC_CRYPTO_BASE + 0x0000)
#define TMXC_CRYPTO_AES_KEY1     (TMXC_CRYPTO_BASE + 0x0004)
#define TMXC_CRYPTO_AES_KEY2     (TMXC_CRYPTO_BASE + 0x0008)
#define TMXC_CRYPTO_AES_KEY3     (TMXC_CRYPTO_BASE + 0x000C)
#define TMXC_CRYPTO_AES_IV0       (TMXC_CRYPTO_BASE + 0x0010)
#define TMXC_CRYPTO_AES_IV1       (TMXC_CRYPTO_BASE + 0x0014)
#define TMXC_CRYPTO_AES_CTRL     (TMXC_CRYPTO_BASE + 0x0018)
#define TMXC_CRYPTO_AES_STATUS   (TMXC_CRYPTO_BASE + 0x001C)
#define TMXC_CRYPTO_AES_DATA_IN  (TMXC_CRYPTO_BASE + 0x0020)
#define TMXC_CRYPTO_AES_DATA_OUT (TMXC_CRYPTO_BASE + 0x0024)

#define TMXC_CRYPTO_SHA_CTRL     (TMXC_CRYPTO_BASE + 0x0040)
#define TMXC_CRYPTO_SHA_STATUS   (TMXC_CRYPTO_BASE + 0x0044)
#define TMXC_CRYPTO_SHA_DATA_IN  (TMXC_CRYPTO_BASE + 0x0048)
#define TMXC_CRYPTO_SHA_DIGEST   (TMXC_CRYPTO_BASE + 0x004C)

#define TMXC_CRYPTO_RSA_MODULUS  (TMXC_CRYPTO_BASE + 0x0080)
#define TMXC_CRYPTO_RSA_EXPONENT (TMXC_CRYPTO_BASE + 0x00C0)
#define TMXC_CRYPTO_RSA_DATA_IN  (TMXC_CRYPTO_BASE + 0x0100)
#define TMXC_CRYPTO_RSA_DATA_OUT (TMXC_CRYPTO_BASE + 0x0140)
#define TMXC_CRYPTO_RSA_CTRL     (TMXC_CRYPTO_BASE + 0x0180)
#define TMXC_CRYPTO_RSA_STATUS   (TMXC_CRYPTO_BASE + 0x0184)

#define TMXC_RNG_DATA            (TMXC_RNG_BASE + 0x0000)
#define TMXC_RNG_STATUS          (TMXC_RNG_BASE + 0x0004)
#define TMXC_RNG_CTRL            (TMXC_RNG_BASE + 0x0008)
#define TMXC_RNG_FIFO_COUNT      (TMXC_RNG_BASE + 0x000C)

#define TMXC_TRNG_DATA           (TMXC_TRNG_BASE + 0x0000)
#define TMXC_TRNG_STATUS         (TMXC_TRNG_BASE + 0x0004)
#define TMXC_TRNG_CTRL           (TMXC_TRNG_BASE + 0x0008)
#define TMXC_TRNG_CONFIG         (TMXC_TRNG_BASE + 0x000C)
#define TMXC_TRNG_HEALTH_TEST    (TMXC_TRNG_BASE + 0x0010)

#define TMXC_MMU_PAGE_ATTR_DEVICE    (0x0ULL)
#define TMXC_MMU_PAGE_ATTR_NORMAL    (0x4ULL)
#define TMXC_MMU_PAGE_ATTR_NORMAL_NC (0x44ULL)

#define TMXC_MMU_AP_EL1_RW_EL0_NONE (0x0ULL << 6)
#define TMXC_MMU_AP_EL1_RW_EL0_RO   (0x1ULL << 6)
#define TMXC_MMU_AP_EL1_RO_EL0_NONE (0x2ULL << 6)
#define TMXC_MMU_AP_EL1_RO_EL0_RO   (0x3ULL << 6)

#define TMXC_MMU_SH_NONE    (0x0ULL << 8)
#define TMXC_MMU_SH_OUTER   (0x2ULL << 8)
#define TMXC_MMU_SH_INNER   (0x3ULL << 8)

#define TMXC_MMU_AF         (1ULL << 10)
#define TMXC_MMU_nG         (1ULL << 11)

#define TMXC_MMU_CONTIGUOUS (1ULL << 52)
#define TMXC_MMU_PXN        (1ULL << 53)
#define TMXC_MMU_UXN        (1ULL << 54)

#define TMXC_MMU_PAGE_TABLE_ENTRY_VALID (1ULL << 0)
#define TMXC_MMU_PAGE_TABLE_ENTRY_TABLE (1ULL << 1)
#define TMXC_MMU_PAGE_TABLE_ENTRY_BLOCK (1ULL << 1)

#define TMXC_MMU_LEVEL1_SHIFT 39
#define TMXC_MMU_LEVEL2_SHIFT 30
#define TMXC_MMU_LEVEL3_SHIFT 21
#define TMXC_MMU_LEVEL4_SHIFT 12

#define TMXC_MMU_LEVEL1_SIZE  512
#define TMXC_MMU_LEVEL2_SIZE  512
#define TMXC_MMU_LEVEL3_SIZE  512
#define TMXC_MMU_LEVEL4_SIZE  512

#define TMXC_MMU_LEVEL1_MASK  0x0000FF8000000000ULL
#define TMXC_MMU_LEVEL2_MASK  0x0000007FE0000000ULL
#define TMXC_MMU_LEVEL3_MASK  0x000000001FF00000ULL
#define TMXC_MMU_LEVEL4_MASK  0x00000000000FF000ULL

#define TMXC_EXCEPTION_SYNC_SP0  0
#define TMXC_EXCEPTION_IRQ_SP0   1
#define TMXC_EXCEPTION_FIQ_SP0   2
#define TMXC_EXCEPTION_SERROR_SP0 3
#define TMXC_EXCEPTION_SYNC_SPX  4
#define TMXC_EXCEPTION_IRQ_SPX   5
#define TMXC_EXCEPTION_FIQ_SPX   6
#define TMXC_EXCEPTION_SERROR_SPX 7
#define TMXC_EXCEPTION_SYNC_EL3  8
#define TMXC_EXCEPTION_IRQ_EL3   9
#define TMXC_EXCEPTION_FIQ_EL3   10
#define TMXC_EXCEPTION_SERROR_EL3 11
#define TMXC_EXCEPTION_SYNC_EL1  12
#define TMXC_EXCEPTION_IRQ_EL1   13
#define TMXC_EXCEPTION_FIQ_EL1   14
#define TMXC_EXCEPTION_SERROR_EL1 15

#define TMXC_SPSR_MODE_EL3T 0b0000
#define TMXC_SPSR_MODE_EL3H 0b0101
#define TMXC_SPSR_MODE_EL2T 0b0000
#define TMXC_SPSR_MODE_EL2H 0b0101
#define TMXC_SPSR_MODE_EL1T 0b0000
#define TMXC_SPSR_MODE_EL1H 0b0101
#define TMXC_SPSR_MODE_EL0T 0b0000

#define TMXC_SPSR_A_BIT (1 << 8)
#define TMXC_SPSR_I_BIT (1 << 7)
#define TMXC_SPSR_F_BIT (1 << 6)

#define TMXC_DAIF_IRQ_BIT (1 << 7)
#define TMXC_DAIF_FIQ_BIT (1 << 6)
#define TMXC_DAIF_SERR_BIT (1 << 8)
#define TMXC_DAIF_DEBUG_BIT (1 << 9)

#define TMXC_HCR_VM_BIT (1 << 0)
#define TMXC_HCR_AMO_BIT (1 << 5)
#define TMXC_HCR_IMO_BIT (1 << 4)
#define TMXC_HCR_FMO_BIT (1 << 3)

#define TMXC_SCR_RW_BIT (1 << 0)
#define TMXC_SCR_NS_BIT (1 << 0)
#define TMXC_SCR_IRQ_BIT (1 << 1)
#define TMXC_SCR_FIQ_BIT (1 << 2)
#define TMXC_SCR_EA_BIT (1 << 3)
#define TMXC_SCR_SMD_BIT (1 << 7)
#define TMXC_SCR_HCE_BIT (1 << 8)
#define TMXC_SCR_ST_BIT (1 << 11)
#define TMXC_SCR_TWI_BIT (1 << 12)
#define TMXC_SCR_TWE_BIT (1 << 13)

#define TMXC_SCTLR_M_BIT (1 << 0)
#define TMXC_SCTLR_A_BIT (1 << 1)
#define TMXC_SCTLR_C_BIT (1 << 2)
#define TMXC_SCTLR_SA_BIT (1 << 3)
#define TMXC_SCTLR_I_BIT (1 << 12)
#define TMXC_SCTLR_EE_BIT (1 << 25)
#define TMXC_SCTLR_WXN_BIT (1 << 19)

#define TMXC_TCR_T0SZ_SHIFT 0
#define TMXC_TCR_T0SZ_MASK (0x3FULL << 0)
#define TMXC_TCR_T1SZ_SHIFT 16
#define TMXC_TCR_T1SZ_MASK (0x3FULL << 16)
#define TMXC_TCR_EPD0_BIT (1 << 7)
#define TMXC_TCR_EPD1_BIT (1 << 23)
#define TMXC_TCR_IRGN0_SHIFT 8
#define TMXC_TCR_IRGN0_MASK (0x3ULL << 8)
#define TMXC_TCR_IRGN1_SHIFT 24
#define TMXC_TCR_IRGN1_MASK (0x3ULL << 24)
#define TMXC_TCR_ORGN0_SHIFT 10
#define TMXC_TCR_ORGN0_MASK (0x3ULL << 10)
#define TMXC_TCR_ORGN1_SHIFT 26
#define TMXC_TCR_ORGN1_MASK (0x3ULL << 26)
#define TMXC_TCR_SH0_SHIFT 12
#define TMXC_TCR_SH0_MASK (0x3ULL << 12)
#define TMXC_TCR_SH1_SHIFT 28
#define TMXC_TCR_SH1_MASK (0x3ULL << 28)
#define TMXC_TCR_TG0_SHIFT 14
#define TMXC_TCR_TG0_MASK (0x3ULL << 14)
#define TMXC_TCR_TG1_SHIFT 30
#define TMXC_TCR_TG1_MASK (0x3ULL << 30)
#define TMXC_TCR_IPS_SHIFT 32
#define TMXC_TCR_IPS_MASK (0x7ULL << 32)
#define TMXC_TCR_AS_BIT (1 << 36)
#define TMXC_TCR_TBI0_BIT (1 << 37)
#define TMXC_TCR_TBI1_BIT (1 << 38)

#define TMXC_PROCESS_STATE_READY 0
#define TMXC_PROCESS_STATE_RUNNING 1
#define TMXC_PROCESS_STATE_BLOCKED 2
#define TMXC_PROCESS_STATE_TERMINATED 3
#define TMXC_PROCESS_STATE_ZOMBIE 4

#define TMXC_THREAD_PRIORITY_MIN 0
#define TMXC_THREAD_PRIORITY_MAX 31
#define TMXC_THREAD_PRIORITY_DEFAULT 15

#define TMXC_IPC_MSG_TYPE_DATA 0
#define TMXC_IPC_MSG_TYPE_CONTROL 1
#define TMXC_IPC_MSG_TYPE_SYNC 2
#define TMXC_IPC_MSG_TYPE_ASYNC 3

#define TMXC_IPC_FLAG_ENCRYPTED (1 << 0)
#define TMXC_IPC_FLAG_SIGNED (1 << 1)
#define TMXC_IPC_FLAG_PRIORITY (1 << 2)

#define TMXC_SECURITY_FLAG_ISOLATED (1 << 0)
#define TMXC_SECURITY_FLAG_TRUSTED (1 << 1)
#define TMXC_SECURITY_FLAG_SECURE (1 << 2)

#define TMXC_ENCRYPTION_ALGORITHM_AES256 0
#define TMXC_ENCRYPTION_ALGORITHM_CHACHA20 1
#define TMXC_ENCRYPTION_ALGORITHM_AES256_XTS 2

#define TMXC_FACEID_FEATURE_COUNT 128
#define TMXC_FACEID_TEMPLATE_SIZE 512
#define TMXC_FACEID_THRESHOLD 0.85

#define TMXC_STEALTHVIEW_ANGLE_THRESHOLD 30
#define TMXC_STEALTHVIEW_BLUR_STRENGTH 8
#define TMXC_STEALTHVIEW_DARKEN_FACTOR 0.1

#define TMXC_GRAPHICS_FORMAT_ARGB8888 0
#define TMXC_GRAPHICS_FORMAT_RGB565 1
#define TMXC_GRAPHICS_FORMAT_XRGB8888 2

#define TMXC_GRAPHICS_TRIPLE_BUFFER 3
#define TMXC_GRAPHICS_DOUBLE_BUFFER 2
#define TMXC_GRAPHICS_SINGLE_BUFFER 1

#define TMXC_LOCKSCREEN_MAX_ATTEMPTS 5
#define TMXC_LOCKSCREEN_LOCKOUT_TIME 30000
#define TMXC_LOCKSCREEN_ENCRYPTION_KEY_SIZE 32

#define TMXC_MENU_MAX_ITEMS 50
#define TMXC_MENU_MAX_DEPTH 10

#define TMXC_NETWORK_WIFI 0
#define TMXC_NETWORK_CELLULAR 1
#define TMXC_NETWORK_BLUETOOTH 2

#define TMXC_POWER_STATE_ON 0
#define TMXC_POWER_STATE_SUSPEND 1
#define TMXC_POWER_STATE_HIBERNATE 2
#define TMXC_POWER_STATE_OFF 3

typedef struct {
    uint64_t pgd;
    uint64_t pud;
    uint64_t pmd;
    uint64_t pte;
} tmxc_page_table_t;

typedef struct {
    uint64_t base;
    uint64_t size;
    uint64_t used;
    uint64_t free;
    uint8_t* bitmap;
} tmxc_memory_pool_t;

typedef struct {
    uint32_t pid;
    uint32_t ppid;
    uint8_t state;
    uint8_t priority;
    uint32_t flags;
    uint64_t stack_base;
    uint64_t stack_size;
    uint64_t heap_base;
    uint64_t heap_size;
    uint64_t page_table;
    uint64_t context[32];
} tmxc_process_t;

typedef struct {
    uint32_t tid;
    uint32_t pid;
    uint8_t state;
    uint8_t priority;
    uint64_t stack_base;
    uint64_t stack_size;
    uint64_t context[32];
} tmxc_thread_t;

typedef struct {
    uint32_t src_pid;
    uint32_t dst_pid;
    uint32_t type;
    uint32_t flags;
    uint64_t data;
    uint64_t size;
    uint64_t timestamp;
} tmxc_ipc_message_t;

typedef struct {
    uint32_t irq;
    void (*handler)(uint32_t irq, void* data);
    void* data;
    uint32_t flags;
} tmxc_irq_handler_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t stride;
    uint64_t framebuffer;
    uint32_t buffer_count;
    uint32_t current_buffer;
} tmxc_framebuffer_t;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t pressure;
    uint32_t gesture;
    uint64_t timestamp;
} tmxc_touch_event_t;

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    int16_t mag_x;
    int16_t mag_y;
    int16_t mag_z;
    int16_t temperature;
    uint16_t light;
    uint8_t proximity;
    uint64_t timestamp;
} tmxc_sensor_data_t;

typedef struct {
    float features[TMXC_FACEID_FEATURE_COUNT];
    uint8_t template[TMXC_FACEID_TEMPLATE_SIZE];
    uint32_t user_id;
    uint64_t timestamp;
} tmxc_faceid_template_t;

typedef struct {
    uint32_t angle;
    uint32_t intensity;
    uint32_t mode;
    uint64_t timestamp;
} tmxc_stealthview_state_t;

typedef struct {
    uint8_t encryption_key[TMXC_LOCKSCREEN_ENCRYPTION_KEY_SIZE];
    uint32_t attempts;
    uint64_t lockout_until;
    uint8_t is_locked;
} tmxc_lockscreen_state_t;

typedef struct {
    uint32_t cpu_id;
    uint64_t frequency;
    uint64_t temperature;
    uint64_t usage;
} tmxc_cpu_info_t;

typedef struct {
    uint64_t total;
    uint64_t available;
    uint64_t cached;
    uint64_t compressed;
} tmxc_memory_info_t;

typedef struct {
    uint32_t voltage;
    uint32_t current;
    uint32_t capacity;
    uint32_t temperature;
    uint8_t charging;
    uint8_t health;
} tmxc_battery_info_t;

typedef struct {
    uint8_t addr[6];
    char name[32];
    int8_t rssi;
    uint8_t device_class;
    uint8_t paired;
    uint8_t connected;
} tmxc_bluetooth_device_t;

typedef struct {
    uint8_t card_number[16];
    uint8_t expiry_month;
    uint8_t expiry_year;
    uint8_t cvv[3];
    uint8_t card_holder_name[32];
    uint8_t token[32];
    uint8_t is_virtual;
    uint8_t is_active;
} tmxc_virtual_card_t;

typedef struct {
    uint8_t device_id[16];
    int8_t rssi;
    uint8_t connected;
    uint8_t is_relay;
    uint32_t last_seen;
} tmxc_mesh_node_t;

static inline void tmxc_write64(volatile uint64_t* addr, uint64_t value) {
    *addr = value;
}

static inline uint64_t tmxc_read64(volatile uint64_t* addr) {
    return *addr;
}

static inline void tmxc_write32(volatile uint32_t* addr, uint32_t value) {
    *addr = value;
}

static inline uint32_t tmxc_read32(volatile uint32_t* addr) {
    return *addr;
}

static inline void tmxc_write16(volatile uint16_t* addr, uint16_t value) {
    *addr = value;
}

static inline uint16_t tmxc_read16(volatile uint16_t* addr) {
    return *addr;
}

static inline void tmxc_write8(volatile uint8_t* addr, uint8_t value) {
    *addr = value;
}

static inline uint8_t tmxc_read8(volatile uint8_t* addr) {
    return *addr;
}

static inline void tmxc_memory_barrier(void) {
    __asm__ volatile("dmb sy" ::: "memory");
}

static inline void tmxc_data_sync_barrier(void) {
    __asm__ volatile("dsb sy" ::: "memory");
}

static inline void tmxc_instruction_sync_barrier(void) {
    __asm__ volatile("isb" ::: "memory");
}

static inline void tmxc_wfi(void) {
    __asm__ volatile("wfi");
}

static inline void tmxc_wfe(void) {
    __asm__ volatile("wfe");
}

static inline void tmxc_sev(void) {
    __asm__ volatile("sev");
}

static inline uint64_t tmxc_get_cycle_count(void) {
    uint64_t count;
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(count));
    return count;
}

static inline uint64_t tmxc_get_frequency(void) {
    uint64_t freq;
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;
}

static inline void tmxc_enable_irqs(void) {
    __asm__ volatile("msr daifclr, #2");
}

static inline void tmxc_disable_irqs(void) {
    __asm__ volatile("msr daifset, #2");
}

static inline void tmxc_enable_fiqs(void) {
    __asm__ volatile("msr daifclr, #1");
}

static inline void tmxc_disable_fiqs(void) {
    __asm__ volatile("msr daifset, #1");
}

static inline uint32_t tmxc_get_current_el(void) {
    uint32_t el;
    __asm__ volatile("mrs %0, CurrentEL" : "=r"(el));
    return (el >> 2) & 0x3;
}

static inline void tmxc_dcache_invalidate(void* addr, size_t size) {
    uint64_t start = (uint64_t)addr;
    uint64_t end = start + size;
    
    start &= ~(TMXC_PAGE_SIZE - 1);
    
    for (uint64_t line = start; line < end; line += 64) {
        __asm__ volatile("dc ivac, %0" : : "r"(line) : "memory");
    }
    
    tmxc_data_sync_barrier();
}

static inline void tmxc_dcache_clean(void* addr, size_t size) {
    uint64_t start = (uint64_t)addr;
    uint64_t end = start + size;
    
    start &= ~(TMXC_PAGE_SIZE - 1);
    
    for (uint64_t line = start; line < end; line += 64) {
        __asm__ volatile("dc cvac, %0" : : "r"(line) : "memory");
    }
    
    tmxc_data_sync_barrier();
}

static inline void tmxc_icache_invalidate(void* addr, size_t size) {
    uint64_t start = (uint64_t)addr;
    uint64_t end = start + size;
    
    start &= ~(TMXC_PAGE_SIZE - 1);
    
    for (uint64_t line = start; line < end; line += 64) {
        __asm__ volatile("ic ivau, %0" : : "r"(line) : "memory");
    }
    
    tmxc_instruction_sync_barrier();
}

static inline void tmxc_flush_cache(void* addr, size_t size) {
    tmxc_dcache_clean(addr, size);
    tmxc_icache_invalidate(addr, size);
}

void tmxc_kernel_main(void);
void tmxc_uart_init(void);
void tmxc_uart_putc(char c);
char tmxc_uart_getc(void);
void tmxc_uart_puts(const char* str);
void tmxc_gic_init(void);
void tmxc_gic_enable_irq(uint32_t irq);
void tmxc_gic_disable_irq(uint32_t irq);
void tmxc_timer_init(void);
uint64_t tmxc_timer_get_ms(void);
void tmxc_timer_delay_ms(uint32_t ms);
void tmxc_memory_init(void);
void* tmxc_malloc(size_t size);
void tmxc_free(void* ptr);
void* tmxc_page_alloc(void);
void tmxc_page_free(void* ptr);
void tmxc_mmu_init(void);
void tmxc_mmu_map_page(uint64_t virt, uint64_t phys, uint64_t attributes);
void tmxc_mmu_unmap_page(uint64_t virt);
void tmxc_process_init(void);
uint32_t tmxc_process_create(void (*entry)(void), uint32_t priority);
void tmxc_process_yield(void);
void tmxc_process_exit(void);
void tmxc_irq_init(void);
void tmxc_irq_register(uint32_t irq, void (*handler)(uint32_t, void*), void* data);
void tmxc_irq_unregister(uint32_t irq);
void tmxc_scheduler_init(void);
void tmxc_scheduler_start(void);
void tmxc_scheduler_tick(void);
void tmxc_ipc_init(void);
int tmxc_ipc_send(uint32_t dst_pid, tmxc_ipc_message_t* msg);
int tmxc_ipc_receive(uint32_t src_pid, tmxc_ipc_message_t* msg);
void tmxc_graphics_init(void);
void tmxc_graphics_clear(uint32_t color);
void tmxc_graphics_pixel(uint32_t x, uint32_t y, uint32_t color);
void tmxc_graphics_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void tmxc_graphics_text(uint32_t x, uint32_t y, const char* str, uint32_t color);
void tmxc_graphics_flip(void);
void tmxc_touch_init(void);
int tmxc_touch_read(tmxc_touch_event_t* event);
void tmxc_sensor_init(void);
int tmxc_sensor_read(tmxc_sensor_data_t* data);
void tmxc_security_init(void);
void tmxc_aslr_init(void);
void tmxc_sandbox_init(void);
void tmxc_faceid_init(void);
int tmxc_faceid_enroll(uint32_t user_id);
int tmxc_faceid_verify(uint32_t user_id);
void tmxc_stealthview_init(void);
void tmxc_stealthview_enable(void);
void tmxc_stealthview_disable(void);
void tmxc_stealthview_update(void);
void tmxc_lockscreen_init(void);
int tmxc_lockscreen_unlock(const uint8_t* pin);
void tmxc_lockscreen_lock(void);
void tmxc_menu_init(void);
void tmxc_menu_show(void);
void tmxc_menu_hide(void);
void tmxc_audio_init(void);
void tmxc_audio_play(const uint8_t* data, size_t size);
void tmxc_audio_stop(void);
void tmxc_camera_init(void);
int tmxc_camera_capture(uint8_t* buffer, size_t size);
void tmxc_network_init(void);
int tmxc_network_connect(const char* ssid, const char* password);
void tmxc_power_init(void);
void tmxc_power_shutdown(void);
void tmxc_power_reboot(void);
void tmxc_power_suspend(void);
void tmxc_power_resume(void);
void tmxc_power_monitor_battery(void);
void tmxc_power_set_auto_suspend(uint8_t enable);
uint8_t tmxc_power_get_auto_suspend(void);
void tmxc_power_set_thresholds(uint64_t low_power, uint64_t critical_power);
uint8_t tmxc_power_get_state(void);
uint64_t tmxc_power_get_battery_voltage(void);
uint64_t tmxc_power_get_battery_capacity(void);
uint8_t tmxc_power_is_charging(void);
tmxc_cpu_info_t tmxc_get_cpu_info(void);
tmxc_memory_info_t tmxc_get_memory_info(void);
tmxc_battery_info_t tmxc_get_battery_info(void);
void tmxc_deadlock_detector_init(void);
void tmxc_lock_acquire(uint64_t lock_addr, uint8_t lock_type);
void tmxc_lock_release(uint64_t lock_addr);
void tmxc_deadlock_detect(void);
void tmxc_deadlock_set_timeout(uint64_t timeout_ms);
void tmxc_deadlock_enable(uint8_t enable);
uint32_t tmxc_deadlock_get_lock_count(void);
uint8_t tmxc_deadlock_is_enabled(void);
void tmxc_memory_guard_enable(uint8_t enable);
uint8_t tmxc_memory_guard_is_enabled(void);
uint32_t tmxc_memory_guard_get_corruption_count(void);
void tmxc_memory_guard_reset_corruption_count(void);
void tmxc_shell_init(void);
void tmxc_shell_process_input(char c);
void tmxc_process_kill(uint32_t pid);
void tmxc_modem_init(void);
int tmxc_modem_send_at_command(const char* cmd, char* response);
int tmxc_modem_dial_number(const char* number);
int tmxc_modem_hangup(void);
int tmxc_modem_answer_call(void);
uint8_t tmxc_modem_is_sim_detected(void);
uint8_t tmxc_modem_is_call_active(void);
uint8_t tmxc_modem_get_signal_strength(void);
const char* tmxc_modem_get_operator_name(void);
const char* tmxc_modem_get_imei(void);
const char* tmxc_modem_get_imsi(void);
void tmxc_dialer_init(void);
void tmxc_dialer_show(void);
void tmxc_dialer_hide(void);
void tmxc_dialer_render(void);
void tmxc_dialer_handle_touch(tmxc_touch_event_t* event);
void tmxc_dialer_clear_number(void);
void tmxc_dialer_backspace(void);
const char* tmxc_dialer_get_number(void);
uint8_t tmxc_dialer_is_visible(void);
uint8_t tmxc_dialer_is_call_active(void);
void tmxc_calculator_init(void);
void tmxc_calculator_show(void);
void tmxc_calculator_hide(void);
void tmxc_calculator_render(void);
void tmxc_calculator_handle_touch(tmxc_touch_event_t* event);
void tmxc_calculator_clear(void);
int64_t tmxc_calculator_get_value(void);
uint8_t tmxc_calculator_is_visible(void);
void tmxc_camera_set_zoom(uint32_t zoom_factor);
uint32_t tmxc_camera_get_zoom(void);
void tmxc_camera_enable_zoom(uint8_t enable);
uint8_t tmxc_camera_is_zoom_enabled(void);
void tmxc_compass_init(void);
int32_t tmxc_compass_read_heading(void);
void tmxc_compass_read_raw(int16_t* x, int16_t* y, int16_t* z);
void tmxc_compass_calibrate(void);
void tmxc_compass_set_offsets(int16_t x, int16_t y, int16_t z);
void tmxc_compass_get_offsets(int16_t* x, int16_t* y, int16_t* z);
uint8_t tmxc_compass_is_calibrated(void);
uint8_t tmxc_compass_is_heading_valid(void);
const char* tmxc_compass_get_direction_string(void);
void tmxc_bluetooth_init(void);
int tmxc_bluetooth_start_scan(void);
int tmxc_bluetooth_stop_scan(void);
uint32_t tmxc_bluetooth_get_devices(tmxc_bluetooth_device_t* devices, uint32_t max_count);
int tmxc_bluetooth_pair_device(const char* mac_addr);
int tmxc_bluetooth_connect(const char* mac_addr);
int tmxc_bluetooth_disconnect(void);
int tmxc_bluetooth_send_data(uint8_t* data, uint32_t size);
int tmxc_bluetooth_receive_data(uint8_t* buffer, uint32_t max_size);
uint8_t tmxc_bluetooth_is_initialized(void);
uint8_t tmxc_bluetooth_is_powered(void);
uint8_t tmxc_bluetooth_is_connected(void);
uint8_t tmxc_bluetooth_is_scanning(void);
const char* tmxc_bluetooth_get_local_addr(void);
void tmxc_share_init(void);
int tmxc_share_file(const char* filename);
int tmxc_share_receive_file(const char* filename);
void tmxc_share_set_encryption(uint8_t enable);
uint8_t tmxc_share_is_encryption_enabled(void);
uint8_t tmxc_share_is_transfer_active(void);
uint32_t tmxc_share_get_progress(void);
const char* tmxc_share_get_current_file(void);
void tmxc_nfc_wallet_init(void);
int tmxc_nfc_wallet_add_card(const uint8_t* card_number, uint8_t expiry_month, uint8_t expiry_year,
                               const uint8_t* cvv, const char* card_holder_name);
int tmxc_nfc_wallet_select_card(uint32_t card_index);
int tmxc_nfc_wallet_activate_field(void);
int tmxc_nfc_wallet_deactivate_field(void);
int tmxc_nfc_wallet_send_apdu(const uint8_t* apdu, uint32_t apdu_len, uint8_t* response, uint32_t max_resp_len);
int tmxc_nfc_wallet_process_payment(uint32_t amount_cents);
int tmxc_nfc_wallet_remove_card(uint32_t card_index);
uint32_t tmxc_nfc_wallet_get_card_count(void);
tmxc_virtual_card_t* tmxc_nfc_wallet_get_card(uint32_t card_index);
void tmxc_nfc_wallet_set_encryption(uint8_t enable);
uint8_t tmxc_nfc_wallet_is_encryption_enabled(void);
uint8_t tmxc_nfc_wallet_is_payment_in_progress(void);
void tmxc_promesh_init(void);
int tmxc_promesh_start_scan(void);
int tmxc_promesh_stop_scan(void);
int tmxc_promesh_connect(const uint8_t* device_id);
int tmxc_promesh_disconnect(void);
int tmxc_promesh_send_data(const uint8_t* data, uint32_t size);
int tmxc_promesh_receive_data(uint8_t* buffer, uint32_t max_size);
int tmxc_promesh_start_video_call(void);
int tmxc_promesh_stop_video_call(void);
int tmxc_promesh_send_video_frame(const uint8_t* frame, uint32_t size);
int tmxc_promesh_send_audio_frame(const uint8_t* frame, uint32_t size);
void tmxc_promesh_enable_proximity(uint8_t enable);
void tmxc_promesh_set_proximity_range(uint32_t range_mm);
uint32_t tmxc_promesh_get_node_count(void);
tmxc_mesh_node_t* tmxc_promesh_get_nodes(void);
uint8_t tmxc_promesh_is_connected(void);
uint8_t tmxc_promesh_is_call_active(void);
uint8_t tmxc_promesh_is_proximity_enabled(void);
void tmxc_ui_dynamic_island_show_notification(uint8_t type, const char* text, uint32_t duration_ms);
void tmxc_ui_dynamic_island_hide(void);
void tmxc_ui_dynamic_island_anim(void);
uint8_t tmxc_ui_dynamic_island_is_animating(void);
void tmxc_graphics_optimize_60fps(void);
uint32_t tmxc_graphics_get_fps(void);
void tmxc_power_pro_init(void);
void tmxc_power_pro_check_wireless_charging(void);
int tmxc_power_pro_fast_charge_trigger(void);
void tmxc_power_pro_disable_fast_charge(void);
void tmxc_power_pro_update_battery_percentage(uint32_t percentage);
void tmxc_power_pro_start_charging_animation(void);
void tmxc_power_pro_stop_charging_animation(void);
void tmxc_power_pro_render_charging_animation(void);
void tmxc_power_pro_thermal_protection_check(void);
uint8_t tmxc_power_pro_is_wireless_charging(void);
uint8_t tmxc_power_pro_is_fast_charge_enabled(void);
uint32_t tmxc_power_pro_get_vbus_voltage(void);
uint32_t tmxc_power_pro_get_charge_current(void);
uint32_t tmxc_power_pro_get_wireless_power(void);
uint8_t tmxc_power_pro_is_thermal_protection_active(void);
uint32_t tmxc_power_pro_get_temperature(void);
void tmxc_ecosystem_init(void);
int tmxc_ecosystem_handshake(const uint8_t* peer_device_id);
int tmxc_ecosystem_send_file(const char* filename);
int tmxc_ecosystem_send_number(const char* phone_number);
int tmxc_ecosystem_receive_file(char* filename, uint32_t max_filename_len);
int tmxc_ecosystem_disconnect(void);
void tmxc_ecosystem_heartbeat_check(void);
void tmxc_ecosystem_set_encryption(uint8_t enable);
uint8_t tmxc_ecosystem_is_encryption_enabled(void);
uint8_t tmxc_ecosystem_is_peer_connected(void);
uint8_t tmxc_ecosystem_is_transfer_active(void);
uint32_t tmxc_ecosystem_get_transfer_progress(void);
uint8_t tmxc_ecosystem_is_connection_stable(void);
void tmxc_failsafe_init(void);
void tmxc_failsafe_update_heartbeat(uint8_t service);
void tmxc_failsafe_check_connections(void);
int tmxc_failsafe_attempt_recovery(uint8_t service);
void tmxc_failsafe_reset_fail_count(void);
uint32_t tmxc_failsafe_get_fail_count(void);
uint8_t tmxc_failsafe_is_critical_failure(void);
uint8_t tmxc_failsafe_is_recovery_mode(void);
uint8_t tmxc_failsafe_is_service_healthy(uint8_t service);
void tmxc_security_shield_init(void);
void tmxc_security_shield_enable_port_stealth(void);
void tmxc_security_shield_disable_port_stealth(void);
void tmxc_security_shield_enable_dark_ip(void);
void tmxc_security_shield_disable_dark_ip(void);
uint8_t tmxc_security_shield_analyze_packet(uint8_t* packet, uint32_t size, uint32_t source_ip);
void tmxc_security_shield_drop_packet(void);
void tmxc_security_shield_enable_ddos_protection(void);
void tmxc_security_shield_disable_ddos_protection(void);
void tmxc_security_shield_set_packet_rate_threshold(uint32_t threshold);
void tmxc_security_shield_enter_lockdown(void);
void tmxc_security_shield_exit_lockdown(void);
uint8_t tmxc_security_shield_is_port_stealth_enabled(void);
uint8_t tmxc_security_shield_is_dark_ip_mode(void);
uint8_t tmxc_security_shield_is_ddos_protection_enabled(void);
uint8_t tmxc_security_shield_is_lockdown_mode(void);
uint32_t tmxc_security_shield_get_packets_dropped(void);
uint32_t tmxc_security_shield_get_packets_analyzed(void);
void tmxc_security_shield_handle_leak_detected(uint32_t dest_ip, uint16_t dest_port, uint32_t src_pid);
void tmxc_security_shield_handle_virus_threat(uint64_t addr, uint32_t size, uint32_t owner_pid);
void tmxc_security_shield_enable_leak_response(void);
void tmxc_security_shield_disable_leak_response(void);
void tmxc_security_shield_enable_virus_response(void);
void tmxc_security_shield_disable_virus_response(void);
uint32_t tmxc_security_shield_get_leaks_blocked(void);
uint32_t tmxc_security_shield_get_virus_threats_blocked(void);
uint8_t tmxc_security_shield_is_leak_response_enabled(void);
uint8_t tmxc_security_shield_is_virus_response_enabled(void);
void tmxc_network_init(void);
int tmxc_network_zero_copy_receive(uint8_t** packet_ptr, uint32_t* size);
int tmxc_network_zero_copy_send(const uint8_t* packet, uint32_t size);
int tmxc_network_scrub_packet(uint8_t* packet, uint32_t size, uint32_t source_ip);
void tmxc_network_enable_traffic_scrubbing(void);
void tmxc_network_disable_traffic_scrubbing(void);
void tmxc_network_enable_zero_copy(void);
void tmxc_network_disable_zero_copy(void);
void tmxc_network_set_scrubbing_threshold(uint32_t threshold);
uint8_t tmxc_network_is_traffic_scrubbing_enabled(void);
uint8_t tmxc_network_is_zero_copy_enabled(void);
uint32_t tmxc_network_get_packets_processed(void);
uint32_t tmxc_network_get_packets_scrubbed(void);
void tmxc_5g_uplift_init(void);
void tmxc_5g_uplift_enable(void);
void tmxc_5g_uplift_disable(void);
void tmxc_5g_uplift_scan_bands(void);
void tmxc_5g_uplift_enable_carrier_aggregation(void);
void tmxc_5g_uplift_enable_mimo(void);
void tmxc_5g_uplift_calculate_virtual_speed(void);
void tmxc_5g_uplift_update_signal(void);
uint32_t tmxc_5g_uplift_get_signal_strength(void);
uint32_t tmxc_5g_uplift_get_virtual_speed(void);
uint32_t tmxc_5g_uplift_get_base_speed(void);
uint32_t tmxc_5g_uplift_get_multiplexing_factor(void);
uint8_t tmxc_5g_uplift_is_enabled(void);
uint8_t tmxc_5g_uplift_is_carrier_aggregation_enabled(void);
uint8_t tmxc_5g_uplift_is_mimo_enabled(void);
void tmxc_lockscreen_init(void);
void tmxc_lockscreen_start_animation(void);
void tmxc_lockscreen_stop_animation(void);
void tmxc_lockscreen_render(void);
uint8_t tmxc_lockscreen_is_animating(void);
uint8_t tmxc_lockscreen_is_lock_formed(void);
uint32_t tmxc_lockscreen_get_frame_count(void);
void tmxc_dvfs_init(void);
void tmxc_dvfs_enable(void);
void tmxc_dvfs_disable(void);
void tmxc_dvfs_set_frequency(uint32_t frequency_mhz);
void tmxc_dvfs_set_voltage(uint32_t voltage_mv);
void tmxc_dvfs_update_temperature(void);
void tmxc_dvfs_cool_down(void);
void tmxc_dvfs_set_performance_level(uint32_t level);
void tmxc_dvfs_set_thermal_thresholds(uint32_t high, uint32_t critical);
uint32_t tmxc_dvfs_get_cpu_temperature(void);
uint32_t tmxc_dvfs_get_current_frequency(void);
uint32_t tmxc_dvfs_get_current_voltage(void);
uint8_t tmxc_dvfs_is_thermal_throttling_active(void);
uint32_t tmxc_dvfs_get_performance_level(void);
uint8_t tmxc_dvfs_is_enabled(void);
void tmxc_security_check_tampering(void);
void tmxc_security_enter_lockdown(void);
void tmxc_security_exit_lockdown(void);
void tmxc_security_encrypt_aes_keys(void);
void tmxc_security_verify_bootloader_integrity(void);
uint8_t tmxc_security_is_tampering_detected(void);
uint8_t tmxc_security_is_lockdown_mode(void);
uint8_t tmxc_security_are_aes_keys_encrypted(void);
uint8_t tmxc_security_is_bootloader_verified(void);
uint8_t tmxc_security_get_tamper_sensor_status(uint8_t sensor_index);
void tmxc_game_mode_init(void);
void tmxc_game_mode_show_warning_dialog(void);
uint8_t tmxc_game_mode_request_activation(void);
void tmxc_game_mode_activate(void);
void tmxc_game_mode_deactivate(void);
void tmxc_game_mode_suspend_background_threads(void);
void tmxc_game_mode_resume_background_threads(void);
void tmxc_game_mode_override_dvfs(void);
void tmxc_game_mode_restore_dvfs(void);
void tmxc_game_mode_enable_vsync_bypass(void);
void tmxc_game_mode_disable_vsync_bypass(void);
void tmxc_game_mode_enable_direct_rendering(void);
void tmxc_game_mode_disable_direct_rendering(void);
void tmxc_thermal_monitor(void);
void tmxc_game_mode_enter_safe_mode(void);
void tmxc_game_mode_exit_safe_mode(void);
uint8_t tmxc_game_mode_is_active(void);
uint8_t tmxc_game_mode_is_safe_mode(void);
uint32_t tmxc_game_mode_get_cpu_allocation(void);
uint32_t tmxc_game_mode_get_temperature(void);
uint8_t tmxc_game_mode_is_vsync_bypass_enabled(void);
uint8_t tmxc_game_mode_is_direct_rendering_enabled(void);
void tmxc_graphics_enable_vsync_bypass(uint8_t enable);
uint8_t tmxc_graphics_is_vsync_bypass_enabled(void);

typedef struct {
    uint32_t dest_ip;
    uint16_t dest_port;
    uint32_t src_pid;
    uint64_t timestamp;
    uint32_t packet_size;
    uint8_t is_encrypted;
    uint8_t was_blocked;
    uint8_t leak_type;
} tmxc_leak_log_entry_t;

typedef void (*tmxc_security_callback_leak_detected)(uint32_t dest_ip, uint16_t dest_port, uint32_t src_pid, uint8_t leak_type);
typedef void (*tmxc_security_callback_threat_detected)(uint64_t addr, uint32_t size, uint32_t owner_pid);
typedef void (*tmxc_security_callback_lockdown_triggered)(uint8_t reason);

typedef struct {
    tmxc_security_callback_leak_detected on_leak_detected;
    tmxc_security_callback_threat_detected on_threat_detected;
    tmxc_security_callback_lockdown_triggered on_lockdown_triggered;
    uint8_t callbacks_enabled;
} tmxc_security_callbacks_t;

void tmxc_virus_scanner_init(void);
uint8_t tmxc_virus_scanner_scan_memory(void* addr, uint32_t size);
uint8_t tmxc_virus_scanner_sandbox_execute(const uint8_t* code, uint32_t size, uint32_t owner_pid);
void tmxc_virus_scanner_enable_heuristic(void);
void tmxc_virus_scanner_disable_heuristic(void);
void tmxc_virus_scanner_enable_sandbox(void);
void tmxc_virus_scanner_disable_sandbox(void);
void tmxc_virus_scanner_set_scan_interval(uint64_t interval_ms);
void tmxc_virus_scanner_background_scan(void);
uint8_t tmxc_virus_scanner_is_heuristic_enabled(void);
uint8_t tmxc_virus_scanner_is_sandbox_enabled(void);
uint32_t tmxc_virus_scanner_get_files_scanned(void);
uint32_t tmxc_virus_scanner_get_threats_detected(void);
uint32_t tmxc_virus_scanner_get_sandbox_executions(void);
void tmxc_virus_scanner_reset_stats(void);

void tmxc_leak_detector_init(void);
uint8_t tmxc_leak_detector_inspect_packet(const uint8_t* packet, uint32_t size, uint32_t src_pid, uint32_t dest_ip, uint16_t dest_port);
void tmxc_leak_detector_add_allowed_ip(uint32_t ip);
void tmxc_leak_detector_remove_allowed_ip(uint32_t ip);
void tmxc_leak_detector_add_blocked_port(uint16_t port);
void tmxc_leak_detector_remove_blocked_port(uint16_t port);
void tmxc_leak_detector_enable_monitoring(void);
void tmxc_leak_detector_disable_monitoring(void);
uint8_t tmxc_leak_detector_is_monitoring_enabled(void);
uint32_t tmxc_leak_detector_get_packets_inspected(void);
uint32_t tmxc_leak_detector_get_leaks_detected(void);
uint32_t tmxc_leak_detector_get_leaks_blocked(void);
tmxc_leak_log_entry_t* tmxc_leak_detector_get_leak_log(void);
uint32_t tmxc_leak_detector_get_leak_log_count(void);
void tmxc_leak_detector_clear_leak_log(void);
void tmxc_leak_detector_reset_stats(void);

void tmxc_security_panel_init(void);
void tmxc_security_panel_show(void);
void tmxc_security_panel_hide(void);
uint8_t tmxc_security_panel_is_visible(void);
void tmxc_security_panel_set_tab(uint32_t tab);
void tmxc_security_panel_update_process_info(void);
void tmxc_security_panel_render(void);
void tmxc_security_panel_handle_touch(tmxc_touch_event_t* event);
void tmxc_security_panel_toggle_auto_lockdown(uint8_t enable);
void tmxc_security_panel_set_threat_threshold(uint32_t threshold);
uint8_t tmxc_security_panel_is_auto_lockdown_enabled(void);
uint32_t tmxc_security_panel_get_threat_threshold(void);

void tmxc_security_callbacks_init(void);
void tmxc_security_callbacks_register_leak_handler(tmxc_security_callback_leak_detected handler);
void tmxc_security_callbacks_register_threat_handler(tmxc_security_callback_threat_detected handler);
void tmxc_security_callbacks_register_lockdown_handler(tmxc_security_callback_lockdown_triggered handler);
void tmxc_security_callbacks_enable(uint8_t enable);
uint8_t tmxc_security_callbacks_are_enabled(void);

void tmxc_localization_init(void);
void tmxc_localization_add_language(const char* code, const char* name, const char* native_name, uint8_t rtl);
void tmxc_localization_set_language(const char* code);
tmxc_language_t* tmxc_localization_get_current_language(void);
tmxc_language_t* tmxc_localization_get_language_by_code(const char* code);
tmxc_language_t* tmxc_localization_get_all_languages(void);
uint32_t tmxc_localization_get_language_count(void);
void tmxc_localization_enable_instant_translation(uint8_t enable);
uint8_t tmxc_localization_is_instant_translation_enabled(void);
const char* tmxc_localization_get_string(const char* key);

void tmxc_boot_animation_init(void);
void tmxc_boot_animation_start(void);
void tmxc_boot_animation_stop(void);
uint8_t tmxc_boot_animation_is_active(void);
void tmxc_boot_animation_update(void);
void tmxc_boot_animation_set_duration(uint32_t duration_ms);
uint8_t tmxc_boot_animation_is_complete(void);

void tmxc_setup_wizard_init(void);
void tmxc_setup_wizard_start(void);
void tmxc_setup_wizard_stop(void);
uint8_t tmxc_setup_wizard_is_active(void);
uint8_t tmxc_setup_wizard_is_blocking_mode(void);
uint8_t tmxc_setup_wizard_is_completed(void);
void tmxc_setup_wizard_next_step(void);
void tmxc_setup_wizard_previous_step(void);
void tmxc_setup_wizard_complete_setup(void);
void tmxc_setup_wizard_render(void);
void tmxc_setup_wizard_handle_touch(tmxc_touch_event_t* event);
void tmxc_setup_wizard_set_language(uint32_t index);
void tmxc_setup_wizard_enable_ecosystem_sync(uint8_t enable);
void tmxc_setup_wizard_set_network_mode(uint8_t mode);
uint8_t tmxc_setup_wizard_get_network_mode(void);

void tmxc_device_calibration_init(void);
void tmxc_device_calibration_start(void);
void tmxc_device_calibration_stop(void);
uint8_t tmxc_device_calibration_is_active(void);
void tmxc_device_calibration_calibrate(void);
uint8_t tmxc_device_calibration_is_completed(void);
void tmxc_device_calibration_set_usage_pattern(uint32_t score);
uint32_t tmxc_device_calibration_get_theme_color(void);
uint32_t tmxc_device_calibration_get_screen_brightness(void);
uint32_t tmxc_device_calibration_get_performance_mode(void);
void tmxc_device_calibration_recalibrate(void);

void tmxc_init_process_init(void);
void tmxc_init_process_start(void);
void tmxc_init_process_update(void);
uint8_t tmxc_init_process_is_blocking_mode(void);
uint8_t tmxc_init_process_is_first_boot(void);
uint8_t tmxc_init_process_is_setup_required(void);
uint8_t tmxc_init_process_is_boot_complete(void);
void tmxc_init_process_force_setup(void);
void tmxc_init_process_skip_setup(void);
uint32_t tmxc_init_process_get_boot_stage(void);

void tmxc_crypto_v1000_encrypt_layer(const uint8_t* input, uint8_t* output, uint64_t key, uint32_t layer);
void tmxc_crypto_v1000_decrypt_layer(const uint8_t* input, uint8_t* output, uint64_t key, uint32_t layer);
void tmxc_crypto_v1000_encrypt(const uint8_t* input, uint8_t* output, uint64_t key, uint32_t size);
void tmxc_crypto_v1000_decrypt(const uint8_t* input, uint8_t* output, uint64_t key, uint32_t size);
uint64_t tmxc_crypto_v1000_generate_key(void);

void tmxc_camera_init(void);
void tmxc_camera_open(uint32_t owner_pid);
void tmxc_camera_close(void);
uint8_t* tmxc_camera_capture_frame(void);
uint8_t* tmxc_camera_decrypt_frame(const uint8_t* encrypted_frame);
void tmxc_camera_render(void);
uint32_t tmxc_camera_get_fps(void);
uint8_t tmxc_camera_is_active(void);
uint8_t tmxc_camera_is_hardware_isolated(void);
uint8_t tmxc_camera_is_encrypted(void);
uint32_t tmxc_camera_get_security_level(void);
void tmxc_camera_rotate_encryption_key(void);

void tmxc_gallery_init(void);
uint8_t tmxc_gallery_add_file(const uint8_t* data, uint32_t size);
uint8_t* tmxc_gallery_get_file(uint32_t file_index);
void tmxc_gallery_record_failed_attempt(void);
void tmxc_gallery_reset_failed_attempts(void);
void tmxc_gallery_enable_self_destruct(uint8_t enable);
void tmxc_gallery_enable_stealth_fs(uint8_t enable);
uint32_t tmxc_gallery_get_file_count(void);
uint8_t tmxc_gallery_is_self_destruct_triggered(void);
uint32_t tmxc_gallery_get_security_level(void);
void tmxc_gallery_render(void);
uint32_t tmxc_gallery_get_fps(void);

void tmxc_filemanager_init(void);
uint8_t tmxc_filemanager_add_file(const char* filename, const uint8_t* data, uint32_t size);
uint8_t* tmxc_filemanager_get_file(const char* filename);
uint8_t tmxc_filemanager_delete_file(const char* filename);
void tmxc_filemanager_enable_stealth_fs(uint8_t enable);
uint32_t tmxc_filemanager_get_file_count(void);
uint32_t tmxc_filemanager_get_security_level(void);
void tmxc_filemanager_render(void);
uint32_t tmxc_filemanager_get_fps(void);

void tmxc_kernel_watchdog_init(void);
void tmxc_kernel_watchdog_start(void);
void tmxc_kernel_watchdog_stop(void);
void tmxc_kernel_watchdog_update(void);
void tmxc_kernel_watchdog_reset_counters(void);
void tmxc_kernel_watchdog_exit_dark_mode(void);
uint8_t tmxc_kernel_watchdog_is_active(void);
uint8_t tmxc_kernel_watchdog_is_intrusion_detected(void);
uint8_t tmxc_kernel_watchdog_is_dark_mode(void);
uint8_t tmxc_kernel_watchdog_is_ram_frozen(void);
uint8_t tmxc_kernel_watchdog_is_network_blocked(void);
uint32_t tmxc_kernel_watchdog_get_security_level(void);
void tmxc_kernel_watchdog_set_scan_threshold(uint32_t threshold);
void tmxc_kernel_watchdog_set_access_threshold(uint32_t threshold);

#endif
