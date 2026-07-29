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
#ifndef TMXC_HARDWARE_RECOVERY_H
#define TMXC_HARDWARE_RECOVERY_H

#include "tmxc_kernel.h"

#define TMXC_RECOVERY_FIRMWARE_BASE 0x80000000
#define TMXC_RECOVERY_FIRMWARE_SIZE (16 * 1024 * 1024)
#define TMXC_RECOVERY_RAM_BASE 0x80000000
#define TMXC_RECOVERY_CHECKSUM_OFFSET (TMXC_RECOVERY_FIRMWARE_SIZE - 4)
#define TMXC_RECOVERY_MAGIC 0x52435652
#define TMXC_RECOVERY_TRIGGER_GPIO 42

typedef enum {
    TMXC_RECOVERY_STATE_IDLE = 0,
    TMXC_RECOVERY_STATE_DETECTING = 1,
    TMXC_RECOVERY_STATE_LOADING = 2,
    TMXC_RECOVERY_STATE_VERIFYING = 3,
    TMXC_RECOVERY_STATE_RESTORING = 4,
    TMXC_RECOVERY_STATE_COMPLETE = 5,
    TMXC_RECOVERY_STATE_ERROR = 6
} tmxc_recovery_state_t;

typedef struct {
    uint32_t magic;
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t version_patch;
    uint32_t firmware_size;
    uint32_t checksum;
    uint64_t build_timestamp;
    uint8_t is_valid;
} tmxc_recovery_header_t;

typedef struct {
    tmxc_recovery_state_t state;
    tmxc_recovery_header_t header;
    uint64_t trigger_time;
    uint64_t load_start_time;
    uint64_t load_end_time;
    uint64_t restore_start_time;
    uint64_t restore_end_time;
    uint32_t bytes_loaded;
    uint32_t bytes_restored;
    uint8_t trigger_detected;
    uint8_t recovery_enabled;
    uint8_t auto_recovery;
} tmxc_hardware_recovery_t;

void tmxc_hardware_recovery_init(void);
void tmxc_recovery_detect_trigger(void);
void tmxc_recovery_start(void);
void tmxc_recovery_read_firmware_header(void);
void tmxc_recovery_load_firmware(void);
void tmxc_recovery_verify_firmware(void);
void tmxc_recovery_restore_firmware(void);
void tmxc_recovery_manual_trigger(void);
void tmxc_recovery_enable(uint8_t enable);
void tmxc_recovery_set_auto_recovery(uint8_t enable);
tmxc_recovery_state_t tmxc_recovery_get_state(void);
tmxc_recovery_header_t* tmxc_recovery_get_header(void);
uint8_t tmxc_recovery_is_trigger_detected(void);
void tmxc_recovery_reset_trigger(void);
void tmxc_hardware_recovery_cleanup(void);

#endif
