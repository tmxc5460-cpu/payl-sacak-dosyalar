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
#ifndef TMXC_PROCESS_ISOLATION_H
#define TMXC_PROCESS_ISOLATION_H

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

void tmxc_sandbox_init(void);
uint8_t tmxc_sandbox_create(uint32_t pid, uint64_t base_address, uint64_t size);
uint8_t tmxc_sandbox_add_permission(uint32_t pid, uint32_t permission);
uint8_t tmxc_sandbox_check_permission(uint32_t pid, uint32_t permission);
uint8_t tmxc_sandbox_check_memory_access(uint32_t src_pid, uint32_t dst_pid, uint64_t address);
void tmxc_sandbox_destroy(uint32_t pid);
void tmxc_sandbox_enable(uint8_t enable);
void tmxc_sandbox_set_strict_mode(uint8_t strict);
uint32_t tmxc_sandbox_get_violation_count(uint32_t pid);
tmxc_sandbox_t* tmxc_sandbox_get_info(uint32_t pid);

#endif
