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
#ifndef TMXC_ADVANCED_SENSORS_H
#define TMXC_ADVANCED_SENSORS_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_THERMAL_CAMERA_WIDTH 640
#define TMXC_THERMAL_CAMERA_HEIGHT 480
#define TMXC_LIDAR_MAX_RANGE 10000
#define TMXC_IN_SCREEN_SENSOR_COUNT 8

typedef struct {
    uint16_t temperature_data[TMXC_THERMAL_CAMERA_WIDTH][TMXC_THERMAL_CAMERA_HEIGHT];
    uint16_t min_temperature;
    uint16_t max_temperature;
    uint8_t camera_enabled;
    uint32_t frame_rate;
    uint64_t last_frame_time;
} tmxc_thermal_camera_t;

typedef struct {
    uint16_t depth_map[TMXC_THERMAL_CAMERA_WIDTH][TMXC_THERMAL_CAMERA_HEIGHT];
    uint16_t point_cloud[TMXC_THERMAL_CAMERA_WIDTH * TMXC_THERMAL_CAMERA_HEIGHT][3];
    uint32_t max_range;
    uint8_t lidar_enabled;
    uint32_t scan_rate;
    uint64_t last_scan_time;
} tmxc_lidar_sensor_t;

typedef struct {
    uint8_t heartbeat_bpm;
    uint16_t blood_oxygen;
    uint8_t blood_glucose_simulated;
    uint8_t stress_level;
    uint64_t last_reading_time;
    uint8_t sensor_active;
} tmxc_in_screen_health_sensor_t;

typedef struct {
    uint16_t ambient_light;
    uint16_t proximity;
    uint16_t barometric_pressure;
    uint16_t humidity;
    int16_t accelerometer_x;
    int16_t accelerometer_y;
    int16_t accelerometer_z;
    int16_t gyroscope_x;
    int16_t gyroscope_y;
    int16_t gyroscope_z;
    int16_t magnetometer_x;
    int16_t magnetometer_y;
    int16_t magnetometer_z;
    uint64_t last_update_time;
} tmxc_environmental_sensors_t;

typedef struct {
    tmxc_thermal_camera_t thermal_camera;
    tmxc_lidar_sensor_t lidar;
    tmxc_in_screen_health_sensor_t health_sensors[TMXC_IN_SCREEN_SENSOR_COUNT];
    tmxc_environmental_sensors_t env_sensors;
    uint8_t sensors_initialized;
} tmxc_advanced_sensors_t;

void tmxc_advanced_sensors_init(void);

void tmxc_thermal_camera_enable(uint8_t enable);
void tmxc_thermal_camera_capture_frame(void);
uint16_t tmxc_thermal_get_temperature(uint32_t x, uint32_t y);
uint16_t tmxc_thermal_get_min_temp(void);
uint16_t tmxc_thermal_get_max_temp(void);

void tmxc_lidar_enable(uint8_t enable);
void tmxc_lidar_scan(void);
uint16_t tmxc_lidar_get_depth(uint32_t x, uint32_t y);
uint16_t* tmxc_lidar_get_point_cloud(void);

void tmxc_health_sensors_enable(uint8_t sensor_index, uint8_t enable);
uint8_t tmxc_health_get_heartbeat(uint8_t sensor_index);
uint16_t tmxc_health_get_blood_oxygen(uint8_t sensor_index);
uint8_t tmxc_health_get_stress_level(uint8_t sensor_index);

void tmxc_env_sensors_update(void);
uint16_t tmxc_env_get_light(void);
uint16_t tmxc_env_get_proximity(void);
int16_t tmxc_env_get_accel_x(void);
int16_t tmxc_env_get_accel_y(void);
int16_t tmxc_env_get_accel_z(void);

#endif
