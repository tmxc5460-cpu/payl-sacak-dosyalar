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

static tmxc_compressed_page_t tmxc_compressed_pages[TMXC_COMPRESSION_MAX_PAGES];
static tmxc_compression_stats_t tmxc_compression_stats;
static uint8_t tmxc_ram_compression_enabled = 1;
static uint8_t tmxc_compression_algorithm = TMXC_COMPRESSION_ALGORITHM_LZ4;
static uint64_t tmxc_last_compression_check = 0;
static uint32_t tmxc_compression_threshold_percent = 70;

void tmxc_ram_compression_init(void) {
    for (uint32_t i = 0; i < TMXC_COMPRESSION_MAX_PAGES; i++) {
        tmxc_compressed_pages[i].original_address = 0;
        tmxc_compressed_pages[i].compressed_address = 0;
        tmxc_compressed_pages[i].original_size = 0;
        tmxc_compressed_pages[i].compressed_size = 0;
        tmxc_compressed_pages[i].pid = 0;
        tmxc_compressed_pages[i].is_compressed = 0;
        tmxc_compressed_pages[i].algorithm = 0;
        tmxc_compressed_pages[i].compression_time = 0;
        tmxc_compressed_pages[i].last_access_time = 0;
        tmxc_compressed_pages[i].access_count = 0;
    }
    
    tmxc_compression_stats.total_compressed_bytes = 0;
    tmxc_compression_stats.total_original_bytes = 0;
    tmxc_compression_stats.compression_savings = 0;
    tmxc_compression_stats.compressed_page_count = 0;
    tmxc_compression_stats.total_compression_time_ms = 0;
    tmxc_compression_stats.total_decompression_time_ms = 0;
    tmxc_compression_stats.compression_cycles = 0;
    tmxc_compression_stats.decompression_cycles = 0;
    
    tmxc_last_compression_check = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[RAM-COMP] RAM Compression Engine initialized\r\n");
}

static uint64_t tmxc_lz4_fast_compress(const uint8_t* src, uint64_t src_size, uint8_t* dst, uint64_t dst_size) {
    if (src == NULL || dst == NULL || src_size == 0 || dst_size == 0) {
        return 0;
    }
    
    uint64_t src_pos = 0;
    uint64_t dst_pos = 0;
    
    while (src_pos < src_size && dst_pos < dst_size - 8) {
        uint64_t literal_len = 0;
        uint64_t match_len = 0;
        uint64_t match_offset = 0;
        
        for (uint64_t offset = 4; offset <= 65535 && offset <= src_pos; offset++) {
            uint64_t len = 0;
            while (src_pos + len < src_size && 
                   src_pos + len - offset >= 0 &&
                   src[src_pos + len] == src[src_pos + len - offset] &&
                   len < 65535) {
                len++;
            }
            
            if (len > match_len) {
                match_len = len;
                match_offset = offset;
            }
        }
        
        if (match_len >= 4) {
            while (literal_len > 0 && dst_pos < dst_size) {
                dst[dst_pos++] = src[src_pos++];
                literal_len--;
            }
            
            if (dst_pos + 4 > dst_size) {
                break;
            }
            
            dst[dst_pos++] = (match_offset >> 8) & 0xFF;
            dst[dst_pos++] = match_offset & 0xFF;
            dst[dst_pos++] = (match_len - 4) & 0xFF;
            dst[dst_pos++] = ((match_len - 4) >> 8) & 0xFF;
            
            src_pos += match_len;
        } else {
            literal_len++;
            src_pos++;
            
            if (literal_len == 15) {
                if (dst_pos + 2 > dst_size) {
                    break;
                }
                dst[dst_pos++] = 15;
                dst[dst_pos++] = 0;
                literal_len = 0;
            }
        }
    }
    
    if (literal_len > 0 && dst_pos < dst_size) {
        dst[dst_pos++] = literal_len;
    }
    
    return dst_pos;
}

static uint64_t tmxc_lz4_fast_decompress(const uint8_t* src, uint64_t src_size, uint8_t* dst, uint64_t dst_size) {
    if (src == NULL || dst == NULL || src_size == 0 || dst_size == 0) {
        return 0;
    }
    
    uint64_t src_pos = 0;
    uint64_t dst_pos = 0;
    
    while (src_pos < src_size && dst_pos < dst_size) {
        uint8_t token = src[src_pos++];
        
        if (token < 16) {
            for (uint8_t i = 0; i <= token && dst_pos < dst_size; i++) {
                dst[dst_pos++] = src[src_pos++];
            }
        } else {
            if (src_pos + 3 > src_size) {
                break;
            }
            
            uint16_t match_offset = ((uint16_t)src[src_pos++] << 8) | src[src_pos++];
            uint16_t match_len = src[src_pos++];
            match_len |= ((uint16_t)src[src_pos++] << 8);
            match_len += 4;
            
            for (uint16_t i = 0; i < match_len && dst_pos < dst_size; i++) {
                if (dst_pos >= match_offset) {
                    dst[dst_pos] = dst[dst_pos - match_offset];
                }
                dst_pos++;
            }
        }
    }
    
    return dst_pos;
}

