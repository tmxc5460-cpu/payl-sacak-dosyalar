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

#define TMXC_MAX_PROFILES 10
#define TMXC_PROFILE_NAME_LENGTH 32
#define TMXC_PROFILE_ENCRYPTION_KEY_LENGTH 32
#define TMXC_GUEST_PROFILE_ID 0xFF

typedef enum {
    TMXC_PROFILE_STATE_INACTIVE = 0,
    TMXC_PROFILE_STATE_ACTIVE = 1,
    TMXC_PROFILE_STATE_LOCKED = 2,
    TMXC_PROFILE_STATE_GUEST = 3
} tmxc_profile_state_t;

typedef struct {
    uint8_t profile_id;
    char profile_name[TMXC_PROFILE_NAME_LENGTH];
    uint8_t encryption_key[TMXC_PROFILE_ENCRYPTION_KEY_LENGTH];
    uint64_t storage_offset;
    uint64_t storage_size;
    uint32_t gallery_count;
    uint32_t app_count;
    tmxc_profile_state_t state;
    uint64_t last_access_time;
    uint8_t biometric_hash[32];
    uint8_t pin_hash[16];
    uint8_t is_isolated;
    uint8_t data_hidden;
} tmxc_profile_t;

typedef struct {
    tmxc_profile_t profiles[TMXC_MAX_PROFILES];
    uint8_t current_profile_id;
    uint8_t guest_mode_enabled;
    uint8_t initialized;
    uint8_t isolation_active;
    uint64_t total_profiles_created;
    uint8_t master_key[32];
} tmxc_multi_account_t;

static tmxc_multi_account_t tmxc_multi_account;

void tmxc_multi_account_init(void) {
    tmxc_multi_account.initialized = 0;
    tmxc_multi_account.current_profile_id = 0;
    tmxc_multi_account.guest_mode_enabled = 0;
    tmxc_multi_account.isolation_active = 1;
    tmxc_multi_account.total_profiles_created = 0;
    
    for (uint32_t i = 0; i < TMXC_MAX_PROFILES; i++) {
        tmxc_multi_account.profiles[i].profile_id = i;
        for (int j = 0; j < TMXC_PROFILE_NAME_LENGTH; j++) {
            tmxc_multi_account.profiles[i].profile_name[j] = 0;
        }
        for (int j = 0; j < TMXC_PROFILE_ENCRYPTION_KEY_LENGTH; j++) {
            tmxc_multi_account.profiles[i].encryption_key[j] = 0;
        }
        tmxc_multi_account.profiles[i].storage_offset = 0;
        tmxc_multi_account.profiles[i].storage_size = 0;
        tmxc_multi_account.profiles[i].gallery_count = 0;
        tmxc_multi_account.profiles[i].app_count = 0;
        tmxc_multi_account.profiles[i].state = TMXC_PROFILE_STATE_INACTIVE;
        tmxc_multi_account.profiles[i].last_access_time = 0;
        for (int j = 0; j < 32; j++) {
            tmxc_multi_account.profiles[i].biometric_hash[j] = 0;
        }
        for (int j = 0; j < 16; j++) {
            tmxc_multi_account.profiles[i].pin_hash[j] = 0;
        }
        tmxc_multi_account.profiles[i].is_isolated = 1;
        tmxc_multi_account.profiles[i].data_hidden = 0;
    }
    
    for (int i = 0; i < 32; i++) {
        tmxc_multi_account.master_key[i] = 0;
    }
    
    tmxc_multi_account.initialized = 1;
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Multi-account system initialized\r\n");
}

