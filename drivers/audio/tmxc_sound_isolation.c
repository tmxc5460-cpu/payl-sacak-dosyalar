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

#define TMXC_AUDIO_BUFFER_SIZE 4096
#define TMXC_NOISE_THRESHOLD 50

typedef enum {
    TMXC_ISOLATION_MODE_OFF = 0,
    TMXC_ISOLATION_MODE_NOISE_CANCEL = 1,
    TMXC_ISOLATION_MODE_VOICE_FOCUS = 2
} tmxc_isolation_mode_t;

typedef struct {
    int16_t audio_buffer[TMXC_AUDIO_BUFFER_SIZE];
    uint32_t buffer_index;
    tmxc_isolation_mode_t mode;
    uint8_t initialized;
    uint8_t isolation_enabled;
    uint8_t voice_isolation_active;
    uint64_t noise_samples_processed;
} tmxc_sound_isolation_t;

static tmxc_sound_isolation_t tmxc_isolation;

void tmxc_sound_isolation_init(void) {
    tmxc_isolation.initialized = 0;
    tmxc_isolation.mode = TMXC_ISOLATION_MODE_VOICE_FOCUS;
    tmxc_isolation.isolation_enabled = 1;
    tmxc_isolation.voice_isolation_active = 0;
    tmxc_isolation.buffer_index = 0;
    tmxc_isolation.noise_samples_processed = 0;
    
    for (uint32_t i = 0; i < TMXC_AUDIO_BUFFER_SIZE; i++) {
        tmxc_isolation.audio_buffer[i] = 0;
    }
    
    tmxc_isolation.initialized = 1;
    
    tmxc_uart_puts("[SOUND-ISOLATION] Sound isolation initialized\r\n");
}

void tmxc_sound_isolation_process_audio(int16_t* audio_data, uint32_t size) {
    if (!tmxc_isolation.initialized || !tmxc_isolation.isolation_enabled || audio_data == NULL || size == 0) {
        return;
    }
    
    for (uint32_t i = 0; i < size; i++) {
        int16_t sample = audio_data[i];
        
        if (tmxc_isolation.mode == TMXC_ISOLATION_MODE_NOISE_CANCEL) {
            if (sample < TMXC_NOISE_THRESHOLD && sample > -TMXC_NOISE_THRESHOLD) {
                audio_data[i] = 0;
            }
        } else if (tmxc_isolation.mode == TMXC_ISOLATION_MODE_VOICE_FOCUS) {
            if (sample < TMXC_NOISE_THRESHOLD * 2 && sample > -TMXC_NOISE_THRESHOLD * 2) {
                audio_data[i] = sample / 2;
            }
        }
        
        tmxc_isolation.noise_samples_processed++;
    }
    
    tmxc_uart_puts("[SOUND-ISOLATION] Audio processed\r\n");
}

void tmxc_sound_isolation_set_mode(tmxc_isolation_mode_t mode) {
    if (!tmxc_isolation.initialized) {
        return;
    }
    
    tmxc_isolation.mode = mode;
    
    switch (mode) {
        case TMXC_ISOLATION_MODE_OFF:
            tmxc_uart_puts("[SOUND-ISOLATION] Mode: OFF\r\n");
            break;
        case TMXC_ISOLATION_MODE_NOISE_CANCEL:
            tmxc_uart_puts("[SOUND-ISOLATION] Mode: NOISE_CANCEL\r\n");
            break;
        case TMXC_ISOLATION_MODE_VOICE_FOCUS:
            tmxc_uart_puts("[SOUND-ISOLATION] Mode: VOICE_FOCUS\r\n");
            break;
    }
}

void tmxc_sound_isolation_enable(uint8_t enable) {
    if (!tmxc_isolation.initialized) {
        return;
    }
    
    tmxc_isolation.isolation_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[SOUND-ISOLATION] Sound isolation enabled\r\n");
    } else {
        tmxc_uart_puts("[SOUND-ISOLATION] Sound isolation disabled\r\n");
    }
}

uint8_t tmxc_sound_isolation_is_enabled(void) {
    return tmxc_isolation.isolation_enabled;
}

uint64_t tmxc_sound_isolation_get_samples_processed(void) {
    return tmxc_isolation.noise_samples_processed;
}

void tmxc_sound_isolation_cleanup(void) {
    if (!tmxc_isolation.initialized) {
        return;
    }
    
    tmxc_sound_isolation_set_mode(TMXC_ISOLATION_MODE_OFF);
    tmxc_isolation.isolation_enabled = 0;
    tmxc_isolation.initialized = 0;
    
    tmxc_uart_puts("[SOUND-ISOLATION] Sound isolation cleaned up\r\n");
}
