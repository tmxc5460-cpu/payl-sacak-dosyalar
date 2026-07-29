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

#define TMXC_SANDBOX_MAX_PERMISSIONS 32
#define TMXC_SANDBOX_MAX_RESOURCES 64
#define TMXC_SANDBOX_MAX_NETWORK_RULES 16

typedef enum {
    TMXC_SANDBOX_PERMISSION_NONE = 0,
    TMXC_SANDBOX_PERMISSION_READ = 1,
    TMXC_SANDBOX_PERMISSION_WRITE = 2,
    TMXC_SANDBOX_PERMISSION_EXECUTE = 4,
    TMXC_SANDBOX_PERMISSION_NETWORK = 8,
    TMXC_SANDBOX_PERMISSION_CAMERA = 16,
    TMXC_SANDBOX_PERMISSION_MICROPHONE = 32,
    TMXC_SANDBOX_PERMISSION_LOCATION = 64,
    TMXC_SANDBOX_PERMISSION_CONTACTS = 128,
    TMXC_SANDBOX_PERMISSION_STORAGE = 256
} tmxc_sandbox_permission_t;

typedef struct {
    uint32_t pid;
    uint64_t base_address;
    uint64_t size;
    uint32_t permissions[TMXC_SANDBOX_MAX_PERMISSIONS];
    uint32_t permission_count;
    uint8_t is_isolated;
    uint8_t strict_mode;
    uint64_t allowed_resources[TMXC_SANDBOX_MAX_RESOURCES];
    uint32_t resource_count;
    uint32_t network_rules[TMXC_SANDBOX_MAX_NETWORK_RULES];
    uint32_t network_rule_count;
    uint64_t creation_time;
    uint64_t last_access_time;
    uint32_t violation_count;
} tmxc_sandbox_t;

typedef struct {
    uint32_t src_pid;
    uint32_t dst_pid;
    uint64_t access_address;
    uint32_t permission_type;
    uint64_t timestamp;
    uint8_t was_blocked;
} tmxc_sandbox_violation_t;

static tmxc_sandbox_t tmxc_sandboxes[TMXC_MAX_PROCESSES];
static tmxc_sandbox_violation_t tmxc_violations[1024];
static uint32_t tmxc_violation_count = 0;
static uint8_t tmxc_sandbox_enabled = 1;
static uint8_t tmxc_strict_mode_default = 0;

void tmxc_sandbox_init(void) {
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        tmxc_sandboxes[i].pid = 0;
        tmxc_sandboxes[i].base_address = 0;
        tmxc_sandboxes[i].size = 0;
        tmxc_sandboxes[i].permission_count = 0;
        tmxc_sandboxes[i].is_isolated = 0;
        tmxc_sandboxes[i].strict_mode = tmxc_strict_mode_default;
        tmxc_sandboxes[i].resource_count = 0;
        tmxc_sandboxes[i].network_rule_count = 0;
        tmxc_sandboxes[i].creation_time = 0;
        tmxc_sandboxes[i].last_access_time = 0;
        tmxc_sandboxes[i].violation_count = 0;
        
        for (uint32_t j = 0; j < TMXC_SANDBOX_MAX_PERMISSIONS; j++) {
            tmxc_sandboxes[i].permissions[j] = 0;
        }
        for (uint32_t j = 0; j < TMXC_SANDBOX_MAX_RESOURCES; j++) {
            tmxc_sandboxes[i].allowed_resources[j] = 0;
        }
        for (uint32_t j = 0; j < TMXC_SANDBOX_MAX_NETWORK_RULES; j++) {
            tmxc_sandboxes[i].network_rules[j] = 0;
        }
    }
    
    for (uint32_t i = 0; i < 1024; i++) {
        tmxc_violations[i].src_pid = 0;
        tmxc_violations[i].dst_pid = 0;
        tmxc_violations[i].access_address = 0;
        tmxc_violations[i].permission_type = 0;
        tmxc_violations[i].timestamp = 0;
        tmxc_violations[i].was_blocked = 0;
    }
    
    tmxc_violation_count = 0;
}

