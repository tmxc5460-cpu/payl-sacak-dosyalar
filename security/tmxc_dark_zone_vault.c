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

#define TMXC_DARK_ZONE_COMBINATION_LENGTH 6
#define TMXC_DARK_ZONE_GUEST_MODE 0
#define TMXC_DARK_ZONE_REAL_MODE 1

typedef struct {
    uint8_t combination[TMXC_DARK_ZONE_COMBINATION_LENGTH];
    uint8_t current_input[TMXC_DARK_ZONE_COMBINATION_LENGTH];
    uint8_t input_position;
    uint8_t vault_mode;
    uint8_t is_active;
    uint64_t vault_base_address;
    uint64_t vault_size;
    uint64_t guest_base_address;
    uint64_t guest_size;
    uint8_t masking_enabled;
    uint8_t kernel_level_mask;
} tmxc_dark_zone_vault_t;

static tmxc_dark_zone_vault_t tmxc_dark_zone;

static void tmxc_dark_zone_mask_memory_regions(void) {
    if (!tmxc_dark_zone.masking_enabled) {
        return;
    }
    
    if (tmxc_dark_zone.vault_mode == TMXC_DARK_ZONE_GUEST_MODE) {
        uint64_t vault_attributes = TMXC_MMU_AP_EL1_RW_EL0_NONE | TMXC_MMU_AF | TMXC_MMU_PXN | TMXC_MMU_UXN;
        
        for (uint64_t addr = tmxc_dark_zone.vault_base_address; 
             addr < tmxc_dark_zone.vault_base_address + tmxc_dark_zone.vault_size; 
             addr += TMXC_PAGE_SIZE) {
            tmxc_mmu_map_page(addr, addr, vault_attributes);
        }
        
        uint64_t guest_attributes = TMXC_MMU_AP_EL1_RW_EL0_RW | TMXC_MMU_AF;
        
        for (uint64_t addr = tmxc_dark_zone.guest_base_address; 
             addr < tmxc_dark_zone.guest_base_address + tmxc_dark_zone.guest_size; 
             addr += TMXC_PAGE_SIZE) {
            tmxc_mmu_map_page(addr, addr, guest_attributes);
        }
    } else {
        uint64_t real_attributes = TMXC_MMU_AP_EL1_RW_EL0_RW | TMXC_MMU_AF;
        
        for (uint64_t addr = tmxc_dark_zone.vault_base_address; 
             addr < tmxc_dark_zone.vault_base_address + tmxc_dark_zone.vault_size; 
             addr += TMXC_PAGE_SIZE) {
            tmxc_mmu_map_page(addr, addr, real_attributes);
        }
    }
    
    __asm__ volatile("isb");
    __asm__ volatile("dsb ish");
}

void tmxc_dark_zone_vault_init(void) {
    for (uint8_t i = 0; i < TMXC_DARK_ZONE_COMBINATION_LENGTH; i++) {
        tmxc_dark_zone.combination[i] = 0;
        tmxc_dark_zone.current_input[i] = 0;
    }
    
    tmxc_dark_zone.input_position = 0;
    tmxc_dark_zone.vault_mode = TMXC_DARK_ZONE_GUEST_MODE;
    tmxc_dark_zone.is_active = 0;
    tmxc_dark_zone.vault_base_address = 0x90000000ULL;
    tmxc_dark_zone.vault_size = 0x10000000ULL;
    tmxc_dark_zone.guest_base_address = 0xA0000000ULL;
    tmxc_dark_zone.guest_size = 0x10000000ULL;
    tmxc_dark_zone.masking_enabled = 1;
    tmxc_dark_zone.kernel_level_mask = 1;
    
    tmxc_uart_puts("[DARK-ZONE] Dark-Zone Vault initialized\r\n");
}

void tmxc_dark_zone_set_combination(const uint8_t* combination) {
    if (combination == NULL) {
        return;
    }
    
    for (uint8_t i = 0; i < TMXC_DARK_ZONE_COMBINATION_LENGTH; i++) {
        tmxc_dark_zone.combination[i] = combination[i];
    }
    
    tmxc_dark_zone.is_active = 1;
    tmxc_uart_puts("[DARK-ZONE] Vault combination set\r\n");
}

