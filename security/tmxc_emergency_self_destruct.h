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
#ifndef TMXC_EMERGENCY_SELF_DESTRUCT_H
#define TMXC_EMERGENCY_SELF_DESTRUCT_H

#include <stdint.h>

#define TMXC_PASSWORD_ATTEMPT_LIMIT 3
#define TMXC_FORCED_ACCESS_THRESHOLD 5

void tmxc_emergency_self_destruct_init(void);
void tmxc_emergency_self_destruct_record_attempt(void);
void tmxc_emergency_self_destruct_record_forced_access(void);
void tmxc_emergency_self_destruct_trigger(void);
void tmxc_emergency_self_destruct_reset_attempts(void);
uint8_t tmxc_emergency_self_destruct_is_triggered(void);
uint8_t tmxc_emergency_self_destruct_is_wipe_in_progress(void);
uint8_t tmxc_emergency_self_destruct_get_attempts(void);
uint8_t tmxc_emergency_self_destruct_get_forced_access_count(void);
void tmxc_emergency_self_destruct_enable_flash_wipe(uint8_t enable);
void tmxc_emergency_self_destruct_enable_multi_pass(uint8_t enable);
void tmxc_emergency_self_destruct_set_vault_region(uint64_t base_address, uint64_t size);

#endif
