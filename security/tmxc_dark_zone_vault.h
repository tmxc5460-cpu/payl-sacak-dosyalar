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
#ifndef TMXC_DARK_ZONE_VAULT_H
#define TMXC_DARK_ZONE_VAULT_H

#include <stdint.h>

#define TMXC_DARK_ZONE_COMBINATION_LENGTH 6

void tmxc_dark_zone_vault_init(void);
void tmxc_dark_zone_set_combination(const uint8_t* combination);
void tmxc_dark_zone_input_key(uint8_t key);
void tmxc_dark_zone_verify_combination(void);
void tmxc_dark_zone_toggle_mode(void);
void tmxc_dark_zone_enable_masking(uint8_t enable);
uint8_t tmxc_dark_zone_is_masking_enabled(void);
uint8_t tmxc_dark_zone_get_mode(void);
uint8_t tmxc_dark_zone_is_real_mode(void);
uint8_t tmxc_dark_zone_is_guest_mode(void);
void tmxc_dark_zone_set_vault_region(uint64_t base_address, uint64_t size);
void tmxc_dark_zone_set_guest_region(uint64_t base_address, uint64_t size);
uint64_t tmxc_dark_zone_get_vault_base(void);
uint64_t tmxc_dark_zone_get_vault_size(void);
void tmxc_dark_zone_force_real_mode(void);
void tmxc_dark_zone_force_guest_mode(void);
uint8_t tmxc_dark_zone_is_active(void);

#endif
