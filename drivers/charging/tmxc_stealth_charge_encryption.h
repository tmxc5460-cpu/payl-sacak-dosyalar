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
#ifndef TMXC_STEALTH_CHARGE_ENCRYPTION_H
#define TMXC_STEALTH_CHARGE_ENCRYPTION_H

#include <stdint.h>

#define TMXC_STEALTH_CHARGE_ENCRYPTION_KEY_SIZE 32

void tmxc_stealth_charge_init(void);
void tmxc_stealth_charge_detect_plug(void);
void tmxc_stealth_charge_enable_encryption(uint8_t enable);
void tmxc_stealth_charge_enable_backup(uint8_t enable);
void tmxc_stealth_charge_enable_cloud_backup(uint8_t enable);
uint8_t tmxc_stealth_charge_is_encryption_enabled(void);
uint8_t tmxc_stealth_charge_is_backup_enabled(void);
uint8_t tmxc_stealth_charge_is_cloud_backup_enabled(void);
uint8_t tmxc_stealth_charge_is_plugged_in(void);
uint32_t tmxc_stealth_charge_get_backup_count(void);
uint64_t tmxc_stealth_charge_get_last_backup_time(void);
void tmxc_stealth_charge_force_backup(void);
void tmxc_stealth_charge_regenerate_key(void);
void tmxc_stealth_charge_set_vault_region(uint64_t base_address, uint64_t size);
void tmxc_stealth_charge_set_backup_region(uint64_t base_address, uint64_t size);

#endif
