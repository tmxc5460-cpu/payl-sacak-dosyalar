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

#define TMXC_STEALTH_CHARGE_VAULT_BASE 0x90000000ULL
#define TMXC_STEALTH_CHARGE_VAULT_SIZE 0x10000000ULL
#define TMXC_STEALTH_CHARGE_BACKUP_SIZE 0x1000000ULL
#define TMXC_STEALTH_CHARGE_ENCRYPTION_KEY_SIZE 32

typedef struct {
    uint8_t encryption_key[TMXC_STEALTH_CHARGE_ENCRYPTION_KEY_SIZE];
    uint8_t is_plugged_in;
    uint8_t encryption_enabled;
    uint8_t backup_enabled;
    uint8_t cloud_backup_enabled;
    uint64_t last_backup_time;
    uint32_t backup_count;
    uint64_t vault_base;
    uint64_t vault_size;
    uint64_t backup_base;
    uint64_t backup_size;
} tmxc_stealth_charge_t;

static tmxc_stealth_charge_t tmxc_stealth_charge;

static void tmxc_stealth_charge_generate_key(void) {
    for (uint8_t i = 0; i < TMXC_STEALTH_CHARGE_ENCRYPTION_KEY_SIZE; i++) {
        uint32_t trng_data = tmxc_read32((volatile uint32_t*)TMXC_TRNG_DATA);
        tmxc_stealth_charge.encryption_key[i] = (uint8_t)(trng_data & 0xFF);
    }
    
    tmxc_uart_puts("[STEALTH-CHARGE] Encryption key generated\r\n");
}

static void tmxc_stealth_charge_encrypt_data(uint8_t* data, uint32_t size) {
    if (data == NULL || size == 0) {
        return;
    }
    
    for (uint32_t i = 0; i < size; i++) {
        data[i] ^= tmxc_stealth_charge.encryption_key[i % TMXC_STEALTH_CHARGE_ENCRYPTION_KEY_SIZE];
    }
}

static void tmxc_stealth_charge_decrypt_data(uint8_t* data, uint32_t size) {
    tmxc_stealth_charge_encrypt_data(data, size);
}

static void tmxc_stealth_charge_backup_to_flash(void) {
    if (!tmxc_stealth_charge.backup_enabled) {
        return;
    }
    
    tmxc_uart_puts("[STEALTH-CHARGE] Starting backup to flash...\r\n");
    
    for (uint64_t src_addr = tmxc_stealth_charge.vault_base; 
         src_addr < tmxc_stealth_charge.vault_base + tmxc_stealth_charge.backup_size; 
         src_addr += 0x1000) {
        
        uint64_t dst_addr = tmxc_stealth_charge.backup_base + (src_addr - tmxc_stealth_charge.vault_base);
        
        uint8_t chunk[0x1000];
        for (uint32_t i = 0; i < 0x1000; i++) {
            chunk[i] = *((volatile uint8_t*)src_addr + i);
        }
        
        if (tmxc_stealth_charge.encryption_enabled) {
            tmxc_stealth_charge_encrypt_data(chunk, 0x1000);
        }
        
        for (uint32_t i = 0; i < 0x1000; i++) {
            *((volatile uint8_t*)dst_addr + i) = chunk[i];
        }
    }
    
    tmxc_stealth_charge.last_backup_time = tmxc_get_cycle_count();
    tmxc_stealth_charge.backup_count++;
    
    tmxc_uart_puts("[STEALTH-CHARGE] Backup completed\r\n");
}

static void tmxc_stealth_charge_backup_to_cloud(void) {
    if (!tmxc_stealth_charge.cloud_backup_enabled) {
        return;
    }
    
    tmxc_uart_puts("[STEALTH-CHARGE] Starting cloud backup...\r\n");
    
    uint8_t backup_data[0x1000];
    for (uint32_t i = 0; i < 0x1000; i++) {
        backup_data[i] = *((volatile uint8_t*)tmxc_stealth_charge.vault_base + i);
    }
    
    if (tmxc_stealth_charge.encryption_enabled) {
        tmxc_stealth_charge_encrypt_data(backup_data, 0x1000);
    }
    
    tmxc_cloud_upload(backup_data, 0x1000);
    
    tmxc_uart_puts("[STEALTH-CHARGE] Cloud backup completed\r\n");
}

