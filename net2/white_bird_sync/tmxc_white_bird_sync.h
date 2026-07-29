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
#ifndef TMXC_WHITE_BIRD_SYNC_H
#define TMXC_WHITE_BIRD_SYNC_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_SYNC_MAX_DEVICES 4
#define TMXC_SYNC_MAX_HISTORY 100
#define TMXC_SYNC_BUFFER_SIZE 4096
#define TMXC_SYNC_DEVICE_NAME_LENGTH 32

typedef enum {
    TMXC_DEVICE_TYPE_PHONE = 0,
    TMXC_DEVICE_TYPE_TABLET = 1
} tmxc_device_type_t;

typedef enum {
    TMXC_SYNC_STATUS_DISCONNECTED = 0,
    TMXC_SYNC_STATUS_CONNECTING = 1,
    TMXC_SYNC_STATUS_CONNECTED = 2,
    TMXC_SYNC_STATUS_SYNCING = 3,
    TMXC_SYNC_STATUS_ERROR = 4
} tmxc_sync_status_t;

typedef struct {
    char device_name[TMXC_SYNC_DEVICE_NAME_LENGTH];
    char device_id[64];
    tmxc_device_type_t device_type;
    tmxc_sync_status_t status;
    uint64_t last_sync_time;
    uint64_t last_seen_time;
    uint8_t is_primary;
    uint8_t active;
} tmxc_sync_device_t;

typedef struct {
    char query[256];
    char result[512];
    uint64_t timestamp;
    uint8_t synced;
} tmxc_sync_history_entry_t;

typedef struct {
    tmxc_sync_device_t devices[TMXC_SYNC_MAX_DEVICES];
    uint32_t device_count;
    
    tmxc_sync_history_entry_t history[TMXC_SYNC_MAX_HISTORY];
    uint32_t history_index;
    uint32_t history_count;
    
    uint8_t sync_enabled;
    uint8_t auto_sync_enabled;
    uint8_t initialized;
    uint64_t total_syncs;
    uint64_t successful_syncs;
    
    char local_device_id[64];
    tmxc_device_type_t local_device_type;
} tmxc_white_bird_sync_t;

void tmxc_white_bird_sync_init(void);
void tmxc_white_bird_sync_set_local_device(const char* device_id, tmxc_device_type_t device_type);

void tmxc_white_bird_sync_enable_sync(uint8_t enable);
void tmxc_white_bird_sync_enable_auto_sync(uint8_t enable);
uint8_t tmxc_white_bird_sync_is_sync_enabled(void);
uint8_t tmxc_white_bird_sync_is_auto_sync_enabled(void);

uint32_t tmxc_white_bird_sync_add_device(const char* device_name, const char* device_id, tmxc_device_type_t device_type);
void tmxc_white_bird_sync_remove_device(const char* device_id);
void tmxc_white_bird_sync_set_primary_device(const char* device_id);

tmxc_sync_status_t tmxc_white_bird_sync_get_device_status(const char* device_id);
void tmxc_white_bird_sync_connect_device(const char* device_id);
void tmxc_white_bird_sync_disconnect_device(const char* device_id);

void tmxc_white_bird_sync_add_history(const char* query, const char* result);
void tmxc_white_bird_sync_sync_history(void);
tmxc_sync_history_entry_t* tmxc_white_bird_sync_get_history(uint32_t index);

void tmxc_white_bird_sync_sync_query(const char* query, const char* result);
void tmxc_white_bird_sync_process_pending_syncs(void);

uint64_t tmxc_white_bird_sync_get_total_syncs(void);
uint64_t tmxc_white_bird_sync_get_successful_syncs(void);
float tmxc_white_bird_sync_get_sync_success_rate(void);

void tmxc_white_bird_sync_cleanup(void);

#endif
