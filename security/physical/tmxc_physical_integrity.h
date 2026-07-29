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
#ifndef TMXC_PHYSICAL_INTEGRITY_H
#define TMXC_PHYSICAL_INTEGRITY_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_SAFE_ZONE_RADIUS_METERS 10
#define TMXC_MOTION_THRESHOLD 0.5
#define TMXC_TAMPER_THRESHOLD 2.0
#define TMXC_LOCKDOWN_COOLDOWN_MS 30000

typedef enum {
    TMXC_INTEGRITY_STATE_SECURE = 0,
    TMXC_INTEGRITY_STATE_WARNING = 1,
    TMXC_INTEGRITY_STATE_LOCKDOWN = 2,
    TMXC_INTEGRITY_STATE_TAMPER_DETECTED = 3
} tmxc_integrity_state_t;

void tmxc_physical_integrity_init(void);
void tmxc_integrity_add_safe_zone(double latitude, double longitude, double altitude);
void tmxc_integrity_remove_safe_zone(uint32_t index);
void tmxc_integrity_set_baseline_motion(double x_accel, double y_accel, double z_accel, double x_gyro, double y_gyro, double z_gyro);
void tmxc_integrity_update_motion(double x_accel, double y_accel, double z_accel, double x_gyro, double y_gyro, double z_gyro);
void tmxc_integrity_check_location(double current_latitude, double current_longitude, double current_altitude);
void tmxc_integrity_enter_lockdown(void);
uint8_t tmxc_integrity_attempt_unlock(const uint8_t* biometric_data, const uint8_t* pin_data);
void tmxc_integrity_exit_lockdown(void);
void tmxc_integrity_set_biometric_hash(const uint8_t* hash);
void tmxc_integrity_set_pin_hash(const uint8_t* hash);
void tmxc_integrity_enable_monitoring(uint8_t enable);
tmxc_integrity_state_t tmxc_integrity_get_state(void);
uint8_t tmxc_integrity_is_lockdown_active(void);
uint8_t tmxc_integrity_is_device_lifted(void);
uint8_t tmxc_integrity_is_tamper_detected(void);
void tmxc_integrity_reset_motion_detection(void);
void tmxc_physical_integrity_cleanup(void);

#endif
