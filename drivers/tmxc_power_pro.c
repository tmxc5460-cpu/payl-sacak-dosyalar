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
    uint8_t initialized;
    uint8_t wireless_charging_detected;
    uint8_t fast_charge_enabled;
    uint32_t vbus_voltage;
    uint32_t charge_current;
    uint32_t battery_percentage;
    uint8_t charging_animation_active;
    uint32_t animation_frame;
    uint64_t last_animation_update;
    uint8_t qi_compatible;
    uint32_t wireless_power;
    uint8_t thermal_protection_active;
    uint32_t temperature;
} tmxc_power_pro_state_t;

static tmxc_power_pro_state_t tmxc_power_pro;

#define TMXC_POWER_PRO_BASE 0xB0000000
#define TMXC_POWER_PRO_CTRL 0x00
#define TMXC_POWER_PRO_STATUS 0x04
#define TMXC_POWER_PRO_VBUS 0x08
#define TMXC_POWER_PRO_CURRENT 0x0C
#define TMXC_POWER_PRO_TEMP 0x10
#define TMXC_POWER_PRO_IRQ 0x14

#define TMXC_POWER_PRO_CMD_WIRELESS_DETECT 0x01
#define TMXC_POWER_PRO_CMD_FAST_CHARGE 0x02
#define TMXC_POWER_PRO_CMD_THERMAL_PROTECT 0x03

extern void tmxc_graphics_clear(uint32_t color);
extern void tmxc_graphics_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
extern void tmxc_graphics_text(uint32_t x, uint32_t y, const char* str, uint32_t color);
extern void tmxc_graphics_flip(void);
extern uint64_t tmxc_get_cycle_count(void);
extern uint64_t tmxc_get_frequency(void);

void tmxc_power_pro_init(void) {
    tmxc_uart_puts("[POWER-PRO] Initializing advanced power management...\r\n");
    
    tmxc_power_pro.initialized = 0;
    tmxc_power_pro.wireless_charging_detected = 0;
    tmxc_power_pro.fast_charge_enabled = 0;
    tmxc_power_pro.vbus_voltage = 0;
    tmxc_power_pro.charge_current = 0;
    tmxc_power_pro.battery_percentage = 0;
    tmxc_power_pro.charging_animation_active = 0;
    tmxc_power_pro.animation_frame = 0;
    tmxc_power_pro.last_animation_update = 0;
    tmxc_power_pro.qi_compatible = 1;
    tmxc_power_pro.wireless_power = 0;
    tmxc_power_pro.thermal_protection_active = 0;
    tmxc_power_pro.temperature = 25;
    
    uint32_t power_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_CTRL));
    power_ctrl |= TMXC_POWER_PRO_CMD_WIRELESS_DETECT;
    tmxc_write32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_CTRL), power_ctrl);
    
    tmxc_timer_delay_ms(100);
    
    uint32_t power_status = tmxc_read32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_STATUS));
    
    if (power_status & (1 << 0)) {
        tmxc_power_pro.wireless_charging_detected = 1;
        tmxc_uart_puts("[POWER-PRO] Wireless charging detected\r\n");
    }
    
    tmxc_power_pro.initialized = 1;
    tmxc_uart_puts("[POWER-PRO] Advanced power management initialized\r\n");
}

void tmxc_power_pro_check_wireless_charging(void) {
    if (!tmxc_power_pro.initialized) {
        return;
    }
    
    uint32_t power_status = tmxc_read32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_STATUS));
    
    uint8_t wireless_detected = (power_status & (1 << 0)) ? 1 : 0;
    
    if (wireless_detected != tmxc_power_pro.wireless_charging_detected) {
        tmxc_power_pro.wireless_charging_detected = wireless_detected;
        
        if (wireless_detected) {
            tmxc_uart_puts("[POWER-PRO] Wireless charger connected\r\n");
            tmxc_power_pro.wireless_power = tmxc_read32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_STATUS)) >> 8;
        } else {
            tmxc_uart_puts("[POWER-PRO] Wireless charger disconnected\r\n");
            tmxc_power_pro.wireless_power = 0;
        }
    }
    
    if (tmxc_power_pro.wireless_charging_detected) {
        tmxc_power_pro.wireless_power = (tmxc_read32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_STATUS)) >> 8) & 0xFFFF;
    }
}

