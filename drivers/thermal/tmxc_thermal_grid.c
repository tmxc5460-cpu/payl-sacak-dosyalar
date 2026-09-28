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

#define TMXC_THERMAL_GRID_SIZE 8
#define TMXC_THERMAL_HOT_THRESHOLD 70
#define TMXC_THERMAL_CRITICAL_THRESHOLD 85
#define TMXC_THERMAL_COLD_THRESHOLD 20

typedef struct {
    double temperature_c;
    uint8_t cpu_core_id;
    uint8_t is_hotspot;
    uint8_t workload;
} tmxc_thermal_cell_t;

typedef struct {
    tmxc_thermal_cell_t grid[TMXC_THERMAL_GRID_SIZE];
    uint8_t initialized;
    uint8_t thermal_management_enabled;
    uint8_t thermal_throttling_active;
    double ambient_temperature;
    uint8_t hottest_cell_index;
    uint8_t coldest_cell_index;
    uint64_t last_update_time;
} tmxc_thermal_grid_t;

static tmxc_thermal_grid_t tmxc_thermal;

void tmxc_thermal_grid_init(void) {
    tmxc_thermal.initialized = 0;
    tmxc_thermal.thermal_management_enabled = 1;
    tmxc_thermal.thermal_throttling_active = 0;
    tmxc_thermal.ambient_temperature = 25.0;
    tmxc_thermal.hottest_cell_index = 0;
    tmxc_thermal.coldest_cell_index = 0;
    tmxc_thermal.last_update_time = tmxc_get_cycle_count();
    
    for (uint32_t i = 0; i < TMXC_THERMAL_GRID_SIZE; i++) {
        tmxc_thermal.grid[i].temperature_c = 35.0;
        tmxc_thermal.grid[i].cpu_core_id = i % TMXC_MAX_CPUS;
        tmxc_thermal.grid[i].is_hotspot = 0;
        tmxc_thermal.grid[i].workload = 0;
    }
    
    tmxc_thermal.initialized = 1;
    
    tmxc_uart_puts("[THERMAL] Thermal grid initialized\r\n");
}

void tmxc_thermal_update_cell(uint8_t cell_index, double temperature) {
    if (!tmxc_thermal.initialized || cell_index >= TMXC_THERMAL_GRID_SIZE) {
        return;
    }
    
    tmxc_thermal.grid[cell_index].temperature_c = temperature;
    
    if (temperature > TMXC_THERMAL_HOT_THRESHOLD) {
        tmxc_thermal.grid[cell_index].is_hotspot = 1;
    } else {
        tmxc_thermal.grid[cell_index].is_hotspot = 0;
    }
    
    tmxc_thermal.last_update_time = tmxc_get_cycle_count();
}

void tmxc_thermal_redistribute_workload(void) {
    if (!tmxc_thermal.initialized || !tmxc_thermal.thermal_management_enabled) {
        return;
    }
    
    double max_temp = 0;
    double min_temp = 100;
    uint8_t hottest = 0;
    uint8_t coldest = 0;
    
    for (uint32_t i = 0; i < TMXC_THERMAL_GRID_SIZE; i++) {
        if (tmxc_thermal.grid[i].temperature_c > max_temp) {
            max_temp = tmxc_thermal.grid[i].temperature_c;
            hottest = i;
        }
        if (tmxc_thermal.grid[i].temperature_c < min_temp) {
            min_temp = tmxc_thermal.grid[i].temperature_c;
            coldest = i;
        }
    }
    
    tmxc_thermal.hottest_cell_index = hottest;
    tmxc_thermal.coldest_cell_index = coldest;
    
    if (max_temp > TMXC_THERMAL_HOT_THRESHOLD && (max_temp - min_temp) > 10) {
        uint8_t hot_core = tmxc_thermal.grid[hottest].cpu_core_id;
        uint8_t cold_core = tmxc_thermal.grid[coldest].cpu_core_id;
        
        uint64_t hot_freq = tmxc_cpu_get_frequency(hot_core);
        uint64_t cold_freq = tmxc_cpu_get_frequency(cold_core);
        
        if (hot_freq > 1200000000ULL && cold_freq < 2800000000ULL) {
            tmxc_cpu_set_frequency(hot_core, hot_freq - 400000000ULL);
            tmxc_cpu_set_frequency(cold_core, cold_freq + 400000000ULL);
            
            tmxc_uart_puts("[THERMAL] Workload redistributed from hot to cold zone\r\n");
        }
    }
}

void tmxc_thermal_enable_throttling(void) {
    if (!tmxc_thermal.initialized) {
        return;
    }
    
    tmxc_thermal.thermal_throttling_active = 1;
    
    for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
        tmxc_cpu_set_frequency(i, 1200000000ULL);
    }
    
    tmxc_uart_puts("[THERMAL] Thermal throttling enabled\r\n");
}

void tmxc_thermal_disable_throttling(void) {
    if (!tmxc_thermal.initialized) {
        return;
    }
    
    tmxc_thermal.thermal_throttling_active = 0;
    
    for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
        tmxc_cpu_set_frequency(i, 2400000000ULL);
    }
    
    tmxc_thermal.last_update_time = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[THERMAL] Thermal throttling disabled\r\n");
}

void tmxc_thermal_update(void) {
    if (!tmxc_thermal.initialized) {
        return;
    }
    
    double max_temp = 0;
    for (uint32_t i = 0; i < TMXC_THERMAL_GRID_SIZE; i++) {
        if (tmxc_thermal.grid[i].temperature_c > max_temp) {
            max_temp = tmxc_thermal.grid[i].temperature_c;
        }
    }
    
    if (max_temp > TMXC_THERMAL_CRITICAL_THRESHOLD && !tmxc_thermal.thermal_throttling_active) {
        tmxc_thermal_enable_throttling();
    } else if (max_temp < TMXC_THERMAL_HOT_THRESHOLD && tmxc_thermal.thermal_throttling_active) {
        tmxc_thermal_disable_throttling();
    }
    
    tmxc_thermal_redistribute_workload();
}

double tmxc_thermal_get_max_temperature(void) {
    if (!tmxc_thermal.initialized) {
        return 0;
    }
    
    double max_temp = 0;
    for (uint32_t i = 0; i < TMXC_THERMAL_GRID_SIZE; i++) {
        if (tmxc_thermal.grid[i].temperature_c > max_temp) {
            max_temp = tmxc_thermal.grid[i].temperature_c;
        }
    }
    
    return max_temp;
}

uint8_t tmxc_thermal_is_throttling(void) {
    return tmxc_thermal.thermal_throttling_active;
}

void tmxc_thermal_cleanup(void) {
    if (!tmxc_thermal.initialized) {
        return;
    }
    
    tmxc_thermal_disable_throttling();
    tmxc_thermal.thermal_management_enabled = 0;
    tmxc_thermal.initialized = 0;
    
    tmxc_uart_puts("[THERMAL] Thermal grid cleaned up\r\n");
}
