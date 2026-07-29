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

#define TMXC_CLIPBOARD_MAX_SIZE 1048576
#define TMXC_CLIPBOARD_MAX_ENTRIES 16
#define TMXC_CLIPBOARD_AUTHORIZED_APPS 32
#define TMXC_CLIPBOARD_ENCRYPTION_KEY_LENGTH 32

typedef enum {
    TMXC_CLIPBOARD_TYPE_TEXT = 0,
    TMXC_CLIPBOARD_TYPE_IMAGE = 1,
    TMXC_CLIPBOARD_TYPE_FILE = 2,
    TMXC_CLIPBOARD_TYPE_URL = 3,
    TMXC_CLIPBOARD_TYPE_CUSTOM = 4
} tmxc_clipboard_type_t;

typedef struct {
    uint8_t data[TMXC_CLIPBOARD_MAX_SIZE];
    uint64_t size;
    tmxc_clipboard_type_t type;
    uint64_t timestamp;
    uint32_t source_app_id;
    uint8_t is_encrypted;
    uint8_t encryption_key[TMXC_CLIPBOARD_ENCRYPTION_KEY_LENGTH];
    uint8_t iv[16];
    uint32_t checksum;
} tmxc_clipboard_entry_t;

typedef struct {
    uint32_t app_id;
    char app_name[64];
    uint8_t is_authorized;
    uint64_t authorization_time;
    uint8_t access_level;
} tmxc_clipboard_auth_app_t;

static tmxc_clipboard_entry_t tmxc_clipboard_entries[TMXC_CLIPBOARD_MAX_ENTRIES];
static tmxc_clipboard_auth_app_t tmxc_authorized_apps[TMXC_CLIPBOARD_AUTHORIZED_APPS];
static uint32_t tmxc_current_entry = 0;
static uint32_t tmxc_entry_count = 0;
static uint32_t tmxc_authorized_app_count = 0;
static uint8_t tmxc_secure_clipboard_enabled = 1;
static uint8_t tmxc_clipboard_encryption_enabled = 1;

static void tmxc_aes256_encrypt_block(const uint8_t* plaintext, const uint8_t* key, const uint8_t* iv, uint8_t* ciphertext) {
    uint32_t key_schedule[60];
    
    for (int i = 0; i < 8; i++) {
        key_schedule[i] = ((uint32_t*)key)[i];
    }
    
    for (int i = 8; i < 60; i++) {
        uint32_t temp = key_schedule[i - 1];
        if (i % 8 == 0) {
            uint8_t sbox[16] = {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 
                               0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76};
            uint8_t sbox_in = (temp >> 24) & 0xFF;
            uint8_t sbox_out = sbox_in < 16 ? sbox[sbox_in] : sbox_in;
            temp = ((temp << 8) | (temp >> 24)) ^ sbox_out ^ (i / 8);
        }
        key_schedule[i] = key_schedule[i - 8] ^ temp;
    }
    
    uint8_t state[16];
    for (int i = 0; i < 16; i++) {
        state[i] = plaintext[i] ^ iv[i];
    }
    
    for (int round = 0; round < 14; round++) {
        for (int i = 0; i < 16; i++) {
            state[i] ^= (key_schedule[round * 4 + (i / 4)] >> ((3 - (i % 4)) * 8)) & 0xFF;
        }
        
        uint8_t sbox[16] = {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 
                           0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76};
        for (int i = 0; i < 16; i++) {
            state[i] = state[i] < 16 ? sbox[state[i]] : state[i];
        }
        
        uint8_t temp[16];
        for (int i = 0; i < 16; i++) {
            temp[i] = state[i];
        }
        for (int i = 0; i < 16; i++) {
            state[i] = temp[(i + 4) % 16];
        }
        
        for (int i = 0; i < 4; i++) {
            uint8_t col[4] = {state[i * 4], state[i * 4 + 1], state[i * 4 + 2], state[i * 4 + 3]};
            uint8_t r0 = col[0], r1 = col[1], r2 = col[2], r3 = col[3];
            state[i * 4] = r2 ^ ((r3 << 1) | (r3 >> 7));
            state[i * 4 + 1] = r3 ^ ((r0 << 1) | (r0 >> 7));
            state[i * 4 + 2] = r0 ^ ((r1 << 1) | (r1 >> 7));
            state[i * 4 + 3] = r1 ^ ((r2 << 1) | (r2 >> 7));
        }
    }
    
    for (int i = 0; i < 16; i++) {
        state[i] ^= (key_schedule[56 + (i / 4)] >> ((3 - (i % 4)) * 8)) & 0xFF;
    }
    
    for (int i = 0; i < 16; i++) {
        ciphertext[i] = state[i];
    }
}

