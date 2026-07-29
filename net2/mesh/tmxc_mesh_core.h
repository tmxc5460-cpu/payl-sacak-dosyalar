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
#ifndef TMXC_MESH_CORE_H
#define TMXC_MESH_CORE_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_MESH_MAX_NODES 1000
#define TMXC_MESH_MAX_ROUTES 32
#define TMXC_MESH_PACKET_SIZE 2048
#define TMXC_MESH_TTL 16

typedef enum {
    TMXC_MESH_PACKET_DATA = 0,
    TMXC_MESH_PACKET_CONTROL = 1,
    TMXC_MESH_PACKET_DISCOVERY = 2,
    TMXC_MESH_PACKET_ACK = 3,
    TMXC_MESH_PACKET_QKD_KEY = 4,
    TMXC_MESH_PACKET_QKD_ACK = 5
} tmxc_mesh_packet_type_t;

typedef struct {
    uint8_t src_id[32];
    uint8_t dst_id[32];
    uint8_t next_hop[32];
    tmxc_mesh_packet_type_t type;
    uint8_t ttl;
    uint16_t sequence;
    uint8_t data[TMXC_MESH_PACKET_SIZE];
    uint32_t data_size;
    uint64_t timestamp;
} tmxc_mesh_packet_t;

typedef struct {
    uint8_t node_id[32];
    uint64_t last_seen;
    uint8_t hop_count;
    uint64_t latency_ns;
    uint8_t is_online;
    uint32_t packet_count;
} tmxc_mesh_node_t;

typedef struct {
    uint8_t dst_id[32];
    uint8_t next_hop[32];
    uint8_t hop_count;
    uint64_t metric;
    uint64_t last_update;
} tmxc_mesh_route_t;

typedef struct {
    uint8_t qkd_key[32];
    uint8_t key_index;
    uint8_t key_valid;
    uint64_t key_timestamp;
} tmxc_qkd_key_t;

typedef struct {
    tmxc_mesh_node_t nodes[TMXC_MESH_MAX_NODES];
    tmxc_mesh_route_t routes[TMXC_MESH_MAX_ROUTES];
    tmxc_qkd_key_t qkd_keys[TMXC_MESH_MAX_NODES];
    uint32_t node_count;
    uint32_t route_count;
    uint8_t mesh_enabled;
    uint64_t packets_sent;
    uint64_t packets_received;
    uint64_t packets_dropped;
    uint8_t mesh_initialized;
    uint8_t qkd_enabled;
    uint8_t d2d_mode;
} tmxc_mesh_core_t;

void tmxc_mesh_core_init(void);
void tmxc_mesh_core_enable(uint8_t enable);
int tmxc_mesh_core_add_node(const uint8_t* node_id);
int tmxc_mesh_core_remove_node(const uint8_t* node_id);
tmxc_mesh_node_t* tmxc_mesh_core_get_node(const uint8_t* node_id);

int tmxc_mesh_core_send_packet(const uint8_t* dst_id, const uint8_t* data, uint32_t size);
int tmxc_mesh_core_receive_packet(tmxc_mesh_packet_t* packet);
void tmxc_mesh_core_forward_packet(tmxc_mesh_packet_t* packet);

int tmxc_mesh_core_add_route(const uint8_t* dst_id, const uint8_t* next_hop, uint8_t hop_count, uint64_t metric);
void tmxc_mesh_core_update_routes(void);
void tmxc_mesh_core_discovery(void);

uint32_t tmxc_mesh_core_get_node_count(void);
uint64_t tmxc_mesh_core_get_packets_sent(void);
uint64_t tmxc_mesh_core_get_packets_received(void);

void tmxc_mesh_qkd_enable(uint8_t enable);
uint8_t tmxc_mesh_qkd_is_enabled(void);
void tmxc_mesh_qkd_generate_key(const uint8_t* node_id);
int tmxc_mesh_qkd_send_key(const uint8_t* dst_id);
int tmxc_mesh_qkd_receive_key(const uint8_t* src_id, const uint8_t* key);
uint8_t* tmxc_mesh_qkd_get_key(const uint8_t* node_id);
void tmxc_mesh_qkd_encrypt_data(uint8_t* data, uint32_t size, const uint8_t* key);
void tmxc_mesh_qkd_decrypt_data(uint8_t* data, uint32_t size, const uint8_t* key);
void tmxc_mesh_d2d_enable(uint8_t enable);
uint8_t tmxc_mesh_d2d_is_enabled(void);

#endif
