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

typedef struct {
    tmxc_haptic_pulse_t pattern[TMXC_HAPTIC_PATTERN_LENGTH];
    uint8_t pattern_length;
    tmxc_haptic_event_t event_type;
    uint8_t repeat_count;
    uint8_t enabled;
} tmxc_haptic_pattern_t;

typedef struct {
    tmxc_haptic_pattern_t patterns[TMXC_HAPTIC_MAX_PATTERNS];
    uint8_t initialized;
    uint8_t haptic_enabled;
    uint8_t global_intensity;
    uint8_t current_pattern_index;
    uint8_t is_playing;
    uint64_t last_pulse_time;
} tmxc_haptic_t;

static tmxc_haptic_t tmxc_haptic;

void tmxc_haptic_init(void) {
    tmxc_haptic.initialized = 0;
    tmxc_haptic.haptic_enabled = 1;
    tmxc_haptic.global_intensity = 3;
    tmxc_haptic.current_pattern_index = 0;
    tmxc_haptic.is_playing = 0;
    tmxc_haptic.last_pulse_time = 0;
    
    for (uint32_t i = 0; i < TMXC_HAPTIC_MAX_PATTERNS; i++) {
        for (uint32_t j = 0; j < TMXC_HAPTIC_PATTERN_LENGTH; j++) {
            tmxc_haptic.patterns[i].pattern[j].duration_ms = 0;
            tmxc_haptic.patterns[i].pattern[j].intensity = 0;
            tmxc_haptic.patterns[i].delay_ms = 0;
        }
        tmxc_haptic.patterns[i].pattern_length = 0;
        tmxc_haptic.patterns[i].event_type = TMXC_HAPTIC_EVENT_CUSTOM;
        tmxc_haptic.patterns[i].repeat_count = 1;
        tmxc_haptic.patterns[i].enabled = 0;
    }
    
    tmxc_haptic.initialized = 1;
    
    tmxc_uart_puts("[HAPTIC] Haptic system initialized\r\n");
}

