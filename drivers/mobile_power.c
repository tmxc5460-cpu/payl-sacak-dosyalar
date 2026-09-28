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
#include "../kernel/tmxc_kernel.h"

typedef struct {
    uint8_t power_state;
    uint64_t battery_voltage;
    uint64_t battery_current;
    uint64_t battery_capacity;
    uint8_t battery_charging;
    uint64_t last_battery_check;
    uint8_t graphics_power;
    uint8_t camera_power;
    uint8_t audio_power;
    uint8_t touchscreen_power;
    uint64_t suspend_time;
    uint8_t auto_suspend_enabled;
    uint64_t low_power_threshold;
    uint64_t critical_power_threshold;
} tmxc_power_state_t;

static tmxc_power_state_t tmxc_power;

extern void tmxc_graphics_power_enable(uint8_t enable);
extern void tmxc_camera_power_enable(uint8_t enable);
extern void tmxc_audio_power_enable(uint8_t enable);
extern void tmxc_touchscreen_power_enable(uint8_t enable);

void tmxc_power_init(void) {
    tmxc_power.power_state = TMXC_POWER_STATE_ON;
    tmxc_power.battery_voltage = 3700;
    tmxc_power.battery_current = 0;
    tmxc_power.battery_capacity = 100;
    tmxc_power.battery_charging = 0;
    tmxc_power.last_battery_check = tmxc_get_cycle_count();
    tmxc_power.graphics_power = 1;
    tmxc_power.camera_power = 0;
    tmxc_power.audio_power = 1;
    tmxc_power.touchscreen_power = 1;
    tmxc_power.suspend_time = 0;
    tmxc_power.auto_suspend_enabled = 1;
    tmxc_power.low_power_threshold = 3000;
    tmxc_power.critical_power_threshold = 2500;
}

void tmxc_power_suspend(void) {
    if (tmxc_power.power_state == TMXC_POWER_STATE_SUSPEND) {
        return;
    }
    
    tmxc_power.power_state = TMXC_POWER_STATE_SUSPEND;
    tmxc_power.suspend_time = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[PWR] Entering suspend mode...\r\n");
    
    tmxc_graphics_power_enable(0);
    tmxc_power.graphics_power = 0;
    
    tmxc_camera_power_enable(0);
    tmxc_power.camera_power = 0;
    
    tmxc_audio_power_enable(0);
    tmxc_power.audio_power = 0;
    
    tmxc_touchscreen_power_enable(1);
    tmxc_power.touchscreen_power = 1;
    
    tmxc_uart_puts("[PWR] Graphics powered down\r\n");
    tmxc_uart_puts("[PWR] Camera powered down\r\n");
    tmxc_uart_puts("[PWR] Audio powered down\r\n");
    tmxc_uart_puts("[PWR] Touchscreen kept active for wake interrupt\r\n");
    
    tmxc_data_sync_barrier();
    tmxc_instruction_sync_barrier();
    
    for (int cpu = 0; cpu < TMXC_MAX_CPUS; cpu++) {
        if (cpu != 0) {
            uint64_t mpidr = cpu << 8;
            __asm__ volatile("msr mpidr_el1, %0" : : "r"(mpidr));
        }
    }
    
    tmxc_uart_puts("[PWR] CPU cores entering low power mode (WFI)\r\n");
}

void tmxc_power_resume(void) {
    if (tmxc_power.power_state != TMXC_POWER_STATE_SUSPEND) {
        return;
    }
    
    tmxc_uart_puts("[PWR] Resuming from suspend mode...\r\n");
    
    tmxc_power.power_state = TMXC_POWER_STATE_ON;
    
    uint64_t suspend_duration = tmxc_get_cycle_count() - tmxc_power.suspend_time;
    uint64_t suspend_ms = suspend_duration / tmxc_get_frequency();
    
    tmxc_uart_puts("[PWR] Suspend duration: ");
    uint64_t suspend_sec = suspend_ms / 1000;
    uint64_t suspend_min = suspend_sec / 60;
    tmxc_uart_puts("0x");
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[17];
    buffer[16] = '\0';
    for (int i = 15; i >= 0; i--) {
        buffer[i] = hex_chars[suspend_min & 0xF];
        suspend_min >>= 4;
    }
    tmxc_uart_puts(buffer);
    tmxc_uart_puts(" minutes\r\n");
    
    tmxc_graphics_power_enable(1);
    tmxc_power.graphics_power = 1;
    
    tmxc_camera_power_enable(0);
    tmxc_power.camera_power = 0;
    
    tmxc_audio_power_enable(1);
    tmxc_power.audio_power = 1;
    
    tmxc_touchscreen_power_enable(1);
    tmxc_power.touchscreen_power = 1;
    
    tmxc_uart_puts("[PWR] Graphics powered up\r\n");
    tmxc_uart_puts("[PWR] Audio powered up\r\n");
    tmxc_uart_puts("[PWR] Touchscreen powered up\r\n");
    
    tmxc_data_sync_barrier();
    tmxc_instruction_sync_barrier();
    
    for (int cpu = 1; cpu < TMXC_MAX_CPUS; cpu++) {
        uint64_t mpidr = cpu << 8;
        __asm__ volatile("msr mpidr_el1, %0" : : "r"(mpidr));
        __asm__ volatile("sev");
    }
    
    tmxc_uart_puts("[PWR] CPU cores resumed\r\n");
    tmxc_uart_puts("[PWR] Resume complete\r\n");
}