uint8_t tmxc_compress_page(uint64_t page_address, uint32_t pid) {
    if (!tmxc_ram_compression_enabled) {
        return 0;
    }
    
    uint64_t start_time = tmxc_get_cycle_count();
    uint64_t frequency = tmxc_get_frequency();
    
    for (uint32_t i = 0; i < TMXC_COMPRESSION_MAX_PAGES; i++) {
        if (tmxc_compressed_pages[i].original_address == page_address) {
            return 0;
        }
    }
    
    for (uint32_t i = 0; i < TMXC_COMPRESSION_MAX_PAGES; i++) {
        if (tmxc_compressed_pages[i].original_address == 0) {
            uint8_t* page_data = (uint8_t*)page_address;
            uint8_t* compressed_buffer = (uint8_t*)tmxc_malloc(TMXC_PAGE_SIZE);
            
            if (compressed_buffer == NULL) {
                return 0;
            }
            
            uint64_t compressed_size = 0;
            
            if (tmxc_compression_algorithm == TMXC_COMPRESSION_ALGORITHM_LZ4) {
                compressed_size = tmxc_lz4_fast_compress(page_data, TMXC_PAGE_SIZE, 
                                                        compressed_buffer, TMXC_PAGE_SIZE);
            }
            
            if (compressed_size > 0 && compressed_size < TMXC_PAGE_SIZE) {
                tmxc_compressed_pages[i].original_address = page_address;
                tmxc_compressed_pages[i].compressed_address = (uint64_t)compressed_buffer;
                tmxc_compressed_pages[i].original_size = TMXC_PAGE_SIZE;
                tmxc_compressed_pages[i].compressed_size = compressed_size;
                tmxc_compressed_pages[i].pid = pid;
                tmxc_compressed_pages[i].is_compressed = 1;
                tmxc_compressed_pages[i].algorithm = tmxc_compression_algorithm;
                tmxc_compressed_pages[i].compression_time = tmxc_get_cycle_count();
                tmxc_compressed_pages[i].last_access_time = tmxc_get_cycle_count();
                tmxc_compressed_pages[i].access_count = 0;
                
                tmxc_compression_stats.total_compressed_bytes += compressed_size;
                tmxc_compression_stats.total_original_bytes += TMXC_PAGE_SIZE;
                tmxc_compression_stats.compression_savings += (TMXC_PAGE_SIZE - compressed_size);
                tmxc_compression_stats.compressed_page_count++;
                tmxc_compression_stats.compression_cycles++;
                
                uint64_t end_time = tmxc_get_cycle_count();
                uint64_t compression_time_ms = ((end_time - start_time) * 1000) / frequency;
                tmxc_compression_stats.total_compression_time_ms += compression_time_ms;
                
                tmxc_uart_puts("[RAM-COMP] Compressed page at 0x");
                char hex_chars[] = "0123456789ABCDEF";
                char hex_buffer[17];
                hex_buffer[16] = '\0';
                uint64_t addr = page_address;
                for (int j = 15; j >= 0; j--) {
                    hex_buffer[j] = hex_chars[addr & 0xF];
                    addr >>= 4;
                }
                tmxc_uart_puts(hex_buffer);
                tmxc_uart_puts(", Ratio: ");
                uint64_t ratio = (compressed_size * 100) / TMXC_PAGE_SIZE;
                char buffer[21];
                int pos = 20;
                buffer[pos] = '\0';
                uint64_t temp = ratio;
                while (temp > 0 && pos > 0) {
                    pos--;
                    buffer[pos] = '0' + (temp % 10);
                    temp /= 10;
                }
                tmxc_uart_puts(&buffer[pos]);
                tmxc_uart_puts("%\r\n");
                
                return 1;
            } else {
                tmxc_free(compressed_buffer);
                return 0;
            }
        }
    }
    
    return 0;
}

