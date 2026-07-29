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

#define TMXC_MAX_FROZEN_APPS 32
#define TMXC_FREEZE_TIMEOUT_MS 300000

typedef enum {
    TMXC_APP_STATE_ACTIVE = 0,
    TMXC_APP_STATE_FROZEN = 1,
    TMXC_APP_STATE_SUSPENDED = 2
} tmxc_app_state_t;

typedef struct {
    uint32_t pid;
    char app_name[32];
    tmxc_app_state_t state;
    uint64_t memory_before_freeze;
    uint64_t last_activity;
    uint8_t is_frozen;
} tmxc_frozen_app_t;

typedef struct {
    tmxc_frozen_app_t frozen_apps[TMXC_MAX_FROZEN_APPS];
    uint32_t frozen_count;
    uint8_t initialized;
    uint8_t auto_freeze_enabled;
    uint64_t total_memory_saved;
    uint32_t apps_frozen_total;
} tmxc_app_freezer_t;

static tmxc_app_freezer_t tmxc_freezer;

void tmxc_app_freezer_init(void) {
    tmxc_freezer.initialized = 0;
    tmxc_freezer.auto_freeze_enabled = 1;
    tmxc_freezer.frozen_count = 0;
    tmxc_freezer.total_memory_saved = 0;
    tmxc_freezer.apps_frozen_total = 0;
    
    for (uint32_t i = 0; i < TMXC_MAX_FROZEN_APPS; i++) {
        tmxc_freezer.frozen_apps[i].pid = 0;
        for (int j = 0; j < 32; j++) {
            tmxc_freezer.frozen_apps[i].app_name[j] = 0;
        }
        tmxc_freezer.frozen_apps[i].state = TMXC_APP_STATE_ACTIVE;
        tmxc_freezer.frozen_apps[i].memory_before_freeze = 0;
        tmxc_freezer.frozen_apps[i].last_activity = 0;
        tmxc_freezer.frozen_apps[i].is_frozen = 0;
    }
    
    tmxc_freezer.initialized = 1;
    
    tmxc_uart_puts("[APP-FREEZER] App freezer initialized\r\n");
}

uint32_t tmxc_app_freeze(uint32_t pid, const char* app_name) {
    if (!tmxc_freezer.initialized || tmxc_freezer.frozen_count >= TMXC_MAX_FROZEN_APPS) {
        return TMXC_MAX_FROZEN_APPS;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_FROZEN_APPS; i++) {
        if (tmxc_freezer.frozen_apps[i].pid == pid && tmxc_freezer.frozen_apps[i].is_frozen) {
            return i;
        }
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_FROZEN_APPS; i++) {
        if (!tmxc_freezer.frozen_apps[i].is_frozen) {
            tmxc_freezer.frozen_apps[i].pid = pid;
            tmxc_freezer.frozen_apps[i].state = TMXC_APP_STATE_FROZEN;
            tmxc_freezer.frozen_apps[i].memory_before_freeze = tmxc_pmm.free_pages * TMXC_PAGE_SIZE;
            tmxc_freezer.frozen_apps[i].last_activity = tmxc_get_cycle_count();
            tmxc_freezer.frozen_apps[i].is_frozen = 1;
            
            if (app_name != NULL) {
                for (int j = 0; j < 32 && app_name[j] != 0; j++) {
                    tmxc_freezer.frozen_apps[i].app_name[j] = app_name[j];
                }
            }
            
            tmxc_freezer.frozen_count++;
            tmxc_freezer.apps_frozen_total++;
            
            tmxc_uart_puts("[APP-FREEZER] Frozen app: ");
            tmxc_uart_puts(app_name ? app_name : "Unknown");
            tmxc_uart_puts("\r\n");
            
            return i;
        }
    }
    
    return TMXC_MAX_FROZEN_APPS;
}

void tmxc_app_thaw(uint32_t pid) {
    if (!tmxc_freezer.initialized) {
        return;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_FROZEN_APPS; i++) {
        if (tmxc_freezer.frozen_apps[i].pid == pid && tmxc_freezer.frozen_apps[i].is_frozen) {
            tmxc_freezer.frozen_apps[i].state = TMXC_APP_STATE_ACTIVE;
            tmxc_freezer.frozen_apps[i].is_frozen = 0;
            tmxc_freezer.frozen_count--;
            
            tmxc_uart_puts("[APP-FREEZER] Thawed app: ");
            tmxc_uart_puts(tmxc_freezer.frozen_apps[i].app_name);
            tmxc_uart_puts("\r\n");
            
            return;
        }
    }
}

void tmxc_app_freezer_check_inactive(void) {
    if (!tmxc_freezer.initialized || !tmxc_freezer.auto_freeze_enabled) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    
    for (uint32_t i = 0; i < TMXC_MAX_FROZEN_APPS; i++) {
        if (tmxc_freezer.frozen_apps[i].is_frozen) {
            uint64_t elapsed_cycles = current_time - tmxc_freezer.frozen_apps[i].last_activity;
            uint64_t elapsed_ms = elapsed_cycles * 1000 / tmxc_get_frequency();
            
            if (elapsed_ms > TMXC_FREEZE_TIMEOUT_MS) {
                tmxc_app_freeze(tmxc_freezer.frozen_apps[i].pid, tmxc_freezer.frozen_apps[i].app_name);
            }
        }
    }
}

void tmxc_app_freezer_freeze_all_background(void) {
    if (!tmxc_freezer.initialized) {
        return;
    }
    
    tmxc_uart_puts("[APP-FREEZER] Freezing all background apps...\r\n");
    
    for (uint32_t i = 0; i < TMXC_MAX_FROZEN_APPS; i++) {
        if (tmxc_freezer.frozen_apps[i].state == TMXC_APP_STATE_ACTIVE && !tmxc_freezer.frozen_apps[i].is_frozen) {
            tmxc_app_freeze(tmxc_freezer.frozen_apps[i].pid, tmxc_freezer.frozen_apps[i].app_name);
        }
    }
}

void tmxc_app_freezer_thaw_all(void) {
    if (!tmxc_freezer.initialized) {
        return;
    }
    
    tmxc_uart_puts("[APP-FREEZER] Thawing all apps...\r\n");
    
    for (uint32_t i = 0; i < TMXC_MAX_FROZEN_APPS; i++) {
        if (tmxc_freezer.frozen_apps[i].is_frozen) {
            tmxc_app_thaw(tmxc_freezer.frozen_apps[i].pid);
        }
    }
}

void tmxc_app_freezer_enable_auto_freeze(uint8_t enable) {
    if (!tmxc_freezer.initialized) {
        return;
    }
    
    tmxc_freezer.auto_freeze_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[APP-FREEZER] Auto-freeze enabled\r\n");
    } else {
        tmxc_uart_puts("[APP-FREEZER] Auto-freeze disabled\r\n");
    }
}

uint32_t tmxc_app_freezer_get_frozen_count(void) {
    return tmxc_freezer.frozen_count;
}

uint64_t tmxc_app_freezer_get_memory_saved(void) {
    return tmxc_freezer.total_memory_saved;
}

void tmxc_app_freezer_cleanup(void) {
    if (!tmxc_freezer.initialized) {
        return;
    }
    
    tmxc_app_freezer_thaw_all();
    tmxc_freezer.initialized = 0;
    
    tmxc_uart_puts("[APP-FREEZER] App freezer cleaned up\r\n");
}