void tmxc_dark_zone_input_key(uint8_t key) {
    if (!tmxc_dark_zone.is_active) {
        return;
    }
    
    if (tmxc_dark_zone.input_position < TMXC_DARK_ZONE_COMBINATION_LENGTH) {
        tmxc_dark_zone.current_input[tmxc_dark_zone.input_position] = key;
        tmxc_dark_zone.input_position++;
        
        if (tmxc_dark_zone.input_position == TMXC_DARK_ZONE_COMBINATION_LENGTH) {
            tmxc_dark_zone_verify_combination();
        }
    }
}

void tmxc_dark_zone_verify_combination(void) {
    uint8_t match = 1;
    
    for (uint8_t i = 0; i < TMXC_DARK_ZONE_COMBINATION_LENGTH; i++) {
        if (tmxc_dark_zone.current_input[i] != tmxc_dark_zone.combination[i]) {
            match = 0;
            break;
        }
    }
    
    if (match) {
        tmxc_dark_zone_toggle_mode();
    }
    
    for (uint8_t i = 0; i < TMXC_DARK_ZONE_COMBINATION_LENGTH; i++) {
        tmxc_dark_zone.current_input[i] = 0;
    }
    tmxc_dark_zone.input_position = 0;
}

void tmxc_dark_zone_toggle_mode(void) {
    if (tmxc_dark_zone.vault_mode == TMXC_DARK_ZONE_GUEST_MODE) {
        tmxc_dark_zone.vault_mode = TMXC_DARK_ZONE_REAL_MODE;
        tmxc_uart_puts("[DARK-ZONE] Switching to REAL MODE - Vault data accessible\r\n");
    } else {
        tmxc_dark_zone.vault_mode = TMXC_DARK_ZONE_GUEST_MODE;
        tmxc_uart_puts("[DARK-ZONE] Switching to GUEST MODE - Vault data hidden\r\n");
    }
    
    tmxc_dark_zone_mask_memory_regions();
}

void tmxc_dark_zone_enable_masking(uint8_t enable) {
    tmxc_dark_zone.masking_enabled = enable;
    tmxc_uart_puts("[DARK-ZONE] Memory masking ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
    
    if (enable) {
        tmxc_dark_zone_mask_memory_regions();
    }
}

uint8_t tmxc_dark_zone_is_masking_enabled(void) {
    return tmxc_dark_zone.masking_enabled;
}

uint8_t tmxc_dark_zone_get_mode(void) {
    return tmxc_dark_zone.vault_mode;
}

uint8_t tmxc_dark_zone_is_real_mode(void) {
    return tmxc_dark_zone.vault_mode == TMXC_DARK_ZONE_REAL_MODE;
}

uint8_t tmxc_dark_zone_is_guest_mode(void) {
    return tmxc_dark_zone.vault_mode == TMXC_DARK_ZONE_GUEST_MODE;
}

void tmxc_dark_zone_set_vault_region(uint64_t base_address, uint64_t size) {
    tmxc_dark_zone.vault_base_address = base_address;
    tmxc_dark_zone.vault_size = size;
}

void tmxc_dark_zone_set_guest_region(uint64_t base_address, uint64_t size) {
    tmxc_dark_zone.guest_base_address = base_address;
    tmxc_dark_zone.guest_size = size;
}

uint64_t tmxc_dark_zone_get_vault_base(void) {
    if (tmxc_dark_zone.vault_mode == TMXC_DARK_ZONE_REAL_MODE) {
        return tmxc_dark_zone.vault_base_address;
    }
    return tmxc_dark_zone.guest_base_address;
}

uint64_t tmxc_dark_zone_get_vault_size(void) {
    if (tmxc_dark_zone.vault_mode == TMXC_DARK_ZONE_REAL_MODE) {
        return tmxc_dark_zone.vault_size;
    }
    return tmxc_dark_zone.guest_size;
}

void tmxc_dark_zone_force_real_mode(void) {
    tmxc_dark_zone.vault_mode = TMXC_DARK_ZONE_REAL_MODE;
    tmxc_dark_zone_mask_memory_regions();
    tmxc_uart_puts("[DARK-ZONE] Forced REAL MODE\r\n");
}

void tmxc_dark_zone_force_guest_mode(void) {
    tmxc_dark_zone.vault_mode = TMXC_DARK_ZONE_GUEST_MODE;
    tmxc_dark_zone_mask_memory_regions();
    tmxc_uart_puts("[DARK-ZONE] Forced GUEST MODE\r\n");
}

uint8_t tmxc_dark_zone_is_active(void) {
    return tmxc_dark_zone.is_active;
}
