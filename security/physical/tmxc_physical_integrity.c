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

typedef struct {
    double latitude;
    double longitude;
    double altitude;
    uint8_t is_active;
    uint64_t created_time;
} tmxc_safe_zone_t;

typedef struct {
    double x_acceleration;
    double y_acceleration;
    double z_acceleration;
    double x_gyro;
    double y_gyro;
    double z_gyro;
    uint64_t timestamp;
} tmxc_motion_data_t;

typedef struct {
    uint8_t biometric_hash[32];
    uint8_t pin_hash[16];
    uint8_t unlock_attempts;
    uint64_t last_attempt_time;
    uint8_t locked;
} tmxc_auth_data_t;

typedef struct {
    tmxc_integrity_state_t current_state;
    tmxc_safe_zone_t safe_zones[5];
    uint32_t safe_zone_count;
    tmxc_motion_data_t current_motion;
    tmxc_motion_data_t baseline_motion;
    tmxc_auth_data_t auth_data;
    uint8_t initialized;
    uint8_t monitoring_enabled;
    uint8_t lockdown_active;
    uint64_t lockdown_start_time;
    uint8_t motion_detected;
    uint64_t last_motion_time;
    uint8_t tamper_detected;
    uint64_t last_tamper_time;
    uint8_t device_lifted;
    uint8_t device_moved;
} tmxc_physical_integrity_t;

static tmxc_physical_integrity_t tmxc_integrity;

void tmxc_physical_integrity_init(void) {
    tmxc_integrity.initialized = 0;
    tmxc_integrity.current_state = TMXC_INTEGRITY_STATE_SECURE;
    tmxc_integrity.safe_zone_count = 0;
    tmxc_integrity.monitoring_enabled = 1;
    tmxc_integrity.lockdown_active = 0;
    tmxc_integrity.lockdown_start_time = 0;
    tmxc_integrity.motion_detected = 0;
    tmxc_integrity.last_motion_time = 0;
    tmxc_integrity.tamper_detected = 0;
    tmxc_integrity.last_tamper_time = 0;
    tmxc_integrity.device_lifted = 0;
    tmxc_integrity.device_moved = 0;
    
    for (uint32_t i = 0; i < 5; i++) {
        tmxc_integrity.safe_zones[i].latitude = 0;
        tmxc_integrity.safe_zones[i].longitude = 0;
        tmxc_integrity.safe_zones[i].altitude = 0;
        tmxc_integrity.safe_zones[i].is_active = 0;
        tmxc_integrity.safe_zones[i].created_time = 0;
    }
    
    tmxc_integrity.current_motion.x_acceleration = 0;
    tmxc_integrity.current_motion.y_acceleration = 0;
    tmxc_integrity.current_motion.z_acceleration = 0;
    tmxc_integrity.current_motion.x_gyro = 0;
    tmxc_integrity.current_motion.y_gyro = 0;
    tmxc_integrity.current_motion.z_gyro = 0;
    tmxc_integrity.current_motion.timestamp = 0;
    
    tmxc_integrity.baseline_motion.x_acceleration = 0;
    tmxc_integrity.baseline_motion.y_acceleration = 0;
    tmxc_integrity.baseline_motion.z_acceleration = 0;
    tmxc_integrity.baseline_motion.x_gyro = 0;
    tmxc_integrity.baseline_motion.y_gyro = 0;
    tmxc_integrity.baseline_motion.z_gyro = 0;
    tmxc_integrity.baseline_motion.timestamp = 0;
    
    for (int i = 0; i < 32; i++) {
        tmxc_integrity.auth_data.biometric_hash[i] = 0;
    }
    for (int i = 0; i < 16; i++) {
        tmxc_integrity.auth_data.pin_hash[i] = 0;
    }
    tmxc_integrity.auth_data.unlock_attempts = 0;
    tmxc_integrity.auth_data.last_attempt_time = 0;
    tmxc_integrity.auth_data.locked = 0;
    
    tmxc_integrity.initialized = 1;
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] Physical integrity system initialized\r\n");
}

void tmxc_integrity_add_safe_zone(double latitude, double longitude, double altitude) {
    if (!tmxc_integrity.initialized || tmxc_integrity.safe_zone_count >= 5) {
        return;
    }
    
    uint32_t index = tmxc_integrity.safe_zone_count;
    
    tmxc_integrity.safe_zones[index].latitude = latitude;
    tmxc_integrity.safe_zones[index].longitude = longitude;
    tmxc_integrity.safe_zones[index].altitude = altitude;
    tmxc_integrity.safe_zones[index].is_active = 1;
    tmxc_integrity.safe_zones[index].created_time = tmxc_get_cycle_count();
    
    tmxc_integrity.safe_zone_count++;
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] Safe zone added\r\n");
}

