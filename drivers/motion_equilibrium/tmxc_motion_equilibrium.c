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
#include "tmxc_motion_equilibrium.h"

static tmxc_motion_equilibrium_t tmxc_motion;
static uint32_t tmxc_motion_threshold = TMXC_MOTION_THRESHOLD;
static uint32_t tmxc_motion_sync_factor = TMXC_MOTION_SYNC_FACTOR;

static int32_t tmxc_abs(int32_t value) {
    return value < 0 ? -value : value;
}

static int32_t tmxc_clamp(int32_t value, int32_t min, int32_t max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void tmxc_motion_equilibrium_init(void) {
    tmxc_motion.history_index = 0;
    tmxc_motion.avg_accel_x = 0;
    tmxc_motion.avg_accel_y = 0;
    tmxc_motion.avg_accel_z = 0;
    tmxc_motion.avg_gyro_x = 0;
    tmxc_motion.avg_gyro_y = 0;
    tmxc_motion.avg_gyro_z = 0;
    tmxc_motion.motion_intensity = 0;
    tmxc_motion.in_vehicle = 0;
    tmxc_motion.motion_sync_enabled = 1;
    tmxc_motion.screen_offset_x = 0;
    tmxc_motion.screen_offset_y = 0;
    tmxc_motion.initialized = 0;
    tmxc_motion.motion_detected = 0;
    tmxc_motion.last_motion_time = 0;
    
    for (uint32_t i = 0; i < TMXC_MOTION_HISTORY_SIZE; i++) {
        tmxc_motion.history[i].accel_x = 0;
        tmxc_motion.history[i].accel_y = 0;
        tmxc_motion.history[i].accel_z = 0;
        tmxc_motion.history[i].gyro_x = 0;
        tmxc_motion.history[i].gyro_y = 0;
        tmxc_motion.history[i].gyro_z = 0;
        tmxc_motion.history[i].timestamp = 0;
    }
    
    tmxc_motion.initialized = 1;
    
    tmxc_uart_puts("[MOTION-EQUILIBRIUM] Smart Motion Equilibrium initialized\r\n");
    tmxc_uart_puts("[MOTION-EQUILIBRIUM] Motion-Sync anti-motion sickness system ready\r\n");
}

void tmxc_motion_equilibrium_enable_motion_sync(uint8_t enable) {
    if (!tmxc_motion.initialized) {
        return;
    }
    
    tmxc_motion.motion_sync_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[MOTION-EQUILIBRIUM] Motion-Sync ENABLED\r\n");
    } else {
        tmxc_uart_puts("[MOTION-EQUILIBRIUM] Motion-Sync DISABLED\r\n");
    }
}

uint8_t tmxc_motion_equilibrium_is_motion_sync_enabled(void) {
    return tmxc_motion.motion_sync_enabled;
}

void tmxc_motion_equilibrium_update_accelerometer(int16_t x, int16_t y, int16_t z) {
    if (!tmxc_motion.initialized) {
        return;
    }
    
    uint32_t idx = tmxc_motion.history_index;
    tmxc_motion.history[idx].accel_x = x;
    tmxc_motion.history[idx].accel_y = y;
    tmxc_motion.history[idx].accel_z = z;
    tmxc_motion.history[idx].timestamp = tmxc_get_cycle_count();
}

void tmxc_motion_equilibrium_update_gyroscope(int16_t x, int16_t y, int16_t z) {
    if (!tmxc_motion.initialized) {
        return;
    }
    
    uint32_t idx = tmxc_motion.history_index;
    tmxc_motion.history[idx].gyro_x = x;
    tmxc_motion.history[idx].gyro_y = y;
    tmxc_motion.history[idx].gyro_z = z;
    
    tmxc_motion.history_index = (tmxc_motion.history_index + 1) % TMXC_MOTION_HISTORY_SIZE;
}