void tmxc_stealth_charge_init(void) {
    for (uint8_t i = 0; i < TMXC_STEALTH_CHARGE_ENCRYPTION_KEY_SIZE; i++) {
        tmxc_stealth_charge.encryption_key[i] = 0;
    }
    
    tmxc_stealth_charge.is_plugged_in = 0;
    tmxc_stealth_charge.encryption_enabled = 1;
    tmxc_stealth_charge.backup_enabled = 1;
    tmxc_stealth_charge.cloud_backup_enabled = 0;
    tmxc_stealth_charge.last_backup_time = 0;
    tmxc_stealth_charge.backup_count = 0;
    tmxc_stealth_charge.vault_base = TMXC_STEALTH_CHARGE_VAULT_BASE;
    tmxc_stealth_charge.vault_size = TMXC_STEALTH_CHARGE_VAULT_SIZE;
    tmxc_stealth_charge.backup_base = 0xB0000000ULL;
    tmxc_stealth_charge.backup_size = TMXC_STEALTH_CHARGE_BACKUP_SIZE;
    
    tmxc_stealth_charge_generate_key();
    
    tmxc_uart_puts("[STEALTH-CHARGE] Stealth-Charge Encryption initialized\r\n");
}

void tmxc_stealth_charge_detect_plug(void) {
    uint32_t charge_status = tmxc_read32((volatile uint32_t*)TMXC_CHARGE_STATUS);
    
    uint8_t was_plugged = tmxc_stealth_charge.is_plugged_in;
    tmxc_stealth_charge.is_plugged_in = (charge_status & (1 << 0)) ? 1 : 0;
    
    if (!was_plugged && tmxc_stealth_charge.is_plugged_in) {
        tmxc_uart_puts("[STEALTH-CHARGE] Device plugged in - initiating stealth backup\r\n");
        
        if (tmxc_stealth_charge.encryption_enabled) {
            tmxc_uart_puts("[STEALTH-CHARGE] Encrypting vault data...\r\n");
        }
        
        tmxc_stealth_charge_backup_to_flash();
        
        if (tmxc_stealth_charge.cloud_backup_enabled) {
            tmxc_stealth_charge_backup_to_cloud();
        }
    } else if (was_plugged && !tmxc_stealth_charge.is_plugged_in) {
        tmxc_uart_puts("[STEALTH-CHARGE] Device unplugged\r\n");
    }
}

void tmxc_stealth_charge_enable_encryption(uint8_t enable) {
    tmxc_stealth_charge.encryption_enabled = enable;
    tmxc_uart_puts("[STEALTH-CHARGE] Encryption ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

void tmxc_stealth_charge_enable_backup(uint8_t enable) {
    tmxc_stealth_charge.backup_enabled = enable;
    tmxc_uart_puts("[STEALTH-CHARGE] Flash backup ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

void tmxc_stealth_charge_enable_cloud_backup(uint8_t enable) {
    tmxc_stealth_charge.cloud_backup_enabled = enable;
    tmxc_uart_puts("[STEALTH-CHARGE] Cloud backup ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

uint8_t tmxc_stealth_charge_is_encryption_enabled(void) {
    return tmxc_stealth_charge.encryption_enabled;
}

uint8_t tmxc_stealth_charge_is_backup_enabled(void) {
    return tmxc_stealth_charge.backup_enabled;
}

uint8_t tmxc_stealth_charge_is_cloud_backup_enabled(void) {
    return tmxc_stealth_charge.cloud_backup_enabled;
}

uint8_t tmxc_stealth_charge_is_plugged_in(void) {
    return tmxc_stealth_charge.is_plugged_in;
}

uint32_t tmxc_stealth_charge_get_backup_count(void) {
    return tmxc_stealth_charge.backup_count;
}

uint64_t tmxc_stealth_charge_get_last_backup_time(void) {
    return tmxc_stealth_charge.last_backup_time;
}

void tmxc_stealth_charge_force_backup(void) {
    tmxc_uart_puts("[STEALTH-CHARGE] Forcing backup...\r\n");
    tmxc_stealth_charge_backup_to_flash();
    
    if (tmxc_stealth_charge.cloud_backup_enabled) {
        tmxc_stealth_charge_backup_to_cloud();
    }
}

void tmxc_stealth_charge_regenerate_key(void) {
    tmxc_stealth_charge_generate_key();
    tmxc_uart_puts("[STEALTH-CHARGE] Encryption key regenerated\r\n");
}

void tmxc_stealth_charge_set_vault_region(uint64_t base_address, uint64_t size) {
    tmxc_stealth_charge.vault_base = base_address;
    tmxc_stealth_charge.vault_size = size;
}

void tmxc_stealth_charge_set_backup_region(uint64_t base_address, uint64_t size) {
    tmxc_stealth_charge.backup_base = base_address;
    tmxc_stealth_charge.backup_size = size;
}
