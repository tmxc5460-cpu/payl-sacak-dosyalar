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

#define TMXC_HARD_BLACK_BATTERY_THRESHOLD 5
#define TMXC_HARD_BLACK_DURATION_MS (3 * 24 * 60 * 60 * 1000)
#define TMXC_PASSIVE_CHARGING_MAX_GAIN 50

typedef enum {
    TMXC_POWER_MODE_NORMAL = 0,
    TMXC_POWER_MODE_POWER_SAVE = 1,
    TMXC_POWER_MODE_HARD_BLACK = 2,
    TMXC_POWER_MODE_ULTRA_LOW = 3
} tmxc_power_mode_t;

typedef struct {
    uint64_t light_energy_joules;
    uint64_t rf_energy_joules;
    uint64_t thermal_energy_joules;
    uint64_t total_harvested_joules;
    uint8_t harvesting_enabled;
    uint8_t light_sensor_present;
    uint8_t rf_harvester_present;
} tmxc_passive_charging_t;

typedef struct {
    uint8_t gpu_enabled;
    uint8_t cpu_cores_active;
    uint64_t cpu_frequency_hz;
    uint8_t display_enabled;
    uint8_t display_brightness;
    uint8_t background_apps_allowed;
    uint8_t network_enabled;
    uint8_t bluetooth_enabled;
    uint8_t gps_enabled;
    uint8_t sensors_enabled;
} tmxc_power_state_t;

typedef struct {
    tmxc_power_mode_t current_mode;
    tmxc_power_state_t power_state;
    tmxc_passive_charging_t passive_charging;
    uint8_t battery_level;
    uint64_t battery_capacity_mah;
    uint8_t initialized;
    uint64_t hard_black_start_time;
    uint8_t hard_black_active;
    uint8_t emergency_mode_active;
} tmxc_power_management_t;

static tmxc_power_management_t tmxc_power;

void tmxc_power_management_init(void) {
    tmxc_power.initialized = 0;
    tmxc_power.current_mode = TMXC_POWER_MODE_NORMAL;
    tmxc_power.battery_level = 100;
    tmxc_power.battery_capacity_mah = 5000;
    tmxc_power.hard_black_start_time = 0;
    tmxc_power.hard_black_active = 0;
    tmxc_power.emergency_mode_active = 0;
    
    tmxc_power.power_state.gpu_enabled = 1;
    tmxc_power.power_state.cpu_cores_active = TMXC_MAX_CPUS;
    tmxc_power.power_state.cpu_frequency_hz = 2400000000ULL;
    tmxc_power.power_state.display_enabled = 1;
    tmxc_power.power_state.display_brightness = 80;
    tmxc_power.power_state.background_apps_allowed = 1;
    tmxc_power.power_state.network_enabled = 1;
    tmxc_power.power_state.bluetooth_enabled = 1;
    tmxc_power.power_state.gps_enabled = 1;
    tmxc_power.power_state.sensors_enabled = 1;
    
    tmxc_power.passive_charging.light_energy_joules = 0;
    tmxc_power.passive_charging.rf_energy_joules = 0;
    tmxc_power.passive_charging.thermal_energy_joules = 0;
    tmxc_power.passive_charging.total_harvested_joules = 0;
    tmxc_power.passive_charging.harvesting_enabled = 1;
    tmxc_power.passive_charging.light_sensor_present = 1;
    tmxc_power.passive_charging.rf_harvester_present = 1;
    
    tmxc_power.initialized = 1;
    
    tmxc_uart_puts("[POWER] Power management initialized\r\n");
}

