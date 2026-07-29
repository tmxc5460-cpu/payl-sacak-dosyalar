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
#ifndef TMXC_VIRUS_SCANNER_H
#define TMXC_VIRUS_SCANNER_H

#include "../kernel/tmxc_kernel.h"

#define TMXC_VIRUS_SIGNATURE_DB_SIZE 2048
#define TMXC_VIRUS_MAX_SCAN_QUEUE 32
#define TMXC_VIRUS_SCAN_BLOCK_SIZE 4096

typedef struct {
    uint8_t signature[32];
    uint32_t signature_length;
    uint32_t threat_level;
    char threat_name[64];
    uint8_t active;
} tmxc_virus_signature_t;

typedef struct {
    const char* file_path;
    uint64_t file_size;
    uint32_t scan_status;
    uint8_t threat_detected;
    uint32_t threat_signature_index;
    uint64_t scan_start_time;
    uint64_t scan_end_time;
    uint8_t in_progress;
} tmxc_scan_job_t;

typedef enum {
    TMXC_SCAN_STATUS_PENDING = 0,
    TMXC_SCAN_STATUS_SCANNING = 1,
    TMXC_SCAN_STATUS_COMPLETED = 2,
    TMXC_SCAN_STATUS_THREAT_FOUND = 3,
    TMXC_SCAN_STATUS_ERROR = 4
} tmxc_scan_status_t;

typedef struct {
    tmxc_virus_signature_t signature_db[TMXC_VIRUS_SIGNATURE_DB_SIZE];
    uint32_t signature_count;
    tmxc_scan_job_t scan_queue[TMXC_VIRUS_MAX_SCAN_QUEUE];
    uint32_t queue_head;
    uint32_t queue_tail;
    uint8_t scanner_active;
    uint8_t hyper_mode_enabled;
    uint8_t atomic_scan_enabled;
    uint64_t total_files_scanned;
    uint64_t total_threats_found;
    uint64_t last_scan_time;
    uint32_t cpu_usage_cycles;
    uint8_t initialized;
} tmxc_virus_scanner_t;

void tmxc_virus_scanner_init(void);
void tmxc_virus_scanner_enable_hyper_mode(uint8_t enable);
void tmxc_virus_scanner_enable_atomic_scan(uint8_t enable);
uint8_t tmxc_virus_scanner_is_hyper_mode_enabled(void);
uint8_t tmxc_virus_scanner_is_atomic_scan_enabled(void);

uint32_t tmxc_virus_scanner_add_signature(const uint8_t* signature, uint32_t length, 
                                          uint32_t threat_level, const char* threat_name);
void tmxc_virus_scanner_remove_signature(uint32_t signature_index);

uint32_t tmxc_virus_scanner_queue_scan(const char* file_path, uint64_t file_size);
void tmxc_virus_scanner_process_queue(void);
uint8_t tmxc_virus_scanner_scan_file(const char* file_path);
uint8_t tmxc_virus_scanner_scan_memory(const uint8_t* data, uint32_t size);

uint8_t tmxc_virus_scanner_is_scan_complete(uint32_t job_id);
tmxc_scan_status_t tmxc_virus_scanner_get_scan_status(uint32_t job_id);
uint8_t tmxc_virus_scanner_was_threat_detected(uint32_t job_id);
const char* tmxc_virus_scanner_get_threat_name(uint32_t job_id);

void tmxc_virus_scanner_start_background_monitoring(void);
void tmxc_virus_scanner_stop_background_monitoring(void);
void tmxc_virus_scanner_on_file_download(const char* file_path, uint64_t file_size);

uint64_t tmxc_virus_scanner_get_total_files_scanned(void);
uint64_t tmxc_virus_scanner_get_total_threats_found(void);
uint32_t tmxc_virus_scanner_get_cpu_usage(void);

void tmxc_virus_scanner_cleanup(void);

#endif
