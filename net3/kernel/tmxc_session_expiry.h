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
#ifndef TMXC_SESSION_EXPIRY_H
#define TMXC_SESSION_EXPIRY_H

#include "tmxc_kernel.h"

#define TMXC_MAX_SESSIONS 64
#define TMXC_SESSION_TIMEOUT_MS 300000

void tmxc_session_expiry_init(void);
uint32_t tmxc_session_create(uint32_t pid, const char* app_name, uint64_t memory_size);
void tmxc_session_update_activity(uint32_t session_id);
void tmxc_session_expire(uint32_t session_id);
void tmxc_session_check_expiry(void);
void tmxc_session_expire_all(void);
void tmxc_session_enable_auto_cleanup(uint8_t enable);
uint32_t tmxc_session_get_active_count(void);
uint64_t tmxc_session_get_memory_freed(void);
void tmxc_session_expiry_cleanup(void);

#endif