void tmxc_power_set_battery_level(uint8_t level) {
    if (!tmxc_power.initialized) {
        return;
    }
    
    tmxc_power.battery_level = level;
    
    if (level <= TMXC_HARD_BLACK_BATTERY_THRESHOLD && !tmxc_power.hard_black_active) {
        tmxc_power_enter_hard_black_mode();
    }
    
    tmxc_uart_puts("[POWER] Battery level: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = level;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("%\r\n");
}

void tmxc_power_enter_hard_black_mode(void) {
    if (!tmxc_power.initialized || tmxc_power.hard_black_active) {
        return;
    }
    
    tmxc_uart_puts("[POWER] Entering Hard-Black mode...\r\n");
    
    tmxc_power.current_mode = TMXC_POWER_MODE_HARD_BLACK;
    tmxc_power.hard_black_active = 1;
    tmxc_power.hard_black_start_time = tmxc_get_cycle_count();
    
    tmxc_power.power_state.gpu_enabled = 0;
    tmxc_power.power_state.cpu_cores_active = 1;
    tmxc_power.power_state.cpu_frequency_hz = 800000000ULL;
    tmxc_power.power_state.display_enabled = 0;
    tmxc_power.power_state.display_brightness = 0;
    tmxc_power.power_state.background_apps_allowed = 0;
    tmxc_power.power_state.network_enabled = 0;
    tmxc_power.power_state.bluetooth_enabled = 0;
    tmxc_power.power_state.gps_enabled = 0;
    tmxc_power.power_state.sensors_enabled = 1;
    
    for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
        if (i == 0) {
            tmxc_cpu_set_frequency(i, 800000000ULL);
        } else {
            tmxc_cpu_set_power_state(i, 1);
        }
    }
    
    tmxc_uart_puts("[POWER] Hard-Black mode active - 3 day emergency mode\r\n");
}

void tmxc_power_exit_hard_black_mode(void) {
    if (!tmxc_power.initialized || !tmxc_power.hard_black_active) {
        return;
    }
    
    tmxc_uart_puts("[POWER] Exiting Hard-Black mode...\r\n");
    
    tmxc_power.current_mode = TMXC_POWER_MODE_NORMAL;
    tmxc_power.hard_black_active = 0;
    
    tmxc_power.power_state.gpu_enabled = 1;
    tmxc_power.power_state.cpu_cores_active = TMXC_MAX_CPUS;
    tmxc_power.power_state.cpu_frequency_hz = 2400000000ULL;
    tmxc_power.power_state.display_enabled = 1;
    tmxc_power.power_state.display_brightness = 80;
    tmxc_power.power_state.background_apps_allowed = 1;
    tmxc_power.power_state.network_enabled = 1;
    tmxc_power.power_state.bluetooth_enabled = 1;
    tmxc_power.power_state.gps_enabled = 1;
    tmxc_power.power_state.sensors_enabled = 1;
    
    for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
        tmxc_cpu_set_frequency(i, 2400000000ULL);
        tmxc_cpu_set_power_state(i, 0);
    }
    
    tmxc_uart_puts("[POWER] Hard-Black mode exited\r\n");
}

void tmxc_power_enable_passive_charging(uint8_t enable) {
    if (!tmxc_power.initialized) {
        return;
    }
    
    tmxc_power.passive_charging.harvesting_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[POWER] Passive charging enabled\r\n");
    } else {
        tmxc_uart_puts("[POWER] Passive charging disabled\r\n");
    }
}

void tmxc_power_harvest_light_energy(uint64_t lux_value) {
    if (!tmxc_power.initialized || !tmxc_power.passive_charging.harvesting_enabled) {
        return;
    }
    
    uint64_t harvested_joules = (lux_value * 10) / 1000;
    if (harvested_joules > TMXC_PASSIVE_CHARGING_MAX_GAIN) {
        harvested_joules = TMXC_PASSIVE_CHARGING_MAX_GAIN;
    }
    
    tmxc_power.passive_charging.light_energy_joules += harvested_joules;
    tmxc_power.passive_charging.total_harvested_joules += harvested_joules;
}

void tmxc_power_harvest_rf_energy(uint64_t signal_strength_dbm) {
    if (!tmxc_power.initialized || !tmxc_power.passive_charging.harvesting_enabled) {
        return;
    }
    
    if (signal_strength_dbm > 0) {
        signal_strength_dbm = 0;
    }
    
    uint64_t harvested_joules = ((-signal_strength_dbm) * 5) / 1000;
    if (harvested_joules > TMXC_PASSIVE_CHARGING_MAX_GAIN / 2) {
        harvested_joules = TMXC_PASSIVE_CHARGING_MAX_GAIN / 2;
    }
    
    tmxc_power.passive_charging.rf_energy_joules += harvested_joules;
    tmxc_power.passive_charging.total_harvested_joules += harvested_joules;
}

