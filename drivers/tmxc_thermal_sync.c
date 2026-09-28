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
#include "tmxc_kernel.h"

#define TMXC_THERMAL_MAX_SENSORS 8
#define TMXC_THERMAL_CHECK_INTERVAL_MS 1000
#define TMXC_THERMAL_CRITICAL_TEMP_C 85
#define TMXC_THERMAL_HIGH_TEMP_C 75
#define TMXC_THERMAL_NORMAL_TEMP_C 45
#define TMXC_THERMAL_EFFICIENCY_MODE_THRESHOLD_C 70

typedef enum {
    TMXC_THERMAL_MODE_PERFORMANCE = 0,
    TMXC_THERMAL_MODE_BALANCED = 1,
    TMXC_THERMAL_MODE_EFFICIENCY = 2,
    TMXC_THERMAL_MODE_CRITICAL = 3
} tmxc_thermal_mode_t;

typedef struct {
    uint32_t sensor_id;
    char sensor_name[32];
    uint64_t base_address;
    int32_t current_temp_c;
    int32_t max_temp_c;
    int32_t threshold_temp_c;
    uint32_t sample_count;
    uint64_t last_update_time;
    uint8_t is_critical;
} tmxc_thermal_sensor_t;

typedef struct {
    uint32_t cpu_id;
    uint64_t current_frequency_hz;
    uint64_t max_frequency_hz;
    uint64_t min_frequency_hz;
    uint64_t efficiency_frequency_hz;
    uint32_t current_voltage_mv;
    uint8_t is_throttled;
    uint8_t is_offline;
} tmxc_cpu_thermal_state_t;

typedef struct {
    tmxc_thermal_mode_t current_mode;
    tmxc_thermal_mode_t target_mode;
    uint64_t mode_switch_time;
    uint32_t mode_switch_count;
    uint64_t total_throttle_time_ms;
    uint32_t throttle_events;
    uint8_t auto_mode_switching;
    uint8_t efficiency_mode_active;
} tmxc_thermal_sync_state_t;

static tmxc_thermal_sensor_t tmxc_thermal_sensors[TMXC_THERMAL_MAX_SENSORS];
static tmxc_cpu_thermal_state_t tmxc_cpu_states[TMXC_MAX_CPUS];
static tmxc_thermal_sync_state_t tmxc_thermal_sync;
static uint8_t tmxc_thermal_sync_enabled = 1;
static uint64_t tmxc_last_thermal_check = 0;

void tmxc_thermal_sync_init(void) {
    for (uint32_t i = 0; i < TMXC_THERMAL_MAX_SENSORS; i++) {
        tmxc_thermal_sensors[i].sensor_id = i;
        tmxc_thermal_sensors[i].sensor_name[0] = '\0';
        tmxc_thermal_sensors[i].base_address = TMXC_SENSOR_BASE + (i * 0x1000);
        tmxc_thermal_sensors[i].current_temp_c = 0;
        tmxc_thermal_sensors[i].max_temp_c = 100;
        tmxc_thermal_sensors[i].threshold_temp_c = TMXC_THERMAL_HIGH_TEMP_C;
        tmxc_thermal_sensors[i].sample_count = 0;
        tmxc_thermal_sensors[i].last_update_time = 0;
        tmxc_thermal_sensors[i].is_critical = 0;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
        tmxc_cpu_states[i].cpu_id = i;
        tmxc_cpu_states[i].current_frequency_hz = 2400000000ULL;
        tmxc_cpu_states[i].max_frequency_hz = 2800000000ULL;
        tmxc_cpu_states[i].min_frequency_hz = 800000000ULL;
        tmxc_cpu_states[i].efficiency_frequency_hz = 1200000000ULL;
        tmxc_cpu_states[i].current_voltage_mv = 1100;
        tmxc_cpu_states[i].is_throttled = 0;
        tmxc_cpu_states[i].is_offline = 0;
    }
    
    tmxc_thermal_sync.current_mode = TMXC_THERMAL_MODE_BALANCED;
    tmxc_thermal_sync.target_mode = TMXC_THERMAL_MODE_BALANCED;
    tmxc_thermal_sync.mode_switch_time = 0;
    tmxc_thermal_sync.mode_switch_count = 0;
    tmxc_thermal_sync.total_throttle_time_ms = 0;
    tmxc_thermal_sync.throttle_events = 0;
    tmxc_thermal_sync.auto_mode_switching = 1;
    tmxc_thermal_sync.efficiency_mode_active = 0;
    
    tmxc_last_thermal_check = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[THERMAL] Thermal Sync driver initialized\r\n");
}

