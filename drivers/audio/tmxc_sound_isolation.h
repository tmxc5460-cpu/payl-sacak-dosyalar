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
#ifndef TMXC_SOUND_ISOLATION_H
#define TMXC_SOUND_ISOLATION_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_AUDIO_BUFFER_SIZE 4096
#define TMXC_NOISE_THRESHOLD 50

typedef enum {
    TMXC_ISOLATION_MODE_OFF = 0,
    TMXC_ISOLATION_MODE_NOISE_CANCEL = 1,
    TMXC_ISOLATION_MODE_VOICE_FOCUS = 2
} tmxc_isolation_mode_t;

void tmxc_sound_isolation_init(void);
void tmxc_sound_isolation_process_audio(int16_t* audio_data, uint32_t size);
void tmxc_sound_isolation_set_mode(tmxc_isolation_mode_t mode);
void tmxc_sound_isolation_enable(uint8_t enable);
uint8_t tmxc_sound_isolation_is_enabled(void);
uint64_t tmxc_sound_isolation_get_samples_processed(void);
void tmxc_sound_isolation_cleanup(void);

#endif
