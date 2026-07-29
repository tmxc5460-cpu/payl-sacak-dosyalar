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
#ifndef TMXC_VAULT_VAULT_H
#define TMXC_VAULT_VAULT_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_VAULT_MAX_FILES 100
#define TMXC_VAULT_MAX_NESTING 5
#define TMXC_ENCRYPTION_KEY_SIZE 32

void tmxc_vault_vault_init(void);
void tmxc_vault_set_master_key(const uint8_t* key);
uint32_t tmxc_vault_add_file(const char* filename, uint64_t size, uint8_t nesting_level);
void tmxc_vault_encrypt_file(uint32_t file_index);
void tmxc_vault_decrypt_file(uint32_t file_index);
void tmxc_vault_lock(void);
void tmxc_vault_unlock(const uint8_t* key);
uint8_t tmxc_vault_is_locked(void);
uint32_t tmxc_vault_get_file_count(void);
uint64_t tmxc_vault_get_encrypted_size(void);
void tmxc_vault_vault_cleanup(void);

#endif
