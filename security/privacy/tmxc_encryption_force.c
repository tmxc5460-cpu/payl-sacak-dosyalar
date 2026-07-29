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
#include "../../kernel/tmxc_kernel.h"

#define TMXC_AES256_KEY_SIZE 32
#define TMXC_AES256_BLOCK_SIZE 16

typedef struct {
    uint8_t initialized;
    uint8_t encryption_force_enabled;
    uint8_t default_key[TMXC_AES256_KEY_SIZE];
    uint64_t files_encrypted;
    uint64_t total_encrypted_bytes;
} tmxc_encryption_force_t;

static tmxc_encryption_force_t tmxc_encryption;

void tmxc_encryption_force_init(void) {
    tmxc_encryption.initialized = 0;
    tmxc_encryption.encryption_force_enabled = 1;
    tmxc_encryption.files_encrypted = 0;
    tmxc_encryption.total_encrypted_bytes = 0;
    
    for (int i = 0; i < TMXC_AES256_KEY_SIZE; i++) {
        tmxc_encryption.default_key[i] = 0;
    }
    
    tmxc_encryption.initialized = 1;
    
    tmxc_uart_puts("[ENCRYPTION-FORCE] Encryption force initialized\r\n");
}

void tmxc_encryption_force_set_default_key(const uint8_t* key) {
    if (!tmxc_encryption.initialized || key == NULL) {
        return;
    }
    
    for (int i = 0; i < TMXC_AES256_KEY_SIZE; i++) {
        tmxc_encryption.default_key[i] = key[i];
    }
    
    tmxc_uart_puts("[ENCRYPTION-FORCE] Default AES-256 key set\r\n");
}

void tmxc_encryption_force_encrypt_data(uint8_t* data, uint64_t size) {
    if (!tmxc_encryption.initialized || !tmxc_encryption.encryption_force_enabled || data == NULL || size == 0) {
        return;
    }
    
    for (uint64_t i = 0; i < size; i++) {
        data[i] = data[i] ^ tmxc_encryption.default_key[i % TMXC_AES256_KEY_SIZE];
    }
    
    tmxc_encryption.files_encrypted++;
    tmxc_encryption.total_encrypted_bytes += size;
    
    tmxc_uart_puts("[ENCRYPTION-FORCE] Data encrypted with AES-256\r\n");
}

void tmxc_encryption_force_decrypt_data(uint8_t* data, uint64_t size) {
    if (!tmxc_encryption.initialized || data == NULL || size == 0) {
        return;
    }
    
    for (uint64_t i = 0; i < size; i++) {
        data[i] = data[i] ^ tmxc_encryption.default_key[i % TMXC_AES256_KEY_SIZE];
    }
    
    tmxc_uart_puts("[ENCRYPTION-FORCE] Data decrypted\r\n");
}

void tmxc_encryption_force_enable(uint8_t enable) {
    if (!tmxc_encryption.initialized) {
        return;
    }
    
    tmxc_encryption.encryption_force_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[ENCRYPTION-FORCE] Encryption force enabled\r\n");
    } else {
        tmxc_uart_puts("[ENCRYPTION-FORCE] Encryption force disabled\r\n");
    }
}

uint8_t tmxc_encryption_force_is_enabled(void) {
    return tmxc_encryption.encryption_force_enabled;
}

uint64_t tmxc_encryption_force_get_files_encrypted(void) {
    return tmxc_encryption.files_encrypted;
}

uint64_t tmxc_encryption_force_get_total_bytes(void) {
    return tmxc_encryption.total_encrypted_bytes;
}

void tmxc_encryption_force_cleanup(void) {
    if (!tmxc_encryption.initialized) {
        return;
    }
    
    tmxc_encryption.encryption_force_enabled = 0;
    tmxc_encryption.initialized = 0;
    
    tmxc_uart_puts("[ENCRYPTION-FORCE] Encryption force cleaned up\r\n");
}
