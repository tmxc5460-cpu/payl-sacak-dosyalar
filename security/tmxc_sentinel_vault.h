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

#ifndef TMXC_SENTINEL_VAULT_H
#define TMXC_SENTINEL_VAULT_H

#include "../net/kernel/tmxc_kernel.h"

#define SENTINEL_VAULT_LAYERS 1000
#define SENTINEL_KEY_SIZE 32
#define SENTINEL_BLOCK_SIZE 16
#define SENTINEL_VAULT_SIZE (1024 * 1024)  // 1MB vault
#define SENTINEL_MAX_ATTEMPTS 3
#define SENTINEL_ROTATION_INTERVAL 60  // seconds

typedef struct {
    uint8_t keys[SENTINEL_VAULT_LAYERS][SENTINEL_KEY_SIZE];
    uint8_t current_layer;
    uint64_t last_rotation;
    uint32_t access_attempts;
    uint8_t vault_locked;
    uint8_t flash_zero_triggered;
    uint8_t vault_data[SENTINEL_VAULT_SIZE];
    uint64_t vault_metadata[256];
} sentinel_vault_t;

typedef struct {
    uint8_t data[SENTINEL_BLOCK_SIZE];
    uint8_t layer_index;
    uint64_t timestamp;
    uint32_t checksum;
} sentinel_block_t;

void tmxc_sentinel_vault_init(void);
int tmxc_sentinel_vault_encrypt(const uint8_t* plaintext, uint8_t* ciphertext, size_t length);
int tmxc_sentinel_vault_decrypt(const uint8_t* ciphertext, uint8_t* plaintext, size_t length);
int tmxc_sentinel_vault_store(const uint8_t* data, size_t length, uint64_t offset);
int tmxc_sentinel_vault_retrieve(uint8_t* data, size_t length, uint64_t offset);
void tmxc_sentinel_vault_rotate_keys(void);
void tmxc_sentinel_vault_flash_zero(void);
void tmxc_sentinel_vault_detect_anomaly(void);
void tmxc_sentinel_vault_secure_wipe(void);
uint8_t tmxc_sentinel_vault_is_locked(void);
void tmxc_sentinel_vault_unlock(const uint8_t* master_key);

#endif
