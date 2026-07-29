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
#ifndef TMXC_ANTI_TRACKING_H
#define TMXC_ANTI_TRACKING_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_TRACKING_SIGNATURES_MAX 100
#define TMXC_TRACKING_PATTERN_LENGTH 64

void tmxc_anti_tracking_init(void);
void tmxc_anti_tracking_add_signature(const char* signature);
uint8_t tmxc_anti_tracking_check_request(const char* request_data);
void tmxc_anti_tracking_enable(uint8_t enable);
uint64_t tmxc_anti_tracking_get_trackers_blocked(void);
uint64_t tmxc_anti_tracking_get_total_analyzed(void);
void tmxc_anti_tracking_cleanup(void);

#endif