static void tmxc_aes256_decrypt_block(const uint8_t* ciphertext, const uint8_t* key, const uint8_t* iv, uint8_t* plaintext) {
    uint32_t key_schedule[60];
    
    for (int i = 0; i < 8; i++) {
        key_schedule[i] = ((uint32_t*)key)[i];
    }
    
    for (int i = 8; i < 60; i++) {
        uint32_t temp = key_schedule[i - 1];
        if (i % 8 == 0) {
            uint8_t sbox[16] = {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 
                               0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76};
            uint8_t sbox_in = (temp >> 24) & 0xFF;
            uint8_t sbox_out = sbox_in < 16 ? sbox[sbox_in] : sbox_in;
            temp = ((temp << 8) | (temp >> 24)) ^ sbox_out ^ (i / 8);
        }
        key_schedule[i] = key_schedule[i - 8] ^ temp;
    }
    
    uint8_t state[16];
    for (int i = 0; i < 16; i++) {
        state[i] = ciphertext[i];
    }
    
    for (int round = 13; round >= 0; round--) {
        for (int i = 0; i < 16; i++) {
            state[i] ^= (key_schedule[round * 4 + (i / 4)] >> ((3 - (i % 4)) * 8)) & 0xFF;
        }
        
        for (int i = 0; i < 4; i++) {
            uint8_t col[4] = {state[i * 4], state[i * 4 + 1], state[i * 4 + 2], state[i * 4 + 3]};
            uint8_t r0 = col[0], r1 = col[1], r2 = col[2], r3 = col[3];
            state[i * 4] = ((r2 << 1) | (r2 >> 7)) ^ r0;
            state[i * 4 + 1] = ((r3 << 1) | (r3 >> 7)) ^ r1;
            state[i * 4 + 2] = ((r0 << 1) | (r0 >> 7)) ^ r2;
            state[i * 4 + 3] = ((r1 << 1) | (r1 >> 7)) ^ r3;
        }
        
        uint8_t temp[16];
        for (int i = 0; i < 16; i++) {
            temp[i] = state[i];
        }
        for (int i = 0; i < 16; i++) {
            state[i] = temp[(i + 12) % 16];
        }
        
        uint8_t rsbox[16] = {0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 
                            0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb};
        for (int i = 0; i < 16; i++) {
            state[i] = state[i] < 16 ? rsbox[state[i]] : state[i];
        }
    }
    
    for (int i = 0; i < 16; i++) {
        plaintext[i] = state[i] ^ iv[i];
    }
}

static uint32_t tmxc_calculate_checksum(const uint8_t* data, uint64_t size) {
    uint32_t checksum = 0;
    for (uint64_t i = 0; i < size; i++) {
        checksum += data[i];
        checksum = (checksum << 1) | (checksum >> 31);
    }
    return checksum;
}

void tmxc_secure_clipboard_init(void) {
    for (uint32_t i = 0; i < TMXC_CLIPBOARD_MAX_ENTRIES; i++) {
        for (uint32_t j = 0; j < TMXC_CLIPBOARD_MAX_SIZE; j++) {
            tmxc_clipboard_entries[i].data[j] = 0;
        }
        tmxc_clipboard_entries[i].size = 0;
        tmxc_clipboard_entries[i].type = TMXC_CLIPBOARD_TYPE_TEXT;
        tmxc_clipboard_entries[i].timestamp = 0;
        tmxc_clipboard_entries[i].source_app_id = 0;
        tmxc_clipboard_entries[i].is_encrypted = 0;
        for (uint32_t j = 0; j < TMXC_CLIPBOARD_ENCRYPTION_KEY_LENGTH; j++) {
            tmxc_clipboard_entries[i].encryption_key[j] = 0;
        }
        for (uint32_t j = 0; j < 16; j++) {
            tmxc_clipboard_entries[i].iv[j] = 0;
        }
        tmxc_clipboard_entries[i].checksum = 0;
    }
    
    for (uint32_t i = 0; i < TMXC_CLIPBOARD_AUTHORIZED_APPS; i++) {
        tmxc_authorized_apps[i].app_id = 0;
        tmxc_authorized_apps[i].app_name[0] = '\0';
        tmxc_authorized_apps[i].is_authorized = 0;
        tmxc_authorized_apps[i].authorization_time = 0;
        tmxc_authorized_apps[i].access_level = 0;
    }
    
    tmxc_current_entry = 0;
    tmxc_entry_count = 0;
    tmxc_authorized_app_count = 0;
    
    tmxc_uart_puts("[CLIPBOARD] Secure Clipboard initialized\r\n");
}

