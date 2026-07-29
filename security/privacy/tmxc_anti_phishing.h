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
#ifndef TMXC_ANTI_PHISHING_H
#define TMXC_ANTI_PHISHING_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_PHISHING_DB_SIZE 1000
#define TMXC_URL_MAX_LENGTH 256
#define TMXC_CONFIDENCE_THRESHOLD 70

typedef enum {
    TMXC_PHISHING_SAFE = 0,
    TMXC_PHISHING_SUSPICIOUS = 1,
    TMXC_PHISHING_MALICIOUS = 2
} tmxc_phishing_result_t;

void tmxc_anti_phishing_init(void);
void tmxc_anti_phishing_add_pattern(const char* url_pattern, uint8_t threat_level);
tmxc_phishing_result_t tmxc_anti_phishing_check_url(const char* url);
void tmxc_anti_phishing_enable(uint8_t enable);
uint32_t tmxc_anti_phishing_get_threats_blocked(void);
uint64_t tmxc_anti_phishing_get_total_checked(void);
void tmxc_anti_phishing_cleanup(void);

#endif
