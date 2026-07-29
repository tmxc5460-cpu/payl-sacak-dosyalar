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
#include "../kernel/tmxc_kernel.h"

typedef struct {
    uint8_t device_id[16];
    int8_t rssi;
    uint8_t connected;
    uint8_t is_relay;
    uint32_t last_seen;
} tmxc_mesh_node_t;

typedef struct {
    uint8_t initialized;
    uint8_t scanning;
    uint8_t connected;
    tmxc_mesh_node_t nodes[32];
    uint32_t node_count;
    uint8_t local_device_id[16];
    uint8_t proximity_enabled;
    uint32_t proximity_range;
    uint8_t call_active;
    uint8_t video_enabled;
    uint8_t audio_enabled;
    uint32_t bytes_sent;
    uint32_t bytes_received;
} tmxc_promesh_state_t;

static tmxc_promesh_state_t tmxc_promesh;

#define TMXC_PROMESH_BASE 0xA0000000
#define TMXC_PROMESH_CTRL 0x00
#define TMXC_PROMESH_STATUS 0x04
#define TMXC_PROMESH_DATA 0x08
#define TMXC_PROMESH_IRQ 0x0C
#define TMXC_PROMESH_RSSI 0x10

#define TMXC_PROMESH_CMD_INIT 0x01
#define TMXC_PROMESH_CMD_SCAN_START 0x02
#define TMXC_PROMESH_CMD_SCAN_STOP 0x03
#define TMXC_PROMESH_CMD_CONNECT 0x04
#define TMXC_PROMESH_CMD_DISCONNECT 0x05
#define TMXC_PROMESH_CMD_SEND 0x06
#define TMXC_PROMESH_CMD_RECV 0x07
#define TMXC_PROMESH_CMD_PROXIMITY_ENABLE 0x08

#define TMXC_PROMESH_PKT_HELLO 0x01
#define TMXC_PROMESH_PKT_DATA 0x02
#define TMXC_PROMESH_PKT_VIDEO 0x03
#define TMXC_PROMESH_PKT_AUDIO 0x04
#define TMXC_PROMESH_PKT_HANDSHAKE 0x05

extern void tmxc_aes256_encrypt(const uint8_t* plaintext, uint64_t plaintext_len, 
                                const uint8_t* key, uint8_t* ciphertext);
extern void tmxc_aes256_decrypt(const uint8_t* ciphertext, uint64_t ciphertext_len, 
                                const uint8_t* key, uint8_t* plaintext);

static uint8_t mesh_encryption_key[32] = {
    0x5C, 0x8F, 0x36, 0x27, 0x49, 0xBF, 0xE3, 0xC7,
    0xDC, 0xA8, 0x36, 0x99, 0x2A, 0xD0, 0x5F, 0x4E,
    0x5C, 0x8F, 0x36, 0x27, 0x49, 0xBF, 0xE3, 0xC7,
    0xDC, 0xA8, 0x36, 99, 0x2A, 0xD0, 0x5F, 0x4E
};

void tmxc_promesh_init(void) {
    tmxc_uart_puts("[PROMESH] Initializing ProMesh driver...\r\n");
    
    tmxc_promesh.initialized = 0;
    tmxc_promesh.scanning = 0;
    tmxc_promesh.connected = 0;
    tmxc_promesh.node_count = 0;
    tmxc_promesh.proximity_enabled = 1;
    tmxc_promesh.proximity_range = 5000;
    tmxc_promesh.call_active = 0;
    tmxc_promesh.video_enabled = 0;
    tmxc_promesh.audio_enabled = 0;
    tmxc_promesh.bytes_sent = 0;
    tmxc_promesh.bytes_received = 0;
    
    for (uint32_t i = 0; i < 32; i++) {
        for (uint8_t j = 0; j < 16; j++) {
            tmxc_promesh.nodes[i].device_id[j] = 0;
        }
        tmxc_promesh.nodes[i].rssi = 0;
        tmxc_promesh.nodes[i].connected = 0;
        tmxc_promesh.nodes[i].is_relay = 0;
        tmxc_promesh.nodes[i].last_seen = 0;
    }
    
    for (uint8_t i = 0; i < 16; i++) {
        tmxc_promesh.local_device_id[i] = 0;
    }
    
    uint64_t device_id = tmxc_get_cycle_count();
    for (uint8_t i = 0; i < 8; i++) {
        tmxc_promesh.local_device_id[i] = (device_id >> (i * 8)) & 0xFF;
    }
    
    uint32_t mesh_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL));
    mesh_ctrl |= TMXC_PROMESH_CMD_INIT;
    tmxc_write32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL), mesh_ctrl);
    
    tmxc_timer_delay_ms(100);
    
    uint32_t mesh_status = tmxc_read32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_STATUS));
    
    if (mesh_status & (1 << 0)) {
        tmxc_promesh.initialized = 1;
        tmxc_uart_puts("[PROMESH] ProMesh initialized\r\n");
        tmxc_uart_puts("[PROMESH] Local Device ID: ");
        char hex_chars[] = "0123456789ABCDEF";
        for (int i = 0; i < 16; i++) {
            tmxc_uart_putc(hex_chars[(tmxc_promesh.local_device_id[i] >> 4) & 0x0F]);
            tmxc_uart_putc(hex_chars[tmxc_promesh.local_device_id[i] & 0x0F]);
        }
        tmxc_uart_puts("\r\n");
    } else {
        tmxc_uart_puts("[PROMESH] ProMesh initialization failed\r\n");
    }
}

