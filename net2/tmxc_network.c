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
    uint8_t initialized;
    uint8_t traffic_scrubbing_enabled;
    uint32_t packets_processed;
    uint32_t packets_scrubbed;
    uint32_t zero_copy_enabled;
    uint8_t packet_buffer[65536];
    uint32_t buffer_head;
    uint32_t buffer_tail;
    uint32_t buffer_size;
    uint32_t scrubbing_threshold;
} tmxc_network_t;

static tmxc_network_t tmxc_network;

#define TMXC_NETWORK_BASE 0xD0000000
#define TMXC_NETWORK_CTRL 0x00
#define TMXC_NETWORK_STATUS 0x04
#define TMXC_NETWORK_RX_BASE 0x08
#define TMXC_NETWORK_TX_BASE 0x0C
#define TMXC_NETWORK_DMA_CTRL 0x10

extern uint8_t tmxc_security_shield_analyze_packet(uint8_t* packet, uint32_t size, uint32_t source_ip);
extern void tmxc_security_shield_drop_packet(void);
extern uint8_t tmxc_leak_detector_inspect_packet(const uint8_t* packet, uint32_t size, uint32_t src_pid, uint32_t dest_ip, uint16_t dest_port);
extern uint32_t tmxc_scheduler_current_process;

void tmxc_network_init(void) {
    tmxc_uart_puts("[NETWORK] Initializing network driver...\r\n");
    
    tmxc_network.initialized = 0;
    tmxc_network.traffic_scrubbing_enabled = 1;
    tmxc_network.packets_processed = 0;
    tmxc_network.packets_scrubbed = 0;
    tmxc_network.zero_copy_enabled = 1;
    tmxc_network.buffer_head = 0;
    tmxc_network.buffer_tail = 0;
    tmxc_network.buffer_size = 65536;
    tmxc_network.scrubbing_threshold = 100;
    
    for (uint32_t i = 0; i < 65536; i++) {
        tmxc_network.packet_buffer[i] = 0;
    }
    
    uint32_t net_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_CTRL));
    net_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_CTRL), net_ctrl);
    
    if (tmxc_network.zero_copy_enabled) {
        uint32_t dma_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_DMA_CTRL));
        dma_ctrl |= (1 << 0);
        tmxc_write32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_DMA_CTRL), dma_ctrl);
    }
    
    tmxc_network.initialized = 1;
    tmxc_uart_puts("[NETWORK] Network driver initialized with Zero-Copy DMA\r\n");
}

int tmxc_network_zero_copy_receive(uint8_t** packet_ptr, uint32_t* size) {
    if (!tmxc_network.initialized || !tmxc_network.zero_copy_enabled) {
        return -1;
    }
    
    if (packet_ptr == NULL || size == NULL) {
        return -2;
    }
    
    uint32_t net_status = tmxc_read32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_STATUS));
    
    if (!(net_status & (1 << 0))) {
        return 0;
    }
    
    uint32_t rx_addr = tmxc_read32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_RX_BASE));
    uint32_t packet_size = tmxc_read32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_RX_BASE + 4));
    
    if (packet_size > tmxc_network.buffer_size) {
        return -3;
    }
    
    *packet_ptr = (uint8_t*)rx_addr;
    *size = packet_size;
    
    return 0;
}

int tmxc_network_zero_copy_send(const uint8_t* packet, uint32_t size) {
    if (!tmxc_network.initialized || !tmxc_network.zero_copy_enabled) {
        return -1;
    }
    
    if (packet == NULL || size == 0) {
        return -2;
    }
    
    uint32_t dest_ip = 0;
    uint16_t dest_port = 0;
    
    if (size >= 6) {
        dest_ip = ((uint32_t)packet[0] << 24) | ((uint32_t)packet[1] << 16) | 
                   ((uint32_t)packet[2] << 8) | (uint32_t)packet[3];
        dest_port = ((uint16_t)packet[4] << 8) | (uint16_t)packet[5];
    }
    
    uint8_t should_block = tmxc_leak_detector_inspect_packet(packet, size, 
                                                             tmxc_scheduler_current_process, 
                                                             dest_ip, dest_port);
    
    if (should_block) {
        tmxc_uart_puts("[NETWORK] Packet blocked by leak detector\r\n");
        return -3;
    }
    
    uint32_t tx_addr = (uint32_t)packet;
    
    tmxc_write32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_TX_BASE), tx_addr);
    tmxc_write32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_TX_BASE + 4), size);
    
    uint32_t net_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_CTRL));
    net_ctrl |= (1 << 1);
    tmxc_write32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_CTRL), net_ctrl);
    
    return 0;
}

int tmxc_network_scrub_packet(uint8_t* packet, uint32_t size, uint32_t source_ip) {
    if (!tmxc_network.initialized || !tmxc_network.traffic_scrubbing_enabled) {
        return 0;
    }
    
    if (packet == NULL || size == 0) {
        return 0;
    }
    
    tmxc_network.packets_processed++;
    
    uint8_t should_drop = tmxc_security_shield_analyze_packet(packet, size, source_ip);
    
    if (should_drop) {
        tmxc_network.packets_scrubbed++;
        tmxc_security_shield_drop_packet();
        
        if (tmxc_network.zero_copy_enabled) {
            uint32_t net_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_CTRL));
            net_ctrl |= (1 << 2);
            tmxc_write32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_CTRL), net_ctrl);
        }
        
        return 1;
    }
    
    return 0;
}

void tmxc_network_enable_traffic_scrubbing(void) {
    tmxc_network.traffic_scrubbing_enabled = 1;
    tmxc_uart_puts("[NETWORK] Traffic scrubbing enabled\r\n");
}

void tmxc_network_disable_traffic_scrubbing(void) {
    tmxc_network.traffic_scrubbing_enabled = 0;
    tmxc_uart_puts("[NETWORK] Traffic scrubbing disabled\r\n");
}

void tmxc_network_enable_zero_copy(void) {
    if (!tmxc_network.initialized) {
        return;
    }
    
    uint32_t dma_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_DMA_CTRL));
    dma_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_DMA_CTRL), dma_ctrl);
    
    tmxc_network.zero_copy_enabled = 1;
    tmxc_uart_puts("[NETWORK] Zero-Copy DMA enabled\r\n");
}

void tmxc_network_disable_zero_copy(void) {
    if (!tmxc_network.initialized) {
        return;
    }
    
    uint32_t dma_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_DMA_CTRL));
    dma_ctrl &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)(TMXC_NETWORK_BASE + TMXC_NETWORK_DMA_CTRL), dma_ctrl);
    
    tmxc_network.zero_copy_enabled = 0;
    tmxc_uart_puts("[NETWORK] Zero-Copy DMA disabled\r\n");
}

void tmxc_network_set_scrubbing_threshold(uint32_t threshold) {
    tmxc_network.scrubbing_threshold = threshold;
}

uint8_t tmxc_network_is_traffic_scrubbing_enabled(void) {
    return tmxc_network.traffic_scrubbing_enabled;
}

uint8_t tmxc_network_is_zero_copy_enabled(void) {
    return tmxc_network.zero_copy_enabled;
}

uint32_t tmxc_network_get_packets_processed(void) {
    return tmxc_network.packets_processed;
}

uint32_t tmxc_network_get_packets_scrubbed(void) {
    return tmxc_network.packets_scrubbed;
}
