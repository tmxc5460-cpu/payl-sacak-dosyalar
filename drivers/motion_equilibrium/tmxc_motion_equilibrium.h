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
#ifndef TMXC_MOTION_EQUILIBRIUM_H
#define TMXC_MOTION_EQUILIBRIUM_H

#include "../kernel/tmxc_kernel.h"

#define TMXC_MOTION_SAMPLE_RATE 100
#define TMXC_MOTION_HISTORY_SIZE 50
#define TMXC_MOTION_THRESHOLD 150
#define TMXC_MOTION_SYNC_FACTOR 80

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
    uint64_t timestamp;
} tmxc_motion_sample_t;

typedef struct {
    tmxc_motion_sample_t history[TMXC_MOTION_HISTORY_SIZE];
    uint32_t history_index;
    int32_t avg_accel_x;
    int32_t avg_accel_y;
    int32_t avg_accel_z;
    int32_t avg_gyro_x;
    int32_t avg_gyro_y;
    int32_t avg_gyro_z;
    int32_t motion_intensity;
    uint8_t in_vehicle;
    uint8_t motion_sync_enabled;
    int32_t screen_offset_x;
    int32_t screen_offset_y;
    uint8_t initialized;
    uint8_t motion_detected;
    uint64_t last_motion_time;
} tmxc_motion_equilibrium_t;

void tmxc_motion_equilibrium_init(void);
void tmxc_motion_equilibrium_enable_motion_sync(uint8_t enable);
uint8_t tmxc_motion_equilibrium_is_motion_sync_enabled(void);

void tmxc_motion_equilibrium_update_accelerometer(int16_t x, int16_t y, int16_t z);
void tmxc_motion_equilibrium_update_gyroscope(int16_t x, int16_t y, int16_t z);

void tmxc_motion_equilibrium_process(void);
void tmxc_motion_equilibrium_apply_screen_offset(int32_t* offset_x, int32_t* offset_y);

uint8_t tmxc_motion_equilibrium_is_in_vehicle(void);
int32_t tmxc_motion_equilibrium_get_motion_intensity(void);
uint8_t tmxc_motion_equilibrium_is_motion_detected(void);

void tmxc_motion_equilibrium_set_threshold(uint32_t threshold);
void tmxc_motion_equilibrium_set_sync_factor(uint32_t factor);

void tmxc_motion_equilibrium_cleanup(void);

#endif