void tmxc_haptic_set_pattern(tmxc_haptic_event_t event, const tmxc_haptic_pulse_t* pulses, uint8_t length) {
    if (!tmxc_haptic.initialized || pulses == NULL || length == 0 || length > TMXC_HAPTIC_PATTERN_LENGTH) {
        return;
    }
    
    uint8_t pattern_index = (uint8_t)event;
    if (pattern_index >= TMXC_HAPTIC_MAX_PATTERNS) {
        pattern_index = TMXC_HAPTIC_MAX_PATTERNS - 1;
    }
    
    for (uint32_t i = 0; i < length; i++) {
        tmxc_haptic.patterns[pattern_index].pattern[i].duration_ms = pulses[i].duration_ms;
        tmxc_haptic.patterns[pattern_index].pattern[i].intensity = pulses[i].intensity;
        tmxc_haptic.patterns[pattern_index].pattern[i].delay_ms = pulses[i].delay_ms;
    }
    
    tmxc_haptic.patterns[pattern_index].pattern_length = length;
    tmxc_haptic.patterns[pattern_index].event_type = event;
    tmxc_haptic.patterns[pattern_index].enabled = 1;
    
    tmxc_uart_puts("[HAPTIC] Pattern set for event: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = event;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

void tmxc_haptic_set_default_patterns(void) {
    if (!tmxc_haptic.initialized) {
        return;
    }
    
    tmxc_haptic_pulse_t beyaz_kus_pattern[] = {
        {50, 4, 30}, {30, 3, 20}, {50, 4, 0}
    };
    tmxc_haptic_set_pattern(TMXC_HAPTIC_EVENT_BEYAZ_KUS, beyaz_kus_pattern, 3);
    
    tmxc_haptic_pulse_t security_alert_pattern[] = {
        {100, 5, 50}, {100, 5, 50}, {100, 5, 0}
    };
    tmxc_haptic_set_pattern(TMXC_HAPTIC_EVENT_SECURITY_ALERT, security_alert_pattern, 3);
    
    tmxc_haptic_pulse_t message_pattern[] = {
        {40, 3, 40}, {40, 3, 0}
    };
    tmxc_haptic_set_pattern(TMXC_HAPTIC_EVENT_MESSAGE, message_pattern, 2);
    
    tmxc_haptic_pulse_t call_pattern[] = {
        {80, 4, 100}, {80, 4, 100}, {80, 4, 0}
    };
    tmxc_haptic_set_pattern(TMXC_HAPTIC_EVENT_CALL, call_pattern, 3);
    
    tmxc_haptic_pulse_t email_pattern[] = {
        {60, 3, 80}, {60, 3, 0}
    };
    tmxc_haptic_set_pattern(TMXC_HAPTIC_EVENT_EMAIL, email_pattern, 2);
    
    tmxc_haptic_pulse_t battery_low_pattern[] = {
        {200, 5, 300}, {200, 5, 0}
    };
    tmxc_haptic_set_pattern(TMXC_HAPTIC_EVENT_BATTERY_LOW, battery_low_pattern, 2);
    
    tmxc_haptic_pulse_t lockdown_pattern[] = {
        {150, 5, 50}, {150, 5, 50}, {150, 5, 50}, {150, 5, 0}
    };
    tmxc_haptic_set_pattern(TMXC_HAPTIC_EVENT_LOCKDOWN, lockdown_pattern, 4);
    
    tmxc_haptic_pulse_t guest_mode_pattern[] = {
        {30, 2, 30}, {30, 2, 30}, {30, 2, 30}, {30, 2, 0}
    };
    tmxc_haptic_set_pattern(TMXC_HAPTIC_EVENT_GUEST_MODE, guest_mode_pattern, 4);
    
    tmxc_haptic_pulse_t profile_switch_pattern[] = {
        {40, 3, 20}, {40, 3, 20}, {40, 3, 0}
    };
    tmxc_haptic_set_pattern(TMXC_HAPTIC_EVENT_PROFILE_SWITCH, profile_switch_pattern, 3);
    
    tmxc_uart_puts("[HAPTIC] Default patterns set\r\n");
}

void tmxc_haptic_trigger(tmxc_haptic_event_t event) {
    if (!tmxc_haptic.initialized || !tmxc_haptic.haptic_enabled) {
        return;
    }
    
    uint8_t pattern_index = (uint8_t)event;
    if (pattern_index >= TMXC_HAPTIC_MAX_PATTERNS) {
        pattern_index = TMXC_HAPTIC_MAX_PATTERNS - 1;
    }
    
    if (!tmxc_haptic.patterns[pattern_index].enabled) {
        return;
    }
    
    tmxc_haptic.current_pattern_index = pattern_index;
    tmxc_haptic.is_playing = 1;
    tmxc_haptic.last_pulse_time = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[HAPTIC] Triggered event: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = event;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

void tmxc_haptic_stop(void) {
    if (!tmxc_haptic.initialized) {
        return;
    }
    
    tmxc_haptic.is_playing = 0;
    
    tmxc_uart_puts("[HAPTIC] Haptic stopped\r\n");
}

void tmxc_haptic_set_global_intensity(uint8_t intensity) {
    if (!tmxc_haptic.initialized || intensity > TMXC_HAPTIC_INTENSITY_LEVELS) {
        return;
    }
    
    tmxc_haptic.global_intensity = intensity;
    
    tmxc_uart_puts("[HAPTIC] Global intensity set to: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = intensity;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

void tmxc_haptic_enable(uint8_t enable) {
    if (!tmxc_haptic.initialized) {
        return;
    }
    
    tmxc_haptic.haptic_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[HAPTIC] Haptic enabled\r\n");
    } else {
        tmxc_uart_puts("[HAPTIC] Haptic disabled\r\n");
    }
}

uint8_t tmxc_haptic_is_playing(void) {
    return tmxc_haptic.is_playing;
}

void tmxc_haptic_update(void) {
    if (!tmxc_haptic.initialized || !tmxc_haptic.is_playing) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed_ns = current_time - tmxc_haptic.last_pulse_time;
    uint64_t elapsed_ms = elapsed_ns * 1000 / tmxc_get_frequency();
    
    tmxc_haptic_pattern_t* pattern = &tmxc_haptic.patterns[tmxc_haptic.current_pattern_index];
    
    static uint8_t pulse_index = 0;
    static uint8_t repeat_index = 0;
    
    if (pulse_index >= pattern->pattern_length) {
        repeat_index++;
        pulse_index = 0;
        
        if (repeat_index >= pattern->repeat_count) {
            tmxc_haptic.is_playing = 0;
            pulse_index = 0;
            repeat_index = 0;
            return;
        }
    }
    
    tmxc_haptic_pulse_t* pulse = &pattern->pattern[pulse_index];
    
    if (elapsed_ms >= pulse->duration_ms + pulse->delay_ms) {
        pulse_index++;
        tmxc_haptic.last_pulse_time = current_time;
    }
}

void tmxc_haptic_cleanup(void) {
    if (!tmxc_haptic.initialized) {
        return;
    }
    
    tmxc_haptic_stop();
    tmxc_haptic.initialized = 0;
    
    tmxc_uart_puts("[HAPTIC] Haptic system cleaned up\r\n");
}
