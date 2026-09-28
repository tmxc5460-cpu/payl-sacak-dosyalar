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
#ifndef TMXC_PREMIUM_HAPTIC_H
#define TMXC_PREMIUM_HAPTIC_H

#include "../kernel/tmxc_kernel.h"

#define TMXC_HAPTIC_MAX_PROFILES 32
#define TMXC_HAPTIC_WAVEFORM_SIZE 64

typedef enum {
    TMXC_HAPTIC_PROFILE_NONE = 0,
    TMXC_HAPTIC_PROFILE_GLASS_TAP = 1,
    TMXC_HAPTIC_PROFILE_GLASS_SWIPE = 2,
    TMXC_HAPTIC_PROFILE_GLASS_LONG_PRESS = 3,
    TMXC_HAPTIC_PROFILE_NOTIFICATION = 4,
    TMXC_HAPTIC_PROFILE_SUCCESS = 5,
    TMXC_HAPTIC_PROFILE_ERROR = 6,
    TMXC_HAPTIC_PROFILE_WARNING = 7,
    TMXC_HAPTIC_PROFILE_BUTTON_PRESS = 8,
    TMXC_HAPTIC_PROFILE_SLIDER = 9,
    TMXC_HAPTIC_PROFILE_SCROLL = 10,
    TMXC_HAPTIC_PROFILE_CUSTOM = 255
} tmxc_haptic_profile_t;

typedef struct {
    uint16_t waveform[TMXC_HAPTIC_WAVEFORM_SIZE];
    uint32_t duration_ms;
    uint8_t intensity;
    uint8_t sharpness;
    uint8_t profile_id;
    char profile_name[32];
    uint8_t active;
} tmxc_haptic_profile_t;

typedef struct {
    tmxc_haptic_profile_t profiles[TMXC_HAPTIC_MAX_PROFILES];
    uint32_t profile_count;
    uint8_t haptic_enabled;
    uint8_t glass_touch_enabled;
    uint8_t initialized;
    uint32_t haptic_motor_base;
    uint8_t current_intensity;
    uint8_t current_sharpness;
} tmxc_premium_haptic_t;

void tmxc_premium_haptic_init(void);
void tmxc_premium_haptic_enable(uint8_t enable);
void tmxc_premium_haptic_enable_glass_touch(uint8_t enable);
uint8_t tmxc_premium_haptic_is_enabled(void);
uint8_t tmxc_premium_haptic_is_glass_touch_enabled(void);

uint32_t tmxc_premium_haptic_add_profile(const char* name, const uint16_t* waveform, 
                                         uint32_t duration, uint8_t intensity, uint8_t sharpness);
void tmxc_premium_haptic_remove_profile(uint32_t profile_id);

void tmxc_premium_haptic_trigger_profile(tmxc_haptic_profile_t profile);
void tmxc_premium_haptic_trigger_custom(const uint16_t* waveform, uint32_t duration, 
                                        uint8_t intensity, uint8_t sharpness);

void tmxc_premium_haptic_set_intensity(uint8_t intensity);
void tmxc_premium_haptic_set_sharpness(uint8_t sharpness);
uint8_t tmxc_premium_haptic_get_intensity(void);
uint8_t tmxc_premium_haptic_get_sharpness(void);

void tmxc_premium_haptic_on_touch(uint32_t x, uint32_t y, uint8_t is_tap, uint8_t is_long_press);
void tmxc_premium_haptic_on_swipe(int32_t delta_x, int32_t delta_y, uint32_t velocity);

void tmxc_premium_haptic_stop(void);
void tmxc_premium_haptic_cleanup(void);

#endif
