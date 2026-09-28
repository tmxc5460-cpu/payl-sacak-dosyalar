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
#ifndef TMXC_DISPLAY_REFRESH_H
#define TMXC_DISPLAY_REFRESH_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_REFRESH_60HZ 60
#define TMXC_REFRESH_120HZ 120
#define TMXC_REFRESH_160HZ 160
#define TMXC_REFRESH_MAX 160

typedef enum {
    TMXC_REFRESH_MODE_60HZ = 0,
    TMXC_REFRESH_MODE_120HZ = 1,
    TMXC_REFRESH_MODE_160HZ = 2,
    TMXC_REFRESH_MODE_ADAPTIVE = 3
} tmxc_refresh_mode_t;

typedef struct {
    uint32_t current_refresh_rate;
    tmxc_refresh_mode_t current_mode;
    uint32_t target_refresh_rate;
    uint8_t adaptive_enabled;
    uint32_t frame_count;
    uint64_t last_frame_time;
    uint32_t actual_fps;
    uint8_t vsync_enabled;
    uint8_t refresh_initialized;
} tmxc_display_refresh_t;

void tmxc_display_refresh_init(void);
void tmxc_display_refresh_set_mode(tmxc_refresh_mode_t mode);
void tmxc_display_refresh_set_rate(uint32_t rate);
uint32_t tmxc_display_refresh_get_rate(void);
void tmxc_display_refresh_enable_adaptive(uint8_t enable);
void tmxc_display_refresh_enable_vsync(uint8_t enable);
void tmxc_display_refresh_update(void);
uint32_t tmxc_display_refresh_get_actual_fps(void);

#endif
