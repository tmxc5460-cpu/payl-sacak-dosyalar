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
#include "tmxc_premium_haptic.h"

static tmxc_premium_haptic_t tmxc_haptic;

static void tmxc_haptic_write_motor(uint16_t value) {
    uint32_t haptic_reg = tmxc_read32((volatile uint32_t*)TMXC_HAPTIC_BASE);
    haptic_reg = (haptic_reg & 0xFFFF0000) | value;
    tmxc_write32((volatile uint32_t*)TMXC_HAPTIC_BASE, haptic_reg);
}

static void tmxc_haptic_set_frequency(uint32_t frequency) {
    uint32_t freq_reg = tmxc_read32((volatile uint32_t*)(TMXC_HAPTIC_BASE + 0x04));
    freq_reg = (freq_reg & 0xFFFF0000) | frequency;
    tmxc_write32((volatile uint32_t*)(TMXC_HAPTIC_BASE + 0x04), freq_reg);
}

static void tmxc_haptic_enable_output(uint8_t enable) {
    uint32_t ctrl_reg = tmxc_read32((volatile uint32_t*)(TMXC_HAPTIC_BASE + 0x08));
    if (enable) {
        ctrl_reg |= (1 << 0);
    } else {
        ctrl_reg &= ~(1 << 0);
    }
    tmxc_write32((volatile uint32_t*)(TMXC_HAPTIC_BASE + 0x08), ctrl_reg);
}

static void tmxc_haptic_play_waveform(const uint16_t* waveform, uint32_t length, 
                                      uint8_t intensity, uint8_t sharpness) {
    if (!tmxc_haptic.haptic_enabled || waveform == NULL || length == 0) {
        return;
    }
    
    uint32_t scaled_intensity = (intensity * 255) / 100;
    uint32_t scaled_sharpness = (sharpness * 255) / 100;
    
    for (uint32_t i = 0; i < length && i < TMXC_HAPTIC_WAVEFORM_SIZE; i++) {
        uint16_t sample = waveform[i];
        
        uint32_t adjusted_sample = (sample * scaled_intensity) / 255;
        
        if (sharpness > 50) {
            adjusted_sample = (adjusted_sample * scaled_sharpness) / 128;
        }
        
        tmxc_haptic_write_motor((uint16_t)adjusted_sample);
        
        uint32_t delay_us = 1000 / (length * 2);
        tmxc_timer_delay_us(delay_us);
    }
    
    tmxc_haptic_write_motor(0);
}

