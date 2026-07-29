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
#ifndef TMXC_LOCALIZATION_H
#define TMXC_LOCALIZATION_H

#include "tmxc_kernel.h"

#define TMXC_MAX_LANGUAGES 32
#define TMXC_LANGUAGE_CODE_LENGTH 3
#define TMXC_MAX_STRING_LENGTH 128

typedef struct {
    char code[TMXC_LANGUAGE_CODE_LENGTH];
    char name[32];
    char native_name[32];
    uint8_t rtl;
} tmxc_language_t;

typedef struct {
    char welcome_message[TMXC_MAX_STRING_LENGTH];
    char setup_wizard_title[TMXC_MAX_STRING_LENGTH];
    char ecosystem_sync_title[TMXC_MAX_STRING_LENGTH];
    char ecosystem_sync_desc[TMXC_MAX_STRING_LENGTH];
    char security_setup_title[TMXC_MAX_STRING_LENGTH];
    char security_setup_desc[TMXC_MAX_STRING_LENGTH];
    char network_stealth_title[TMXC_MAX_STRING_LENGTH];
    char network_stealth_desc[TMXC_MAX_STRING_LENGTH];
    char calibration_title[TMXC_MAX_STRING_LENGTH];
    char calibration_desc[TMXC_MAX_STRING_LENGTH];
    char complete_title[TMXC_MAX_STRING_LENGTH];
    char complete_message[TMXC_MAX_STRING_LENGTH];
    char next_button[TMXC_MAX_STRING_LENGTH];
    char back_button[TMXC_MAX_STRING_LENGTH];
    char skip_button[TMXC_MAX_STRING_LENGTH];
    char stealth_mode[TMXC_MAX_STRING_LENGTH];
    char full_speed[TMXC_MAX_STRING_LENGTH];
    char balanced[TMXC_MAX_STRING_LENGTH];
} tmxc_localization_strings_t;

typedef struct {
    tmxc_language_t languages[TMXC_MAX_LANGUAGES];
    uint32_t language_count;
    uint32_t current_language_index;
    uint8_t initialized;
    uint8_t instant_translation_enabled;
} tmxc_localization_t;

void tmxc_localization_init(void);
void tmxc_localization_add_language(const char* code, const char* name, const char* native_name, uint8_t rtl);
void tmxc_localization_set_language(const char* code);
tmxc_language_t* tmxc_localization_get_current_language(void);
tmxc_language_t* tmxc_localization_get_language_by_code(const char* code);
tmxc_language_t* tmxc_localization_get_all_languages(void);
uint32_t tmxc_localization_get_language_count(void);
void tmxc_localization_enable_instant_translation(uint8_t enable);
uint8_t tmxc_localization_is_instant_translation_enabled(void);
const char* tmxc_localization_get_string(const char* key);

#endif
