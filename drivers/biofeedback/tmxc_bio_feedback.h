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
#ifndef TMXC_BIO_FEEDBACK_H
#define TMXC_BIO_FEEDBACK_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_BIO_SENSORS_COUNT 8
#define TMXC_HEART_RATE_NORMAL_MIN 60
#define TMXC_HEART_RATE_NORMAL_MAX 100
#define TMXC_STRESS_THRESHOLD_HIGH 75
#define TMXC_STRESS_THRESHOLD_CRITICAL 90

typedef enum {
    TMXC_CALM_MODE_OFF = 0,
    TMXC_CALM_MODE_LOW = 1,
    TMXC_CALM_MODE_MEDIUM = 2,
    TMXC_CALM_MODE_HIGH = 3
} tmxc_calm_mode_t;

typedef struct {
    uint8_t sensor_id;
    uint16_t vibration_level;
    uint8_t heart_rate_detected;
    uint8_t stress_level;
    uint64_t last_reading_time;
    uint8_t sensor_active;
} tmxc_bio_sensor_t;

typedef struct {
    uint8_t screen_brightness;
    uint8_t notification_volume;
    uint8_t haptic_frequency;
    uint8_t haptic_intensity;
    uint8_t blue_light_filter;
    uint8_t calm_mode_active;
} tmxc_calm_settings_t;

typedef struct {
    tmxc_bio_sensor_t sensors[TMXC_BIO_SENSORS_COUNT];
    tmxc_calm_settings_t calm_settings;
    uint8_t current_heart_rate;
    uint8_t current_stress_level;
    tmxc_calm_mode_t calm_mode;
    uint64_t last_heartbeat_time;
    uint8_t bio_feedback_initialized;
    uint8_t auto_calm_enabled;
} tmxc_bio_feedback_t;

void tmxc_bio_feedback_init(void);
void tmxc_bio_feedback_enable(uint8_t enable);
void tmxc_bio_feedback_enable_auto_calm(uint8_t enable);

void tmxc_bio_sensor_read(uint8_t sensor_id);
uint8_t tmxc_bio_get_heart_rate(void);
uint8_t tmxc_bio_get_stress_level(void);

void tmxc_calm_mode_activate(tmxc_calm_mode_t mode);
void tmxc_calm_mode_deactivate(void);
tmxc_calm_mode_t tmxc_calm_mode_get_current(void);

void tmxc_calm_adjust_screen_brightness(uint8_t brightness);
void tmxc_calm_mute_notifications(void);
void tmxc_calm_enable_haptic_soothing(uint8_t enable);
void tmxc_calm_enable_blue_light_filter(uint8_t enable);

void tmxc_bio_feedback_monitor(void);
uint8_t tmxc_bio_feedback_should_activate_calm(void);

#endif