uint8_t tmxc_sandbox_create(uint32_t pid, uint64_t base_address, uint64_t size) {
    if (pid >= TMXC_MAX_PROCESSES || !tmxc_sandbox_enabled) {
        return 0;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (tmxc_sandboxes[i].pid == pid) {
            return 0;
        }
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (tmxc_sandboxes[i].pid == 0) {
            tmxc_sandboxes[i].pid = pid;
            tmxc_sandboxes[i].base_address = base_address;
            tmxc_sandboxes[i].size = size;
            tmxc_sandboxes[i].is_isolated = 1;
            tmxc_sandboxes[i].strict_mode = tmxc_strict_mode_default;
            tmxc_sandboxes[i].creation_time = tmxc_get_cycle_count();
            tmxc_sandboxes[i].last_access_time = tmxc_get_cycle_count();
            tmxc_sandboxes[i].violation_count = 0;
            
            tmxc_uart_puts("[SANDBOX] Created sandbox for PID: ");
            char buffer[21];
            int pos = 20;
            buffer[pos] = '\0';
            uint64_t temp = pid;
            while (temp > 0 && pos > 0) {
                pos--;
                buffer[pos] = '0' + (temp % 10);
                temp /= 10;
            }
            tmxc_uart_puts(&buffer[pos]);
            tmxc_uart_puts("\r\n");
            
            return 1;
        }
    }
    
    return 0;
}

uint8_t tmxc_sandbox_add_permission(uint32_t pid, uint32_t permission) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return 0;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (tmxc_sandboxes[i].pid == pid) {
            if (tmxc_sandboxes[i].permission_count < TMXC_SANDBOX_MAX_PERMISSIONS) {
                tmxc_sandboxes[i].permissions[tmxc_sandboxes[i].permission_count] = permission;
                tmxc_sandboxes[i].permission_count++;
                return 1;
            }
        }
    }
    
    return 0;
}

uint8_t tmxc_sandbox_check_permission(uint32_t pid, uint32_t permission) {
    if (pid >= TMXC_MAX_PROCESSES || !tmxc_sandbox_enabled) {
        return 1;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (tmxc_sandboxes[i].pid == pid) {
            if (!tmxc_sandboxes[i].is_isolated) {
                return 1;
            }
            
            for (uint32_t j = 0; j < tmxc_sandboxes[i].permission_count; j++) {
                if (tmxc_sandboxes[i].permissions[j] == permission) {
                    tmxc_sandboxes[i].last_access_time = tmxc_get_cycle_count();
                    return 1;
                }
            }
            
            tmxc_sandboxes[i].violation_count++;
            
            if (tmxc_violation_count < 1024) {
                tmxc_violations[tmxc_violation_count].src_pid = pid;
                tmxc_violations[tmxc_violation_count].permission_type = permission;
                tmxc_violations[tmxc_violation_count].timestamp = tmxc_get_cycle_count();
                tmxc_violations[tmxc_violation_count].was_blocked = 1;
                tmxc_violation_count++;
            }
            
            tmxc_uart_puts("[SANDBOX] Permission denied for PID: ");
            char buffer[21];
            int pos = 20;
            buffer[pos] = '\0';
            uint64_t temp = pid;
            while (temp > 0 && pos > 0) {
                pos--;
                buffer[pos] = '0' + (temp % 10);
                temp /= 10;
            }
            tmxc_uart_puts(&buffer[pos]);
            tmxc_uart_puts(", Permission: ");
            pos = 20;
            buffer[pos] = '\0';
            temp = permission;
            while (temp > 0 && pos > 0) {
                pos--;
                buffer[pos] = '0' + (temp % 10);
                temp /= 10;
            }
            tmxc_uart_puts(&buffer[pos]);
            tmxc_uart_puts("\r\n");
            
            return 0;
        }
    }
    
    return 1;
}