int tmxc_promesh_start_scan(void) {
    if (!tmxc_promesh.initialized) {
        return -1;
    }
    
    if (tmxc_promesh.scanning) {
        return -2;
    }
    
    tmxc_uart_puts("[PROMESH] Starting mesh scan...\r\n");
    
    uint32_t mesh_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL));
    mesh_ctrl |= TMXC_PROMESH_CMD_SCAN_START;
    tmxc_write32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL), mesh_ctrl);
    
    tmxc_promesh.scanning = 1;
    tmxc_promesh.node_count = 0;
    
    return 0;
}

int tmxc_promesh_stop_scan(void) {
    if (!tmxc_promesh.initialized) {
        return -1;
    }
    
    if (!tmxc_promesh.scanning) {
        return -2;
    }
    
    uint32_t mesh_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL));
    mesh_ctrl |= TMXC_PROMESH_CMD_SCAN_STOP;
    tmxc_write32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL), mesh_ctrl);
    
    tmxc_promesh.scanning = 0;
    
    tmxc_uart_puts("[PROMESH] Scan stopped. Found ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = tmxc_promesh.node_count;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" nodes\r\n");
    
    return 0;
}

int tmxc_promesh_connect(const uint8_t* device_id) {
    if (!tmxc_promesh.initialized || device_id == NULL) {
        return -1;
    }
    
    tmxc_uart_puts("[PROMESH] Connecting to device...\r\n");
    
    uint8_t connect_pkt[32];
    connect_pkt[0] = TMXC_PROMESH_PKT_HANDSHAKE;
    
    for (uint8_t i = 0; i < 16; i++) {
        connect_pkt[1 + i] = tmxc_promesh.local_device_id[i];
    }
    
    for (uint8_t i = 0; i < 16; i++) {
        connect_pkt[17 + i] = device_id[i];
    }
    
    for (uint32_t i = 0; i < 32; i++) {
        tmxc_write8((volatile uint8_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_DATA), connect_pkt[i]);
    }
    
    uint32_t mesh_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL));
    mesh_ctrl |= TMXC_PROMESH_CMD_CONNECT;
    tmxc_write32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL), mesh_ctrl);
    
    tmxc_timer_delay_ms(100);
    
    uint32_t mesh_status = tmxc_read32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_STATUS));
    
    if (mesh_status & (1 << 2)) {
        tmxc_promesh.connected = 1;
        
        for (uint32_t i = 0; i < 32; i++) {
            uint8_t match = 1;
            for (uint8_t j = 0; j < 16; j++) {
                if (tmxc_promesh.nodes[i].device_id[j] != device_id[j]) {
                    match = 0;
                    break;
                }
            }
            
            if (match) {
                tmxc_promesh.nodes[i].connected = 1;
                break;
            }
        }
        
        tmxc_uart_puts("[PROMESH] Connected successfully\r\n");
        return 0;
    }
    
    tmxc_uart_puts("[PROMESH] Connection failed\r\n");
    return -2;
}

