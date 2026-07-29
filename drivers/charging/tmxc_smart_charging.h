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
#ifndef TMXC_SMART_CHARGING_H
#define TMXC_SMART_CHARGING_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_BYPASS_THRESHOLD 100
#define TMXC_BYPASS_HYSTERESIS 98

typedef enum {
    TMXC_CHARGING_MODE_NORMAL = 0,
    TMXC_CHARGING_MODE_BYPASS = 1,
    TMXC_CHARGING_MODE_TRICKLE = 2
} tmxc_charging_mode_t;

void tmxc_smart_charging_init(void);
void tmxc_smart_charging_set_battery_level(uint8_t level);
void tmxc_smart_charging_enable_bypass(void);
void tmxc_smart_charging_disable_bypass(void);
void tmxc_smart_charging_set_connected(uint8_t connected);
void tmxc_smart_charging_enable_bypass_feature(uint8_t enable);
tmxc_charging_mode_t tmxc_smart_charging_get_mode(void);
uint8_t tmxc_smart_charging_is_bypass_active(void);
uint64_t tmxc_smart_charging_get_total_bypass_time(void);
void tmxc_smart_charging_cleanup(void);

#endif