uint8_t tmxc_sandbox_check_memory_access(uint32_t src_pid, uint32_t dst_pid, uint64_t address) {
    if (src_pid >= TMXC_MAX_PROCESSES || dst_pid >= TMXC_MAX_PROCESSES || !tmxc_sandbox_enabled) {
        return 1;
    }
    
    if (src_pid == dst_pid) {
        return 1;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (tmxc_sandboxes[i].pid == src_pid) {
            if (!tmxc_sandboxes[i].is_isolated) {
                return 1;
            }
            
            for (uint32_t j = 0; j < TMXC_MAX_PROCESSES; j++) {
                if (tmxc_sandboxes[j].pid == dst_pid) {
                    if (address >= tmxc_sandboxes[j].base_address && 
                        address < tmxc_sandboxes[j].base_address + tmxc_sandboxes[j].size) {
                        
                        tmxc_sandboxes[i].violation_count++;
                        
                        if (tmxc_violation_count < 1024) {
                            tmxc_violations[tmxc_violation_count].src_pid = src_pid;
                            tmxc_violations[tmxc_violation_count].dst_pid = dst_pid;
                            tmxc_violations[tmxc_violation_count].access_address = address;
                            tmxc_violations[tmxc_violation_count].timestamp = tmxc_get_cycle_count();
                            tmxc_violations[tmxc_violation_count].was_blocked = 1;
                            tmxc_violation_count++;
                        }
                        
                        tmxc_uart_puts("[SANDBOX] Cross-process memory access blocked: PID ");
                        char buffer[21];
                        int pos = 20;
                        buffer[pos] = '\0';
                        uint64_t temp = src_pid;
                        while (temp > 0 && pos > 0) {
                            pos--;
                            buffer[pos] = '0' + (temp % 10);
                            temp /= 10;
                        }
                        tmxc_uart_puts(&buffer[pos]);
                        tmxc_uart_puts(" -> PID ");
                        pos = 20;
                        buffer[pos] = '\0';
                        temp = dst_pid;
                        while (temp > 0 && pos > 0) {
                            pos--;
                            buffer[pos] = '0' + (temp % 10);
                            temp /= 10;
                        }
                        tmxc_uart_puts(&buffer[pos]);
                        tmxc_uart_puts("\r\n");
                        
                        return 0;
                    }
                }
            }
        }
    }
    
    return 1;
}

void tmxc_sandbox_destroy(uint32_t pid) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (tmxc_sandboxes[i].pid == pid) {
            tmxc_uart_puts("[SANDBOX] Destroyed sandbox for PID: ");
            char buffer[21];
            int pos = 20;
            buffer[pos] = '\0';
            uint64_t temp = pid;
            while (temp > 0 && pos > 0) {
                pos--;
                buffer[pos] = '0' + (temp % 10);
                temp /= 10;
            }
            tmxc_uart_puts(&buffer[pos]);
            tmxc_uart_puts(", Violations: ");
            pos = 20;
            buffer[pos] = '\0';
            temp = tmxc_sandboxes[i].violation_count;
            while (temp > 0 && pos > 0) {
                pos--;
                buffer[pos] = '0' + (temp % 10);
                temp /= 10;
            }
            tmxc_uart_puts(&buffer[pos]);
            tmxc_uart_puts("\r\n");
            
            tmxc_sandboxes[i].pid = 0;
            tmxc_sandboxes[i].base_address = 0;
            tmxc_sandboxes[i].size = 0;
            tmxc_sandboxes[i].permission_count = 0;
            tmxc_sandboxes[i].is_isolated = 0;
            tmxc_sandboxes[i].resource_count = 0;
            tmxc_sandboxes[i].network_rule_count = 0;
            return;
        }
    }
}

void tmxc_sandbox_enable(uint8_t enable) {
    tmxc_sandbox_enabled = enable;
    tmxc_uart_puts("[SANDBOX] Sandbox system ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

void tmxc_sandbox_set_strict_mode(uint8_t strict) {
    tmxc_strict_mode_default = strict;
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (tmxc_sandboxes[i].pid != 0) {
            tmxc_sandboxes[i].strict_mode = strict;
        }
    }
}

uint32_t tmxc_sandbox_get_violation_count(uint32_t pid) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return 0;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (tmxc_sandboxes[i].pid == pid) {
            return tmxc_sandboxes[i].violation_count;
        }
    }
    
    return 0;
}

tmxc_sandbox_t* tmxc_sandbox_get_info(uint32_t pid) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return NULL;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (tmxc_sandboxes[i].pid == pid) {
            return &tmxc_sandboxes[i];
        }
    }
    
    return NULL;
}
