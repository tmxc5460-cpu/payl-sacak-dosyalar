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
#include "tmxc_display_power.h"

static tmxc_display_power_t tmxc_display_power;

void tmxc_display_power_init(void) {
    tmxc_display_power.current_state = TMXC_POWER_STATE_OFF;
    tmxc_display_power.backlight_level = 100;
    tmxc_display_power.voltage_mv = 3300;
    tmxc_display_power.current_ma = 0;
    tmxc_display_power.temperature_c = 25;
    tmxc_display_power.power_initialized = 1;
    tmxc_display_power.auto_dim_enabled = 1;
    tmxc_display_power.last_activity_time = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[DISPLAY-POWER] Display power management initialized\r\n");
}

void tmxc_display_power_set_state(tmxc_power_state_t state) {
    if (!tmxc_display_power.power_initialized) {
        return;
    }
    
    uint32_t power_ctrl = 0;
    
    switch (state) {
        case TMXC_POWER_STATE_OFF:
            power_ctrl = 0x00;
            break;
        case TMXC_POWER_STATE_STANDBY:
            power_ctrl = 0x01;
            break;
        case TMXC_POWER_STATE_ON:
            power_ctrl = 0x03;
            break;
        case TMXC_POWER_STATE_SLEEP:
            power_ctrl = 0x02;
            break;
    }
    
    __asm__ volatile("str %0, [%1]" : : "r"(power_ctrl), "r"(TMXC_POWER_CTRL_REG));
    tmxc_display_power.current_state = state;
    
    tmxc_uart_puts("[DISPLAY-POWER] Power state set to ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = state;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

tmxc_power_state_t tmxc_display_power_get_state(void) {
    return tmxc_display_power.current_state;
}

void tmxc_display_power_set_backlight(uint8_t level) {
    if (!tmxc_display_power.power_initialized) {
        return;
    }
    
    if (level > 100) {
        level = 100;
    }
    
    tmxc_display_power.backlight_level = level;
    
    uint32_t backlight_value = (level * 255) / 100;
    __asm__ volatile("str %0, [%1]" : : "r"(backlight_value), "r"(TMXC_BACKLIGHT_REG));
}

uint8_t tmxc_display_power_get_backlight(void) {
    return tmxc_display_power.backlight_level;
}

void tmxc_display_power_enable_auto_dim(uint8_t enable) {
    tmxc_display_power.auto_dim_enabled = enable;
}

void tmxc_display_power_monitor(void) {
    if (!tmxc_display_power.power_initialized) {
        return;
    }
    
    uint32_t status;
    __asm__ volatile("ldr %0, [%1]" : "=r"(status) : "r"(TMXC_POWER_STATUS_REG));
    
    uint32_t voltage;
    __asm__ volatile("ldr %0, [%1]" : "=r"(voltage) : "r"(TMXC_VOLTAGE_REG));
    tmxc_display_power.voltage_mv = voltage;
    
    uint32_t current;
    __asm__ volatile("ldr %0, [%1]" : "=r"(current) : "r"(TMXC_CURRENT_REG));
    tmxc_display_power.current_ma = current;
    
    uint32_t temp;
    __asm__ volatile("ldr %0, [%1]" : "=r"(temp) : "r"(TMXC_TEMP_REG));
    tmxc_display_power.temperature_c = temp;
    
    if (tmxc_display_power.auto_dim_enabled) {
        uint64_t idle_time = tmxc_get_cycle_count() - tmxc_display_power.last_activity_time;
        uint64_t idle_ms = idle_time * 1000 / tmxc_get_frequency();
        
        if (idle_ms > 30000 && tmxc_display_power.backlight_level > 50) {
            tmxc_display_power_set_backlight(tmxc_display_power.backlight_level - 10);
        } else if (idle_ms > 60000 && tmxc_display_power.backlight_level > 20) {
            tmxc_display_power_set_backlight(tmxc_display_power.backlight_level - 5);
        }
    }
    
    if (tmxc_display_power.temperature_c > 60) {
        tmxc_display_power_set_backlight(tmxc_display_power.backlight_level - 20);
        tmxc_uart_puts("[DISPLAY-POWER] Thermal throttling activated\r\n");
    }
}

uint32_t tmxc_display_power_get_voltage(void) {
    return tmxc_display_power.voltage_mv;
}

uint32_t tmxc_display_power_get_current(void) {
    return tmxc_display_power.current_ma;
}

uint32_t tmxc_display_power_get_temperature(void) {
    return tmxc_display_power.temperature_c;
}
