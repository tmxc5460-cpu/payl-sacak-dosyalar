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
#include "../kernel/tmxc_kernel.h"

#define TMXC_VAULT_BASE_ADDRESS 0x90000000ULL
#define TMXC_VAULT_SIZE 0x10000000ULL
#define TMXC_PASSWORD_ATTEMPT_LIMIT 3
#define TMXC_FORCED_ACCESS_THRESHOLD 5

typedef struct {
    uint8_t password_attempts;
    uint8_t forced_access_count;
    uint64_t last_attempt_time;
    uint8_t destruct_triggered;
    uint8_t wipe_in_progress;
    uint64_t vault_base;
    uint64_t vault_size;
    uint8_t flash_wipe_enabled;
    uint8_t multi_pass_wipe;
} tmxc_emergency_self_destruct_t;

static tmxc_emergency_self_destruct_t tmxc_self_destruct;

static void tmxc_flash_sector_erase(uint64_t address) {
    uint32_t flash_ctrl = tmxc_read32((volatile uint32_t*)TMXC_FLASH_CTRL);
    flash_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_FLASH_CTRL, flash_ctrl);
    
    tmxc_write32((volatile uint32_t*)TMXC_FLASH_ADDR, (uint32_t)address);
    
    uint32_t erase_cmd = tmxc_read32((volatile uint32_t*)TMXC_FLASH_CMD);
    erase_cmd |= (1 << 2);
    tmxc_write32((volatile uint32_t*)TMXC_FLASH_CMD, erase_cmd);
    
    while (tmxc_read32((volatile uint32_t*)TMXC_FLASH_STATUS) & (1 << 0)) {
        tmxc_timer_delay_ms(1);
    }
}

static void tmxc_flash_write_zero(uint64_t address) {
    tmxc_write32((volatile uint32_t*)address, 0x00000000);
    tmxc_write32((volatile uint32_t*)(address + 4), 0x00000000);
    tmxc_write32((volatile uint32_t*)(address + 8), 0x00000000);
    tmxc_write32((volatile uint32_t*)(address + 12), 0x00000000);
}

static void tmxc_flash_write_pattern(uint64_t address, uint32_t pattern) {
    for (uint64_t addr = address; addr < address + 0x1000; addr += 4) {
        tmxc_write32((volatile uint32_t*)addr, pattern);
    }
}

static void tmxc_multi_pass_wipe(uint64_t base_address, uint64_t size) {
    uint32_t patterns[] = {0x00000000, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAA, 0x00000000};
    
    for (uint32_t pass = 0; pass < 5; pass++) {
        for (uint64_t addr = base_address; addr < base_address + size; addr += 0x1000) {
            tmxc_flash_sector_erase(addr);
            tmxc_flash_write_pattern(addr, patterns[pass]);
        }
        tmxc_uart_puts("[SELF-DESTRUCT] Pass ");
        char buffer[21];
        int pos = 20;
        buffer[pos] = '\0';
        uint64_t temp = pass + 1;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts("/5 complete\r\n");
    }
}

void tmxc_emergency_self_destruct_init(void) {
    tmxc_self_destruct.password_attempts = 0;
    tmxc_self_destruct.forced_access_count = 0;
    tmxc_self_destruct.last_attempt_time = 0;
    tmxc_self_destruct.destruct_triggered = 0;
    tmxc_self_destruct.wipe_in_progress = 0;
    tmxc_self_destruct.vault_base = TMXC_VAULT_BASE_ADDRESS;
    tmxc_self_destruct.vault_size = TMXC_VAULT_SIZE;
    tmxc_self_destruct.flash_wipe_enabled = 1;
    tmxc_self_destruct.multi_pass_wipe = 1;
    
    tmxc_uart_puts("[SELF-DESTRUCT] Emergency Self-Destruct initialized\r\n");
}