int tmxc_power_pro_fast_charge_trigger(void) {
    if (!tmxc_power_pro.initialized) {
        return -1;
    }
    
    tmxc_power_pro.vbus_voltage = tmxc_read32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_VBUS));
    
    if (tmxc_power_pro.vbus_voltage < 4500) {
        tmxc_uart_puts("[POWER-PRO] Vbus voltage too low for fast charge\r\n");
        return -2;
    }
    
    if (tmxc_power_pro.vbus_voltage >= 9000) {
        tmxc_uart_puts("[POWER-PRO] High voltage fast charge available\r\n");
        tmxc_power_pro.fast_charge_enabled = 1;
        
        uint32_t power_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_CTRL));
        power_ctrl |= TMXC_POWER_PRO_CMD_FAST_CHARGE;
        tmxc_write32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_CTRL), power_ctrl);
        
        tmxc_power_pro.charge_current = 3000;
        
        return 0;
    } else if (tmxc_power_pro.vbus_voltage >= 5000) {
        tmxc_uart_puts("[POWER-PRO] Standard fast charge available\r\n");
        tmxc_power_pro.fast_charge_enabled = 1;
        
        uint32_t power_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_CTRL));
        power_ctrl |= TMXC_POWER_PRO_CMD_FAST_CHARGE;
        tmxc_write32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_CTRL), power_ctrl);
        
        tmxc_power_pro.charge_current = 2000;
        
        return 0;
    } else {
        tmxc_uart_puts("[POWER-PRO] Standard charging only\r\n");
        tmxc_power_pro.fast_charge_enabled = 0;
        tmxc_power_pro.charge_current = 1000;
        
        return 1;
    }
}

void tmxc_power_pro_disable_fast_charge(void) {
    if (!tmxc_power_pro.initialized) {
        return;
    }
    
    uint32_t power_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_CTRL));
    power_ctrl &= ~TMXC_POWER_PRO_CMD_FAST_CHARGE;
    tmxc_write32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_CTRL), power_ctrl);
    
    tmxc_power_pro.fast_charge_enabled = 0;
    tmxc_power_pro.charge_current = 1000;
    
    tmxc_uart_puts("[POWER-PRO] Fast charge disabled\r\n");
}

void tmxc_power_pro_update_battery_percentage(uint32_t percentage) {
    if (percentage > 100) {
        percentage = 100;
    }
    
    tmxc_power_pro.battery_percentage = percentage;
}

void tmxc_power_pro_start_charging_animation(void) {
    if (!tmxc_power_pro.initialized) {
        return;
    }
    
    tmxc_power_pro.charging_animation_active = 1;
    tmxc_power_pro.animation_frame = 0;
    tmxc_power_pro.last_animation_update = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[POWER-PRO] Charging animation started\r\n");
}

void tmxc_power_pro_stop_charging_animation(void) {
    tmxc_power_pro.charging_animation_active = 0;
    
    tmxc_uart_puts("[POWER-PRO] Charging animation stopped\r\n");
}

