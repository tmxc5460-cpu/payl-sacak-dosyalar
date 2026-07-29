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
#ifndef TMXC_HACKER_TRACE_COUNTER_ATTACK_H
#define TMXC_HACKER_TRACE_COUNTER_ATTACK_H

#include <stdint.h>

#define TMXC_HACKER_TRACE_MAX_LOGS 100

typedef struct {
    uint64_t hacker_ip;
    uint8_t hacker_mac[6];
    uint64_t timestamp;
    uint8_t attack_type;
    uint32_t port;
    uint8_t location_revealed;
    char location_str[64];
} tmxc_hacker_log_t;

void tmxc_hacker_trace_init(void);
void tmxc_hacker_trace_detect_intrusion(uint64_t src_ip, const uint8_t* src_mac, uint16_t src_port, uint8_t attack_type);
void tmxc_hacker_trace_enable(uint8_t enable);
void tmxc_hacker_trace_enable_ascii_response(uint8_t enable);
void tmxc_hacker_trace_enable_location_reveal(uint8_t enable);
uint8_t tmxc_hacker_trace_is_enabled(void);
uint32_t tmxc_hacker_trace_get_log_count(void);
tmxc_hacker_log_t* tmxc_hacker_trace_get_log(uint32_t index);
uint64_t tmxc_hacker_trace_get_total_attacks_blocked(void);
void tmxc_hacker_trace_clear_logs(void);

#endif