void tmxc_integrity_remove_safe_zone(uint32_t index) {
    if (!tmxc_integrity.initialized || index >= tmxc_integrity.safe_zone_count) {
        return;
    }
    
    tmxc_integrity.safe_zones[index].is_active = 0;
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] Safe zone removed\r\n");
}

void tmxc_integrity_set_baseline_motion(double x_accel, double y_accel, double z_accel, double x_gyro, double y_gyro, double z_gyro) {
    if (!tmxc_integrity.initialized) {
        return;
    }
    
    tmxc_integrity.baseline_motion.x_acceleration = x_accel;
    tmxc_integrity.baseline_motion.y_acceleration = y_accel;
    tmxc_integrity.baseline_motion.z_acceleration = z_accel;
    tmxc_integrity.baseline_motion.x_gyro = x_gyro;
    tmxc_integrity.baseline_motion.y_gyro = y_gyro;
    tmxc_integrity.baseline_motion.z_gyro = z_gyro;
    tmxc_integrity.baseline_motion.timestamp = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] Baseline motion set\r\n");
}

void tmxc_integrity_update_motion(double x_accel, double y_accel, double z_accel, double x_gyro, double y_gyro, double z_gyro) {
    if (!tmxc_integrity.initialized || !tmxc_integrity.monitoring_enabled) {
        return;
    }
    
    tmxc_integrity.current_motion.x_acceleration = x_accel;
    tmxc_integrity.current_motion.y_acceleration = y_accel;
    tmxc_integrity.current_motion.z_acceleration = z_accel;
    tmxc_integrity.current_motion.x_gyro = x_gyro;
    tmxc_integrity.current_motion.y_gyro = y_gyro;
    tmxc_integrity.current_motion.z_gyro = z_gyro;
    tmxc_integrity.current_motion.timestamp = tmxc_get_cycle_count();
    
    double accel_magnitude = (x_accel * x_accel) + (y_accel * y_accel) + (z_accel * z_accel);
    double baseline_accel = (tmxc_integrity.baseline_motion.x_acceleration * tmxc_integrity.baseline_motion.x_acceleration) +
                            (tmxc_integrity.baseline_motion.y_acceleration * tmxc_integrity.baseline_motion.y_acceleration) +
                            (tmxc_integrity.baseline_motion.z_acceleration * tmxc_integrity.baseline_motion.z_acceleration);
    
    double accel_diff = accel_magnitude - baseline_accel;
    
    if (accel_diff > TMXC_TAMPER_THRESHOLD) {
        tmxc_integrity.tamper_detected = 1;
        tmxc_integrity.last_tamper_time = tmxc_get_cycle_count();
        tmxc_integrity.current_state = TMXC_INTEGRITY_STATE_TAMPER_DETECTED;
        
        tmxc_uart_puts("[PHYSICAL-INTEGRITY] Tamper detected!\r\n");
    } else if (accel_diff > TMXC_MOTION_THRESHOLD) {
        tmxc_integrity.motion_detected = 1;
        tmxc_integrity.last_motion_time = tmxc_get_cycle_count();
        tmxc_integrity.device_moved = 1;
        
        if (!tmxc_integrity.device_lifted) {
            tmxc_integrity.device_lifted = 1;
            tmxc_uart_puts("[PHYSICAL-INTEGRITY] Device lifted\r\n");
        }
    }
}

void tmxc_integrity_check_location(double current_latitude, double current_longitude, double current_altitude) {
    if (!tmxc_integrity.initialized || !tmxc_integrity.monitoring_enabled) {
        return;
    }
    
    if (tmxc_integrity.safe_zone_count == 0) {
        return;
    }
    
    uint8_t in_safe_zone = 0;
    
    for (uint32_t i = 0; i < tmxc_integrity.safe_zone_count; i++) {
        if (!tmxc_integrity.safe_zones[i].is_active) {
            continue;
        }
        
        double lat_diff = current_latitude - tmxc_integrity.safe_zones[i].latitude;
        double lon_diff = current_longitude - tmxc_integrity.safe_zones[i].longitude;
        double alt_diff = current_altitude - tmxc_integrity.safe_zones[i].altitude;
        
        double distance = (lat_diff * lat_diff) + (lon_diff * lon_diff) + (alt_diff * alt_diff);
        distance = distance * 111320;
        
        if (distance < TMXC_SAFE_ZONE_RADIUS_METERS) {
            in_safe_zone = 1;
            break;
        }
    }
    
    if (!in_safe_zone && tmxc_integrity.device_lifted) {
        tmxc_integrity_enter_lockdown();
    }
}

