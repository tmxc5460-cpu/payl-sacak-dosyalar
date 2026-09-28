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
#include "../../kernel/tmxc_kernel.h"

#define TMXC_BYPASS_THRESHOLD 100
#define TMXC_BYPASS_HYSTERESIS 98

typedef enum {
    TMXC_CHARGING_MODE_NORMAL = 0,
    TMXC_CHARGING_MODE_BYPASS = 1,
    TMXC_CHARGING_MODE_TRICKLE = 2
} tmxc_charging_mode_t;

typedef struct {
    uint8_t battery_level;
    tmxc_charging_mode_t current_mode;
    uint8_t bypass_enabled;
    uint8_t direct_power_active;
    uint64_t bypass_start_time;
    uint64_t total_bypass_time_ms;
    uint8_t initialized;
    uint8_t charging_connected;
} tmxc_smart_charging_t;

static tmxc_smart_charging_t tmxc_charging;

void tmxc_smart_charging_init(void) {
    tmxc_charging.initialized = 0;
    tmxc_charging.battery_level = 0;
    tmxc_charging.current_mode = TMXC_CHARGING_MODE_NORMAL;
    tmxc_charging.bypass_enabled = 1;
    tmxc_charging.direct_power_active = 0;
    tmxc_charging.bypass_start_time = 0;
    tmxc_charging.total_bypass_time_ms = 0;
    tmxc_charging.charging_connected = 0;
    
    tmxc_charging.initialized = 1;
    
    tmxc_uart_puts("[SMART-CHARGING] Smart charging initialized\r\n");
}

void tmxc_smart_charging_set_battery_level(uint8_t level) {
    if (!tmxc_charging.initialized) {
        return;
    }
    
    tmxc_charging.battery_level = level;
    
    if (tmxc_charging.charging_connected && tmxc_charging.bypass_enabled) {
        if (level >= TMXC_BYPASS_THRESHOLD && tmxc_charging.current_mode != TMXC_CHARGING_MODE_BYPASS) {
            tmxc_smart_charging_enable_bypass();
        } else if (level <= TMXC_BYPASS_HYSTERESIS && tmxc_charging.current_mode == TMXC_CHARGING_MODE_BYPASS) {
            tmxc_smart_charging_disable_bypass();
        }
    }
}

void tmxc_smart_charging_enable_bypass(void) {
    if (!tmxc_charging.initialized || !tmxc_charging.bypass_enabled) {
        return;
    }
    
    tmxc_uart_puts("[SMART-CHARGING] Encharging bypass at 100%...\r\n");
    
    tmxc_charging.current_mode = TMXC_CHARGING_MODE_BYPASS;
    tmxc_charging.direct_power_active = 1;
    tmxc_charging.bypass_start_time = tmxc_get_cycle_count();
    
    for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
        tmxc_cpu_set_frequency(i, 3200000000ULL);
    }
    
    tmxc_uart_puts("[SMART-CHARGING] Bypass active - Power routed directly to CPU\r\n");
}

void tmxc_smart_charging_disable_bypass(void) {
    if (!tmxc_charging.initialized) {
        return;
    }
    
    tmxc_uart_puts("[SMART-CHARGING] Disabling bypass...\r\n");
    
    if (tmxc_charging.direct_power_active) {
        uint64_t elapsed_cycles = tmxc_get_cycle_count() - tmxc_charging.bypass_start_time;
        uint64_t elapsed_ms = elapsed_cycles * 1000 / tmxc_get_frequency();
        tmxc_charging.total_bypass_time_ms += elapsed_ms;
    }
    
    tmxc_charging.current_mode = TMXC_CHARGING_MODE_NORMAL;
    tmxc_charging.direct_power_active = 0;
    tmxc_charging.bypass_start_time = 0;
    
    for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
        tmxc_cpu_set_frequency(i, 2400000000ULL);
    }
    
    tmxc_uart_puts("[SMART-CHARGING] Bypass disabled - Normal charging resumed\r\n");
}

void tmxc_smart_charging_set_connected(uint8_t connected) {
    if (!tmxc_charging.initialized) {
        return;
    }
    
    tmxc_charging.charging_connected = connected;
    
    if (!connected && tmxc_charging.current_mode == TMXC_CHARGING_MODE_BYPASS) {
        tmxc_smart_charging_disable_bypass();
    }
}

void tmxc_smart_charging_enable_bypass_feature(uint8_t enable) {
    if (!tmxc_charging.initialized) {
        return;
    }
    
    tmxc_charging.bypass_enabled = enable;
    
    if (!enable && tmxc_charging.current_mode == TMXC_CHARGING_MODE_BYPASS) {
        tmxc_smart_charging_disable_bypass();
    }
    
    if (enable) {
        tmxc_uart_puts("[SMART-CHARGING] Bypass feature enabled\r\n");
    } else {
        tmxc_uart_puts("[SMART-CHARGING] Bypass feature disabled\r\n");
    }
}

tmxc_charging_mode_t tmxc_smart_charging_get_mode(void) {
    return tmxc_charging.current_mode;
}

uint8_t tmxc_smart_charging_is_bypass_active(void) {
    return tmxc_charging.direct_power_active;
}

uint64_t tmxc_smart_charging_get_total_bypass_time(void) {
    return tmxc_charging.total_bypass_time_ms;
}

void tmxc_smart_charging_cleanup(void) {
    if (!tmxc_charging.initialized) {
        return;
    }
    
    tmxc_smart_charging_disable_bypass();
    tmxc_charging.initialized = 0;
    
    tmxc_uart_puts("[SMART-CHARGING] Smart charging cleaned up\r\n");
}
