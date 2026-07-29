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
#ifndef TMXC_MULTI_ACCOUNT_H
#define TMXC_MULTI_ACCOUNT_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_MAX_PROFILES 10
#define TMXC_PROFILE_NAME_LENGTH 32
#define TMXC_PROFILE_ENCRYPTION_KEY_LENGTH 32
#define TMXC_GUEST_PROFILE_ID 0xFF

typedef enum {
    TMXC_PROFILE_STATE_INACTIVE = 0,
    TMXC_PROFILE_STATE_ACTIVE = 1,
    TMXC_PROFILE_STATE_LOCKED = 2,
    TMXC_PROFILE_STATE_GUEST = 3
} tmxc_profile_state_t;

typedef struct {
    uint8_t profile_id;
    char profile_name[TMXC_PROFILE_NAME_LENGTH];
    uint8_t encryption_key[TMXC_PROFILE_ENCRYPTION_KEY_LENGTH];
    uint64_t storage_offset;
    uint64_t storage_size;
    uint32_t gallery_count;
    uint32_t app_count;
    tmxc_profile_state_t state;
    uint64_t last_access_time;
    uint8_t biometric_hash[32];
    uint8_t pin_hash[16];
    uint8_t is_isolated;
    uint8_t data_hidden;
} tmxc_profile_t;

void tmxc_multi_account_init(void);
uint8_t tmxc_create_profile(const char* profile_name, const uint8_t* encryption_key);
uint8_t tmxc_switch_profile(uint8_t profile_id, const uint8_t* auth_data);
void tmxc_enable_guest_mode(void);
void tmxc_disable_guest_mode(void);
uint8_t tmxc_isolate_profile_data(uint8_t profile_id);
uint8_t tmxc_hide_profile_data(uint8_t profile_id);
uint8_t tmxc_show_profile_data(uint8_t profile_id);
uint8_t tmxc_delete_profile(uint8_t profile_id);
uint8_t tmxc_get_current_profile_id(void);
tmxc_profile_t* tmxc_get_profile(uint8_t profile_id);
uint8_t tmxc_is_guest_mode_active(void);
uint8_t tmxc_is_data_isolated(void);
uint32_t tmxc_get_total_profiles(void);
void tmxc_multi_account_cleanup(void);

#endif
