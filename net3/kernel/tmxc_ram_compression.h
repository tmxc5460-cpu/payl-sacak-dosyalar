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
#ifndef TMXC_RAM_COMPRESSION_H
#define TMXC_RAM_COMPRESSION_H

#include "tmxc_kernel.h"

#define TMXC_COMPRESSION_MAX_PAGES 4096
#define TMXC_COMPRESSION_TARGET_RATIO 40
#define TMXC_COMPRESSION_CHECK_INTERVAL_MS 5000
#define TMXC_COMPRESSION_ALGORITHM_LZ4 0
#define TMXC_COMPRESSION_ALGORITHM_ZSTD 1

typedef struct {
    uint64_t original_address;
    uint64_t compressed_address;
    uint64_t original_size;
    uint64_t compressed_size;
    uint32_t pid;
    uint8_t is_compressed;
    uint8_t algorithm;
    uint64_t compression_time;
    uint64_t last_access_time;
    uint32_t access_count;
} tmxc_compressed_page_t;

typedef struct {
    uint64_t total_compressed_bytes;
    uint64_t total_original_bytes;
    uint64_t compression_savings;
    uint32_t compressed_page_count;
    uint64_t total_compression_time_ms;
    uint64_t total_decompression_time_ms;
    uint32_t compression_cycles;
    uint32_t decompression_cycles;
} tmxc_compression_stats_t;

void tmxc_ram_compression_init(void);
uint8_t tmxc_compress_page(uint64_t page_address, uint32_t pid);
uint8_t tmxc_decompress_page(uint64_t page_address);
void tmxc_ram_compression_background_task(void);
tmxc_compression_stats_t* tmxc_ram_compression_get_stats(void);
void tmxc_ram_compression_enable(uint8_t enable);
void tmxc_ram_compression_set_algorithm(uint8_t algorithm);
void tmxc_ram_compression_set_threshold(uint32_t percent);
void tmxc_ram_compression_cleanup(void);

#endif