uint8_t tmxc_decompress_page(uint64_t page_address) {
    if (!tmxc_ram_compression_enabled) {
        return 0;
    }
    
    uint64_t start_time = tmxc_get_cycle_count();
    uint64_t frequency = tmxc_get_frequency();
    
    for (uint32_t i = 0; i < TMXC_COMPRESSION_MAX_PAGES; i++) {
        if (tmxc_compressed_pages[i].original_address == page_address && 
            tmxc_compressed_pages[i].is_compressed) {
            
            uint8_t* compressed_data = (uint8_t*)tmxc_compressed_pages[i].compressed_address;
            uint8_t* page_data = (uint8_t*)page_address;
            
            uint64_t decompressed_size = 0;
            
            if (tmxc_compressed_pages[i].algorithm == TMXC_COMPRESSION_ALGORITHM_LZ4) {
                decompressed_size = tmxc_lz4_fast_decompress(compressed_data, 
                                                             tmxc_compressed_pages[i].compressed_size,
                                                             page_data, TMXC_PAGE_SIZE);
            }
            
            if (decompressed_size == TMXC_PAGE_SIZE) {
                tmxc_compressed_pages[i].last_access_time = tmxc_get_cycle_count();
                tmxc_compressed_pages[i].access_count++;
                tmxc_compression_stats.decompression_cycles++;
                
                uint64_t end_time = tmxc_get_cycle_count();
                uint64_t decompression_time_ms = ((end_time - start_time) * 1000) / frequency;
                tmxc_compression_stats.total_decompression_time_ms += decompression_time_ms;
                
                return 1;
            }
        }
    }
    
    return 0;
}

void tmxc_ram_compression_background_task(void) {
    if (!tmxc_ram_compression_enabled) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t frequency = tmxc_get_frequency();
    uint64_t elapsed_ms = ((current_time - tmxc_last_compression_check) * 1000) / frequency;
    
    if (elapsed_ms < TMXC_COMPRESSION_CHECK_INTERVAL_MS) {
        return;
    }
    
    tmxc_last_compression_check = current_time;
    
    tmxc_memory_info_t mem_info = tmxc_get_memory_info();
    uint64_t used_percent = (mem_info.total - mem_info.available) * 100 / mem_info.total;
    
    if (used_percent < tmxc_compression_threshold_percent) {
        return;
    }
    
    uint32_t compressed_count = 0;
    uint32_t target_pages = (TMXC_COMPRESSION_MAX_PAGES * 40) / 100;
    
    for (uint32_t i = 0; i < TMXC_COMPRESSION_MAX_PAGES && compressed_count < target_pages; i++) {
        if (tmxc_compressed_pages[i].is_compressed) {
            uint64_t access_age = current_time - tmxc_compressed_pages[i].last_access_time;
            uint64_t access_age_ms = (access_age * 1000) / frequency;
            
            if (access_age_ms > 30000 && tmxc_compressed_pages[i].access_count < 5) {
                tmxc_free((void*)tmxc_compressed_pages[i].compressed_address);
                tmxc_compressed_pages[i].original_address = 0;
                tmxc_compressed_pages[i].compressed_address = 0;
                tmxc_compressed_pages[i].is_compressed = 0;
                tmxc_compression_stats.compressed_page_count--;
                compressed_count++;
            }
        }
    }
}

tmxc_compression_stats_t* tmxc_ram_compression_get_stats(void) {
    return &tmxc_compression_stats;
}

void tmxc_ram_compression_enable(uint8_t enable) {
    tmxc_ram_compression_enabled = enable;
    tmxc_uart_puts("[RAM-COMP] RAM Compression ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

void tmxc_ram_compression_set_algorithm(uint8_t algorithm) {
    tmxc_compression_algorithm = algorithm;
    tmxc_uart_puts("[RAM-COMP] Algorithm set to ");
    tmxc_uart_puts(algorithm == TMXC_COMPRESSION_ALGORITHM_LZ4 ? "LZ4" : "ZSTD");
    tmxc_uart_puts("\r\n");
}

void tmxc_ram_compression_set_threshold(uint32_t percent) {
    if (percent > 0 && percent <= 100) {
        tmxc_compression_threshold_percent = percent;
    }
}

void tmxc_ram_compression_cleanup(void) {
    for (uint32_t i = 0; i < TMXC_COMPRESSION_MAX_PAGES; i++) {
        if (tmxc_compressed_pages[i].is_compressed) {
            tmxc_free((void*)tmxc_compressed_pages[i].compressed_address);
            tmxc_compressed_pages[i].original_address = 0;
            tmxc_compressed_pages[i].compressed_address = 0;
            tmxc_compressed_pages[i].is_compressed = 0;
        }
    }
    
    tmxc_compression_stats.total_compressed_bytes = 0;
    tmxc_compression_stats.total_original_bytes = 0;
    tmxc_compression_stats.compression_savings = 0;
    tmxc_compression_stats.compressed_page_count = 0;
    
    tmxc_uart_puts("[RAM-COMP] RAM Compression cleaned up\r\n");
}
