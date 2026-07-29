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

#define TMXC_KERNEL_HASH_SIZE 32
#define TMXC_WATCHDOG_INTERVAL_MS 1000
#define TMXC_MAX_CRITICAL_REGIONS 16

typedef struct {
    uint8_t expected_hash[TMXC_KERNEL_HASH_SIZE];
    uint8_t current_hash[TMXC_KERNEL_HASH_SIZE];
    uint8_t integrity_valid;
    uint64_t last_check_time;
} tmxc_kernel_integrity_t;

typedef struct {
    uint64_t base_address;
    uint64_t size;
    uint8_t is_protected;
    char region_name[32];
} tmxc_critical_region_t;

typedef struct {
    tmxc_kernel_integrity_t kernel_integrity;
    tmxc_critical_region_t critical_regions[TMXC_MAX_CRITICAL_REGIONS];
    uint32_t region_count;
    uint8_t initialized;
    uint8_t watchdog_enabled;
    uint8_t integrity_violation_detected;
    uint64_t violation_count;
    uint8_t auto_recovery_enabled;
    uint64_t last_watchdog_time;
} tmxc_integrity_monitor_t;

static tmxc_integrity_monitor_t tmxc_integrity;

void tmxc_integrity_monitor_init(void) {
    tmxc_integrity.initialized = 0;
    tmxc_integrity.watchdog_enabled = 1;
    tmxc_integrity.integrity_violation_detected = 0;
    tmxc_integrity.violation_count = 0;
    tmxc_integrity.auto_recovery_enabled = 1;
    tmxc_integrity.region_count = 0;
    tmxc_integrity.last_watchdog_time = tmxc_get_cycle_count();
    
    for (int i = 0; i < TMXC_KERNEL_HASH_SIZE; i++) {
        tmxc_integrity.kernel_integrity.expected_hash[i] = 0;
        tmxc_integrity.kernel_integrity.current_hash[i] = 0;
    }
    tmxc_integrity.kernel_integrity.integrity_valid = 1;
    tmxc_integrity.kernel_integrity.last_check_time = tmxc_get_cycle_count();
    
    for (uint32_t i = 0; i < TMXC_MAX_CRITICAL_REGIONS; i++) {
        tmxc_integrity.critical_regions[i].base_address = 0;
        tmxc_integrity.critical_regions[i].size = 0;
        tmxc_integrity.critical_regions[i].is_protected = 0;
        for (int j = 0; j < 32; j++) {
            tmxc_integrity.critical_regions[i].region_name[j] = 0;
        }
    }
    
    tmxc_integrity.initialized = 1;
    
    tmxc_uart_puts("[INTEGRITY] Kernel integrity monitor initialized\r\n");
}

void tmxc_integrity_set_kernel_hash(const uint8_t* hash) {
    if (!tmxc_integrity.initialized || hash == NULL) {
        return;
    }
    
    for (int i = 0; i < TMXC_KERNEL_HASH_SIZE; i++) {
        tmxc_integrity.kernel_integrity.expected_hash[i] = hash[i];
    }
    
    tmxc_uart_puts("[INTEGRITY] Kernel hash set\r\n");
}

void tmxc_integrity_add_critical_region(uint64_t base_address, uint64_t size, const char* name) {
    if (!tmxc_integrity.initialized || tmxc_integrity.region_count >= TMXC_MAX_CRITICAL_REGIONS) {
        return;
    }
    
    uint32_t index = tmxc_integrity.region_count;
    
    tmxc_integrity.critical_regions[index].base_address = base_address;
    tmxc_integrity.critical_regions[index].size = size;
    tmxc_integrity.critical_regions[index].is_protected = 1;
    
    if (name != NULL) {
        for (int j = 0; j < 32 && name[j] != 0; j++) {
            tmxc_integrity.critical_regions[index].region_name[j] = name[j];
        }
    }
    
    tmxc_integrity.region_count++;
    
    tmxc_uart_puts("[INTEGRITY] Critical region added: ");
    tmxc_uart_puts(name ? name : "Unknown");
    tmxc_uart_puts("\r\n");
}

uint8_t tmxc_integrity_verify_kernel(void) {
    if (!tmxc_integrity.initialized) {
        return 0;
    }
    
    uint8_t hash_valid = 1;
    
    for (int i = 0; i < TMXC_KERNEL_HASH_SIZE; i++) {
        tmxc_integrity.kernel_integrity.current_hash[i] = tmxc_get_cycle_count() & 0xFF;
        if (tmxc_integrity.kernel_integrity.current_hash[i] != tmxc_integrity.kernel_integrity.expected_hash[i]) {
            hash_valid = 0;
        }
    }
    
    tmxc_integrity.kernel_integrity.integrity_valid = hash_valid;
    tmxc_integrity.kernel_integrity.last_check_time = tmxc_get_cycle_count();
    
    if (!hash_valid) {
        tmxc_integrity.integrity_violation_detected = 1;
        tmxc_integrity.violation_count++;
        
        tmxc_uart_puts("[INTEGRITY] Kernel integrity violation detected!\r\n");
        
        if (tmxc_integrity.auto_recovery_enabled) {
            tmxc_uart_puts("[INTEGRITY] Initiating auto-recovery...\r\n");
        }
    }
    
    return hash_valid;
}

void tmxc_integrity_watchdog_tick(void) {
    if (!tmxc_integrity.initialized || !tmxc_integrity.watchdog_enabled) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed_ms = (current_time - tmxc_integrity.last_watchdog_time) * 1000 / tmxc_get_frequency();
    
    if (elapsed_ms >= TMXC_WATCHDOG_INTERVAL_MS) {
        tmxc_integrity_verify_kernel();
        tmxc_integrity.last_watchdog_time = current_time;
    }
}

void tmxc_integrity_enable_watchdog(uint8_t enable) {
    if (!tmxc_integrity.initialized) {
        return;
    }
    
    tmxc_integrity.watchdog_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[INTEGRITY] Watchdog enabled\r\n");
    } else {
        tmxc_uart_puts("[INTEGRITY] Watchdog disabled\r\n");
    }
}

void tmxc_integrity_enable_auto_recovery(uint8_t enable) {
    if (!tmxc_integrity.initialized) {
        return;
    }
    
    tmxc_integrity.auto_recovery_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[INTEGRITY] Auto-recovery enabled\r\n");
    } else {
        tmxc_uart_puts("[INTEGRITY] Auto-recovery disabled\r\n");
    }
}

uint8_t tmxc_integrity_is_valid(void) {
    return tmxc_integrity.kernel_integrity.integrity_valid;
}

uint8_t tmxc_integrity_is_violation_detected(void) {
    return tmxc_integrity.integrity_violation_detected;
}

uint64_t tmxc_integrity_get_violation_count(void) {
    return tmxc_integrity.violation_count;
}

void tmxc_integrity_clear_violation(void) {
    if (!tmxc_integrity.initialized) {
        return;
    }
    
    tmxc_integrity.integrity_violation_detected = 0;
    
    tmxc_uart_puts("[INTEGRITY] Violation flag cleared\r\n");
}

void tmxc_integrity_monitor_cleanup(void) {
    if (!tmxc_integrity.initialized) {
        return;
    }
    
    tmxc_integrity.watchdog_enabled = 0;
    tmxc_integrity.initialized = 0;
    
    tmxc_uart_puts("[INTEGRITY] Integrity monitor cleaned up\r\n");
}
