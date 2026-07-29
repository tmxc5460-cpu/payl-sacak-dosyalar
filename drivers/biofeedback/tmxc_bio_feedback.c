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
#include "tmxc_bio_feedback.h"

static tmxc_bio_feedback_t tmxc_bio_feedback;

void tmxc_bio_feedback_init(void) {
    for (int i = 0; i < TMXC_BIO_SENSORS_COUNT; i++) {
        tmxc_bio_feedback.sensors[i].sensor_id = i;
        tmxc_bio_feedback.sensors[i].vibration_level = 0;
        tmxc_bio_feedback.sensors[i].heart_rate_detected = 0;
        tmxc_bio_feedback.sensors[i].stress_level = 0;
        tmxc_bio_feedback.sensors[i].last_reading_time = 0;
        tmxc_bio_feedback.sensors[i].sensor_active = 1;
    }
    
    tmxc_bio_feedback.calm_settings.screen_brightness = 100;
    tmxc_bio_feedback.calm_settings.notification_volume = 100;
    tmxc_bio_feedback.calm_settings.haptic_frequency = 200;
    tmxc_bio_feedback.calm_settings.haptic_intensity = 50;
    tmxc_bio_feedback.calm_settings.blue_light_filter = 0;
    tmxc_bio_feedback.calm_settings.calm_mode_active = 0;
    
    tmxc_bio_feedback.current_heart_rate = 72;
    tmxc_bio_feedback.current_stress_level = 20;
    tmxc_bio_feedback.calm_mode = TMXC_CALM_MODE_OFF;
    tmxc_bio_feedback.last_heartbeat_time = 0;
    tmxc_bio_feedback.bio_feedback_initialized = 1;
    tmxc_bio_feedback.auto_calm_enabled = 1;
    
    tmxc_uart_puts("[BIO] Bio-Feedback Integration initialized\r\n");
}

void tmxc_bio_feedback_enable(uint8_t enable) {
    for (int i = 0; i < TMXC_BIO_SENSORS_COUNT; i++) {
        tmxc_bio_feedback.sensors[i].sensor_active = enable;
    }
    
    if (enable) {
        tmxc_uart_puts("[BIO] Bio-feedback sensors enabled\r\n");
    } else {
        tmxc_uart_puts("[BIO] Bio-feedback sensors disabled\r\n");
    }
}

void tmxc_bio_feedback_enable_auto_calm(uint8_t enable) {
    tmxc_bio_feedback.auto_calm_enabled = enable;
}

void tmxc_bio_sensor_read(uint8_t sensor_id) {
    if (sensor_id >= TMXC_BIO_SENSORS_COUNT) {
        return;
    }
    
    if (!tmxc_bio_feedback.sensors[sensor_id].sensor_active) {
        return;
    }
    
    uint16_t vibration = tmxc_quantum_get_random_byte() * 10;
    tmxc_bio_feedback.sensors[sensor_id].vibration_level = vibration;
    
    if (vibration > 500 && vibration < 1500) {
        tmxc_bio_feedback.sensors[sensor_id].heart_rate_detected = 1;
        uint8_t heart_rate = 60 + (vibration / 50);
        tmxc_bio_feedback.current_heart_rate = heart_rate;
        tmxc_bio_feedback.last_heartbeat_time = tmxc_get_cycle_count();
    }
    
    if (vibration > 2000) {
        tmxc_bio_feedback.sensors[sensor_id].stress_level = (vibration - 2000) / 50;
        if (tmxc_bio_feedback.sensors[sensor_id].stress_level > 100) {
            tmxc_bio_feedback.sensors[sensor_id].stress_level = 100;
        }
        tmxc_bio_feedback.current_stress_level = tmxc_bio_feedback.sensors[sensor_id].stress_level;
    }
    
    tmxc_bio_feedback.sensors[sensor_id].last_reading_time = tmxc_get_cycle_count();
}

uint8_t tmxc_bio_get_heart_rate(void) {
    return tmxc_bio_feedback.current_heart_rate;
}

uint8_t tmxc_bio_get_stress_level(void) {
    return tmxc_bio_feedback.current_stress_level;
}

