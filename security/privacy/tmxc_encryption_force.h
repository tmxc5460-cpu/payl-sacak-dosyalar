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
#ifndef TMXC_ENCRYPTION_FORCE_H
#define TMXC_ENCRYPTION_FORCE_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_AES256_KEY_SIZE 32
#define TMXC_AES256_BLOCK_SIZE 16

void tmxc_encryption_force_init(void);
void tmxc_encryption_force_set_default_key(const uint8_t* key);
void tmxc_encryption_force_encrypt_data(uint8_t* data, uint64_t size);
void tmxc_encryption_force_decrypt_data(uint8_t* data, uint64_t size);
void tmxc_encryption_force_enable(uint8_t enable);
uint8_t tmxc_encryption_force_is_enabled(void);
uint64_t tmxc_encryption_force_get_files_encrypted(void);
uint64_t tmxc_encryption_force_get_total_bytes(void);
void tmxc_encryption_force_cleanup(void);

#endif
