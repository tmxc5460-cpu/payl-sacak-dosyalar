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
#ifndef TMXC_INTEGRITY_MONITOR_H
#define TMXC_INTEGRITY_MONITOR_H

#include "tmxc_kernel.h"

#define TMXC_KERNEL_HASH_SIZE 32
#define TMXC_WATCHDOG_INTERVAL_MS 1000
#define TMXC_MAX_CRITICAL_REGIONS 16

void tmxc_integrity_monitor_init(void);
void tmxc_integrity_set_kernel_hash(const uint8_t* hash);
void tmxc_integrity_add_critical_region(uint64_t base_address, uint64_t size, const char* name);
uint8_t tmxc_integrity_verify_kernel(void);
void tmxc_integrity_watchdog_tick(void);
void tmxc_integrity_enable_watchdog(uint8_t enable);
void tmxc_integrity_enable_auto_recovery(uint8_t enable);
uint8_t tmxc_integrity_is_valid(void);
uint8_t tmxc_integrity_is_violation_detected(void);
uint64_t tmxc_integrity_get_violation_count(void);
void tmxc_integrity_clear_violation(void);
void tmxc_integrity_monitor_cleanup(void);

#endif