void tmxc_power_pro_render_charging_animation(void) {
    if (!tmxc_power_pro.charging_animation_active) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed_cycles = current_time - tmxc_power_pro.last_animation_update;
    uint64_t elapsed_ms = (elapsed_cycles * 1000) / tmxc_get_frequency();
    
    if (elapsed_ms >= 16) {
        tmxc_power_pro.animation_frame++;
        tmxc_power_pro.last_animation_update = current_time;
    }
    
    uint32_t center_x = 1920 / 2;
    uint32_t center_y = 1080 / 2;
    uint32_t radius = 150;
    
    uint32_t percentage = tmxc_power_pro.battery_percentage;
    uint32_t filled_angle = (percentage * 360) / 100;
    
    tmxc_graphics_clear(0xFF000000);
    
    uint32_t bg_color = 0xFF1A1A1A;
    uint32_t fg_color = 0xFF00FF00;
    
    if (percentage < 20) {
        fg_color = 0xFFFF0000;
    } else if (percentage < 50) {
        fg_color = 0xFFFFAA00;
    }
    
    for (int angle = 0; angle < 360; angle++) {
        int32_t x1 = center_x + (int32_t)(radius * 1000 / 1000);
        int32_t y1 = center_y;
        
        int32_t angle_rad = (angle * 314) / 18000;
        int32_t sin_val = angle_rad;
        int32_t cos_val = 1000 - (angle_rad * angle_rad) / 2000;
        
        int32_t x2 = center_x + (radius * cos_val) / 1000;
        int32_t y2 = center_y + (radius * sin_val) / 1000;
        
        uint32_t color = (angle < (int)filled_angle) ? fg_color : bg_color;
        
        tmxc_graphics_rect(x2, y2, 4, 4, color);
    }
    
    char percentage_str[16];
    int pos = 0;
    if (percentage == 100) {
        percentage_str[pos++] = '1';
        percentage_str[pos++] = '0';
        percentage_str[pos++] = '0';
    } else if (percentage >= 10) {
        percentage_str[pos++] = '0' + (percentage / 10);
        percentage_str[pos++] = '0' + (percentage % 10);
    } else {
        percentage_str[pos++] = '0' + percentage;
    }
    percentage_str[pos++] = '%';
    percentage_str[pos] = '\0';
    
    tmxc_graphics_text(center_x - 20, center_y - 8, percentage_str, 0xFFFFFFFF);
    
    if (tmxc_power_pro.fast_charge_enabled) {
        tmxc_graphics_text(center_x - 40, center_y + 40, "HIZLI SARJ", 0xFF00FF00);
    } else if (tmxc_power_pro.wireless_charging_detected) {
        tmxc_graphics_text(center_x - 50, center_y + 40, "KABLOSUZ", 0xFF00FFFF);
    }
    
    if (tmxc_power_pro.wireless_power > 0) {
        char power_str[32];
        int p = 0;
        uint64_t temp = tmxc_power_pro.wireless_power;
        while (temp > 0 && p < 30) {
            power_str[p++] = '0' + (temp % 10);
            temp /= 10;
        }
        for (int i = 0; i < p / 2; i++) {
            char t = power_str[i];
            power_str[i] = power_str[p - 1 - i];
            power_str[p - 1 - i] = t;
        }
        power_str[p++] = 'm';
        power_str[p++] = 'W';
        power_str[p] = '\0';
        
        tmxc_graphics_text(center_x - 30, center_y + 60, power_str, 0xFFAAAAAA);
    }
    
    tmxc_graphics_flip();
}

void tmxc_power_pro_thermal_protection_check(void) {
    if (!tmxc_power_pro.initialized) {
        return;
    }
    
    tmxc_power_pro.temperature = tmxc_read32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_TEMP)) & 0xFFFF;
    
    if (tmxc_power_pro.temperature > 45) {
        if (!tmxc_power_pro.thermal_protection_active) {
            tmxc_power_pro.thermal_protection_active = 1;
            
            if (tmxc_power_pro.fast_charge_enabled) {
                tmxc_power_pro_disable_fast_charge();
            }
            
            uint32_t power_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_CTRL));
            power_ctrl |= TMXC_POWER_PRO_CMD_THERMAL_PROTECT;
            tmxc_write32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_CTRL), power_ctrl);
            
            tmxc_uart_puts("[POWER-PRO] Thermal protection activated\r\n");
        }
        
        tmxc_power_pro.charge_current = 500;
    } else if (tmxc_power_pro.temperature < 40 && tmxc_power_pro.thermal_protection_active) {
        tmxc_power_pro.thermal_protection_active = 0;
        
        uint32_t power_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_CTRL));
        power_ctrl &= ~TMXC_POWER_PRO_CMD_THERMAL_PROTECT;
        tmxc_write32((volatile uint32_t*)(TMXC_POWER_PRO_BASE + TMXC_POWER_PRO_CTRL), power_ctrl);
        
        tmxc_uart_puts("[POWER-PRO] Thermal protection deactivated\r\n");
    }
}

uint8_t tmxc_power_pro_is_wireless_charging(void) {
    return tmxc_power_pro.wireless_charging_detected;
}

uint8_t tmxc_power_pro_is_fast_charge_enabled(void) {
    return tmxc_power_pro.fast_charge_enabled;
}

uint32_t tmxc_power_pro_get_vbus_voltage(void) {
    return tmxc_power_pro.vbus_voltage;
}

uint32_t tmxc_power_pro_get_charge_current(void) {
    return tmxc_power_pro.charge_current;
}

uint32_t tmxc_power_pro_get_wireless_power(void) {
    return tmxc_power_pro.wireless_power;
}

uint8_t tmxc_power_pro_is_thermal_protection_active(void) {
    return tmxc_power_pro.thermal_protection_active;
}

uint32_t tmxc_power_pro_get_temperature(void) {
    return tmxc_power_pro.temperature;
}