void tmxc_power_monitor_battery(void) {
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed_ms = (current_time - tmxc_power.last_battery_check) / tmxc_get_frequency();
    
    if (elapsed_ms < 1000) {
        return;
    }
    
    tmxc_power.last_battery_check = current_time;
    
    uint32_t pmu_status = tmxc_read32((volatile uint32_t*)TMXC_PMU_STATUS);
    
    tmxc_power.battery_voltage = 3500 + ((pmu_status >> 0) & 0xFFF);
    tmxc_power.battery_current = ((pmu_status >> 12) & 0xFFF) * 10;
    tmxc_power.battery_charging = (pmu_status >> 24) & 0x1;
    
    if (tmxc_power.battery_voltage > 4200) {
        tmxc_power.battery_voltage = 4200;
    }
    
    uint64_t voltage_range = 4200 - 3000;
    uint64_t current_range = tmxc_power.battery_voltage - 3000;
    tmxc_power.battery_capacity = (current_range * 100) / voltage_range;
    
    if (tmxc_power.battery_capacity > 100) {
        tmxc_power.battery_capacity = 100;
    }
    
    if (tmxc_power.auto_suspend_enabled) {
        if (tmxc_power.battery_voltage < tmxc_power.critical_power_threshold) {
            tmxc_uart_puts("[PWR] CRITICAL BATTERY: ");
            char buffer[21];
            int pos = 20;
            buffer[pos] = '\0';
            uint64_t temp = tmxc_power.battery_voltage;
            while (temp > 0 && pos > 0) {
                pos--;
                buffer[pos] = '0' + (temp % 10);
                temp /= 10;
            }
            tmxc_uart_puts(&buffer[pos]);
            tmxc_uart_puts("mV - Entering emergency power save\r\n");
            
            tmxc_power_suspend();
        } else if (tmxc_power.battery_voltage < tmxc_power.low_power_threshold) {
            tmxc_uart_puts("[PWR] LOW BATTERY: ");
            char buffer[21];
            int pos = 20;
            buffer[pos] = '\0';
            uint64_t temp = tmxc_power.battery_voltage;
            while (temp > 0 && pos > 0) {
                pos--;
                buffer[pos] = '0' + (temp % 10);
                temp /= 10;
            }
            tmxc_uart_puts(&buffer[pos]);
            tmxc_uart_puts("mV - Reducing CPU frequency\r\n");
            
            for (int cpu = 0; cpu < TMXC_MAX_CPUS; cpu++) {
                uint32_t clk_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_CLOCK_CPU_CLK + cpu * 0x10));
                clk_ctrl = (clk_ctrl & ~0xFF) | 0x0C;
                tmxc_write32((volatile uint32_t*)(TMXC_CLOCK_CPU_CLK + cpu * 0x10), clk_ctrl);
            }
        }
    }
}

void tmxc_power_set_auto_suspend(uint8_t enable) {
    tmxc_power.auto_suspend_enabled = enable;
}

uint8_t tmxc_power_get_auto_suspend(void) {
    return tmxc_power.auto_suspend_enabled;
}

void tmxc_power_set_thresholds(uint64_t low_power, uint64_t critical_power) {
    tmxc_power.low_power_threshold = low_power;
    tmxc_power.critical_power_threshold = critical_power;
}

uint8_t tmxc_power_get_state(void) {
    return tmxc_power.power_state;
}

uint64_t tmxc_power_get_battery_voltage(void) {
    return tmxc_power.battery_voltage;
}

uint64_t tmxc_power_get_battery_capacity(void) {
    return tmxc_power.battery_capacity;
}

uint8_t tmxc_power_is_charging(void) {
    return tmxc_power.battery_charging;
}

void tmxc_power_shutdown(void) {
    tmxc_uart_puts("[PWR] System shutdown initiated\r\n");
    
    tmxc_graphics_power_enable(0);
    tmxc_camera_power_enable(0);
    tmxc_audio_power_enable(0);
    tmxc_touchscreen_power_enable(0);
    
    tmxc_data_sync_barrier();
    tmxc_instruction_sync_barrier();
    
    uint32_t power_ctrl = tmxc_read32((volatile uint32_t*)TMXC_POWER_BASE);
    power_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_POWER_BASE, power_ctrl);
    
    while (1) {
        tmxc_wfi();
    }
}

void tmxc_power_reboot(void) {
    tmxc_uart_puts("[PWR] System reboot initiated\r\n");
    
    tmxc_data_sync_barrier();
    tmxc_instruction_sync_barrier();
    
    uint32_t reset_ctrl = tmxc_read32((volatile uint32_t*)TMXC_RESET_BASE);
    reset_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_RESET_BASE, reset_ctrl);
    
    while (1) {
        tmxc_wfi();
    }
}