uint8_t tmxc_create_profile(const char* profile_name, const uint8_t* encryption_key) {
    if (!tmxc_multi_account.initialized || profile_name == NULL) {
        return TMXC_GUEST_PROFILE_ID;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_PROFILES; i++) {
        if (tmxc_multi_account.profiles[i].state == TMXC_PROFILE_STATE_INACTIVE) {
            for (int j = 0; j < TMXC_PROFILE_NAME_LENGTH && profile_name[j] != 0; j++) {
                tmxc_multi_account.profiles[i].profile_name[j] = profile_name[j];
            }
            
            if (encryption_key != NULL) {
                for (int j = 0; j < TMXC_PROFILE_ENCRYPTION_KEY_LENGTH; j++) {
                    tmxc_multi_account.profiles[i].encryption_key[j] = encryption_key[j];
                }
            } else {
                for (int j = 0; j < TMXC_PROFILE_ENCRYPTION_KEY_LENGTH; j++) {
                    tmxc_multi_account.profiles[i].encryption_key[j] = tmxc_get_cycle_count() & 0xFF;
                }
            }
            
            tmxc_multi_account.profiles[i].storage_offset = i * (1024 * 1024 * 1024);
            tmxc_multi_account.profiles[i].storage_size = 1024 * 1024 * 1024;
            tmxc_multi_account.profiles[i].state = TMXC_PROFILE_STATE_LOCKED;
            tmxc_multi_account.profiles[i].last_access_time = tmxc_get_cycle_count();
            tmxc_multi_account.total_profiles_created++;
            
            tmxc_uart_puts("[MULTI-ACCOUNT] Profile created: ");
            tmxc_uart_puts(profile_name);
            tmxc_uart_puts("\r\n");
            
            return i;
        }
    }
    
    return TMXC_GUEST_PROFILE_ID;
}

uint8_t tmxc_switch_profile(uint8_t profile_id, const uint8_t* auth_data) {
    if (!tmxc_multi_account.initialized || profile_id >= TMXC_MAX_PROFILES) {
        return 0;
    }
    
    if (tmxc_multi_account.profiles[profile_id].state == TMXC_PROFILE_STATE_INACTIVE) {
        return 0;
    }
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Switching to profile: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = profile_id;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
    
    if (tmxc_multi_account.current_profile_id != TMXC_GUEST_PROFILE_ID && 
        tmxc_multi_account.current_profile_id < TMXC_MAX_PROFILES) {
        tmxc_multi_account.profiles[tmxc_multi_account.current_profile_id].state = TMXC_PROFILE_STATE_LOCKED;
        tmxc_multi_account.profiles[tmxc_multi_account.current_profile_id].data_hidden = 1;
    }
    
    tmxc_multi_account.current_profile_id = profile_id;
    tmxc_multi_account.profiles[profile_id].state = TMXC_PROFILE_STATE_ACTIVE;
    tmxc_multi_account.profiles[profile_id].data_hidden = 0;
    tmxc_multi_account.profiles[profile_id].last_access_time = tmxc_get_cycle_count();
    
    tmxc_multi_account.isolation_active = 1;
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Profile switched successfully\r\n");
    
    return 1;
}

void tmxc_enable_guest_mode(void) {
    if (!tmxc_multi_account.initialized) {
        return;
    }
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Enabling guest mode...\r\n");
    
    if (tmxc_multi_account.current_profile_id != TMXC_GUEST_PROFILE_ID && 
        tmxc_multi_account.current_profile_id < TMXC_MAX_PROFILES) {
        tmxc_multi_account.profiles[tmxc_multi_account.current_profile_id].state = TMXC_PROFILE_STATE_LOCKED;
        tmxc_multi_account.profiles[tmxc_multi_account.current_profile_id].data_hidden = 1;
    }
    
    tmxc_multi_account.current_profile_id = TMXC_GUEST_PROFILE_ID;
    tmxc_multi_account.guest_mode_enabled = 1;
    
    for (uint32_t i = 0; i < TMXC_MAX_PROFILES; i++) {
        if (tmxc_multi_account.profiles[i].state != TMXC_PROFILE_STATE_INACTIVE) {
            tmxc_multi_account.profiles[i].data_hidden = 1;
        }
    }
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Guest mode enabled - All personal data hidden\r\n");
}

void tmxc_disable_guest_mode(void) {
    if (!tmxc_multi_account.initialized || !tmxc_multi_account.guest_mode_enabled) {
        return;
    }
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Disabling guest mode...\r\n");
    
    tmxc_multi_account.guest_mode_enabled = 0;
    
    for (uint32_t i = 0; i < TMXC_MAX_PROFILES; i++) {
        tmxc_multi_account.profiles[i].data_hidden = 0;
    }
    
    tmxc_multi_account.current_profile_id = 0;
    tmxc_multi_account.profiles[0].state = TMXC_PROFILE_STATE_ACTIVE;
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Guest mode disabled\r\n");
}