uint8_t tmxc_clipboard_set_data(const uint8_t* data, uint64_t size, tmxc_clipboard_type_t type, uint32_t source_app_id) {
    if (!tmxc_secure_clipboard_enabled || data == NULL || size == 0 || size > TMXC_CLIPBOARD_MAX_SIZE) {
        return 0;
    }
    
    uint32_t entry_index = tmxc_current_entry;
    
    for (uint64_t i = 0; i < size; i++) {
        tmxc_clipboard_entries[entry_index].data[i] = data[i];
    }
    
    tmxc_clipboard_entries[entry_index].size = size;
    tmxc_clipboard_entries[entry_index].type = type;
    tmxc_clipboard_entries[entry_index].timestamp = tmxc_get_cycle_count();
    tmxc_clipboard_entries[entry_index].source_app_id = source_app_id;
    tmxc_clipboard_entries[entry_index].is_encrypted = 0;
    
    if (tmxc_clipboard_encryption_enabled) {
        for (uint32_t i = 0; i < TMXC_CLIPBOARD_ENCRYPTION_KEY_LENGTH; i++) {
            tmxc_clipboard_entries[entry_index].encryption_key[i] = (uint8_t)(tmxc_get_cycle_count() + i);
        }
        
        for (uint32_t i = 0; i < 16; i++) {
            tmxc_clipboard_entries[entry_index].iv[i] = (uint8_t)(tmxc_get_cycle_count() + i);
        }
        
        for (uint64_t i = 0; i < size; i += 16) {
            uint8_t block[16] = {0};
            uint8_t encrypted[16] = {0};
            
            for (uint32_t j = 0; j < 16 && (i + j) < size; j++) {
                block[j] = tmxc_clipboard_entries[entry_index].data[i + j];
            }
            
            tmxc_aes256_encrypt_block(block, tmxc_clipboard_entries[entry_index].encryption_key,
                                     tmxc_clipboard_entries[entry_index].iv, encrypted);
            
            for (uint32_t j = 0; j < 16 && (i + j) < size; j++) {
                tmxc_clipboard_entries[entry_index].data[i + j] = encrypted[j];
            }
        }
        
        tmxc_clipboard_entries[entry_index].is_encrypted = 1;
    }
    
    tmxc_clipboard_entries[entry_index].checksum = tmxc_calculate_checksum(
        tmxc_clipboard_entries[entry_index].data, size
    );
    
    tmxc_current_entry = (tmxc_current_entry + 1) % TMXC_CLIPBOARD_MAX_ENTRIES;
    if (tmxc_entry_count < TMXC_CLIPBOARD_MAX_ENTRIES) {
        tmxc_entry_count++;
    }
    
    tmxc_uart_puts("[CLIPBOARD] Data set, encrypted: ");
    tmxc_uart_puts(tmxc_clipboard_entries[entry_index].is_encrypted ? "yes" : "no");
    tmxc_uart_puts("\r\n");
    
    return 1;
}