int tmxc_promesh_disconnect(void) {
    if (!tmxc_promesh.initialized) {
        return -1;
    }
    
    if (!tmxc_promesh.connected) {
        return -2;
    }
    
    uint32_t mesh_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL));
    mesh_ctrl |= TMXC_PROMESH_CMD_DISCONNECT;
    tmxc_write32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL), mesh_ctrl);
    
    for (uint32_t i = 0; i < 32; i++) {
        tmxc_promesh.nodes[i].connected = 0;
    }
    
    tmxc_promesh.connected = 0;
    tmxc_promesh.call_active = 0;
    
    tmxc_uart_puts("[PROMESH] Disconnected\r\n");
    
    return 0;
}

int tmxc_promesh_send_data(const uint8_t* data, uint32_t size) {
    if (!tmxc_promesh.initialized || !tmxc_promesh.connected) {
        return -1;
    }
    
    if (data == NULL || size == 0) {
        return -2;
    }
    
    uint8_t* encrypted_buffer = (uint8_t*)tmxc_malloc(size + 16);
    
    if (encrypted_buffer == NULL) {
        return -3;
    }
    
    encrypted_buffer[0] = TMXC_PROMESH_PKT_DATA;
    encrypted_buffer[1] = (size >> 24) & 0xFF;
    encrypted_buffer[2] = (size >> 16) & 0xFF;
    encrypted_buffer[3] = (size >> 8) & 0xFF;
    encrypted_buffer[4] = size & 0xFF;
    
    tmxc_aes256_encrypt(data, size, mesh_encryption_key, encrypted_buffer + 5);
    
    for (uint32_t i = 0; i < size + 5; i++) {
        tmxc_write8((volatile uint8_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_DATA), encrypted_buffer[i]);
    }
    
    uint32_t mesh_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL));
    mesh_ctrl |= TMXC_PROMESH_CMD_SEND;
    tmxc_write32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL), mesh_ctrl);
    
    tmxc_promesh.bytes_sent += size;
    
    tmxc_free(encrypted_buffer);
    
    tmxc_uart_puts("[PROMESH] Data sent: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = size;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" bytes\r\n");
    
    return 0;
}

int tmxc_promesh_receive_data(uint8_t* buffer, uint32_t max_size) {
    if (!tmxc_promesh.initialized || !tmxc_promesh.connected) {
        return -1;
    }
    
    if (buffer == NULL || max_size == 0) {
        return -2;
    }
    
    uint32_t mesh_status = tmxc_read32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_STATUS));
    
    if (!(mesh_status & (1 << 3))) {
        return 0;
    }
    
    uint8_t pkt_header[5];
    for (uint32_t i = 0; i < 5; i++) {
        pkt_header[i] = tmxc_read8((volatile uint8_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_DATA));
    }
    
    if (pkt_header[0] != TMXC_PROMESH_PKT_DATA) {
        return -3;
    }
    
    uint32_t data_size = (pkt_header[1] << 24) | (pkt_header[2] << 16) | 
                        (pkt_header[3] << 8) | pkt_header[4];
    
    if (data_size > max_size) {
        data_size = max_size;
    }
    
    uint8_t* encrypted_data = (uint8_t*)tmxc_malloc(data_size);
    
    if (encrypted_data == NULL) {
        return -4;
    }
    
    for (uint32_t i = 0; i < data_size; i++) {
        encrypted_data[i] = tmxc_read8((volatile uint8_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_DATA));
    }
    
    tmxc_aes256_decrypt(encrypted_data, data_size, mesh_encryption_key, buffer);
    
    tmxc_promesh.bytes_received += data_size;
    
    tmxc_free(encrypted_data);
    
    tmxc_uart_puts("[PROMESH] Data received: ");
    char buf[21];
    int p = 20;
    buf[p] = '\0';
    uint64_t t = data_size;
    while (t > 0 && p > 0) {
        p--;
        buf[p] = '0' + (t % 10);
        t /= 10;
    }
    tmxc_uart_puts(&buf[p]);
    tmxc_uart_puts(" bytes\r\n");
    
    return data_size;
}

int tmxc_promesh_start_video_call(void) {
    if (!tmxc_promesh.initialized || !tmxc_promesh.connected) {
        return -1;
    }
    
    if (tmxc_promesh.call_active) {
        return -2;
    }
    
    tmxc_promesh.call_active = 1;
    tmxc_promesh.video_enabled = 1;
    tmxc_promesh.audio_enabled = 1;
    
    tmxc_uart_puts("[PROMESH] Video call started\r\n");
    
    return 0;
}