void tmxc_premium_haptic_init(void) {
    tmxc_haptic.profile_count = 0;
    tmxc_haptic.haptic_enabled = 1;
    tmxc_haptic.glass_touch_enabled = 1;
    tmxc_haptic.initialized = 0;
    tmxc_haptic.haptic_motor_base = TMXC_HAPTIC_BASE;
    tmxc_haptic.current_intensity = 70;
    tmxc_haptic.current_sharpness = 60;
    
    for (uint32_t i = 0; i < TMXC_HAPTIC_MAX_PROFILES; i++) {
        for (uint32_t j = 0; j < TMXC_HAPTIC_WAVEFORM_SIZE; j++) {
            tmxc_haptic.profiles[i].waveform[j] = 0;
        }
        tmxc_haptic.profiles[i].duration_ms = 0;
        tmxc_haptic.profiles[i].intensity = 0;
        tmxc_haptic.profiles[i].sharpness = 0;
        tmxc_haptic.profiles[i].profile_id = 0;
        for (int j = 0; j < 32; j++) {
            tmxc_haptic.profiles[i].profile_name[j] = 0;
        }
        tmxc_haptic.profiles[i].active = 0;
    }
    
    uint16_t glass_tap_waveform[TMXC_HAPTIC_WAVEFORM_SIZE] = {
        0, 50, 200, 350, 500, 400, 300, 200, 100, 50, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    tmxc_premium_haptic_add_profile("Glass Tap", glass_tap_waveform, 10, 70, 80);
    
    uint16_t glass_swipe_waveform[TMXC_HAPTIC_WAVEFORM_SIZE] = {
        0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 280, 260, 240, 220, 200,
        180, 160, 140, 120, 100, 80, 60, 40, 20, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    tmxc_premium_haptic_add_profile("Glass Swipe", glass_swipe_waveform, 16, 50, 40);
    
    uint16_t glass_long_press_waveform[TMXC_HAPTIC_WAVEFORM_SIZE] = {
        0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300,
        320, 340, 360, 380, 400, 420, 440, 460, 480, 500, 480, 460, 440, 420, 400, 380,
        360, 340, 320, 300, 280, 260, 240, 220, 200, 180, 160, 140, 120, 100, 80, 60,
        40, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    tmxc_premium_haptic_add_profile("Glass Long Press", glass_long_press_waveform, 32, 60, 50);
    
    uint16_t notification_waveform[TMXC_HAPTIC_WAVEFORM_SIZE] = {
        0, 100, 200, 300, 400, 300, 200, 100, 0, 0, 100, 200, 300, 400, 300, 200,
        100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    tmxc_premium_haptic_add_profile("Notification", notification_waveform, 16, 80, 70);
    
    uint16_t success_waveform[TMXC_HAPTIC_WAVEFORM_SIZE] = {
        0, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 450, 400, 350, 300, 250,
        200, 150, 100, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    tmxc_premium_haptic_add_profile("Success", success_waveform, 16, 75, 65);
    
    uint16_t error_waveform[TMXC_HAPTIC_WAVEFORM_SIZE] = {
        0, 200, 400, 300, 200, 100, 0, 100, 200, 300, 400, 300, 200, 100, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    tmxc_premium_haptic_add_profile("Error", error_waveform, 16, 85, 90);
    
    uint16_t button_press_waveform[TMXC_HAPTIC_WAVEFORM_SIZE] = {
        0, 80, 160, 240, 320, 240, 160, 80, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    tmxc_premium_haptic_add_profile("Button Press", button_press_waveform, 8, 65, 75);
    
    tmxc_haptic_set_frequency(200);
    tmxc_haptic_enable_output(0);
    
    tmxc_haptic.initialized = 1;
    
    tmxc_uart_puts("[PREMIUM-HAPTIC] Premium Haptic Feedback initialized\r\n");
    tmxc_uart_puts("[PREMIUM-HAPTIC] Apple-style glass-touch profiles loaded\r\n");
}

void tmxc_premium_haptic_enable(uint8_t enable) {
    if (!tmxc_haptic.initialized) {
        return;
    }
    
    tmxc_haptic.haptic_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[PREMIUM-HAPTIC] Haptic feedback enabled\r\n");
    } else {
        tmxc_uart_puts("[PREMIUM-HAPTIC] Haptic feedback disabled\r\n");
    }
}

void tmxc_premium_haptic_enable_glass_touch(uint8_t enable) {
    if (!tmxc_haptic.initialized) {
        return;
    }
    
    tmxc_haptic.glass_touch_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[PREMIUM-HAPTIC] Glass-touch profiles enabled\r\n");
    } else {
        tmxc_uart_puts("[PREMIUM-HAPTIC] Glass-touch profiles disabled\r\n");
    }
}

uint8_t tmxc_premium_haptic_is_enabled(void) {
    return tmxc_haptic.haptic_enabled;
}

uint8_t tmxc_premium_haptic_is_glass_touch_enabled(void) {
    return tmxc_haptic.glass_touch_enabled;
}

uint32_t tmxc_premium_haptic_add_profile(const char* name, const uint16_t* waveform, 
                                         uint32_t duration, uint8_t intensity, uint8_t sharpness) {
    if (!tmxc_haptic.initialized || tmxc_haptic.profile_count >= TMXC_HAPTIC_MAX_PROFILES) {
        return 0;
    }
    
    if (name == NULL || waveform == NULL || duration == 0) {
        return 0;
    }
    
    uint32_t idx = tmxc_haptic.profile_count;
    
    for (uint32_t i = 0; i < TMXC_HAPTIC_WAVEFORM_SIZE; i++) {
        tmxc_haptic.profiles[idx].waveform[i] = waveform[i];
    }
    
    tmxc_haptic.profiles[idx].duration_ms = duration;
    tmxc_haptic.profiles[idx].intensity = intensity;
    tmxc_haptic.profiles[idx].sharpness = sharpness;
    tmxc_haptic.profiles[idx].profile_id = idx + 1;
    
    for (int i = 0; i < 32 && name[i] != '\0'; i++) {
        tmxc_haptic.profiles[idx].profile_name[i] = name[i];
    }
    
    tmxc_haptic.profiles[idx].active = 1;
    tmxc_haptic.profile_count++;
    
    return idx;
}

void tmxc_premium_haptic_remove_profile(uint32_t profile_id) {
    if (!tmxc_haptic.initialized || profile_id >= tmxc_haptic.profile_count) {
        return;
    }
    
    tmxc_haptic.profiles[profile_id].active = 0;
}

void tmxc_premium_haptic_trigger_profile(tmxc_haptic_profile_t profile) {
    if (!tmxc_haptic.initialized || !tmxc_haptic.haptic_enabled) {
        return;
    }
    
    for (uint32_t i = 0; i < tmxc_haptic.profile_count; i++) {
        if (tmxc_haptic.profiles[i].active && tmxc_haptic.profiles[i].profile_id == profile) {
            tmxc_haptic_enable_output(1);
            tmxc_haptic_play_waveform(tmxc_haptic.profiles[i].waveform, 
                                     tmxc_haptic.profiles[i].duration_ms,
                                     tmxc_haptic.profiles[i].intensity,
                                     tmxc_haptic.profiles[i].sharpness);
            tmxc_haptic_enable_output(0);
            break;
        }
    }
}

void tmxc_premium_haptic_trigger_custom(const uint16_t* waveform, uint32_t duration, 
                                        uint8_t intensity, uint8_t sharpness) {
    if (!tmxc_haptic.initialized || !tmxc_haptic.haptic_enabled) {
        return;
    }
    
    if (waveform == NULL || duration == 0) {
        return;
    }
    
    tmxc_haptic_enable_output(1);
    tmxc_haptic_play_waveform(waveform, duration, intensity, sharpness);
    tmxc_haptic_enable_output(0);
}

void tmxc_premium_haptic_set_intensity(uint8_t intensity) {
    if (!tmxc_haptic.initialized) {
        return;
    }
    
    if (intensity > 100) intensity = 100;
    tmxc_haptic.current_intensity = intensity;
}

void tmxc_premium_haptic_set_sharpness(uint8_t sharpness) {
    if (!tmxc_haptic.initialized) {
        return;
    }
    
    if (sharpness > 100) sharpness = 100;
    tmxc_haptic.current_sharpness = sharpness;
}

uint8_t tmxc_premium_haptic_get_intensity(void) {
    return tmxc_haptic.current_intensity;
}

uint8_t tmxc_premium_haptic_get_sharpness(void) {
    return tmxc_haptic.current_sharpness;
}

void tmxc_premium_haptic_on_touch(uint32_t x, uint32_t y, uint8_t is_tap, uint8_t is_long_press) {
    if (!tmxc_haptic.initialized || !tmxc_haptic.haptic_enabled || !tmxc_haptic.glass_touch_enabled) {
        return;
    }
    
    if (is_long_press) {
        tmxc_premium_haptic_trigger_profile(TMXC_HAPTIC_PROFILE_GLASS_LONG_PRESS);
    } else if (is_tap) {
        tmxc_premium_haptic_trigger_profile(TMXC_HAPTIC_PROFILE_GLASS_TAP);
    }
}

void tmxc_premium_haptic_on_swipe(int32_t delta_x, int32_t delta_y, uint32_t velocity) {
    if (!tmxc_haptic.initialized || !tmxc_haptic.haptic_enabled || !tmxc_haptic.glass_touch_enabled) {
        return;
    }
    
    if (velocity > 500) {
        tmxc_premium_haptic_trigger_profile(TMXC_HAPTIC_PROFILE_GLASS_SWIPE);
    }
}

void tmxc_premium_haptic_stop(void) {
    if (!tmxc_haptic.initialized) {
        return;
    }
    
    tmxc_haptic_write_motor(0);
    tmxc_haptic_enable_output(0);
}

void tmxc_premium_haptic_cleanup(void) {
    if (!tmxc_haptic.initialized) {
        return;
    }
    
    tmxc_premium_haptic_stop();
    
    tmxc_haptic.profile_count = 0;
    tmxc_haptic.haptic_enabled = 0;
    tmxc_haptic.glass_touch_enabled = 0;
    tmxc_haptic.initialized = 0;
    
    tmxc_uart_puts("[PREMIUM-HAPTIC] Premium Haptic Feedback cleaned up\r\n");
}
