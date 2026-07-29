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
#ifndef TMXC_APP_FREEZER_H
#define TMXC_APP_FREEZER_H

#include "tmxc_kernel.h"

#define TMXC_MAX_FROZEN_APPS 32
#define TMXC_FREEZE_TIMEOUT_MS 300000

typedef enum {
    TMXC_APP_STATE_ACTIVE = 0,
    TMXC_APP_STATE_FROZEN = 1,
    TMXC_APP_STATE_SUSPENDED = 2
} tmxc_app_state_t;

void tmxc_app_freezer_init(void);
uint32_t tmxc_app_freeze(uint32_t pid, const char* app_name);
void tmxc_app_thaw(uint32_t pid);
void tmxc_app_freezer_check_inactive(void);
void tmxc_app_freezer_freeze_all_background(void);
void tmxc_app_freezer_thaw_all(void);
void tmxc_app_freezer_enable_auto_freeze(uint8_t enable);
uint32_t tmxc_app_freezer_get_frozen_count(void);
uint64_t tmxc_app_freezer_get_memory_saved(void);
void tmxc_app_freezer_cleanup(void);

#endif
