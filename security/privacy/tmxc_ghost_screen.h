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
#ifndef TMXC_GHOST_SCREEN_H
#define TMXC_GHOST_SCREEN_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_VIEW_ANGLE_THRESHOLD 30
#define TMXC_PIXEL_MASK_SIZE 64

typedef enum {
    TMXC_GHOST_MODE_OFF = 0,
    TMXC_GHOST_MODE_PRIVACY = 1,
    TMXC_GHOST_MODE_STEALTH = 2
} tmxc_ghost_mode_t;

void tmxc_ghost_screen_init(void);
void tmxc_ghost_screen_update_viewing_angle(double angle);
void tmxc_ghost_screen_generate_pixel_mask(void);
void tmxc_ghost_screen_set_mode(tmxc_ghost_mode_t mode);
void tmxc_ghost_screen_enable(uint8_t enable);
uint8_t tmxc_ghost_screen_is_privacy_active(void);
double tmxc_ghost_screen_get_viewing_angle(void);
void tmxc_ghost_screen_cleanup(void);

#endif