uint8_t tmxc_isolate_profile_data(uint8_t profile_id) {
    if (!tmxc_multi_account.initialized || profile_id >= TMXC_MAX_PROFILES) {
        return 0;
    }
    
    tmxc_multi_account.profiles[profile_id].is_isolated = 1;
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Profile data isolated\r\n");
    
    return 1;
}

uint8_t tmxc_hide_profile_data(uint8_t profile_id) {
    if (!tmxc_multi_account.initialized || profile_id >= TMXC_MAX_PROFILES) {
        return 0;
    }
    
    tmxc_multi_account.profiles[profile_id].data_hidden = 1;
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Profile data hidden\r\n");
    
    return 1;
}

uint8_t tmxc_show_profile_data(uint8_t profile_id) {
    if (!tmxc_multi_account.initialized || profile_id >= TMXC_MAX_PROFILES) {
        return 0;
    }
    
    tmxc_multi_account.profiles[profile_id].data_hidden = 0;
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Profile data visible\r\n");
    
    return 1;
}

uint8_t tmxc_delete_profile(uint8_t profile_id) {
    if (!tmxc_multi_account.initialized || profile_id >= TMXC_MAX_PROFILES) {
        return 0;
    }
    
    if (tmxc_multi_account.current_profile_id == profile_id) {
        return 0;
    }
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Deleting profile...\r\n");
    
    for (int j = 0; j < TMXC_PROFILE_NAME_LENGTH; j++) {
        tmxc_multi_account.profiles[profile_id].profile_name[j] = 0;
    }
    for (int j = 0; j < TMXC_PROFILE_ENCRYPTION_KEY_LENGTH; j++) {
        tmxc_multi_account.profiles[profile_id].encryption_key[j] = 0;
    }
    tmxc_multi_account.profiles[profile_id].storage_offset = 0;
    tmxc_multi_account.profiles[profile_id].storage_size = 0;
    tmxc_multi_account.profiles[profile_id].gallery_count = 0;
    tmxc_multi_account.profiles[profile_id].app_count = 0;
    tmxc_multi_account.profiles[profile_id].state = TMXC_PROFILE_STATE_INACTIVE;
    tmxc_multi_account.profiles[profile_id].last_access_time = 0;
    for (int j = 0; j < 32; j++) {
        tmxc_multi_account.profiles[profile_id].biometric_hash[j] = 0;
    }
    for (int j = 0; j < 16; j++) {
        tmxc_multi_account.profiles[profile_id].pin_hash[j] = 0;
    }
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Profile deleted\r\n");
    
    return 1;
}

uint8_t tmxc_get_current_profile_id(void) {
    return tmxc_multi_account.current_profile_id;
}

tmxc_profile_t* tmxc_get_profile(uint8_t profile_id) {
    if (!tmxc_multi_account.initialized || profile_id >= TMXC_MAX_PROFILES) {
        return NULL;
    }
    
    return &tmxc_multi_account.profiles[profile_id];
}

uint8_t tmxc_is_guest_mode_active(void) {
    return tmxc_multi_account.guest_mode_enabled;
}

uint8_t tmxc_is_data_isolated(void) {
    return tmxc_multi_account.isolation_active;
}

uint32_t tmxc_get_total_profiles(void) {
    return tmxc_multi_account.total_profiles_created;
}

void tmxc_multi_account_cleanup(void) {
    if (!tmxc_multi_account.initialized) {
        return;
    }
    
    tmxc_disable_guest_mode();
    
    for (uint32_t i = 0; i < TMXC_MAX_PROFILES; i++) {
        tmxc_multi_account.profiles[i].state = TMXC_PROFILE_STATE_LOCKED;
        tmxc_multi_account.profiles[i].data_hidden = 1;
    }
    
    tmxc_multi_account.initialized = 0;
    
    tmxc_uart_puts("[MULTI-ACCOUNT] Multi-account system cleaned up\r\n");
}