void tmxc_power_harvest_thermal_energy(uint64_t temperature_c) {
    if (!tmxc_power.initialized || !tmxc_power.passive_charging.harvesting_enabled) {
        return;
    }
    
    if (temperature_c > 25) {
        uint64_t harvested_joules = ((temperature_c - 25) * 2) / 1000;
        if (harvested_joules > TMXC_PASSIVE_CHARGING_MAX_GAIN / 3) {
            harvested_joules = TMXC_PASSIVE_CHARGING_MAX_GAIN / 3;
        }
        
        tmxc_power.passive_charging.thermal_energy_joules += harvested_joules;
        tmxc_power.passive_charging.total_harvested_joules += harvested_joules;
    }
}

uint64_t tmxc_power_get_harvested_energy(void) {
    if (!tmxc_power.initialized) {
        return 0;
    }
    
    return tmxc_power.passive_charging.total_harvested_joules;
}

void tmxc_power_set_mode(tmxc_power_mode_t mode) {
    if (!tmxc_power.initialized) {
        return;
    }
    
    switch (mode) {
        case TMXC_POWER_MODE_NORMAL:
            tmxc_power_exit_hard_black_mode();
            break;
        case TMXC_POWER_MODE_POWER_SAVE:
            tmxc_power.power_state.cpu_frequency_hz = 1200000000ULL;
            tmxc_power.power_state.display_brightness = 50;
            tmxc_power.power_state.background_apps_allowed = 0;
            for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
                tmxc_cpu_set_frequency(i, 1200000000ULL);
            }
            break;
        case TMXC_POWER_MODE_HARD_BLACK:
            tmxc_power_enter_hard_black_mode();
            break;
        case TMXC_POWER_MODE_ULTRA_LOW:
            tmxc_power.power_state.cpu_cores_active = 1;
            tmxc_power.power_state.cpu_frequency_hz = 600000000ULL;
            tmxc_power.power_state.display_enabled = 0;
            tmxc_power.power_state.network_enabled = 0;
            for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
                if (i == 0) {
                    tmxc_cpu_set_frequency(i, 600000000ULL);
                } else {
                    tmxc_cpu_set_power_state(i, 1);
                }
            }
            break;
    }
    
    tmxc_power.current_mode = mode;
    
    tmxc_uart_puts("[POWER] Power mode set\r\n");
}

tmxc_power_mode_t tmxc_power_get_mode(void) {
    return tmxc_power.current_mode;
}

uint8_t tmxc_power_get_battery_level(void) {
    return tmxc_power.battery_level;
}

uint8_t tmxc_power_is_hard_black_active(void) {
    return tmxc_power.hard_black_active;
}

void tmxc_power_update(void) {
    if (!tmxc_power.initialized) {
        return;
    }
    
    if (tmxc_power.hard_black_active) {
        uint64_t elapsed_ms = (tmxc_get_cycle_count() - tmxc_power.hard_black_start_time) * 1000 / tmxc_get_frequency();
        
        if (elapsed_ms >= TMXC_HARD_BLACK_DURATION_MS) {
            tmxc_uart_puts("[POWER] Hard-Black duration exceeded\r\n");
        }
    }
    
    if (tmxc_power.passive_charging.harvesting_enabled) {
        uint64_t harvested_mah = tmxc_power.passive_charging.total_harvested_joules / 3600;
        
        if (harvested_mah > 0 && tmxc_power.battery_level < 100) {
            tmxc_power.battery_level += (harvested_mah * 100) / tmxc_power.battery_capacity_mah;
            if (tmxc_power.battery_level > 100) {
                tmxc_power.battery_level = 100;
            }
        }
    }
}

void tmxc_power_cleanup(void) {
    if (!tmxc_power.initialized) {
        return;
    }
    
    tmxc_power_exit_hard_black_mode();
    tmxc_power.initialized = 0;
    
    tmxc_uart_puts("[POWER] Power management cleaned up\r\n");
}