void tmxc_calm_mode_activate(tmxc_calm_mode_t mode) {
    if (mode == TMXC_CALM_MODE_OFF) {
        tmxc_calm_mode_deactivate();
        return;
    }
    
    tmxc_bio_feedback.calm_mode = mode;
    tmxc_bio_feedback.calm_settings.calm_mode_active = 1;
    
    switch (mode) {
        case TMXC_CALM_MODE_LOW:
            tmxc_calm_adjust_screen_brightness(80);
            tmxc_calm_enable_haptic_soothing(1);
            break;
        case TMXC_CALM_MODE_MEDIUM:
            tmxc_calm_adjust_screen_brightness(50);
            tmxc_calm_mute_notifications();
            tmxc_calm_enable_haptic_soothing(1);
            tmxc_calm_enable_blue_light_filter(1);
            break;
        case TMXC_CALM_MODE_HIGH:
            tmxc_calm_adjust_screen_brightness(30);
            tmxc_calm_mute_notifications();
            tmxc_calm_enable_haptic_soothing(1);
            tmxc_calm_enable_blue_light_filter(1);
            tmxc_bio_feedback.calm_settings.haptic_frequency = 100;
            break;
        default:
            break;
    }
    
    tmxc_uart_puts("[BIO] Calm Mode activated: Level ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = mode;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

void tmxc_calm_mode_deactivate(void) {
    tmxc_bio_feedback.calm_mode = TMXC_CALM_MODE_OFF;
    tmxc_bio_feedback.calm_settings.calm_mode_active = 0;
    
    tmxc_calm_adjust_screen_brightness(100);
    tmxc_calm_enable_blue_light_filter(0);
    tmxc_bio_feedback.calm_settings.haptic_frequency = 200;
    
    tmxc_uart_puts("[BIO] Calm Mode deactivated\r\n");
}

tmxc_calm_mode_t tmxc_calm_mode_get_current(void) {
    return tmxc_bio_feedback.calm_mode;
}

void tmxc_calm_adjust_screen_brightness(uint8_t brightness) {
    if (brightness > 100) {
        brightness = 100;
    }
    tmxc_bio_feedback.calm_settings.screen_brightness = brightness;
}

void tmxc_calm_mute_notifications(void) {
    tmxc_bio_feedback.calm_settings.notification_volume = 0;
}

void tmxc_calm_enable_haptic_soothing(uint8_t enable) {
    if (enable) {
        tmxc_bio_feedback.calm_settings.haptic_frequency = 100;
        tmxc_bio_feedback.calm_settings.haptic_intensity = 30;
    } else {
        tmxc_bio_feedback.calm_settings.haptic_frequency = 200;
        tmxc_bio_feedback.calm_settings.haptic_intensity = 50;
    }
}

void tmxc_calm_enable_blue_light_filter(uint8_t enable) {
    tmxc_bio_feedback.calm_settings.blue_light_filter = enable;
}

void tmxc_bio_feedback_monitor(void) {
    if (!tmxc_bio_feedback.bio_feedback_initialized || !tmxc_bio_feedback.auto_calm_enabled) {
        return;
    }
    
    for (int i = 0; i < TMXC_BIO_SENSORS_COUNT; i++) {
        tmxc_bio_sensor_read(i);
    }
    
    if (tmxc_bio_feedback_should_activate_calm()) {
        if (tmxc_bio_feedback.current_stress_level > TMXC_STRESS_THRESHOLD_CRITICAL) {
            tmxc_calm_mode_activate(TMXC_CALM_MODE_HIGH);
        } else if (tmxc_bio_feedback.current_stress_level > TMXC_STRESS_THRESHOLD_HIGH) {
            tmxc_calm_mode_activate(TMXC_CALM_MODE_MEDIUM);
        } else {
            tmxc_calm_mode_activate(TMXC_CALM_MODE_LOW);
        }
    } else {
        if (tmxc_bio_feedback.calm_mode != TMXC_CALM_MODE_OFF) {
            tmxc_calm_mode_deactivate();
        }
    }
}

uint8_t tmxc_bio_feedback_should_activate_calm(void) {
    return (tmxc_bio_feedback.current_stress_level > TMXC_STRESS_THRESHOLD_HIGH);
}
