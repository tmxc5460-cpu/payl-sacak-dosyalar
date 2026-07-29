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
#ifndef TMXC_HAPTIC_H
#define TMXC_HAPTIC_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_HAPTIC_MAX_PATTERNS 32
#define TMXC_HAPTIC_PATTERN_LENGTH 16
#define TMXC_HAPTIC_INTENSITY_LEVELS 5

typedef enum {
    TMXC_HAPTIC_EVENT_BEYAZ_KUS = 0,
    TMXC_HAPTIC_EVENT_SECURITY_ALERT = 1,
    TMXC_HAPTIC_EVENT_MESSAGE = 2,
    TMXC_HAPTIC_EVENT_CALL = 3,
    TMXC_HAPTIC_EVENT_EMAIL = 4,
    TMXC_HAPTIC_EVENT_CALENDAR = 5,
    TMXC_HAPTIC_EVENT_ALARM = 6,
    TMXC_HAPTIC_EVENT_SYSTEM = 7,
    TMXC_HAPTIC_EVENT_BATTERY_LOW = 8,
    TMXC_HAPTIC_EVENT_CHARGING = 9,
    TMXC_HAPTIC_EVENT_LOCKDOWN = 10,
    TMXC_HAPTIC_EVENT_GUEST_MODE = 11,
    TMXC_HAPTIC_EVENT_PROFILE_SWITCH = 12,
    TMXC_HAPTIC_EVENT_CUSTOM = 13
} tmxc_haptic_event_t;

typedef struct {
    uint16_t duration_ms;
    uint8_t intensity;
    uint16_t delay_ms;
} tmxc_haptic_pulse_t;

void tmxc_haptic_init(void);
void tmxc_haptic_set_pattern(tmxc_haptic_event_t event, const tmxc_haptic_pulse_t* pulses, uint8_t length);
void tmxc_haptic_set_default_patterns(void);
void tmxc_haptic_trigger(tmxc_haptic_event_t event);
void tmxc_haptic_stop(void);
void tmxc_haptic_set_global_intensity(uint8_t intensity);
void tmxc_haptic_enable(uint8_t enable);
uint8_t tmxc_haptic_is_playing(void);
void tmxc_haptic_update(void);
void tmxc_haptic_cleanup(void);

#endif
