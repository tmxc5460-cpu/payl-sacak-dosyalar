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
#ifndef TMXC_DYNAMIC_REFRESH_H
#define TMXC_DYNAMIC_REFRESH_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_REFRESH_MIN_HZ 1
#define TMXC_REFRESH_MAX_HZ 160

typedef enum {
    TMXC_REFRESH_MODE_STATIC = 0,
    TMXC_REFRESH_MODE_DYNAMIC = 1,
    TMXC_REFRESH_MODE_GAME = 2
} tmxc_refresh_mode_t;

void tmxc_dynamic_refresh_init(void);
void tmxc_dynamic_refresh_set_hz(uint32_t hz);
void tmxc_dynamic_refresh_update(void);
void tmxc_dynamic_refresh_auto_adjust(uint32_t fps);
void tmxc_dynamic_refresh_set_mode(tmxc_refresh_mode_t mode);
void tmxc_dynamic_refresh_enable(uint8_t enable);
uint32_t tmxc_dynamic_refresh_get_current_hz(void);
uint32_t tmxc_dynamic_refresh_get_target_hz(void);
void tmxc_dynamic_refresh_cleanup(void);

#endif
