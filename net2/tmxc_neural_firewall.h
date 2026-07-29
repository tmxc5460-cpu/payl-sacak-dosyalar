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
#ifndef TMXC_NEURAL_FIREWALL_H
#define TMXC_NEURAL_FIREWALL_H

#include "../kernel/tmxc_kernel.h"

#define TMXC_NEURAL_FIREWALL_MAX_RULES 128
#define TMXC_NEURAL_FIREWALL_MAX_CONNECTIONS 512
#define TMXC_NEURAL_FIREWALL_MAX_PATTERNS 64
#define TMXC_NEURAL_FIREWALL_LEARNING_WINDOW_MS 3600000

typedef enum {
    TMXC_FW_ACTION_ALLOW = 0,
    TMXC_FW_ACTION_BLOCK = 1,
    TMXC_FW_ACTION_LOG = 2,
    TMXC_FW_ACTION_QUARANTINE = 3
} tmxc_firewall_action_t;

typedef enum {
    TMXC_FW_PROTOCOL_TCP = 0,
    TMXC_FW_PROTOCOL_UDP = 1,
    TMXC_FW_PROTOCOL_HTTP = 2,
    TMXC_FW_PROTOCOL_HTTPS = 3,
    TMXC_FW_PROTOCOL_CUSTOM = 4
} tmxc_firewall_protocol_t;

typedef struct {
    uint32_t app_id;
    uint64_t src_ip;
    uint64_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    tmxc_firewall_protocol_t protocol;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint64_t connection_start;
    uint64_t connection_end;
    uint8_t is_active;
    tmxc_firewall_action_t action;
} tmxc_firewall_connection_t;

typedef struct {
    uint32_t rule_id;
    uint32_t app_id;
    uint64_t ip_address;
    uint16_t port;
    tmxc_firewall_protocol_t protocol;
    tmxc_firewall_action_t action;
    uint8_t is_enabled;
    uint64_t creation_time;
    uint32_t match_count;
} tmxc_firewall_rule_t;

typedef struct {
    uint32_t pattern_id;
    uint32_t app_id;
    uint64_t ip_pattern;
    uint16_t port_pattern;
    uint32_t frequency;
    float anomaly_score;
    uint8_t is_suspicious;
    uint64_t last_seen;
} tmxc_neural_pattern_t;

typedef struct {
    uint32_t total_connections;
    uint32_t blocked_connections;
    uint32_t allowed_connections;
    uint32_t quarantined_connections;
    uint64_t total_bytes_analyzed;
    uint32_t anomaly_detected;
    uint32_t data_leak_prevented;
} tmxc_firewall_stats_t;

void tmxc_neural_firewall_init(void);
tmxc_firewall_action_t tmxc_neural_firewall_check_connection(uint32_t app_id, uint64_t dst_ip, uint16_t dst_port, tmxc_firewall_protocol_t protocol);
float tmxc_neural_analyze_connection(uint32_t app_id, uint64_t dst_ip, uint16_t dst_port, tmxc_firewall_protocol_t protocol);
void tmxc_neural_firewall_log_connection(uint32_t app_id, uint64_t src_ip, uint64_t dst_ip, uint16_t src_port, uint16_t dst_port, tmxc_firewall_protocol_t protocol, tmxc_firewall_action_t action);
void tmxc_neural_firewall_update_connection(uint32_t connection_id, uint64_t bytes_sent, uint64_t bytes_received);
void tmxc_neural_firewall_close_connection(uint32_t connection_id);
void tmxc_neural_learn_pattern(uint32_t app_id, uint64_t dst_ip, uint16_t dst_port);
void tmxc_neural_firewall_add_rule(uint32_t app_id, uint64_t ip_address, uint16_t port, tmxc_firewall_protocol_t protocol, tmxc_firewall_action_t action);
void tmxc_neural_firewall_enable(uint8_t enable);
void tmxc_neural_ai_enable(uint8_t enable);
tmxc_firewall_stats_t* tmxc_neural_firewall_get_stats(void);
tmxc_neural_pattern_t* tmxc_neural_firewall_get_patterns(uint32_t* count);
void tmxc_neural_firewall_cleanup(void);

#endif
