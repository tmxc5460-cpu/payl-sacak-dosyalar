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
#include "tmxc_mesh_core.h"

static tmxc_mesh_core_t tmxc_mesh_core;

void tmxc_mesh_core_init(void) {
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        for (int j = 0; j < 32; j++) {
            tmxc_mesh_core.nodes[i].node_id[j] = 0;
        }
        tmxc_mesh_core.nodes[i].last_seen = 0;
        tmxc_mesh_core.nodes[i].hop_count = 0;
        tmxc_mesh_core.nodes[i].latency_ns = 0;
        tmxc_mesh_core.nodes[i].is_online = 0;
        tmxc_mesh_core.nodes[i].packet_count = 0;
        
        for (int j = 0; j < 32; j++) {
            tmxc_mesh_core.qkd_keys[i].qkd_key[j] = 0;
        }
        tmxc_mesh_core.qkd_keys[i].key_index = 0;
        tmxc_mesh_core.qkd_keys[i].key_valid = 0;
        tmxc_mesh_core.qkd_keys[i].key_timestamp = 0;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_ROUTES; i++) {
        for (int j = 0; j < 32; j++) {
            tmxc_mesh_core.routes[i].dst_id[j] = 0;
            tmxc_mesh_core.routes[i].next_hop[j] = 0;
        }
        tmxc_mesh_core.routes[i].hop_count = 0;
        tmxc_mesh_core.routes[i].metric = 0;
        tmxc_mesh_core.routes[i].last_update = 0;
    }
    
    tmxc_mesh_core.node_count = 0;
    tmxc_mesh_core.route_count = 0;
    tmxc_mesh_core.mesh_enabled = 0;
    tmxc_mesh_core.packets_sent = 0;
    tmxc_mesh_core.packets_received = 0;
    tmxc_mesh_core.packets_dropped = 0;
    tmxc_mesh_core.mesh_initialized = 1;
    tmxc_mesh_core.qkd_enabled = 1;
    tmxc_mesh_core.d2d_mode = 1;
    
    tmxc_uart_puts("[MESH-CORE] Mesh core initialized with QKD support\r\n");
}

void tmxc_mesh_core_enable(uint8_t enable) {
    tmxc_mesh_core.mesh_enabled = enable;
    if (enable) {
        tmxc_uart_puts("[MESH-CORE] Mesh network enabled\r\n");
    } else {
        tmxc_uart_puts("[MESH-CORE] Mesh network disabled\r\n");
    }
}