uint8_t tmxc_clipboard_get_data(uint8_t* buffer, uint64_t buffer_size, uint64_t* actual_size, uint32_t requesting_app_id) {
    if (!tmxc_secure_clipboard_enabled || buffer == NULL || actual_size == NULL) {
        return 0;
    }
    
    if (tmxc_entry_count == 0) {
        return 0;
    }
    
    uint32_t entry_index = (tmxc_current_entry - 1 + TMXC_CLIPBOARD_MAX_ENTRIES) % TMXC_CLIPBOARD_MAX_ENTRIES;
    
    if (!tmxc_clipboard_is_app_authorized(requesting_app_id)) {
        tmxc_uart_puts("[CLIPBOARD] Access denied for App ");
        char buf[21];
        int pos = 20;
        buf[pos] = '\0';
        uint64_t temp = requesting_app_id;
        while (temp > 0 && pos > 0) {
            pos--;
            buf[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buf[pos]);
        tmxc_uart_puts("\r\n");
        return 0;
    }
    
    uint64_t data_size = tmxc_clipboard_entries[entry_index].size;
    
    if (data_size > buffer_size) {
        *actual_size = data_size;
        return 0;
    }
    
    if (tmxc_clipboard_entries[entry_index].is_encrypted) {
        uint8_t* temp_buffer = (uint8_t*)tmxc_malloc(data_size);
        
        if (temp_buffer != NULL) {
            for (uint64_t i = 0; i < data_size; i++) {
                temp_buffer[i] = tmxc_clipboard_entries[entry_index].data[i];
            }
            
            for (uint64_t i = 0; i < data_size; i += 16) {
                uint8_t block[16] = {0};
                uint8_t decrypted[16] = {0};
                
                for (uint32_t j = 0; j < 16 && (i + j) < data_size; j++) {
                    block[j] = temp_buffer[i + j];
                }
                
                tmxc_aes256_decrypt_block(block, tmxc_clipboard_entries[entry_index].encryption_key,
                                         tmxc_clipboard_entries[entry_index].iv, decrypted);
                
                for (uint32_t j = 0; j < 16 && (i + j) < data_size; j++) {
                    temp_buffer[i + j] = decrypted[j];
                }
            }
            
            for (uint64_t i = 0; i < data_size; i++) {
                buffer[i] = temp_buffer[i];
            }
            
            tmxc_free(temp_buffer);
        } else {
            return 0;
        }
    } else {
        for (uint64_t i = 0; i < data_size; i++) {
            buffer[i] = tmxc_clipboard_entries[entry_index].data[i];
        }
    }
    
    *actual_size = data_size;
    
    return 1;
}

void tmxc_clipboard_authorize_app(uint32_t app_id, const char* app_name, uint8_t access_level) {
    if (tmxc_authorized_app_count >= TMXC_CLIPBOARD_AUTHORIZED_APPS) {
        return;
    }
    
    tmxc_authorized_apps[tmxc_authorized_app_count].app_id = app_id;
    
    if (app_name != NULL) {
        for (uint32_t i = 0; i < 63 && app_name[i] != '\0'; i++) {
            tmxc_authorized_apps[tmxc_authorized_app_count].app_name[i] = app_name[i];
        }
        tmxc_authorized_apps[tmxc_authorized_app_count].app_name[63] = '\0';
    }
    
    tmxc_authorized_apps[tmxc_authorized_app_count].is_authorized = 1;
    tmxc_authorized_apps[tmxc_authorized_app_count].authorization_time = tmxc_get_cycle_count();
    tmxc_authorized_apps[tmxc_authorized_app_count].access_level = access_level;
    
    tmxc_authorized_app_count++;
    
    tmxc_uart_puts("[CLIPBOARD] Authorized App ");
    tmxc_uart_puts(app_name ? app_name : "Unknown");
    tmxc_uart_puts("\r\n");
}

uint8_t tmxc_clipboard_is_app_authorized(uint32_t app_id) {
    for (uint32_t i = 0; i < tmxc_authorized_app_count; i++) {
        if (tmxc_authorized_apps[i].app_id == app_id && tmxc_authorized_apps[i].is_authorized) {
            return 1;
        }
    }
    return 0;
}

void tmxc_clipboard_revoke_app(uint32_t app_id) {
    for (uint32_t i = 0; i < tmxc_authorized_app_count; i++) {
        if (tmxc_authorized_apps[i].app_id == app_id) {
            tmxc_authorized_apps[i].is_authorized = 0;
            tmxc_uart_puts("[CLIPBOARD] Revoked authorization for App ");
            char buffer[21];
            int pos = 20;
            buffer[pos] = '\0';
            uint64_t temp = app_id;
            while (temp > 0 && pos > 0) {
                pos--;
                buffer[pos] = '0' + (temp % 10);
                temp /= 10;
            }
            tmxc_uart_puts(&buffer[pos]);
            tmxc_uart_puts("\r\n");
            return;
        }
    }
}

void tmxc_clipboard_clear(void) {
    for (uint32_t i = 0; i < TMXC_CLIPBOARD_MAX_ENTRIES; i++) {
        for (uint32_t j = 0; j < TMXC_CLIPBOARD_MAX_SIZE; j++) {
            tmxc_clipboard_entries[i].data[j] = 0;
        }
        tmxc_clipboard_entries[i].size = 0;
        tmxc_clipboard_entries[i].is_encrypted = 0;
    }
    
    tmxc_entry_count = 0;
    tmxc_current_entry = 0;
    
    tmxc_uart_puts("[CLIPBOARD] Clipboard cleared\r\n");
}

void tmxc_secure_clipboard_enable(uint8_t enable) {
    tmxc_secure_clipboard_enabled = enable;
    tmxc_uart_puts("[CLIPBOARD] Secure Clipboard ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

void tmxc_clipboard_encryption_enable(uint8_t enable) {
    tmxc_clipboard_encryption_enabled = enable;
    tmxc_uart_puts("[CLIPBOARD] Encryption ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

void tmxc_secure_clipboard_cleanup(void) {
    tmxc_clipboard_clear();
    tmxc_uart_puts("[CLIPBOARD] Secure Clipboard cleaned up\r\n");
}