int32_t tmxc_thermal_read_sensor(uint32_t sensor_id) {
    if (sensor_id >= TMXC_THERMAL_MAX_SENSORS) {
        return 0;
    }
    
    uint64_t sensor_addr = tmxc_thermal_sensors[sensor_id].base_address + TMXC_SENSOR_TEMP;
    
    volatile int32_t* temp_reg = (volatile int32_t*)sensor_addr;
    int32_t temp_c = *temp_reg;
    
    tmxc_thermal_sensors[sensor_id].current_temp_c = temp_c;
    tmxc_thermal_sensors[sensor_id].last_update_time = tmxc_get_cycle_count();
    tmxc_thermal_sensors[sensor_id].sample_count++;
    
    if (temp_c >= TMXC_THERMAL_CRITICAL_TEMP_C) {
        tmxc_thermal_sensors[sensor_id].is_critical = 1;
    } else {
        tmxc_thermal_sensors[sensor_id].is_critical = 0;
    }
    
    return temp_c;
}

int32_t tmxc_thermal_get_max_temp(void) {
    int32_t max_temp = -50;
    
    for (uint32_t i = 0; i < TMXC_THERMAL_MAX_SENSORS; i++) {
        if (tmxc_thermal_sensors[i].sample_count > 0) {
            if (tmxc_thermal_sensors[i].current_temp_c > max_temp) {
                max_temp = tmxc_thermal_sensors[i].current_temp_c;
            }
        }
    }
    
    return max_temp;
}

void tmxc_thermal_set_cpu_frequency(uint32_t cpu_id, uint64_t frequency_hz) {
    if (cpu_id >= TMXC_MAX_CPUS) {
        return;
    }
    
    tmxc_cpu_states[cpu_id].current_frequency_hz = frequency_hz;
    
    uint64_t clk_reg = TMXC_CLOCK_CPU_CLK + (cpu_id * 0x100);
    volatile uint64_t* clk_ctrl = (volatile uint64_t*)clk_reg;
    
    *clk_ctrl = frequency_hz;
    
    tmxc_uart_puts("[THERMAL] CPU ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = cpu_id;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" frequency set to ");
    pos = 20;
    buffer[pos] = '\0';
    temp = frequency_hz / 1000000;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" MHz\r\n");
}

void tmxc_thermal_set_cpu_voltage(uint32_t cpu_id, uint32_t voltage_mv) {
    if (cpu_id >= TMXC_MAX_CPUS) {
        return;
    }
    
    tmxc_cpu_states[cpu_id].current_voltage_mv = voltage_mv;
    
    uint64_t pmu_reg = TMXC_PMU_CPU_PWR + (cpu_id * 0x100);
    volatile uint32_t* pmu_ctrl = (volatile uint32_t*)pmu_reg;
    
    *pmu_ctrl = voltage_mv;
}

void tmxc_thermal_switch_mode(tmxc_thermal_mode_t new_mode) {
    if (tmxc_thermal_sync.current_mode == new_mode) {
        return;
    }
    
    tmxc_thermal_sync.target_mode = new_mode;
    tmxc_thermal_sync.mode_switch_time = tmxc_get_cycle_count();
    tmxc_thermal_sync.mode_switch_count++;
    
    switch (new_mode) {
        case TMXC_THERMAL_MODE_PERFORMANCE:
            for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
                if (!tmxc_cpu_states[i].is_offline) {
                    tmxc_thermal_set_cpu_frequency(i, tmxc_cpu_states[i].max_frequency_hz);
                    tmxc_thermal_set_cpu_voltage(i, 1200);
                    tmxc_cpu_states[i].is_throttled = 0;
                }
            }
            tmxc_thermal_sync.efficiency_mode_active = 0;
            break;
            
        case TMXC_THERMAL_MODE_BALANCED:
            for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
                if (!tmxc_cpu_states[i].is_offline) {
                    tmxc_thermal_set_cpu_frequency(i, 2000000000ULL);
                    tmxc_thermal_set_cpu_voltage(i, 1100);
                    tmxc_cpu_states[i].is_throttled = 0;
                }
            }
            tmxc_thermal_sync.efficiency_mode_active = 0;
            break;
            
        case TMXC_THERMAL_MODE_EFFICIENCY:
            for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
                if (!tmxc_cpu_states[i].is_offline) {
                    tmxc_thermal_set_cpu_frequency(i, tmxc_cpu_states[i].efficiency_frequency_hz);
                    tmxc_thermal_set_cpu_voltage(i, 900);
                    tmxc_cpu_states[i].is_throttled = 1;
                }
            }
            tmxc_thermal_sync.efficiency_mode_active = 1;
            break;
            
        case TMXC_THERMAL_MODE_CRITICAL:
            for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
                if (!tmxc_cpu_states[i].is_offline) {
                    tmxc_thermal_set_cpu_frequency(i, tmxc_cpu_states[i].min_frequency_hz);
                    tmxc_thermal_set_cpu_voltage(i, 800);
                    tmxc_cpu_states[i].is_throttled = 1;
                }
            }
            tmxc_thermal_sync.efficiency_mode_active = 1;
            break;
    }
    
    tmxc_thermal_sync.current_mode = new_mode;
    
    tmxc_uart_puts("[THERMAL] Switched to mode ");
    switch (new_mode) {
        case TMXC_THERMAL_MODE_PERFORMANCE:
            tmxc_uart_puts("PERFORMANCE");
            break;
        case TMXC_THERMAL_MODE_BALANCED:
            tmxc_uart_puts("BALANCED");
            break;
        case TMXC_THERMAL_MODE_EFFICIENCY:
            tmxc_uart_puts("EFFICIENCY");
            break;
        case TMXC_THERMAL_MODE_CRITICAL:
            tmxc_uart_puts("CRITICAL");
            break;
    }
    tmxc_uart_puts("\r\n");
}

