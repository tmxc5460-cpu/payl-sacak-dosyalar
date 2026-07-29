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
#ifndef TMXC_THERMAL_SYNC_H
#define TMXC_THERMAL_SYNC_H

#include "../kernel/tmxc_kernel.h"

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

void tmxc_thermal_sync_init(void);
int32_t tmxc_thermal_read_sensor(uint32_t sensor_id);
int32_t tmxc_thermal_get_max_temp(void);
void tmxc_thermal_set_cpu_frequency(uint32_t cpu_id, uint64_t frequency_hz);
void tmxc_thermal_set_cpu_voltage(uint32_t cpu_id, uint32_t voltage_mv);
void tmxc_thermal_switch_mode(tmxc_thermal_mode_t new_mode);
void tmxc_thermal_sync_background_task(void);
void tmxc_thermal_sync_enable(uint8_t enable);
void tmxc_thermal_set_auto_mode_switching(uint8_t enable);
tmxc_thermal_mode_t tmxc_thermal_get_current_mode(void);
tmxc_thermal_sync_state_t* tmxc_thermal_get_sync_state(void);
tmxc_thermal_sensor_t* tmxc_thermal_get_sensors(uint32_t* count);
tmxc_cpu_thermal_state_t* tmxc_thermal_get_cpu_states(uint32_t* count);
void tmxc_thermal_sync_cleanup(void);

#endif