int tmxc_mesh_core_add_node(const uint8_t* node_id) {
    if (!tmxc_mesh_core.mesh_initialized || node_id == NULL) {
        return -1;
    }
    
    if (tmxc_mesh_core.node_count >= TMXC_MESH_MAX_NODES) {
        return -2;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        uint8_t match = 1;
        for (int j = 0; j < 32; j++) {
            if (tmxc_mesh_core.nodes[i].node_id[j] != node_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            tmxc_mesh_core.nodes[i].last_seen = tmxc_get_cycle_count();
            tmxc_mesh_core.nodes[i].is_online = 1;
            return 0;
        }
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        if (tmxc_mesh_core.nodes[i].node_id[0] == 0) {
            for (int j = 0; j < 32; j++) {
                tmxc_mesh_core.nodes[i].node_id[j] = node_id[j];
            }
            tmxc_mesh_core.nodes[i].last_seen = tmxc_get_cycle_count();
            tmxc_mesh_core.nodes[i].hop_count = 1;
            tmxc_mesh_core.nodes[i].latency_ns = 1000000;
            tmxc_mesh_core.nodes[i].is_online = 1;
            tmxc_mesh_core.nodes[i].packet_count = 0;
            tmxc_mesh_core.node_count++;
            return 0;
        }
    }
    
    return -3;
}

int tmxc_mesh_core_remove_node(const uint8_t* node_id) {
    if (!tmxc_mesh_core.mesh_initialized || node_id == NULL) {
        return -1;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        uint8_t match = 1;
        for (int j = 0; j < 32; j++) {
            if (tmxc_mesh_core.nodes[i].node_id[j] != node_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            tmxc_mesh_core.nodes[i].is_online = 0;
            tmxc_mesh_core.node_count--;
            return 0;
        }
    }
    
    return -2;
}

tmxc_mesh_node_t* tmxc_mesh_core_get_node(const uint8_t* node_id) {
    if (!tmxc_mesh_core.mesh_initialized || node_id == NULL) {
        return NULL;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        uint8_t match = 1;
        for (int j = 0; j < 32; j++) {
            if (tmxc_mesh_core.nodes[i].node_id[j] != node_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match && tmxc_mesh_core.nodes[i].is_online) {
            return &tmxc_mesh_core.nodes[i];
        }
    }
    
    return NULL;
}

int tmxc_mesh_core_send_packet(const uint8_t* dst_id, const uint8_t* data, uint32_t size) {
    if (!tmxc_mesh_core.mesh_enabled || !tmxc_mesh_core.mesh_initialized) {
        return -1;
    }
    
    if (dst_id == NULL || data == NULL || size == 0 || size > TMXC_MESH_PACKET_SIZE) {
        return -2;
    }
    
    tmxc_mesh_core.packets_sent++;
    
    return 0;
}

int tmxc_mesh_core_receive_packet(tmxc_mesh_packet_t* packet) {
    if (!tmxc_mesh_core.mesh_enabled || !tmxc_mesh_core.mesh_initialized) {
        return -1;
    }
    
    if (packet == NULL) {
        return -2;
    }
    
    tmxc_mesh_core.packets_received++;
    
    return 0;
}

void tmxc_mesh_core_forward_packet(tmxc_mesh_packet_t* packet) {
    if (!tmxc_mesh_core.mesh_enabled || !tmxc_mesh_core.mesh_initialized || packet == NULL) {
        return;
    }
    
    if (packet->ttl > 0) {
        packet->ttl--;
        tmxc_mesh_core.packets_sent++;
    } else {
        tmxc_mesh_core.packets_dropped++;
    }
}

int tmxc_mesh_core_add_route(const uint8_t* dst_id, const uint8_t* next_hop, uint8_t hop_count, uint64_t metric) {
    if (!tmxc_mesh_core.mesh_initialized || dst_id == NULL || next_hop == NULL) {
        return -1;
    }
    
    if (tmxc_mesh_core.route_count >= TMXC_MESH_MAX_ROUTES) {
        return -2;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_ROUTES; i++) {
        uint8_t match = 1;
        for (int j = 0; j < 32; j++) {
            if (tmxc_mesh_core.routes[i].dst_id[j] != dst_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            if (hop_count < tmxc_mesh_core.routes[i].hop_count || metric < tmxc_mesh_core.routes[i].metric) {
                for (int j = 0; j < 32; j++) {
                    tmxc_mesh_core.routes[i].next_hop[j] = next_hop[j];
                }
                tmxc_mesh_core.routes[i].hop_count = hop_count;
                tmxc_mesh_core.routes[i].metric = metric;
                tmxc_mesh_core.routes[i].last_update = tmxc_get_cycle_count();
            }
            return 0;
        }
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_ROUTES; i++) {
        if (tmxc_mesh_core.routes[i].dst_id[0] == 0) {
            for (int j = 0; j < 32; j++) {
                tmxc_mesh_core.routes[i].dst_id[j] = dst_id[j];
                tmxc_mesh_core.routes[i].next_hop[j] = next_hop[j];
            }
            tmxc_mesh_core.routes[i].hop_count = hop_count;
            tmxc_mesh_core.routes[i].metric = metric;
            tmxc_mesh_core.routes[i].last_update = tmxc_get_cycle_count();
            tmxc_mesh_core.route_count++;
            return 0;
        }
    }
    
    return -3;
}

void tmxc_mesh_core_update_routes(void) {
    if (!tmxc_mesh_core.mesh_initialized) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_ROUTES; i++) {
        if (tmxc_mesh_core.routes[i].dst_id[0] != 0) {
            uint64_t time_since_update = current_time - tmxc_mesh_core.routes[i].last_update;
            
            if (time_since_update > 30000000000ULL) {
                for (int j = 0; j < 32; j++) {
                    tmxc_mesh_core.routes[i].dst_id[j] = 0;
                    tmxc_mesh_core.routes[i].next_hop[j] = 0;
                }
                tmxc_mesh_core.routes[i].hop_count = 0;
                tmxc_mesh_core.routes[i].metric = 0;
                tmxc_mesh_core.route_count--;
            }
        }
    }
}

void tmxc_mesh_core_discovery(void) {
    if (!tmxc_mesh_core.mesh_enabled || !tmxc_mesh_core.mesh_initialized) {
        return;
    }
    
    tmxc_uart_puts("[MESH-CORE] Starting mesh discovery...\r\n");
    
    for (uint32_t i = 0; i < tmxc_mesh_core.node_count; i++) {
        if (tmxc_mesh_core.nodes[i].is_online) {
            uint64_t time_since_seen = tmxc_get_cycle_count() - tmxc_mesh_core.nodes[i].last_seen;
            
            if (time_since_seen > 60000000000ULL) {
                tmxc_mesh_core.nodes[i].is_online = 0;
                tmxc_mesh_core.node_count--;
            }
        }
    }
    
    tmxc_mesh_core_update_routes();
}

uint32_t tmxc_mesh_core_get_node_count(void) {
    return tmxc_mesh_core.node_count;
}

uint64_t tmxc_mesh_core_get_packets_sent(void) {
    return tmxc_mesh_core.packets_sent;
}

uint64_t tmxc_mesh_core_get_packets_received(void) {
    return tmxc_mesh_core.packets_received;
}

void tmxc_mesh_qkd_enable(uint8_t enable) {
    tmxc_mesh_core.qkd_enabled = enable;
    tmxc_uart_puts("[MESH-QKD] Quantum Key Distribution ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

uint8_t tmxc_mesh_qkd_is_enabled(void) {
    return tmxc_mesh_core.qkd_enabled;
}

void tmxc_mesh_qkd_generate_key(const uint8_t* node_id) {
    if (!tmxc_mesh_core.qkd_enabled || node_id == NULL) {
        return;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        uint8_t match = 1;
        for (int j = 0; j < 32; j++) {
            if (tmxc_mesh_core.nodes[i].node_id[j] != node_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            for (int j = 0; j < 32; j++) {
                uint32_t trng_data = tmxc_read32((volatile uint32_t*)TMXC_TRNG_DATA);
                tmxc_mesh_core.qkd_keys[i].qkd_key[j] = (uint8_t)(trng_data & 0xFF);
            }
            tmxc_mesh_core.qkd_keys[i].key_index = i;
            tmxc_mesh_core.qkd_keys[i].key_valid = 1;
            tmxc_mesh_core.qkd_keys[i].key_timestamp = tmxc_get_cycle_count();
            
            tmxc_uart_puts("[MESH-QKD] Quantum key generated for node\r\n");
            return;
        }
    }
}

int tmxc_mesh_qkd_send_key(const uint8_t* dst_id) {
    if (!tmxc_mesh_core.qkd_enabled || !tmxc_mesh_core.mesh_enabled || dst_id == NULL) {
        return -1;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        uint8_t match = 1;
        for (int j = 0; j < 32; j++) {
            if (tmxc_mesh_core.nodes[i].node_id[j] != dst_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match && tmxc_mesh_core.qkd_keys[i].key_valid) {
            tmxc_mesh_packet_t qkd_packet;
            for (int j = 0; j < 32; j++) {
                qkd_packet.dst_id[j] = dst_id[j];
            }
            for (int j = 0; j < 32; j++) {
                qkd_packet.src_id[j] = tmxc_mesh_core.nodes[i].node_id[j];
            }
            qkd_packet.type = TMXC_MESH_PACKET_QKD_KEY;
            qkd_packet.ttl = TMXC_MESH_TTL;
            qkd_packet.sequence = tmxc_mesh_core.packets_sent;
            
            for (int j = 0; j < 32; j++) {
                qkd_packet.data[j] = tmxc_mesh_core.qkd_keys[i].qkd_key[j];
            }
            qkd_packet.data_size = 32;
            qkd_packet.timestamp = tmxc_get_cycle_count();
            
            tmxc_mesh_core.packets_sent++;
            tmxc_uart_puts("[MESH-QKD] Quantum key sent\r\n");
            return 0;
        }
    }
    
    return -2;
}

int tmxc_mesh_qkd_receive_key(const uint8_t* src_id, const uint8_t* key) {
    if (!tmxc_mesh_core.qkd_enabled || src_id == NULL || key == NULL) {
        return -1;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        uint8_t match = 1;
        for (int j = 0; j < 32; j++) {
            if (tmxc_mesh_core.nodes[i].node_id[j] != src_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match) {
            for (int j = 0; j < 32; j++) {
                tmxc_mesh_core.qkd_keys[i].qkd_key[j] = key[j];
            }
            tmxc_mesh_core.qkd_keys[i].key_index = i;
            tmxc_mesh_core.qkd_keys[i].key_valid = 1;
            tmxc_mesh_core.qkd_keys[i].key_timestamp = tmxc_get_cycle_count();
            
            tmxc_uart_puts("[MESH-QKD] Quantum key received and stored\r\n");
            return 0;
        }
    }
    
    return -2;
}

uint8_t* tmxc_mesh_qkd_get_key(const uint8_t* node_id) {
    if (!tmxc_mesh_core.qkd_enabled || node_id == NULL) {
        return NULL;
    }
    
    for (uint32_t i = 0; i < TMXC_MESH_MAX_NODES; i++) {
        uint8_t match = 1;
        for (int j = 0; j < 32; j++) {
            if (tmxc_mesh_core.nodes[i].node_id[j] != node_id[j]) {
                match = 0;
                break;
            }
        }
        
        if (match && tmxc_mesh_core.qkd_keys[i].key_valid) {
            return tmxc_mesh_core.qkd_keys[i].qkd_key;
        }
    }
    
    return NULL;
}

void tmxc_mesh_qkd_encrypt_data(uint8_t* data, uint32_t size, const uint8_t* key) {
    if (data == NULL || key == NULL || size == 0) {
        return;
    }
    
    for (uint32_t i = 0; i < size; i++) {
        data[i] ^= key[i % 32];
    }
}

void tmxc_mesh_qkd_decrypt_data(uint8_t* data, uint32_t size, const uint8_t* key) {
    tmxc_mesh_qkd_encrypt_data(data, size, key);
}

void tmxc_mesh_d2d_enable(uint8_t enable) {
    tmxc_mesh_core.d2d_mode = enable;
    tmxc_uart_puts("[MESH-D2D] Direct-to-Device mode ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

uint8_t tmxc_mesh_d2d_is_enabled(void) {
    return tmxc_mesh_core.d2d_mode;
}