void tmxc_motion_equilibrium_process(void) {
    if (!tmxc_motion.initialized) {
        return;
    }
    
    int64_t sum_accel_x = 0, sum_accel_y = 0, sum_accel_z = 0;
    int64_t sum_gyro_x = 0, sum_gyro_y = 0, sum_gyro_z = 0;
    uint32_t valid_samples = 0;
    
    for (uint32_t i = 0; i < TMXC_MOTION_HISTORY_SIZE; i++) {
        if (tmxc_motion.history[i].timestamp != 0) {
            sum_accel_x += tmxc_motion.history[i].accel_x;
            sum_accel_y += tmxc_motion.history[i].accel_y;
            sum_accel_z += tmxc_motion.history[i].accel_z;
            sum_gyro_x += tmxc_motion.history[i].gyro_x;
            sum_gyro_y += tmxc_motion.history[i].gyro_y;
            sum_gyro_z += tmxc_motion.history[i].gyro_z;
            valid_samples++;
        }
    }
    
    if (valid_samples == 0) {
        return;
    }
    
    tmxc_motion.avg_accel_x = sum_accel_x / valid_samples;
    tmxc_motion.avg_accel_y = sum_accel_y / valid_samples;
    tmxc_motion.avg_accel_z = sum_accel_z / valid_samples;
    tmxc_motion.avg_gyro_x = sum_gyro_x / valid_samples;
    tmxc_motion.avg_gyro_y = sum_gyro_y / valid_samples;
    tmxc_motion.avg_gyro_z = sum_gyro_z / valid_samples;
    
    int32_t accel_variance = tmxc_abs(tmxc_motion.avg_accel_x) + 
                            tmxc_abs(tmxc_motion.avg_accel_y) + 
                            tmxc_abs(tmxc_motion.avg_accel_z);
    
    int32_t gyro_variance = tmxc_abs(tmxc_motion.avg_gyro_x) + 
                           tmxc_abs(tmxc_motion.avg_gyro_y) + 
                           tmxc_abs(tmxc_motion.avg_gyro_z);
    
    tmxc_motion.motion_intensity = (accel_variance + gyro_variance) / 2;
    
    if (tmxc_motion.motion_intensity > tmxc_motion_threshold) {
        tmxc_motion.motion_detected = 1;
        tmxc_motion.last_motion_time = tmxc_get_cycle_count();
        
        if (tmxc_motion.motion_intensity > tmxc_motion_threshold * 2) {
            tmxc_motion.in_vehicle = 1;
        }
    } else {
        uint64_t time_since_motion = tmxc_get_cycle_count() - tmxc_motion.last_motion_time;
        if (time_since_motion > 5000000) {
            tmxc_motion.motion_detected = 0;
            tmxc_motion.in_vehicle = 0;
        }
    }
    
    if (tmxc_motion.motion_sync_enabled && tmxc_motion.motion_detected) {
        int32_t target_offset_x = -(tmxc_motion.avg_accel_x * tmxc_motion_sync_factor) / 1000;
        int32_t target_offset_y = -(tmxc_motion.avg_accel_y * tmxc_motion_sync_factor) / 1000;
        
        target_offset_x = tmxc_clamp(target_offset_x, -30, 30);
        target_offset_y = tmxc_clamp(target_offset_y, -30, 30);
        
        tmxc_motion.screen_offset_x = target_offset_x;
        tmxc_motion.screen_offset_y = target_offset_y;
    } else {
        tmxc_motion.screen_offset_x = 0;
        tmxc_motion.screen_offset_y = 0;
    }
}

void tmxc_motion_equilibrium_apply_screen_offset(int32_t* offset_x, int32_t* offset_y) {
    if (!tmxc_motion.initialized || !tmxc_motion.motion_sync_enabled) {
        if (offset_x != NULL) *offset_x = 0;
        if (offset_y != NULL) *offset_y = 0;
        return;
    }
    
    if (offset_x != NULL) *offset_x = tmxc_motion.screen_offset_x;
    if (offset_y != NULL) *offset_y = tmxc_motion.screen_offset_y;
}

uint8_t tmxc_motion_equilibrium_is_in_vehicle(void) {
    return tmxc_motion.in_vehicle;
}

int32_t tmxc_motion_equilibrium_get_motion_intensity(void) {
    return tmxc_motion.motion_intensity;
}

uint8_t tmxc_motion_equilibrium_is_motion_detected(void) {
    return tmxc_motion.motion_detected;
}

void tmxc_motion_equilibrium_set_threshold(uint32_t threshold) {
    tmxc_motion_threshold = threshold;
}

void tmxc_motion_equilibrium_set_sync_factor(uint32_t factor) {
    tmxc_motion_sync_factor = factor;
}

void tmxc_motion_equilibrium_cleanup(void) {
    if (!tmxc_motion.initialized) {
        return;
    }
    
    tmxc_motion.history_index = 0;
    tmxc_motion.motion_intensity = 0;
    tmxc_motion.in_vehicle = 0;
    tmxc_motion.motion_sync_enabled = 0;
    tmxc_motion.screen_offset_x = 0;
    tmxc_motion.screen_offset_y = 0;
    tmxc_motion.motion_detected = 0;
    tmxc_motion.initialized = 0;
    
    tmxc_uart_puts("[MOTION-EQUILIBRIUM] Smart Motion Equilibrium cleaned up\r\n");
}
