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

#define TMXC_ENTROPY_POOL_SIZE 256
#define TMXC_ENTROPY_MIN_THRESHOLD 128

typedef struct {
    uint8_t entropy_pool[TMXC_ENTROPY_POOL_SIZE];
    uint32_t pool_index;
    uint32_t entropy_count;
    uint8_t initialized;
    uint64_t total_bytes_generated;
    uint64_t last_noise_sample;
} tmxc_quantum_rng_t;

static tmxc_quantum_rng_t tmxc_qrng;

void tmxc_quantum_rng_init(void) {
    tmxc_qrng.initialized = 0;
    tmxc_qrng.pool_index = 0;
    tmxc_qrng.entropy_count = 0;
    tmxc_qrng.total_bytes_generated = 0;
    tmxc_qrng.last_noise_sample = 0;
    
    for (uint32_t i = 0; i < TMXC_ENTROPY_POOL_SIZE; i++) {
        tmxc_qrng.entropy_pool[i] = 0;
    }
    
    tmxc_qrng.initialized = 1;
    
    tmxc_uart_puts("[QRNG] Quantum RNG initialized\r\n");
}

uint8_t tmxc_qrng_collect_thermal_noise(void) {
    if (!tmxc_qrng.initialized) {
        return 0;
    }
    
    uint64_t cycles = tmxc_get_cycle_count();
    uint64_t noise = cycles ^ (cycles >> 32);
    
    uint8_t entropy_byte = (uint8_t)(noise & 0xFF);
    
    return entropy_byte;
}

uint8_t tmxc_qrng_collect_rf_noise(void) {
    if (!tmxc_qrng.initialized) {
        return 0;
    }
    
    uint64_t timestamp = tmxc_get_uptime();
    uint8_t entropy_byte = (uint8_t)(timestamp & 0xFF);
    
    return entropy_byte;
}

void tmxc_qrng_add_entropy(const uint8_t* data, uint32_t size) {
    if (!tmxc_qrng.initialized || data == NULL || size == 0) {
        return;
    }
    
    for (uint32_t i = 0; i < size && tmxc_qrng.entropy_count < TMXC_ENTROPY_POOL_SIZE; i++) {
        tmxc_qrng.entropy_pool[tmxc_qrng.pool_index] = data[i];
        tmxc_qrng.pool_index = (tmxc_qrng.pool_index + 1) % TMXC_ENTROPY_POOL_SIZE;
        tmxc_qrng.entropy_count++;
    }
}

void tmxc_qrng_collect_hardware_noise(void) {
    if (!tmxc_qrng.initialized) {
        return;
    }
    
    uint8_t thermal = tmxc_qrng_collect_thermal_noise();
    uint8_t rf = tmxc_qrng_collect_rf_noise();
    
    uint8_t combined = thermal ^ rf;
    
    if (tmxc_qrng.entropy_count < TMXC_ENTROPY_POOL_SIZE) {
        tmxc_qrng.entropy_pool[tmxc_qrng.pool_index] = combined;
        tmxc_qrng.pool_index = (tmxc_qrng.pool_index + 1) % TMXC_ENTROPY_POOL_SIZE;
        tmxc_qrng.entropy_count++;
    }
}

uint8_t tmxc_qrng_get_random_byte(void) {
    if (!tmxc_qrng.initialized) {
        return 0;
    }
    
    if (tmxc_qrng.entropy_count < TMXC_ENTROPY_MIN_THRESHOLD) {
        tmxc_qrng_collect_hardware_noise();
    }
    
    if (tmxc_qrng.entropy_count == 0) {
        return 0;
    }
    
    uint32_t read_index = (tmxc_qrng.pool_index - tmxc_qrng.entropy_count + TMXC_ENTROPY_POOL_SIZE) % TMXC_ENTROPY_POOL_SIZE;
    uint8_t random_byte = tmxc_qrng.entropy_pool[read_index];
    
    tmxc_qrng.entropy_count--;
    tmxc_qrng.total_bytes_generated++;
    
    return random_byte;
}

uint32_t tmxc_qrng_get_random_uint32(void) {
    if (!tmxc_qrng.initialized) {
        return 0;
    }
    
    uint32_t random_value = 0;
    for (int i = 0; i < 4; i++) {
        random_value = (random_value << 8) | tmxc_qrng_get_random_byte();
    }
    
    return random_value;
}

uint64_t tmxc_qrng_get_random_uint64(void) {
    if (!tmxc_qrng.initialized) {
        return 0;
    }
    
    uint64_t random_value = 0;
    for (int i = 0; i < 8; i++) {
        random_value = (random_value << 8) | tmxc_qrng_get_random_byte();
    }
    
    return random_value;
}

void tmxc_qrng_fill_buffer(uint8_t* buffer, uint32_t size) {
    if (!tmxc_qrng.initialized || buffer == NULL || size == 0) {
        return;
    }
    
    for (uint32_t i = 0; i < size; i++) {
        buffer[i] = tmxc_qrng_get_random_byte();
    }
}

uint32_t tmxc_qrng_get_entropy_count(void) {
    return tmxc_qrng.entropy_count;
}

uint64_t tmxc_qrng_get_total_generated(void) {
    return tmxc_qrng.total_bytes_generated;
}

void tmxc_quantum_rng_cleanup(void) {
    if (!tmxc_qrng.initialized) {
        return;
    }
    
    for (uint32_t i = 0; i < TMXC_ENTROPY_POOL_SIZE; i++) {
        tmxc_qrng.entropy_pool[i] = 0;
    }
    
    tmxc_qrng.entropy_count = 0;
    tmxc_qrng.initialized = 0;
    
    tmxc_uart_puts("[QRNG] Quantum RNG cleaned up\r\n");
}
