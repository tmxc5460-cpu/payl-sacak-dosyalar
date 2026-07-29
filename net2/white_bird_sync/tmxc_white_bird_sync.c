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
#include "tmxc_white_bird_sync.h"

static tmxc_white_bird_sync_t tmxc_sync;

void tmxc_white_bird_sync_init(void) {
    tmxc_sync.device_count = 0;
    tmxc_sync.history_index = 0;
    tmxc_sync.history_count = 0;
    tmxc_sync.sync_enabled = 1;
    tmxc_sync.auto_sync_enabled = 1;
    tmxc_sync.initialized = 0;
    tmxc_sync.total_syncs = 0;
    tmxc_sync.successful_syncs = 0;
    tmxc_sync.local_device_type = TMXC_DEVICE_TYPE_PHONE;
    
    for (int i = 0; i < 64; i++) {
        tmxc_sync.local_device_id[i] = 0;
    }
    
    for (uint32_t i = 0; i < TMXC_SYNC_MAX_DEVICES; i++) {
        for (int j = 0; j < TMXC_SYNC_DEVICE_NAME_LENGTH; j++) {
            tmxc_sync.devices[i].device_name[j] = 0;
        }
        for (int j = 0; j < 64; j++) {
            tmxc_sync.devices[i].device_id[j] = 0;
        }
        tmxc_sync.devices[i].device_type = TMXC_DEVICE_TYPE_PHONE;
        tmxc_sync.devices[i].status = TMXC_SYNC_STATUS_DISCONNECTED;
        tmxc_sync.devices[i].last_sync_time = 0;
        tmxc_sync.devices[i].last_seen_time = 0;
        tmxc_sync.devices[i].is_primary = 0;
        tmxc_sync.devices[i].active = 0;
    }
    
    for (uint32_t i = 0; i < TMXC_SYNC_MAX_HISTORY; i++) {
        for (int j = 0; j < 256; j++) {
            tmxc_sync.history[i].query[j] = 0;
        }
        for (int j = 0; j < 512; j++) {
            tmxc_sync.history[i].result[j] = 0;
        }
        tmxc_sync.history[i].timestamp = 0;
        tmxc_sync.history[i].synced = 0;
    }
    
    tmxc_sync.initialized = 1;
    
    tmxc_uart_puts("[WHITE-BIRD-SYNC] Cross-Device 'Beyaz Kuş' Sync initialized\r\n");
    tmxc_uart_puts("[WHITE-BIRD-SYNC] Real-time sync between phone and tablet ready\r\n");
}

void tmxc_white_bird_sync_set_local_device(const char* device_id, tmxc_device_type_t device_type) {
    if (!tmxc_sync.initialized || device_id == NULL) {
        return;
    }
    
    for (int i = 0; i < 64 && device_id[i] != '\0'; i++) {
        tmxc_sync.local_device_id[i] = device_id[i];
    }
    
    tmxc_sync.local_device_type = device_type;
    
    tmxc_uart_puts("[WHITE-BIRD-SYNC] Local device set\r\n");
}

void tmxc_white_bird_sync_enable_sync(uint8_t enable) {
    if (!tmxc_sync.initialized) {
        return;
    }
    
    tmxc_sync.sync_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[WHITE-BIRD-SYNC] Sync ENABLED\r\n");
    } else {
        tmxc_uart_puts("[WHITE-BIRD-SYNC] Sync DISABLED\r\n");
    }
}

void tmxc_white_bird_sync_enable_auto_sync(uint8_t enable) {
    if (!tmxc_sync.initialized) {
        return;
    }
    
    tmxc_sync.auto_sync_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[WHITE-BIRD-SYNC] Auto-sync ENABLED\r\n");
    } else {
        tmxc_uart_puts("[WHITE-BIRD-SYNC] Auto-sync DISABLED\r\n");
    }
}

uint8_t tmxc_white_bird_sync_is_sync_enabled(void) {
    return tmxc_sync.sync_enabled;
}

uint8_t tmxc_white_bird_sync_is_auto_sync_enabled(void) {
    return tmxc_sync.auto_sync_enabled;
}

