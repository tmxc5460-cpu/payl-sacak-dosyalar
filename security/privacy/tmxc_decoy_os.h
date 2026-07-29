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
#ifndef TMXC_DECOY_OS_H
#define TMXC_DECOY_OS_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_DECOY_MAX_FILES 50
#define TMXC_DECOY_MAX_APPS 20

void tmxc_decoy_os_init(void);
void tmxc_decoy_add_fake_file(const char* filename, uint64_t size);
void tmxc_decoy_add_fake_app(const char* app_name);
void tmxc_decoy_activate_on_wrong_password(const uint8_t* attempted_password);
void tmxc_decoy_deactivate(void);
uint8_t tmxc_decoy_is_active(void);
uint32_t tmxc_decoy_get_fake_file_count(void);
uint32_t tmxc_decoy_get_fake_app_count(void);
void tmxc_decoy_os_cleanup(void);

#endif
