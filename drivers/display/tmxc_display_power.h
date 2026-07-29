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
#ifndef TMXC_DISPLAY_POWER_H
#define TMXC_DISPLAY_POWER_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_DISPLAY_POWER_BASE 0x1A010000
#define TMXC_POWER_CTRL_REG (TMXC_DISPLAY_POWER_BASE + 0x0000)
#define TMXC_POWER_STATUS_REG (TMXC_DISPLAY_POWER_BASE + 0x0004)
#define TMXC_BACKLIGHT_REG (TMXC_DISPLAY_POWER_BASE + 0x0008)
#define TMXC_VOLTAGE_REG (TMXC_DISPLAY_POWER_BASE + 0x000C)
#define TMXC_CURRENT_REG (TMXC_DISPLAY_POWER_BASE + 0x0010)
#define TMXC_TEMP_REG (TMXC_DISPLAY_POWER_BASE + 0x0014)

typedef enum {
    TMXC_POWER_STATE_OFF = 0,
    TMXC_POWER_STATE_STANDBY = 1,
    TMXC_POWER_STATE_ON = 2,
    TMXC_POWER_STATE_SLEEP = 3
} tmxc_power_state_t;

typedef struct {
    tmxc_power_state_t current_state;
    uint8_t backlight_level;
    uint32_t voltage_mv;
    uint32_t current_ma;
    uint32_t temperature_c;
    uint8_t power_initialized;
    uint8_t auto_dim_enabled;
    uint64_t last_activity_time;
} tmxc_display_power_t;

void tmxc_display_power_init(void);
void tmxc_display_power_set_state(tmxc_power_state_t state);
tmxc_power_state_t tmxc_display_power_get_state(void);
void tmxc_display_power_set_backlight(uint8_t level);
uint8_t tmxc_display_power_get_backlight(void);
void tmxc_display_power_enable_auto_dim(uint8_t enable);
void tmxc_display_power_monitor(void);
uint32_t tmxc_display_power_get_voltage(void);
uint32_t tmxc_display_power_get_current(void);
uint32_t tmxc_display_power_get_temperature(void);

#endif
