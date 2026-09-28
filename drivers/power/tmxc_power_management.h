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
#ifndef TMXC_POWER_MANAGEMENT_H
#define TMXC_POWER_MANAGEMENT_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_HARD_BLACK_BATTERY_THRESHOLD 5
#define TMXC_HARD_BLACK_DURATION_MS (3 * 24 * 60 * 60 * 1000)
#define TMXC_PASSIVE_CHARGING_MAX_GAIN 50

typedef enum {
    TMXC_POWER_MODE_NORMAL = 0,
    TMXC_POWER_MODE_POWER_SAVE = 1,
    TMXC_POWER_MODE_HARD_BLACK = 2,
    TMXC_POWER_MODE_ULTRA_LOW = 3
} tmxc_power_mode_t;

void tmxc_power_management_init(void);
void tmxc_power_set_battery_level(uint8_t level);
void tmxc_power_enter_hard_black_mode(void);
void tmxc_power_exit_hard_black_mode(void);
void tmxc_power_enable_passive_charging(uint8_t enable);
void tmxc_power_harvest_light_energy(uint64_t lux_value);
void tmxc_power_harvest_rf_energy(uint64_t signal_strength_dbm);
void tmxc_power_harvest_thermal_energy(uint64_t temperature_c);
uint64_t tmxc_power_get_harvested_energy(void);
void tmxc_power_set_mode(tmxc_power_mode_t mode);
tmxc_power_mode_t tmxc_power_get_mode(void);
uint8_t tmxc_power_get_battery_level(void);
uint8_t tmxc_power_is_hard_black_active(void);
void tmxc_power_update(void);
void tmxc_power_cleanup(void);

#endif
