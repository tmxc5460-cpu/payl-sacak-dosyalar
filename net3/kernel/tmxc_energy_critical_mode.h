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
#ifndef TMXC_ENERGY_CRITICAL_MODE_H
#define TMXC_ENERGY_CRITICAL_MODE_H

#include "tmxc_kernel.h"

#define TMXC_ENERGY_CRITICAL_THRESHOLD 1
#define TMXC_ENERGY_WARNING_THRESHOLD 5
#define TMXC_ENERGY_TARGET_HOURS 24
#define TMXC_ENERGY_TEXT_BUFFER_SIZE 4096
#define TMXC_ENERGY_MAX_MESSAGES 64

typedef enum {
    TMXC_ENERGY_MODE_NORMAL = 0,
    TMXC_ENERGY_MODE_POWER_SAVE = 1,
    TMXC_ENERGY_MODE_CRITICAL = 2
} tmxc_energy_mode_t;

typedef struct {
    uint8_t message[TMXC_ENERGY_TEXT_BUFFER_SIZE];
    uint32_t size;
    uint64_t timestamp;
    uint8_t is_from_ai;
} tmxc_text_message_t;

typedef struct {
    tmxc_energy_mode_t current_mode;
    uint8_t battery_level;
    uint64_t mode_switch_time;
    uint64_t critical_mode_start_time;
    uint32_t hours_remaining;
    uint8_t gui_disabled;
    uint8_t text_only_mode;
    uint8_t ai_text_only;
    uint32_t cpu_frequency;
    uint32_t screen_brightness;
    uint8_t network_disabled;
    uint8_t bluetooth_disabled;
    uint8_t wifi_disabled;
    uint8_t sensors_disabled;
    tmxc_text_message_t messages[TMXC_ENERGY_MAX_MESSAGES];
    uint32_t message_count;
} tmxc_energy_critical_t;

void tmxc_energy_critical_mode_init(void);
void tmxc_energy_check_battery(void);
void tmxc_energy_enter_critical_mode(void);
void tmxc_energy_exit_critical_mode(void);
void tmxc_energy_disable_gui(void);
void tmxc_energy_enable_gui(void);
void tmxc_energy_reduce_cpu_frequency(void);
void tmxc_energy_restore_cpu_frequency(void);
void tmxc_energy_disable_screen(void);
void tmxc_energy_enable_screen(void);
void tmxc_energy_disable_peripherals(void);
void tmxc_energy_enable_peripherals(void);
void tmxc_energy_calculate_remaining_time(void);
void tmxc_energy_add_text_message(const char* message, uint8_t is_from_ai);
void tmxc_energy_display_text_messages(void);
void tmxc_energy_enable(uint8_t enable);
tmxc_energy_mode_t tmxc_energy_get_mode(void);
uint8_t tmxc_energy_get_battery_level(void);
uint8_t tmxc_energy_is_text_only_mode(void);
tmxc_energy_critical_t* tmxc_energy_get_status(void);
void tmxc_energy_cleanup(void);

#endif
