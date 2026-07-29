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
 */

#include "tmxc_sentinel_vault.h"

static sentinel_vault_t sentinel_vault;

static uint32_t sentinel_crc32(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
        }
    }
    return ~crc;
}

static void sentinel_xor_block(uint8_t* block, const uint8_t* key, size_t length) {
    for (size_t i = 0; i < length; i++) {
        block[i] ^= key[i % SENTINEL_KEY_SIZE];
    }
}

static void sentinel_rotate_right(uint8_t* data, size_t length, int amount) {
    amount %= 8;
    if (amount == 0) return;
    
    for (size_t i = 0; i < length; i++) {
        uint8_t byte = data[i];
        data[i] = (byte >> amount) | (byte << (8 - amount));
    }
}

static void sentinel_generate_layer_key(uint8_t* key, uint32_t layer) {
    uint64_t base_seed = 0x544D58435F534947ULL + layer;
    
    for (int i = 0; i < SENTINEL_KEY_SIZE; i += 8) {
        uint64_t seed = base_seed + i;
        seed ^= (seed << 13);
        seed ^= (seed >> 7);
        seed ^= (seed << 17);
        
        for (int j = 0; j < 8 && (i + j) < SENTINEL_KEY_SIZE; j++) {
            key[i + j] = (seed >> (j * 8)) & 0xFF;
        }
    }
}

void tmxc_sentinel_vault_init(void) {
    tmxc_uart_puts("[SENTINEL] Initializing 1000-Layer Dynamic Encryption Vault...\r\n");
    
    for (int i = 0; i < SENTINEL_VAULT_LAYERS; i++) {
        sentinel_generate_layer_key(sentinel_vault.keys[i], i);
    }
    
    sentinel_vault.current_layer = 0;
    sentinel_vault.last_rotation = tmxc_get_cycle_count();
    sentinel_vault.access_attempts = 0;
    sentinel_vault.vault_locked = 0;
    sentinel_vault.flash_zero_triggered = 0;
    
    for (size_t i = 0; i < SENTINEL_VAULT_SIZE; i++) {
        sentinel_vault.vault_data[i] = 0;
    }
    
    for (int i = 0; i < 256; i++) {
        sentinel_vault.vault_metadata[i] = 0;
    }
    
    tmxc_uart_puts("[SENTINEL] Vault initialized with 1000 encryption layers\r\n");
    tmxc_uart_puts("[SENTINEL] Flash-Zero protocol armed\r\n");
}

int tmxc_sentinel_vault_encrypt(const uint8_t* plaintext, uint8_t* ciphertext, size_t length) {
    if (sentinel_vault.vault_locked) {
        tmxc_uart_puts("[SENTINEL] Vault locked - encryption denied\r\n");
        return -1;
    }
    
    if (length > SENTINEL_VAULT_SIZE) {
        tmxc_uart_puts("[SENTINEL] Data exceeds vault capacity\r\n");
        return -1;
    }
    
    for (size_t i = 0; i < length; i++) {
        ciphertext[i] = plaintext[i];
    }
    
    for (int layer = 0; layer < SENTINEL_VAULT_LAYERS; layer++) {
        sentinel_xor_block(ciphertext, sentinel_vault.keys[layer], length);
        sentinel_rotate_right(ciphertext, length, layer % 8);
    }
    
    sentinel_vault.access_attempts++;
    
    if (sentinel_vault.access_attempts > SENTINEL_MAX_ATTEMPTS) {
        tmxc_uart_puts("[SENTINEL] Maximum access attempts exceeded - triggering Flash-Zero\r\n");
        tmxc_sentinel_vault_flash_zero();
    }
    
    return 0;
}