void tmxc_integrity_enter_lockdown(void) {
    if (!tmxc_integrity.initialized || tmxc_integrity.lockdown_active) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed_ms = (current_time - tmxc_integrity.lockdown_start_time) * 1000 / tmxc_get_frequency();
    
    if (elapsed_ms < TMXC_LOCKDOWN_COOLDOWN_MS && tmxc_integrity.lockdown_start_time != 0) {
        return;
    }
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] Entering LOCKDOWN mode...\r\n");
    
    tmxc_integrity.current_state = TMXC_INTEGRITY_STATE_LOCKDOWN;
    tmxc_integrity.lockdown_active = 1;
    tmxc_integrity.lockdown_start_time = current_time;
    tmxc_integrity.auth_data.locked = 1;
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] Device locked - Biometric authentication required\r\n");
}

uint8_t tmxc_integrity_attempt_unlock(const uint8_t* biometric_data, const uint8_t* pin_data) {
    if (!tmxc_integrity.initialized || !tmxc_integrity.lockdown_active) {
        return 0;
    }
    
    tmxc_integrity.auth_data.unlock_attempts++;
    tmxc_integrity.auth_data.last_attempt_time = tmxc_get_cycle_count();
    
    if (biometric_data != NULL) {
        uint8_t match = 1;
        for (int i = 0; i < 32; i++) {
            if (tmxc_integrity.auth_data.biometric_hash[i] != biometric_data[i]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            tmxc_integrity_exit_lockdown();
            return 1;
        }
    }
    
    if (pin_data != NULL) {
        uint8_t match = 1;
        for (int i = 0; i < 16; i++) {
            if (tmxc_integrity.auth_data.pin_hash[i] != pin_data[i]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            tmxc_integrity_exit_lockdown();
            return 1;
        }
    }
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] Unlock attempt failed\r\n");
    
    return 0;
}

void tmxc_integrity_exit_lockdown(void) {
    if (!tmxc_integrity.initialized || !tmxc_integrity.lockdown_active) {
        return;
    }
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] Exiting LOCKDOWN mode...\r\n");
    
    tmxc_integrity.current_state = TMXC_INTEGRITY_STATE_SECURE;
    tmxc_integrity.lockdown_active = 0;
    tmxc_integrity.lockdown_start_time = 0;
    tmxc_integrity.auth_data.locked = 0;
    tmxc_integrity.auth_data.unlock_attempts = 0;
    tmxc_integrity.device_lifted = 0;
    tmxc_integrity.device_moved = 0;
    tmxc_integrity.motion_detected = 0;
    tmxc_integrity.tamper_detected = 0;
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] Device unlocked\r\n");
}

void tmxc_integrity_set_biometric_hash(const uint8_t* hash) {
    if (!tmxc_integrity.initialized || hash == NULL) {
        return;
    }
    
    for (int i = 0; i < 32; i++) {
        tmxc_integrity.auth_data.biometric_hash[i] = hash[i];
    }
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] Biometric hash set\r\n");
}

void tmxc_integrity_set_pin_hash(const uint8_t* hash) {
    if (!tmxc_integrity.initialized || hash == NULL) {
        return;
    }
    
    for (int i = 0; i < 16; i++) {
        tmxc_integrity.auth_data.pin_hash[i] = hash[i];
    }
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] PIN hash set\r\n");
}

void tmxc_integrity_enable_monitoring(uint8_t enable) {
    if (!tmxc_integrity.initialized) {
        return;
    }
    
    tmxc_integrity.monitoring_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[PHYSICAL-INTEGRITY] Monitoring enabled\r\n");
    } else {
        tmxc_uart_puts("[PHYSICAL-INTEGRITY] Monitoring disabled\r\n");
    }
}

tmxc_integrity_state_t tmxc_integrity_get_state(void) {
    return tmxc_integrity.current_state;
}

uint8_t tmxc_integrity_is_lockdown_active(void) {
    return tmxc_integrity.lockdown_active;
}

uint8_t tmxc_integrity_is_device_lifted(void) {
    return tmxc_integrity.device_lifted;
}

uint8_t tmxc_integrity_is_tamper_detected(void) {
    return tmxc_integrity.tamper_detected;
}

void tmxc_integrity_reset_motion_detection(void) {
    if (!tmxc_integrity.initialized) {
        return;
    }
    
    tmxc_integrity.motion_detected = 0;
    tmxc_integrity.device_moved = 0;
    tmxc_integrity.tamper_detected = 0;
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] Motion detection reset\r\n");
}

void tmxc_physical_integrity_cleanup(void) {
    if (!tmxc_integrity.initialized) {
        return;
    }
    
    tmxc_integrity_exit_lockdown();
    tmxc_integrity.monitoring_enabled = 0;
    
    for (uint32_t i = 0; i < 5; i++) {
        tmxc_integrity.safe_zones[i].is_active = 0;
    }
    
    tmxc_integrity.initialized = 0;
    
    tmxc_uart_puts("[PHYSICAL-INTEGRITY] Physical integrity system cleaned up\r\n");
}
