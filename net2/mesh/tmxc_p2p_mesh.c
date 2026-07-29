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
#include "tmxc_p2p_mesh.h"

static tmxc_mesh_network_t tmxc_mesh;
static tmxc_mesh_config_t tmxc_mesh_config;

void tmxc_mesh_init(void) {
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        for (int j = 0; j < TMXC_MESH_NODE_ID_SIZE; j++) {
            tmxc_mesh.nodes[i].node_id[j] = 0;
        }
        tmxc_mesh.nodes[i].last_seen = 0;
        tmxc_mesh.nodes[i].is_online = 0;
        tmxc_mesh.nodes[i].signal_strength = 0;
        tmxc_mesh.nodes[i].bandwidth = 0;
        tmxc_mesh.nodes[i].hop_count = 0;
        tmxc_mesh.nodes[i].latency_ns = 0;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_ROUTES; i++) {
        for (int j = 0; j < TMXC_MESH_NODE_ID_SIZE; j++) {
            tmxc_mesh.routes[i].dest_node_id[j] = 0;
            tmxc_mesh.routes[i].next_hop[j] = 0;
        }
        tmxc_mesh.routes[i].hop_count = 0;
        tmxc_mesh.routes[i].metric = 0;
        tmxc_mesh.routes[i].last_update = 0;
    }
    
    for (int j = 0; j < TMXC_MESH_NODE_ID_SIZE; j++) {
        tmxc_mesh.my_node_id[j] = 0;
    }
    
    tmxc_mesh.node_count = 0;
    tmxc_mesh.route_count = 0;
    tmxc_mesh.total_bandwidth = 10000000000ULL;
    tmxc_mesh.used_bandwidth = 0;
    tmxc_mesh.mesh_enabled = 0;
    tmxc_mesh.packets_sent = 0;
    tmxc_mesh.packets_received = 0;
    tmxc_mesh.packets_dropped = 0;
    
    tmxc_mesh_config.signal_multiplexing_enabled = 1;
    tmxc_mesh_config.traffic_prioritization_enabled = 1;
    tmxc_mesh_config.smart_roaming_enabled = 1;
    tmxc_mesh_config.rf_frequency = 2400000000ULL;
    tmxc_mesh_config.rf_power = 20;
    tmxc_mesh_config.modulation_scheme = 0;
    
    tmxc_uart_puts("[MESH] P2P Mesh network initialized\r\n");
}

void tmxc_mesh_start(void) {
    tmxc_mesh.mesh_enabled = 1;
    tmxc_uart_puts("[MESH] P2P Mesh network started\r\n");
}

void tmxc_mesh_stop(void) {
    tmxc_mesh.mesh_enabled = 0;
    tmxc_uart_puts("[MESH] P2P Mesh network stopped\r\n");
}

int tmxc_mesh_add_node(const uint8_t* node_id, uint64_t bandwidth) {
    if (node_id == NULL || tmxc_mesh.node_count >= TMXC_MESH_MAX_NODES) {
        return -1;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        if (tmxc_mesh.nodes[i].is_online == 0) {
            for (int j = 0; j < TMXC_MESH_NODE_ID_SIZE; j++) {
                tmxc_mesh.nodes[i].node_id[j] = node_id[j];
            }
            tmxc_mesh.nodes[i].last_seen = tmxc_get_cycle_count();
            tmxc_mesh.nodes[i].is_online = 1;
            tmxc_mesh.nodes[i].signal_strength = 100;
            tmxc_mesh.nodes[i].bandwidth = bandwidth;
            tmxc_mesh.nodes[i].hop_count = 1;
            tmxc_mesh.nodes[i].latency_ns = 1000000;
            tmxc_mesh.node_count++;
            
            tmxc_mesh.total_bandwidth += bandwidth;
            
            return 0;
        }
    }
    
    return -2;
}