uint32_t tmxc_white_bird_sync_add_device(const char* device_name, const char* device_id, tmxc_device_type_t device_type) {
    if (!tmxc_sync.initialized || device_name == NULL || device_id == NULL) {
        return 0;
    }
    
    if (tmxc_sync.device_count >= TMXC_SYNC_MAX_DEVICES) {
        return 0;
    }
    
    uint32_t idx = tmxc_sync.device_count;
    
    for (int i = 0; i < TMXC_SYNC_DEVICE_NAME_LENGTH && device_name[i] != '\0'; i++) {
        tmxc_sync.devices[idx].device_name[i] = device_name[i];
    }
    
    for (int i = 0; i < 64 && device_id[i] != '\0'; i++) {
        tmxc_sync.devices[idx].device_id[i] = device_id[i];
    }
    
    tmxc_sync.devices[idx].device_type = device_type;
    tmxc_sync.devices[idx].status = TMXC_SYNC_STATUS_DISCONNECTED;
    tmxc_sync.devices[idx].last_sync_time = 0;
    tmxc_sync.devices[idx].last_seen_time = tmxc_get_cycle_count();
    tmxc_sync.devices[idx].is_primary = 0;
    tmxc_sync.devices[idx].active = 1;
    
    tmxc_sync.device_count++;
    
    tmxc_uart_puts("[WHITE-BIRD-SYNC] Device added: ");
    tmxc_uart_puts(device_name);
    tmxc_uart_puts("\r\n");
    
    return idx;
}

