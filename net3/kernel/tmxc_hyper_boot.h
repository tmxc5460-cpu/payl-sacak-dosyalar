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
#ifndef TMXC_HYPER_BOOT_H
#define TMXC_HYPER_BOOT_H

#include "tmxc_kernel.h"

#define TMXC_HYPER_BOOT_SNAPSHOT_MAGIC 0x4859504552
#define TMXC_HYPER_BOOT_MAX_SNAPSHOTS 4
#define TMXC_HYPER_BOOT_SNAPSHOT_SIZE (512 * 1024 * 1024)
#define TMXC_HYPER_BOOT_TARGET_TIME_MS 3000

typedef enum {
    TMXC_SNAPSHOT_STATE_INVALID = 0,
    TMXC_SNAPSHOT_STATE_VALID = 1,
    TMXC_SNAPSHOT_STATE_CORRUPTED = 2,
    TMXC_SNAPSHOT_STATE_LOADING = 3
} tmxc_snapshot_state_t;

typedef struct {
    uint64_t magic;
    uint32_t snapshot_id;
    tmxc_snapshot_state_t state;
    uint64_t timestamp;
    uint64_t memory_base;
    uint64_t memory_size;
    uint64_t compressed_size;
    uint32_t checksum;
    uint64_t boot_time_ms;
    uint8_t is_compressed;
} tmxc_snapshot_header_t;

typedef struct {
    tmxc_snapshot_header_t header;
    uint8_t* snapshot_data;
    uint64_t data_size;
    uint8_t is_loaded;
    uint64_t load_time_ms;
} tmxc_snapshot_t;

void tmxc_hyper_boot_init(void);
uint8_t tmxc_hyper_boot_create_snapshot(void);
uint8_t tmxc_hyper_boot_load_snapshot(uint32_t snapshot_id);
uint8_t tmxc_hyper_boot_restore_from_snapshot(void);
void tmxc_hyper_boot_enable(uint8_t enable);
uint8_t tmxc_hyper_boot_is_enabled(void);
tmxc_snapshot_t* tmxc_hyper_boot_get_snapshot(uint32_t snapshot_id);
void tmxc_hyper_boot_cleanup(void);

#endif