int tmxc_mesh_remove_node(const uint8_t* node_id) {
    if (node_id == NULL) {
        return -1;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        uint8_t match = 1;
        for (int j = 0; j < TMXC_MESH_NODE_ID_SIZE; j++) {
            if (tmxc_mesh.nodes[i].node_id[j] != node_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match && tmxc_mesh.nodes[i].is_online) {
            tmxc_mesh.total_bandwidth -= tmxc_mesh.nodes[i].bandwidth;
            tmxc_mesh.nodes[i].is_online = 0;
            tmxc_mesh.node_count--;
            return 0;
        }
    }
    
    return -2;
}

tmxc_mesh_node_t* tmxc_mesh_get_node(const uint8_t* node_id) {
    if (node_id == NULL) {
        return NULL;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        uint8_t match = 1;
        for (int j = 0; j < TMXC_MESH_NODE_ID_SIZE; j++) {
            if (tmxc_mesh.nodes[i].node_id[j] != node_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match && tmxc_mesh.nodes[i].is_online) {
            return &tmxc_mesh.nodes[i];
        }
    }
    
    return NULL;
}

int tmxc_mesh_send_packet(const uint8_t* dst_node_id, const uint8_t* data, uint64_t size, uint8_t priority) {
    if (!tmxc_mesh.mesh_enabled || dst_node_id == NULL || data == NULL || size == 0 || size > TMXC_MESH_PACKET_SIZE) {
        return -1;
    }
    
    tmxc_mesh_packet_t packet;
    for (int j = 0; j < TMXC_MESH_NODE_ID_SIZE; j++) {
        packet.src_node_id[j] = tmxc_mesh.my_node_id[j];
        packet.dst_node_id[j] = dst_node_id[j];
    }
    
    for (uint64_t i = 0; i < size && i < TMXC_MESH_PACKET_SIZE; i++) {
        packet.data[i] = data[i];
    }
    
    packet.data_size = size;
    packet.priority = priority;
    packet.ttl = TMXC_MESH_MAX_HOPS;
    packet.sequence = tmxc_mesh.packets_sent;
    packet.encrypted = 1;
    
    tmxc_mesh.packets_sent++;
    tmxc_mesh.used_bandwidth += size;
    
    return 0;
}

int tmxc_mesh_receive_packet(tmxc_mesh_packet_t* packet) {
    if (!tmxc_mesh.mesh_enabled || packet == NULL) {
        return -1;
    }
    
    tmxc_mesh.packets_received++;
    
    return 0;
}

void tmxc_mesh_update_topology(void) {
    if (!tmxc_mesh.mesh_enabled) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        if (tmxc_mesh.nodes[i].is_online) {
            uint64_t time_since_seen = current_time - tmxc_mesh.nodes[i].last_seen;
            
            if (time_since_seen > 10000000000ULL) {
                tmxc_mesh.nodes[i].is_online = 0;
                tmxc_mesh.node_count--;
                tmxc_mesh.total_bandwidth -= tmxc_mesh.nodes[i].bandwidth;
            }
        }
    }
    
    tmxc_mesh_update_routes();
}

uint32_t tmxc_mesh_get_node_count(void) {
    return tmxc_mesh.node_count;
}

uint64_t tmxc_mesh_get_total_bandwidth(void) {
    return tmxc_mesh.total_bandwidth;
}

void tmxc_mesh_route_discovery(const uint8_t* dest_node_id) {
    if (dest_node_id == NULL) {
        return;
    }
    
    tmxc_uart_puts("[MESH] Initiating route discovery to node\r\n");
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        if (tmxc_mesh.nodes[i].is_online) {
            uint8_t match = 1;
            for (int j = 0; j < TMXC_MESH_NODE_ID_SIZE; j++) {
                if (tmxc_mesh.nodes[i].node_id[j] != dest_node_id[j]) {
                    match = 0;
                    break;
                }
            }
            
            if (match) {
                tmxc_mesh_add_route(dest_node_id, dest_node_id, 1, tmxc_mesh.nodes[i].latency_ns);
                return;
            }
        }
    }
}

int tmxc_mesh_add_route(const uint8_t* dest_node_id, const uint8_t* next_hop, uint8_t hop_count, uint64_t metric) {
    if (dest_node_id == NULL || next_hop == NULL || tmxc_mesh.route_count >= TMXC_MESH_MAX_ROUTES) {
        return -1;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_ROUTES; i++) {
        uint8_t match = 1;
        for (int j = 0; j < TMXC_MESH_NODE_ID_SIZE; j++) {
            if (tmxc_mesh.routes[i].dest_node_id[j] != dest_node_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            if (hop_count < tmxc_mesh.routes[i].hop_count || metric < tmxc_mesh.routes[i].metric) {
                for (int j = 0; j < TMXC_MESH_NODE_ID_SIZE; j++) {
                    tmxc_mesh.routes[i].next_hop[j] = next_hop[j];
                }
                tmxc_mesh.routes[i].hop_count = hop_count;
                tmxc_mesh.routes[i].metric = metric;
                tmxc_mesh.routes[i].last_update = tmxc_get_cycle_count();
            }
            return 0;
        }
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_ROUTES; i++) {
        if (tmxc_mesh.routes[i].dest_node_id[0] == 0) {
            for (int j = 0; j < TMXC_MESH_NODE_ID_SIZE; j++) {
                tmxc_mesh.routes[i].dest_node_id[j] = dest_node_id[j];
                tmxc_mesh.routes[i].next_hop[j] = next_hop[j];
            }
            tmxc_mesh.routes[i].hop_count = hop_count;
            tmxc_mesh.routes[i].metric = metric;
            tmxc_mesh.routes[i].last_update = tmxc_get_cycle_count();
            tmxc_mesh.route_count++;
            return 0;
        }
    }
    
    return -2;
}

void tmxc_mesh_update_routes(void) {
    uint64_t current_time = tmxc_get_cycle_count();
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_ROUTES; i++) {
        if (tmxc_mesh.routes[i].dest_node_id[0] != 0) {
            uint64_t time_since_update = current_time - tmxc_mesh.routes[i].last_update;
            
            if (time_since_update > 30000000000ULL) {
                for (int j = 0; j < TMXC_MESH_NODE_ID_SIZE; j++) {
                    tmxc_mesh.routes[i].dest_node_id[j] = 0;
                    tmxc_mesh.routes[i].next_hop[j] = 0;
                }
                tmxc_mesh.routes[i].hop_count = 0;
                tmxc_mesh.routes[i].metric = 0;
                tmxc_mesh.route_count--;
            }
        }
    }
}

void tmxc_mesh_enable_signal_multiplexing(uint8_t enable) {
    tmxc_mesh_config.signal_multiplexing_enabled = enable;
}

void tmxc_mesh_enable_traffic_prioritization(uint8_t enable) {
    tmxc_mesh_config.traffic_prioritization_enabled = enable;
}

void tmxc_mesh_enable_smart_roaming(uint8_t enable) {
    tmxc_mesh_config.smart_roaming_enabled = enable;
}

void tmxc_mesh_set_rf_frequency(uint64_t frequency) {
    tmxc_mesh_config.rf_frequency = frequency;
}

void tmxc_mesh_set_rf_power(uint64_t power) {
    tmxc_mesh_config.rf_power = power;
}