int tmxc_promesh_stop_video_call(void) {
    if (!tmxc_promesh.initialized) {
        return -1;
    }
    
    tmxc_promesh.call_active = 0;
    tmxc_promesh.video_enabled = 0;
    tmxc_promesh.audio_enabled = 0;
    
    tmxc_uart_puts("[PROMESH] Video call stopped\r\n");
    
    return 0;
}

int tmxc_promesh_send_video_frame(const uint8_t* frame, uint32_t size) {
    if (!tmxc_promesh.initialized || !tmxc_promesh.connected || !tmxc_promesh.video_enabled) {
        return -1;
    }
    
    if (frame == NULL || size == 0) {
        return -2;
    }
    
    uint8_t* compressed_buffer = (uint8_t*)tmxc_malloc(size + 16);
    
    if (compressed_buffer == NULL) {
        return -3;
    }
    
    compressed_buffer[0] = TMXC_PROMESH_PKT_VIDEO;
    compressed_buffer[1] = (size >> 24) & 0xFF;
    compressed_buffer[2] = (size >> 16) & 0xFF;
    compressed_buffer[3] = (size >> 8) & 0xFF;
    compressed_buffer[4] = size & 0xFF;
    
    for (uint32_t i = 0; i < size; i++) {
        compressed_buffer[5 + i] = frame[i];
    }
    
    for (uint32_t i = 0; i < size + 5; i++) {
        tmxc_write8((volatile uint8_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_DATA), compressed_buffer[i]);
    }
    
    uint32_t mesh_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL));
    mesh_ctrl |= TMXC_PROMESH_CMD_SEND;
    tmxc_write32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL), mesh_ctrl);
    
    tmxc_promesh.bytes_sent += size;
    
    tmxc_free(compressed_buffer);
    
    return 0;
}

int tmxc_promesh_send_audio_frame(const uint8_t* frame, uint32_t size) {
    if (!tmxc_promesh.initialized || !tmxc_promesh.connected || !tmxc_promesh.audio_enabled) {
        return -1;
    }
    
    if (frame == NULL || size == 0) {
        return -2;
    }
    
    uint8_t audio_pkt[size + 5];
    audio_pkt[0] = TMXC_PROMESH_PKT_AUDIO;
    audio_pkt[1] = (size >> 24) & 0xFF;
    audio_pkt[2] = (size >> 16) & 0xFF;
    audio_pkt[3] = (size >> 8) & 0xFF;
    audio_pkt[4] = size & 0xFF;
    
    for (uint32_t i = 0; i < size; i++) {
        audio_pkt[5 + i] = frame[i];
    }
    
    for (uint32_t i = 0; i < size + 5; i++) {
        tmxc_write8((volatile uint8_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_DATA), audio_pkt[i]);
    }
    
    uint32_t mesh_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL));
    mesh_ctrl |= TMXC_PROMESH_CMD_SEND;
    tmxc_write32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL), mesh_ctrl);
    
    tmxc_promesh.bytes_sent += size;
    
    return 0;
}

void tmxc_promesh_enable_proximity(uint8_t enable) {
    tmxc_promesh.proximity_enabled = enable;
    
    if (enable) {
        uint32_t mesh_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL));
        mesh_ctrl |= TMXC_PROMESH_CMD_PROXIMITY_ENABLE;
        tmxc_write32((volatile uint32_t*)(TMXC_PROMESH_BASE + TMXC_PROMESH_CTRL), mesh_ctrl);
        
        tmxc_uart_puts("[PROMESH] Proximity detection enabled\r\n");
    }
}

void tmxc_promesh_set_proximity_range(uint32_t range_mm) {
    tmxc_promesh.proximity_range = range_mm;
}

uint32_t tmxc_promesh_get_node_count(void) {
    return tmxc_promesh.node_count;
}

tmxc_mesh_node_t* tmxc_promesh_get_nodes(void) {
    return tmxc_promesh.nodes;
}

uint8_t tmxc_promesh_is_connected(void) {
    return tmxc_promesh.connected;
}

uint8_t tmxc_promesh_is_call_active(void) {
    return tmxc_promesh.call_active;
}

uint8_t tmxc_promesh_is_proximity_enabled(void) {
    return tmxc_promesh.proximity_enabled;
}