void tmxc_thermal_sync_background_task(void) {
    if (!tmxc_thermal_sync_enabled) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t frequency = tmxc_get_frequency();
    uint64_t elapsed_ms = ((current_time - tmxc_last_thermal_check) * 1000) / frequency;
    
    if (elapsed_ms < TMXC_THERMAL_CHECK_INTERVAL_MS) {
        return;
    }
    
    tmxc_last_thermal_check = current_time;
    
    for (uint32_t i = 0; i < TMXC_THERMAL_MAX_SENSORS; i++) {
        tmxc_thermal_read_sensor(i);
    }
    
    int32_t max_temp = tmxc_thermal_get_max_temp();
    
    if (tmxc_thermal_sync.auto_mode_switching) {
        if (max_temp >= TMXC_THERMAL_CRITICAL_TEMP_C) {
            if (tmxc_thermal_sync.current_mode != TMXC_THERMAL_MODE_CRITICAL) {
                tmxc_thermal_switch_mode(TMXC_THERMAL_MODE_CRITICAL);
                tmxc_thermal_sync.throttle_events++;
            }
        } else if (max_temp >= TMXC_THERMAL_EFFICIENCY_MODE_THRESHOLD_C) {
            if (tmxc_thermal_sync.current_mode != TMXC_THERMAL_MODE_EFFICIENCY) {
                tmxc_thermal_switch_mode(TMXC_THERMAL_MODE_EFFICIENCY);
                tmxc_thermal_sync.throttle_events++;
            }
        } else if (max_temp >= TMXC_THERMAL_HIGH_TEMP_C) {
            if (tmxc_thermal_sync.current_mode != TMXC_THERMAL_MODE_BALANCED) {
                tmxc_thermal_switch_mode(TMXC_THERMAL_MODE_BALANCED);
            }
        } else if (max_temp <= TMXC_THERMAL_NORMAL_TEMP_C) {
            if (tmxc_thermal_sync.current_mode != TMXC_THERMAL_MODE_PERFORMANCE) {
                tmxc_thermal_switch_mode(TMXC_THERMAL_MODE_PERFORMANCE);
            }
        }
    }
    
    if (tmxc_thermal_sync.efficiency_mode_active) {
        tmxc_thermal_sync.total_throttle_time_ms += elapsed_ms;
    }
}

void tmxc_thermal_sync_enable(uint8_t enable) {
    tmxc_thermal_sync_enabled = enable;
    tmxc_uart_puts("[THERMAL] Thermal Sync ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

void tmxc_thermal_set_auto_mode_switching(uint8_t enable) {
    tmxc_thermal_sync.auto_mode_switching = enable;
    tmxc_uart_puts("[THERMAL] Auto mode switching ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

tmxc_thermal_mode_t tmxc_thermal_get_current_mode(void) {
    return tmxc_thermal_sync.current_mode;
}

tmxc_thermal_sync_state_t* tmxc_thermal_get_sync_state(void) {
    return &tmxc_thermal_sync;
}

tmxc_thermal_sensor_t* tmxc_thermal_get_sensors(uint32_t* count) {
    if (count != NULL) {
        *count = TMXC_THERMAL_MAX_SENSORS;
    }
    return tmxc_thermal_sensors;
}

tmxc_cpu_thermal_state_t* tmxc_thermal_get_cpu_states(uint32_t* count) {
    if (count != NULL) {
        *count = TMXC_MAX_CPUS;
    }
    return tmxc_cpu_states;
}

void tmxc_thermal_sync_cleanup(void) {
    tmxc_thermal_sync.current_mode = TMXC_THERMAL_MODE_BALANCED;
    tmxc_thermal_sync.efficiency_mode_active = 0;
    
    for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
        if (!tmxc_cpu_states[i].is_offline) {
            tmxc_thermal_set_cpu_frequency(i, 2000000000ULL);
            tmxc_thermal_set_cpu_voltage(i, 1100);
            tmxc_cpu_states[i].is_throttled = 0;
        }
    }
    
    tmxc_uart_puts("[THERMAL] Thermal Sync cleaned up\r\n");
}
