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

#define TMXC_MAX_SESSIONS 64
#define TMXC_SESSION_TIMEOUT_MS 300000

typedef struct {
    uint32_t pid;
    uint64_t start_time;
    uint64_t last_activity;
    uint8_t is_active;
    uint64_t memory_allocated;
    char app_name[32];
} tmxc_session_t;

typedef struct {
    tmxc_session_t sessions[TMXC_MAX_SESSIONS];
    uint32_t active_session_count;
    uint8_t initialized;
    uint8_t auto_cleanup_enabled;
    uint64_t total_memory_freed;
    uint32_t sessions_cleaned;
} tmxc_session_expiry_t;

static tmxc_session_expiry_t tmxc_session;

void tmxc_session_expiry_init(void) {
    tmxc_session.initialized = 0;
    tmxc_session.auto_cleanup_enabled = 1;
    tmxc_session.active_session_count = 0;
    tmxc_session.total_memory_freed = 0;
    tmxc_session.sessions_cleaned = 0;
    
    for (uint32_t i = 0; i < TMXC_MAX_SESSIONS; i++) {
        tmxc_session.sessions[i].pid = 0;
        tmxc_session.sessions[i].start_time = 0;
        tmxc_session.sessions[i].last_activity = 0;
        tmxc_session.sessions[i].is_active = 0;
        tmxc_session.sessions[i].memory_allocated = 0;
        for (int j = 0; j < 32; j++) {
            tmxc_session.sessions[i].app_name[j] = 0;
        }
    }
    
    tmxc_session.initialized = 1;
    
    tmxc_uart_puts("[SESSION] Session expiry system initialized\r\n");
}

uint32_t tmxc_session_create(uint32_t pid, const char* app_name, uint64_t memory_size) {
    if (!tmxc_session.initialized) {
        return TMXC_MAX_SESSIONS;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_SESSIONS; i++) {
        if (!tmxc_session.sessions[i].is_active) {
            tmxc_session.sessions[i].pid = pid;
            tmxc_session.sessions[i].start_time = tmxc_get_cycle_count();
            tmxc_session.sessions[i].last_activity = tmxc_get_cycle_count();
            tmxc_session.sessions[i].is_active = 1;
            tmxc_session.sessions[i].memory_allocated = memory_size;
            
            if (app_name != NULL) {
                for (int j = 0; j < 32 && app_name[j] != 0; j++) {
                    tmxc_session.sessions[i].app_name[j] = app_name[j];
                }
            }
            
            tmxc_session.active_session_count++;
            
            tmxc_uart_puts("[SESSION] Session created for: ");
            tmxc_uart_puts(app_name ? app_name : "Unknown");
            tmxc_uart_puts("\r\n");
            
            return i;
        }
    }
    
    return TMXC_MAX_SESSIONS;
}

void tmxc_session_update_activity(uint32_t session_id) {
    if (!tmxc_session.initialized || session_id >= TMXC_MAX_SESSIONS) {
        return;
    }
    
    if (tmxc_session.sessions[session_id].is_active) {
        tmxc_session.sessions[session_id].last_activity = tmxc_get_cycle_count();
    }
}

void tmxc_session_expire(uint32_t session_id) {
    if (!tmxc_session.initialized || session_id >= TMXC_MAX_SESSIONS) {
        return;
    }
    
    if (!tmxc_session.sessions[session_id].is_active) {
        return;
    }
    
    tmxc_uart_puts("[SESSION] Expiring session: ");
    tmxc_uart_puts(tmxc_session.sessions[session_id].app_name);
    tmxc_uart_puts("\r\n");
    
    tmxc_session.total_memory_freed += tmxc_session.sessions[session_id].memory_allocated;
    
    tmxc_session.sessions[session_id].is_active = 0;
    tmxc_session.sessions[session_id].memory_allocated = 0;
    tmxc_session.active_session_count--;
    tmxc_session.sessions_cleaned++;
}

void tmxc_session_check_expiry(void) {
    if (!tmxc_session.initialized || !tmxc_session.auto_cleanup_enabled) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    
    for (uint32_t i = 0; i < TMXC_MAX_SESSIONS; i++) {
        if (tmxc_session.sessions[i].is_active) {
            uint64_t elapsed_cycles = current_time - tmxc_session.sessions[i].last_activity;
            uint64_t elapsed_ms = elapsed_cycles * 1000 / tmxc_get_frequency();
            
            if (elapsed_ms > TMXC_SESSION_TIMEOUT_MS) {
                tmxc_session_expire(i);
            }
        }
    }
}

void tmxc_session_expire_all(void) {
    if (!tmxc_session.initialized) {
        return;
    }
    
    tmxc_uart_puts("[SESSION] Expiring all sessions...\r\n");
    
    for (uint32_t i = 0; i < TMXC_MAX_SESSIONS; i++) {
        if (tmxc_session.sessions[i].is_active) {
            tmxc_session_expire(i);
        }
    }
}

void tmxc_session_enable_auto_cleanup(uint8_t enable) {
    if (!tmxc_session.initialized) {
        return;
    }
    
    tmxc_session.auto_cleanup_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[SESSION] Auto cleanup enabled\r\n");
    } else {
        tmxc_uart_puts("[SESSION] Auto cleanup disabled\r\n");
    }
}

uint32_t tmxc_session_get_active_count(void) {
    return tmxc_session.active_session_count;
}

uint64_t tmxc_session_get_memory_freed(void) {
    return tmxc_session.total_memory_freed;
}

void tmxc_session_expiry_cleanup(void) {
    if (!tmxc_session.initialized) {
        return;
    }
    
    tmxc_session_expire_all();
    tmxc_session.initialized = 0;
    
    tmxc_uart_puts("[SESSION] Session expiry system cleaned up\r\n");
}