void tmxc_emergency_self_destruct_record_attempt(void) {
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t time_since_last = current_time - tmxc_self_destruct.last_attempt_time;
    
    if (time_since_last > 60000000000ULL) {
        tmxc_self_destruct.password_attempts = 0;
    }
    
    tmxc_self_destruct.password_attempts++;
    tmxc_self_destruct.last_attempt_time = current_time;
    
    tmxc_uart_puts("[SELF-DESTRUCT] Password attempt recorded: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = tmxc_self_destruct.password_attempts;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("/");
    pos = 20;
    buffer[pos] = '\0';
    temp = TMXC_PASSWORD_ATTEMPT_LIMIT;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
    
    if (tmxc_self_destruct.password_attempts >= TMXC_PASSWORD_ATTEMPT_LIMIT) {
        tmxc_emergency_self_destruct_trigger();
    }
}

void tmxc_emergency_self_destruct_record_forced_access(void) {
    tmxc_self_destruct.forced_access_count++;
    
    tmxc_uart_puts("[SELF-DESTRUCT] Forced access detected: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = tmxc_self_destruct.forced_access_count;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("/");
    pos = 20;
    buffer[pos] = '\0';
    temp = TMXC_FORCED_ACCESS_THRESHOLD;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
    
    if (tmxc_self_destruct.forced_access_count >= TMXC_FORCED_ACCESS_THRESHOLD) {
        tmxc_emergency_self_destruct_trigger();
    }
}

void tmxc_emergency_self_destruct_trigger(void) {
    if (tmxc_self_destruct.destruct_triggered) {
        return;
    }
    
    tmxc_self_destruct.destruct_triggered = 1;
    tmxc_self_destruct.wipe_in_progress = 1;
    
    tmxc_uart_puts("[SELF-DESTRUCT] EMERGENCY SELF-DESTRUCT TRIGGERED\r\n");
    tmxc_uart_puts("[SELF-DESTRUCT] Initiating Flash-Level Wipe...\r\n");
    
    if (tmxc_self_destruct.flash_wipe_enabled) {
        if (tmxc_self_destruct.multi_pass_wipe) {
            tmxc_multi_pass_wipe(tmxc_self_destruct.vault_base, tmxc_self_destruct.vault_size);
        } else {
            for (uint64_t addr = tmxc_self_destruct.vault_base; 
                 addr < tmxc_self_destruct.vault_base + tmxc_self_destruct.vault_size; 
                 addr += 0x1000) {
                tmxc_flash_sector_erase(addr);
                tmxc_flash_write_zero(addr);
            }
        }
    }
    
    tmxc_self_destruct.wipe_in_progress = 0;
    
    tmxc_uart_puts("[SELF-DESTRUCT] Vault data permanently destroyed\r\n");
    tmxc_uart_puts("[SELF-DESTRUCT] System will now shutdown\r\n");
    
    tmxc_timer_delay_ms(5000);
    
    uint32_t power_ctrl = tmxc_read32((volatile uint32_t*)TMXC_POWER_CTRL);
    power_ctrl &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_POWER_CTRL, power_ctrl);
}

void tmxc_emergency_self_destruct_reset_attempts(void) {
    tmxc_self_destruct.password_attempts = 0;
    tmxc_self_destruct.forced_access_count = 0;
    tmxc_uart_puts("[SELF-DESTRUCT] Attempt counters reset\r\n");
}

uint8_t tmxc_emergency_self_destruct_is_triggered(void) {
    return tmxc_self_destruct.destruct_triggered;
}

uint8_t tmxc_emergency_self_destruct_is_wipe_in_progress(void) {
    return tmxc_self_destruct.wipe_in_progress;
}

uint8_t tmxc_emergency_self_destruct_get_attempts(void) {
    return tmxc_self_destruct.password_attempts;
}

uint8_t tmxc_emergency_self_destruct_get_forced_access_count(void) {
    return tmxc_self_destruct.forced_access_count;
}

void tmxc_emergency_self_destruct_enable_flash_wipe(uint8_t enable) {
    tmxc_self_destruct.flash_wipe_enabled = enable;
    tmxc_uart_puts("[SELF-DESTRUCT] Flash wipe ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

void tmxc_emergency_self_destruct_enable_multi_pass(uint8_t enable) {
    tmxc_self_destruct.multi_pass_wipe = enable;
    tmxc_uart_puts("[SELF-DESTRUCT] Multi-pass wipe ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

void tmxc_emergency_self_destruct_set_vault_region(uint64_t base_address, uint64_t size) {
    tmxc_self_destruct.vault_base = base_address;
    tmxc_self_destruct.vault_size = size;
}
