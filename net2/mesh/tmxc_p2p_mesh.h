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
#ifndef TMXC_P2P_MESH_H
#define TMXC_P2P_MESH_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_MESH_MAX_NODES 1000
#define TMXC_MESH_MAX_ROUTES 32
#define TMXC_MESH_MAX_HOPS 16
#define TMXC_MESH_NODE_ID_SIZE 32
#define TMXC_MESH_PACKET_SIZE 2048

typedef struct {
    uint8_t node_id[TMXC_MESH_NODE_ID_SIZE];
    uint64_t last_seen;
    uint8_t is_online;
    uint8_t signal_strength;
    uint64_t bandwidth;
    uint8_t hop_count;
    uint64_t latency_ns;
} tmxc_mesh_node_t;

typedef struct {
    uint8_t dest_node_id[TMXC_MESH_NODE_ID_SIZE];
    uint8_t next_hop[TMXC_MESH_NODE_ID_SIZE];
    uint8_t hop_count;
    uint64_t metric;
    uint64_t last_update;
} tmxc_mesh_route_t;

typedef struct {
    uint8_t src_node_id[TMXC_MESH_NODE_ID_SIZE];
    uint8_t dst_node_id[TMXC_MESH_NODE_ID_SIZE];
    uint8_t data[TMXC_MESH_PACKET_SIZE];
    uint64_t data_size;
    uint8_t priority;
    uint8_t ttl;
    uint64_t sequence;
    uint8_t encrypted;
} tmxc_mesh_packet_t;

typedef struct {
    tmxc_mesh_node_t nodes[TMXC_MESH_MAX_NODES];
    tmxc_mesh_route_t routes[TMXC_MESH_MAX_ROUTES];
    uint8_t my_node_id[TMXC_MESH_NODE_ID_SIZE];
    uint32_t node_count;
    uint32_t route_count;
    uint64_t total_bandwidth;
    uint64_t used_bandwidth;
    uint8_t mesh_enabled;
    uint64_t packets_sent;
    uint64_t packets_received;
    uint64_t packets_dropped;
} tmxc_mesh_network_t;

typedef struct {
    uint8_t signal_multiplexing_enabled;
    uint8_t traffic_prioritization_enabled;
    uint8_t smart_roaming_enabled;
    uint64_t rf_frequency;
    uint64_t rf_power;
    uint8_t modulation_scheme;
} tmxc_mesh_config_t;

void tmxc_mesh_init(void);
void tmxc_mesh_start(void);
void tmxc_mesh_stop(void);
int tmxc_mesh_add_node(const uint8_t* node_id, uint64_t bandwidth);
int tmxc_mesh_remove_node(const uint8_t* node_id);
tmxc_mesh_node_t* tmxc_mesh_get_node(const uint8_t* node_id);
int tmxc_mesh_send_packet(const uint8_t* dst_node_id, const uint8_t* data, uint64_t size, uint8_t priority);
int tmxc_mesh_receive_packet(tmxc_mesh_packet_t* packet);
void tmxc_mesh_update_topology(void);
uint32_t tmxc_mesh_get_node_count(void);
uint64_t tmxc_mesh_get_total_bandwidth(void);

void tmxc_mesh_route_discovery(const uint8_t* dest_node_id);
int tmxc_mesh_add_route(const uint8_t* dest_node_id, const uint8_t* next_hop, uint8_t hop_count, uint64_t metric);
void tmxc_mesh_update_routes(void);

void tmxc_mesh_enable_signal_multiplexing(uint8_t enable);
void tmxc_mesh_enable_traffic_prioritization(uint8_t enable);
void tmxc_mesh_enable_smart_roaming(uint8_t enable);
void tmxc_mesh_set_rf_frequency(uint64_t frequency);
void tmxc_mesh_set_rf_power(uint64_t power);

#endif
