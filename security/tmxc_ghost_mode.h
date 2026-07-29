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
#ifndef TMXC_GHOST_MODE_H
#define TMXC_GHOST_MODE_H

#include "../kernel/tmxc_kernel.h"

#define TMXC_GHOST_MAX_DATA_REGIONS 64
#define TMXC_GHOST_ENCRYPTION_KEY_LENGTH 32
#define TMXC_GHOST_MASK_PATTERN_LENGTH 16
#define TMXC_GUEST_UI_FAKE_DATA_SIZE 1024

typedef enum {
    TMXC_GHOST_DATA_GALLERY = 0,
    TMXC_GHOST_DATA_CONTACTS = 1,
    TMXC_GHOST_DATA_NOTES = 2,
    TMXC_GHOST_DATA_MESSAGES = 3,
    TMXC_GHOST_DATA_CALLS = 4,
    TMXC_GHOST_DATA_BROWSER = 5,
    TMXC_GHOST_DATA_APPS = 6,
    TMXC_GHOST_DATA_FILES = 7
} tmxc_ghost_data_type_t;

typedef struct {
    uint64_t original_address;
    uint64_t masked_address;
    uint64_t size;
    tmxc_ghost_data_type_t data_type;
    uint8_t is_masked;
    uint8_t encryption_key[TMXC_GHOST_ENCRYPTION_KEY_LENGTH];
    uint64_t mask_timestamp;
    char original_path[256];
} tmxc_ghost_data_region_t;

typedef struct {
    uint8_t is_active;
    uint8_t encryption_mode;
    uint8_t guest_ui_loaded;
    uint8_t auth_required;
    uint8_t auth_hash[32];
    uint64_t activation_time;
    tmxc_ghost_data_region_t data_regions[TMXC_GHOST_MAX_DATA_REGIONS];
    uint32_t masked_region_count;
    uint8_t fake_ui_data[TMXC_GUEST_UI_FAKE_DATA_SIZE];
    uint32_t fake_ui_size;
} tmxc_ghost_mode_t;

void tmxc_ghost_mode_init(void);
void tmxc_ghost_mask_data_region(uint64_t address, uint64_t size, tmxc_ghost_data_type_t data_type, const char* path);
void tmxc_ghost_activate(const uint8_t* auth_data);
void tmxc_ghost_mask_gallery(void);
void tmxc_ghost_mask_contacts(void);
void tmxc_ghost_mask_notes(void);
void tmxc_ghost_load_guest_ui(void);
void tmxc_ghost_deactivate(const uint8_t* auth_data);
void tmxc_ghost_restore_data(void);
void tmxc_ghost_unload_guest_ui(void);
void tmxc_ghost_set_auth_hash(const uint8_t* hash);
void tmxc_ghost_enable(uint8_t enable);
uint8_t tmxc_ghost_is_active(void);
tmxc_ghost_mode_t* tmxc_ghost_get_status(void);
void tmxc_ghost_cleanup(void);

#endif