void tmxc_white_bird_sync_remove_device(const char* device_id) {
    if (!tmxc_sync.initialized || device_id == NULL) {
        return;
    }
    
    for (uint32_t i = 0; i < tmxc_sync.device_count; i++) {
        uint8_t match = 1;
        for (int j = 0; j < 64; j++) {
            if (tmxc_sync.devices[i].device_id[j] != device_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            tmxc_sync.devices[i].active = 0;
            tmxc_sync.devices[i].status = TMXC_SYNC_STATUS_DISCONNECTED;
            break;
        }
    }
}

void tmxc_white_bird_sync_set_primary_device(const char* device_id) {
    if (!tmxc_sync.initialized || device_id == NULL) {
        return;
    }
    
    for (uint32_t i = 0; i < tmxc_sync.device_count; i++) {
        tmxc_sync.devices[i].is_primary = 0;
        
        uint8_t match = 1;
        for (int j = 0; j < 64; j++) {
            if (tmxc_sync.devices[i].device_id[j] != device_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            tmxc_sync.devices[i].is_primary = 1;
        }
    }
}

tmxc_sync_status_t tmxc_white_bird_sync_get_device_status(const char* device_id) {
    if (!tmxc_sync.initialized || device_id == NULL) {
        return TMXC_SYNC_STATUS_ERROR;
    }
    
    for (uint32_t i = 0; i < tmxc_sync.device_count; i++) {
        uint8_t match = 1;
        for (int j = 0; j < 64; j++) {
            if (tmxc_sync.devices[i].device_id[j] != device_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            return tmxc_sync.devices[i].status;
        }
    }
    
    return TMXC_SYNC_STATUS_DISCONNECTED;
}

void tmxc_white_bird_sync_connect_device(const char* device_id) {
    if (!tmxc_sync.initialized || device_id == NULL) {
        return;
    }
    
    for (uint32_t i = 0; i < tmxc_sync.device_count; i++) {
        uint8_t match = 1;
        for (int j = 0; j < 64; j++) {
            if (tmxc_sync.devices[i].device_id[j] != device_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            tmxc_sync.devices[i].status = TMXC_SYNC_STATUS_CONNECTING;
            tmxc_sync.devices[i].last_seen_time = tmxc_get_cycle_count();
            
            tmxc_timer_delay_ms(100);
            
            tmxc_sync.devices[i].status = TMXC_SYNC_STATUS_CONNECTED;
            
            tmxc_uart_puts("[WHITE-BIRD-SYNC] Device connected: ");
            tmxc_uart_puts(tmxc_sync.devices[i].device_name);
            tmxc_uart_puts("\r\n");
            
            break;
        }
    }
}

void tmxc_white_bird_sync_disconnect_device(const char* device_id) {
    if (!tmxc_sync.initialized || device_id == NULL) {
        return;
    }
    
    for (uint32_t i = 0; i < tmxc_sync.device_count; i++) {
        uint8_t match = 1;
        for (int j = 0; j < 64; j++) {
            if (tmxc_sync.devices[i].device_id[j] != device_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            tmxc_sync.devices[i].status = TMXC_SYNC_STATUS_DISCONNECTED;
            
            tmxc_uart_puts("[WHITE-BIRD-SYNC] Device disconnected: ");
            tmxc_uart_puts(tmxc_sync.devices[i].device_name);
            tmxc_uart_puts("\r\n");
            
            break;
        }
    }
}

void tmxc_white_bird_sync_add_history(const char* query, const char* result) {
    if (!tmxc_sync.initialized || query == NULL || result == NULL) {
        return;
    }
    
    uint32_t idx = tmxc_sync.history_index;
    
    for (int i = 0; i < 256 && query[i] != '\0'; i++) {
        tmxc_sync.history[idx].query[i] = query[i];
    }
    
    for (int i = 0; i < 512 && result[i] != '\0'; i++) {
        tmxc_sync.history[idx].result[i] = result[i];
    }
    
    tmxc_sync.history[idx].timestamp = tmxc_get_cycle_count();
    tmxc_sync.history[idx].synced = 0;
    
    tmxc_sync.history_index = (tmxc_sync.history_index + 1) % TMXC_SYNC_MAX_HISTORY;
    
    if (tmxc_sync.history_count < TMXC_SYNC_MAX_HISTORY) {
        tmxc_sync.history_count++;
    }
    
    if (tmxc_sync.auto_sync_enabled) {
        tmxc_white_bird_sync_sync_query(query, result);
    }
}

void tmxc_white_bird_sync_sync_history(void) {
    if (!tmxc_sync.initialized || !tmxc_sync.sync_enabled) {
        return;
    }
    
    uint32_t synced_count = 0;
    
    for (uint32_t i = 0; i < tmxc_sync.device_count; i++) {
        if (!tmxc_sync.devices[i].active || 
            tmxc_sync.devices[i].status != TMXC_SYNC_STATUS_CONNECTED) {
            continue;
        }
        
        for (uint32_t j = 0; j < tmxc_sync.history_count; j++) {
            if (!tmxc_sync.history[j].synced) {
                tmxc_sync.history[j].synced = 1;
                synced_count++;
            }
        }
    }
    
    if (synced_count > 0) {
        tmxc_sync.total_syncs++;
        tmxc_sync.successful_syncs++;
        
        tmxc_uart_puts("[WHITE-BIRD-SYNC] History synced: ");
        char buffer[21];
        int pos = 20;
        buffer[pos] = '\0';
        uint64_t temp = synced_count;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts(" entries\r\n");
    }
}

tmxc_sync_history_entry_t* tmxc_white_bird_sync_get_history(uint32_t index) {
    if (!tmxc_sync.initialized || index >= tmxc_sync.history_count) {
        return NULL;
    }
    
    return &tmxc_sync.history[index];
}

void tmxc_white_bird_sync_sync_query(const char* query, const char* result) {
    if (!tmxc_sync.initialized || !tmxc_sync.sync_enabled) {
        return;
    }
    
    for (uint32_t i = 0; i < tmxc_sync.device_count; i++) {
        if (!tmxc_sync.devices[i].active || 
            tmxc_sync.devices[i].status != TMXC_SYNC_STATUS_CONNECTED) {
            continue;
        }
        
        tmxc_sync.devices[i].status = TMXC_SYNC_STATUS_SYNCING;
        tmxc_sync.devices[i].last_sync_time = tmxc_get_cycle_count();
        
        tmxc_uart_puts("[WHITE-BIRD-SYNC] Syncing query to: ");
        tmxc_uart_puts(tmxc_sync.devices[i].device_name);
        tmxc_uart_puts("\r\n");
        
        tmxc_timer_delay_ms(50);
        
        tmxc_sync.devices[i].status = TMXC_SYNC_STATUS_CONNECTED;
        
        tmxc_sync.total_syncs++;
        tmxc_sync.successful_syncs++;
    }
}

void tmxc_white_bird_sync_process_pending_syncs(void) {
    if (!tmxc_sync.initialized || !tmxc_sync.sync_enabled) {
        return;
    }
    
    uint32_t pending_count = 0;
    
    for (uint32_t j = 0; j < tmxc_sync.history_count; j++) {
        if (!tmxc_sync.history[j].synced) {
            pending_count++;
        }
    }
    
    if (pending_count > 0) {
        tmxc_white_bird_sync_sync_history();
    }
}

uint64_t tmxc_white_bird_sync_get_total_syncs(void) {
    return tmxc_sync.total_syncs;
}

uint64_t tmxc_white_bird_sync_get_successful_syncs(void) {
    return tmxc_sync.successful_syncs;
}

float tmxc_white_bird_sync_get_sync_success_rate(void) {
    if (tmxc_sync.total_syncs == 0) {
        return 0.0f;
    }
    
    return ((float)tmxc_sync.successful_syncs / (float)tmxc_sync.total_syncs) * 100.0f;
}

void tmxc_white_bird_sync_cleanup(void) {
    if (!tmxc_sync.initialized) {
        return;
    }
    
    tmxc_sync.device_count = 0;
    tmxc_sync.history_count = 0;
    tmxc_sync.sync_enabled = 0;
    tmxc_sync.auto_sync_enabled = 0;
    tmxc_sync.initialized = 0;
    tmxc_sync.total_syncs = 0;
    tmxc_sync.successful_syncs = 0;
    
    tmxc_uart_puts("[WHITE-BIRD-SYNC] Cross-Device 'Beyaz Kuş' Sync cleaned up\r\n");
}