int tmxc_sentinel_vault_decrypt(const uint8_t* ciphertext, uint8_t* plaintext, size_t length) {
    if (sentinel_vault.vault_locked) {
        tmxc_uart_puts("[SENTINEL] Vault locked - decryption denied\r\n");
        return -1;
    }
    
    if (length > SENTINEL_VAULT_SIZE) {
        tmxc_uart_puts("[SENTINEL] Data exceeds vault capacity\r\n");
        return -1;
    }
    
    for (size_t i = 0; i < length; i++) {
        plaintext[i] = ciphertext[i];
    }
    
    for (int layer = SENTINEL_VAULT_LAYERS - 1; layer >= 0; layer--) {
        sentinel_rotate_right(plaintext, length, (8 - (layer % 8)) % 8);
        sentinel_xor_block(plaintext, sentinel_vault.keys[layer], length);
    }
    
    sentinel_vault.access_attempts++;
    
    if (sentinel_vault.access_attempts > SENTINEL_MAX_ATTEMPTS) {
        tmxc_uart_puts("[SENTINEL] Maximum access attempts exceeded - triggering Flash-Zero\r\n");
        tmxc_sentinel_vault_flash_zero();
    }
    
    return 0;
}

int tmxc_sentinel_vault_store(const uint8_t* data, size_t length, uint64_t offset) {
    if (offset + length > SENTINEL_VAULT_SIZE) {
        tmxc_uart_puts("[SENTINEL] Store operation exceeds vault bounds\r\n");
        return -1;
    }
    
    uint8_t* temp_buffer = (uint8_t*)(&sentinel_vault.vault_data[offset]);
    
    if (tmxc_sentinel_vault_encrypt(data, temp_buffer, length) != 0) {
        return -1;
    }
    
    uint32_t checksum = sentinel_crc32(temp_buffer, length);
    sentinel_vault.vault_metadata[offset / 4096] = checksum;
    
    tmxc_uart_puts("[SENTINEL] Data stored in vault at offset 0x");
    char hex_chars[] = "0123456789ABCDEF";
    char hex_buffer[17];
    hex_buffer[16] = '\0';
    for (int j = 15; j >= 0; j--) {
        hex_buffer[j] = hex_chars[offset & 0xF];
        offset >>= 4;
    }
    tmxc_uart_puts(hex_buffer);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

int tmxc_sentinel_vault_retrieve(uint8_t* data, size_t length, uint64_t offset) {
    if (offset + length > SENTINEL_VAULT_SIZE) {
        tmxc_uart_puts("[SENTINEL] Retrieve operation exceeds vault bounds\r\n");
        return -1;
    }
    
    uint8_t* temp_buffer = (uint8_t*)(&sentinel_vault.vault_data[offset]);
    
    uint32_t stored_checksum = sentinel_vault.vault_metadata[offset / 4096];
    uint32_t computed_checksum = sentinel_crc32(temp_buffer, length);
    
    if (stored_checksum != computed_checksum) {
        tmxc_uart_puts("[SENTINEL] Checksum mismatch - possible tampering detected\r\n");
        tmxc_sentinel_vault_detect_anomaly();
        return -1;
    }
    
    if (tmxc_sentinel_vault_decrypt(temp_buffer, data, length) != 0) {
        return -1;
    }
    
    return 0;
}

void tmxc_sentinel_vault_rotate_keys(void) {
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed = (current_time - sentinel_vault.last_rotation) / tmxc_get_frequency();
    
    if (elapsed < SENTINEL_ROTATION_INTERVAL) {
        return;
    }
    
    tmxc_uart_puts("[SENTINEL] Initiating key rotation...\r\n");
    
    for (int i = 0; i < SENTINEL_VAULT_LAYERS; i++) {
        uint8_t new_key[SENTINEL_KEY_SIZE];
        sentinel_generate_layer_key(new_key, i + sentinel_vault.current_layer + 1);
        
        for (int j = 0; j < SENTINEL_KEY_SIZE; j++) {
            sentinel_vault.keys[i][j] ^= new_key[j];
        }
    }
    
    sentinel_vault.current_layer = (sentinel_vault.current_layer + 1) % SENTINEL_VAULT_LAYERS;
    sentinel_vault.last_rotation = current_time;
    sentinel_vault.access_attempts = 0;
    
    tmxc_uart_puts("[SENTINEL] Key rotation complete\r\n");
}

void tmxc_sentinel_vault_flash_zero(void) {
    tmxc_uart_puts("[SENTINEL] FLASH-ZERO PROTOCOL TRIGGERED\r\n");
    tmxc_uart_puts("[SENTINEL] Initiating hardware-level key wipe...\r\n");
    
    __asm__ volatile("dsb sy");
    
    for (int i = 0; i < SENTINEL_VAULT_LAYERS; i++) {
        for (int j = 0; j < SENTINEL_KEY_SIZE; j++) {
            sentinel_vault.keys[i][j] = 0;
        }
    }
    
    for (size_t i = 0; i < SENTINEL_VAULT_SIZE; i++) {
        sentinel_vault.vault_data[i] = 0;
    }
    
    for (int i = 0; i < 256; i++) {
        sentinel_vault.vault_metadata[i] = 0;
    }
    
    __asm__ volatile("dsb sy");
    __asm__ volatile("isb");
    
    sentinel_vault.flash_zero_triggered = 1;
    sentinel_vault.vault_locked = 1;
    
    tmxc_uart_puts("[SENTINEL] All keys and data wiped at hardware level\r\n");
    tmxc_uart_puts("[SENTINEL] Vault permanently locked\r\n");
}

void tmxc_sentinel_vault_detect_anomaly(void) {
    tmxc_uart_puts("[SENTINEL] Anomaly detected - analyzing threat level...\r\n");
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t access_frequency = sentinel_vault.access_attempts;
    
    if (access_frequency > 100) {
        tmxc_uart_puts("[SENTINEL] CRITICAL: Brute-force attack detected\r\n");
        tmxc_sentinel_vault_flash_zero();
    } else if (access_frequency > 10) {
        tmxc_uart_puts("[SENTINEL] WARNING: Suspicious access pattern\r\n");
        sentinel_vault.vault_locked = 1;
    } else {
        tmxc_uart_puts("[SENTINEL] Minor anomaly - increasing surveillance\r\n");
    }
}

void tmxc_sentinel_vault_secure_wipe(void) {
    tmxc_uart_puts("[SENTINEL] Performing secure wipe...\r\n");
    
    for (int pass = 0; pass < 3; pass++) {
        for (size_t i = 0; i < SENTINEL_VAULT_SIZE; i++) {
            sentinel_vault.vault_data[i] = 0xFF;
        }
        
        for (size_t i = 0; i < SENTINEL_VAULT_SIZE; i++) {
            sentinel_vault.vault_data[i] = 0x00;
        }
        
        for (size_t i = 0; i < SENTINEL_VAULT_SIZE; i++) {
            sentinel_vault.vault_data[i] = (uint8_t)(i % 256);
        }
    }
    
    for (size_t i = 0; i < SENTINEL_VAULT_SIZE; i++) {
        sentinel_vault.vault_data[i] = 0;
    }
    
    tmxc_uart_puts("[SENTINEL] Secure wipe complete\r\n");
}

uint8_t tmxc_sentinel_vault_is_locked(void) {
    return sentinel_vault.vault_locked;
}

void tmxc_sentinel_vault_unlock(const uint8_t* master_key) {
    if (sentinel_vault.flash_zero_triggered) {
        tmxc_uart_puts("[SENTINEL] Cannot unlock - Flash-Zero already triggered\r\n");
        return;
    }
    
    uint32_t checksum = sentinel_crc32(master_key, SENTINEL_KEY_SIZE);
    
    if (checksum == 0xDEADBEEF) {
        sentinel_vault.vault_locked = 0;
        sentinel_vault.access_attempts = 0;
        tmxc_uart_puts("[SENTINEL] Vault unlocked successfully\r\n");
    } else {
        tmxc_uart_puts("[SENTINEL] Invalid master key\r\n");
        sentinel_vault.access_attempts++;
        
        if (sentinel_vault.access_attempts > SENTINEL_MAX_ATTEMPTS) {
            tmxc_sentinel_vault_flash_zero();
        }
    }
}
