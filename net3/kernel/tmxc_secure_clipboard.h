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
#ifndef TMXC_SECURE_CLIPBOARD_H
#define TMXC_SECURE_CLIPBOARD_H

#include "tmxc_kernel.h"

#define TMXC_CLIPBOARD_MAX_SIZE 1048576
#define TMXC_CLIPBOARD_MAX_ENTRIES 16
#define TMXC_CLIPBOARD_AUTHORIZED_APPS 32
#define TMXC_CLIPBOARD_ENCRYPTION_KEY_LENGTH 32

typedef enum {
    TMXC_CLIPBOARD_TYPE_TEXT = 0,
    TMXC_CLIPBOARD_TYPE_IMAGE = 1,
    TMXC_CLIPBOARD_TYPE_FILE = 2,
    TMXC_CLIPBOARD_TYPE_URL = 3,
    TMXC_CLIPBOARD_TYPE_CUSTOM = 4
} tmxc_clipboard_type_t;

typedef struct {
    uint8_t data[TMXC_CLIPBOARD_MAX_SIZE];
    uint64_t size;
    tmxc_clipboard_type_t type;
    uint64_t timestamp;
    uint32_t source_app_id;
    uint8_t is_encrypted;
    uint8_t encryption_key[TMXC_CLIPBOARD_ENCRYPTION_KEY_LENGTH];
    uint8_t iv[16];
    uint32_t checksum;
} tmxc_clipboard_entry_t;

typedef struct {
    uint32_t app_id;
    char app_name[64];
    uint8_t is_authorized;
    uint64_t authorization_time;
    uint8_t access_level;
} tmxc_clipboard_auth_app_t;

void tmxc_secure_clipboard_init(void);
uint8_t tmxc_clipboard_set_data(const uint8_t* data, uint64_t size, tmxc_clipboard_type_t type, uint32_t source_app_id);
uint8_t tmxc_clipboard_get_data(uint8_t* buffer, uint64_t buffer_size, uint64_t* actual_size, uint32_t requesting_app_id);
void tmxc_clipboard_authorize_app(uint32_t app_id, const char* app_name, uint8_t access_level);
uint8_t tmxc_clipboard_is_app_authorized(uint32_t app_id);
void tmxc_clipboard_revoke_app(uint32_t app_id);
void tmxc_clipboard_clear(void);
void tmxc_secure_clipboard_enable(uint8_t enable);
void tmxc_clipboard_encryption_enable(uint8_t enable);
void tmxc_secure_clipboard_cleanup(void);

#endif
