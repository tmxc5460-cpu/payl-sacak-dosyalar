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
#include "tmxc_kernel.h"

typedef struct {
    uint32_t dest_ip;
    uint16_t dest_port;
    uint32_t src_pid;
    uint64_t timestamp;
    uint32_t packet_size;
    uint8_t is_encrypted;
    uint8_t was_blocked;
    uint8_t leak_type;
} tmxc_leak_log_entry_t;

typedef struct {
    uint8_t initialized;
    uint8_t monitoring_enabled;
    uint32_t packets_inspected;
    uint32_t leaks_detected;
    uint32_t leaks_blocked;
    tmxc_leak_log_entry_t leak_log[256];
    uint32_t leak_log_count;
    uint32_t allowed_ips[32];
    uint32_t allowed_ip_count;
    uint16_t blocked_ports[64];
    uint32_t blocked_port_count;
    uint8_t zero_copy_enabled;
    uint64_t last_inspection_time;
} tmxc_leak_detector_t;

static tmxc_leak_detector_t tmxc_leak_detector;

#define TMXC_LEAK_DETECTOR_BASE 0xE2000000
#define TMXC_LEAK_DETECTOR_CTRL 0x00
#define TMXC_LEAK_DETECTOR_STATUS 0x04
#define TMXC_LEAK_DETECTOR_DROP_COUNT 0x08

#define TMXC_LEAK_DETECTOR_CMD_MONITOR_ENABLE 0x01
#define TMXC_LEAK_DETECTOR_CMD_MONITOR_DISABLE 0x02
#define TMXC_LEAK_DETECTOR_CMD_ZERO_COPY_ENABLE 0x03

#define TMXC_LEAK_TYPE_UNAUTHORIZED_IP 0
#define TMXC_LEAK_TYPE_ENCRYPTED_PORT 1
#define TMXC_LEAK_TYPE_DATA_EXFILTRATION 2
#define TMXC_LEAK_TYPE_SUSPICIOUS_PATTERN 3

extern void tmxc_island_show_notification(const char* app_id, const char* title, const char* message, uint8_t priority);

static uint8_t tmxc_is_ip_allowed(uint32_t ip) {
    for (uint32_t i = 0; i < tmxc_leak_detector.allowed_ip_count; i++) {
        if (tmxc_leak_detector.allowed_ips[i] == ip) {
            return 1;
        }
    }
    return 0;
}

static uint8_t tmxc_is_port_blocked(uint16_t port) {
    for (uint32_t i = 0; i < tmxc_leak_detector.blocked_port_count; i++) {
        if (tmxc_leak_detector.blocked_ports[i] == port) {
            return 1;
        }
    }
    
    if (port >= 1024 && port <= 1099) {
        return 1;
    }
    
    if (port == 443 || port == 8443) {
        return 0;
    }
    
    if (port >= 10000 && port <= 65535) {
        return 1;
    }
    
    return 0;
}

static void tmxc_log_leak(uint32_t dest_ip, uint16_t dest_port, uint32_t src_pid, 
                         uint32_t packet_size, uint8_t is_encrypted, uint8_t was_blocked, uint8_t leak_type) {
    if (tmxc_leak_detector.leak_log_count >= 256) {
        for (uint32_t i = 0; i < 255; i++) {
            tmxc_leak_detector.leak_log[i] = tmxc_leak_detector.leak_log[i + 1];
        }
        tmxc_leak_detector.leak_log_count--;
    }
    
    uint32_t index = tmxc_leak_detector.leak_log_count;
    tmxc_leak_detector.leak_log[index].dest_ip = dest_ip;
    tmxc_leak_detector.leak_log[index].dest_port = dest_port;
    tmxc_leak_detector.leak_log[index].src_pid = src_pid;
    tmxc_leak_detector.leak_log[index].timestamp = tmxc_get_cycle_count();
    tmxc_leak_detector.leak_log[index].packet_size = packet_size;
    tmxc_leak_detector.leak_log[index].is_encrypted = is_encrypted;
    tmxc_leak_detector.leak_log[index].was_blocked = was_blocked;
    tmxc_leak_detector.leak_log[index].leak_type = leak_type;
    
    tmxc_leak_detector.leak_log_count++;
}

static void tmxc_trigger_leak_alarm(uint32_t dest_ip, uint16_t dest_port, uint32_t src_pid, uint8_t leak_type) {
    tmxc_uart_puts("[LEAK-DETECTOR] DATA LEAK ATTEMPT DETECTED!\r\n");
    
    tmxc_uart_puts("[LEAK-DETECTOR] Source PID: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = src_pid;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("[LEAK-DETECTOR] Destination IP: ");
    char hex_chars[] = "0123456789ABCDEF";
    char hex_buffer[9];
    hex_buffer[8] = '\0';
    for (int j = 7; j >= 0; j--) {
        hex_buffer[j] = hex_chars[dest_ip & 0xF];
        dest_ip >>= 4;
    }
    tmxc_uart_puts(hex_buffer);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("[LEAK-DETECTOR] Destination Port: ");
    pos = 20;
    buffer[pos] = '\0';
    temp = dest_port;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("[LEAK-DETECTOR] Leak Type: ");
    if (leak_type == TMXC_LEAK_TYPE_UNAUTHORIZED_IP) {
        tmxc_uart_puts("Unauthorized IP\r\n");
    } else if (leak_type == TMXC_LEAK_TYPE_ENCRYPTED_PORT) {
        tmxc_uart_puts("Encrypted Port\r\n");
    } else if (leak_type == TMXC_LEAK_TYPE_DATA_EXFILTRATION) {
        tmxc_uart_puts("Data Exfiltration\r\n");
    } else {
        tmxc_uart_puts("Suspicious Pattern\r\n");
    }
    
    char notification_title[64];
    char notification_message[128];
    
    pos = 0;
    temp = src_pid;
    while (temp > 0 && pos < 20) {
        notification_title[pos++] = '0' + (temp % 10);
        temp /= 10;
    }
    for (int i = 0, j = pos - 1; i < j; i++, j--) {
        char t = notification_title[i];
        notification_title[i] = notification_title[j];
        notification_title[j] = t;
    }
    notification_title[pos] = '\0';
    
    if (leak_type == TMXC_LEAK_TYPE_UNAUTHORIZED_IP) {
        const char* msg = "Unauthorized IP access blocked";
        for (int i = 0; msg[i] && i < 127; i++) {
            notification_message[i] = msg[i];
        }
        notification_message[30] = '\0';
    } else if (leak_type == TMXC_LEAK_TYPE_ENCRYPTED_PORT) {
        const char* msg = "Encrypted port access blocked";
        for (int i = 0; msg[i] && i < 127; i++) {
            notification_message[i] = msg[i];
        }
        notification_message[30] = '\0';
    } else {
        const char* msg = "Data leak attempt blocked";
        for (int i = 0; msg[i] && i < 127; i++) {
            notification_message[i] = msg[i];
        }
        notification_message[26] = '\0';
    }
    
    tmxc_island_show_notification("SECURITY", notification_title, notification_message, 10);
    
    extern void tmxc_security_shield_enter_lockdown(void);
    if (tmxc_leak_detector.leaks_detected > 5) {
        tmxc_uart_puts("[LEAK-DETECTOR] Multiple leaks detected - entering lockdown\r\n");
        tmxc_security_shield_enter_lockdown();
    }
}

void tmxc_leak_detector_init(void) {
    tmxc_uart_puts("[LEAK-DETECTOR] Initializing leak detector...\r\n");
    
    tmxc_leak_detector.initialized = 0;
    tmxc_leak_detector.monitoring_enabled = 1;
    tmxc_leak_detector.packets_inspected = 0;
    tmxc_leak_detector.leaks_detected = 0;
    tmxc_leak_detector.leaks_blocked = 0;
    tmxc_leak_detector.leak_log_count = 0;
    tmxc_leak_detector.allowed_ip_count = 0;
    tmxc_leak_detector.blocked_port_count = 0;
    tmxc_leak_detector.zero_copy_enabled = 1;
    tmxc_leak_detector.last_inspection_time = 0;
    
    for (uint32_t i = 0; i < 256; i++) {
        tmxc_leak_detector.leak_log[i].dest_ip = 0;
        tmxc_leak_detector.leak_log[i].dest_port = 0;
        tmxc_leak_detector.leak_log[i].src_pid = 0;
        tmxc_leak_detector.leak_log[i].timestamp = 0;
        tmxc_leak_detector.leak_log[i].packet_size = 0;
        tmxc_leak_detector.leak_log[i].is_encrypted = 0;
        tmxc_leak_detector.leak_log[i].was_blocked = 0;
        tmxc_leak_detector.leak_log[i].leak_type = 0;
    }
    
    tmxc_leak_detector.allowed_ips[0] = 0x0A000001;
    tmxc_leak_detector.allowed_ips[1] = 0x7F000001;
    tmxc_leak_detector.allowed_ip_count = 2;
    
    tmxc_leak_detector.blocked_ports[0] = 6667;
    tmxc_leak_detector.blocked_ports[1] = 6668;
    tmxc_leak_detector.blocked_ports[2] = 6669;
    tmxc_leak_detector.blocked_port_count = 3;
    
    uint32_t detector_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_LEAK_DETECTOR_BASE + TMXC_LEAK_DETECTOR_CTRL));
    detector_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)(TMXC_LEAK_DETECTOR_BASE + TMXC_LEAK_DETECTOR_CTRL), detector_ctrl);
    
    detector_ctrl |= TMXC_LEAK_DETECTOR_CMD_MONITOR_ENABLE;
    detector_ctrl |= TMXC_LEAK_DETECTOR_CMD_ZERO_COPY_ENABLE;
    tmxc_write32((volatile uint32_t*)(TMXC_LEAK_DETECTOR_BASE + TMXC_LEAK_DETECTOR_CTRL), detector_ctrl);
    
    tmxc_leak_detector.initialized = 1;
    tmxc_uart_puts("[LEAK-DETECTOR] Leak detector initialized with Zero-Copy\r\n");
}

uint8_t tmxc_leak_detector_inspect_packet(const uint8_t* packet, uint32_t size, 
                                         uint32_t src_pid, uint32_t dest_ip, uint16_t dest_port) {
    if (!tmxc_leak_detector.initialized || !tmxc_leak_detector.monitoring_enabled) {
        return 0;
    }
    
    if (packet == NULL || size == 0) {
        return 0;
    }
    
    tmxc_leak_detector.packets_inspected++;
    
    uint8_t should_block = 0;
    uint8_t leak_type = 0;
    uint8_t is_encrypted = 0;
    
    if (!tmxc_is_ip_allowed(dest_ip)) {
        should_block = 1;
        leak_type = TMXC_LEAK_TYPE_UNAUTHORIZED_IP;
    }
    
    if (tmxc_is_port_blocked(dest_port)) {
        should_block = 1;
        leak_type = TMXC_LEAK_TYPE_ENCRYPTED_PORT;
    }
    
    if (size > 1024) {
        should_block = 1;
        leak_type = TMXC_LEAK_TYPE_DATA_EXFILTRATION;
    }
    
    for (uint32_t i = 0; i < size - 3; i++) {
        if (packet[i] == 0x00 && packet[i+1] == 0x00 && packet[i+2] == 0x00 && packet[i+3] == 0x00) {
            is_encrypted = 1;
            break;
        }
    }
    
    if (is_encrypted && !tmxc_is_ip_allowed(dest_ip)) {
        should_block = 1;
        leak_type = TMXC_LEAK_TYPE_SUSPICIOUS_PATTERN;
    }
    
    if (should_block) {
        tmxc_leak_detector.leaks_detected++;
        tmxc_leak_detector.leaks_blocked++;
        
        tmxc_log_leak(dest_ip, dest_port, src_pid, size, is_encrypted, 1, leak_type);
        
        uint32_t drop_count = tmxc_read32((volatile uint32_t*)(TMXC_LEAK_DETECTOR_BASE + TMXC_LEAK_DETECTOR_DROP_COUNT));
        drop_count++;
        tmxc_write32((volatile uint32_t*)(TMXC_LEAK_DETECTOR_BASE + TMXC_LEAK_DETECTOR_DROP_COUNT), drop_count);
        
        tmxc_trigger_leak_alarm(dest_ip, dest_port, src_pid, leak_type);
        
        return 1;
    }
    
    tmxc_log_leak(dest_ip, dest_port, src_pid, size, is_encrypted, 0, 0);
    
    tmxc_leak_detector.last_inspection_time = tmxc_get_cycle_count();
    
    return 0;
}

void tmxc_leak_detector_add_allowed_ip(uint32_t ip) {
    if (tmxc_leak_detector.allowed_ip_count >= 32) {
        return;
    }
    
    tmxc_leak_detector.allowed_ips[tmxc_leak_detector.allowed_ip_count] = ip;
    tmxc_leak_detector.allowed_ip_count++;
    
    tmxc_uart_puts("[LEAK-DETECTOR] Added allowed IP: ");
    char hex_chars[] = "0123456789ABCDEF";
    char hex_buffer[9];
    hex_buffer[8] = '\0';
    for (int j = 7; j >= 0; j--) {
        hex_buffer[j] = hex_chars[ip & 0xF];
        ip >>= 4;
    }
    tmxc_uart_puts(hex_buffer);
    tmxc_uart_puts("\r\n");
}

void tmxc_leak_detector_remove_allowed_ip(uint32_t ip) {
    for (uint32_t i = 0; i < tmxc_leak_detector.allowed_ip_count; i++) {
        if (tmxc_leak_detector.allowed_ips[i] == ip) {
            for (uint32_t j = i; j < tmxc_leak_detector.allowed_ip_count - 1; j++) {
                tmxc_leak_detector.allowed_ips[j] = tmxc_leak_detector.allowed_ips[j + 1];
            }
            tmxc_leak_detector.allowed_ip_count--;
            break;
        }
    }
}

void tmxc_leak_detector_add_blocked_port(uint16_t port) {
    if (tmxc_leak_detector.blocked_port_count >= 64) {
        return;
    }
    
    tmxc_leak_detector.blocked_ports[tmxc_leak_detector.blocked_port_count] = port;
    tmxc_leak_detector.blocked_port_count++;
    
    tmxc_uart_puts("[LEAK-DETECTOR] Added blocked port: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = port;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

void tmxc_leak_detector_remove_blocked_port(uint16_t port) {
    for (uint32_t i = 0; i < tmxc_leak_detector.blocked_port_count; i++) {
        if (tmxc_leak_detector.blocked_ports[i] == port) {
            for (uint32_t j = i; j < tmxc_leak_detector.blocked_port_count - 1; j++) {
                tmxc_leak_detector.blocked_ports[j] = tmxc_leak_detector.blocked_ports[j + 1];
            }
            tmxc_leak_detector.blocked_port_count--;
            break;
        }
    }
}

void tmxc_leak_detector_enable_monitoring(void) {
    if (!tmxc_leak_detector.initialized) {
        return;
    }
    
    tmxc_uart_puts("[LEAK-DETECTOR] Enabling outbound monitoring...\r\n");
    
    uint32_t detector_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_LEAK_DETECTOR_BASE + TMXC_LEAK_DETECTOR_CTRL));
    detector_ctrl |= TMXC_LEAK_DETECTOR_CMD_MONITOR_ENABLE;
    tmxc_write32((volatile uint32_t*)(TMXC_LEAK_DETECTOR_BASE + TMXC_LEAK_DETECTOR_CTRL), detector_ctrl);
    
    tmxc_leak_detector.monitoring_enabled = 1;
    
    tmxc_uart_puts("[LEAK-DETECTOR] Outbound monitoring enabled\r\n");
}

void tmxc_leak_detector_disable_monitoring(void) {
    if (!tmxc_leak_detector.initialized) {
        return;
    }
    
    uint32_t detector_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_LEAK_DETECTOR_BASE + TMXC_LEAK_DETECTOR_CTRL));
    detector_ctrl |= TMXC_LEAK_DETECTOR_CMD_MONITOR_DISABLE;
    tmxc_write32((volatile uint32_t*)(TMXC_LEAK_DETECTOR_BASE + TMXC_LEAK_DETECTOR_CTRL), detector_ctrl);
    
    tmxc_leak_detector.monitoring_enabled = 0;
    
    tmxc_uart_puts("[LEAK-DETECTOR] Outbound monitoring disabled\r\n");
}

uint8_t tmxc_leak_detector_is_monitoring_enabled(void) {
    return tmxc_leak_detector.monitoring_enabled;
}

uint32_t tmxc_leak_detector_get_packets_inspected(void) {
    return tmxc_leak_detector.packets_inspected;
}

uint32_t tmxc_leak_detector_get_leaks_detected(void) {
    return tmxc_leak_detector.leaks_detected;
}

uint32_t tmxc_leak_detector_get_leaks_blocked(void) {
    return tmxc_leak_detector.leaks_blocked;
}

tmxc_leak_log_entry_t* tmxc_leak_detector_get_leak_log(void) {
    return tmxc_leak_detector.leak_log;
}

uint32_t tmxc_leak_detector_get_leak_log_count(void) {
    return tmxc_leak_detector.leak_log_count;
}

void tmxc_leak_detector_clear_leak_log(void) {
    tmxc_leak_detector.leak_log_count = 0;
    for (uint32_t i = 0; i < 256; i++) {
        tmxc_leak_detector.leak_log[i].dest_ip = 0;
        tmxc_leak_detector.leak_log[i].dest_port = 0;
        tmxc_leak_detector.leak_log[i].src_pid = 0;
        tmxc_leak_detector.leak_log[i].timestamp = 0;
        tmxc_leak_detector.leak_log[i].packet_size = 0;
        tmxc_leak_detector.leak_log[i].is_encrypted = 0;
        tmxc_leak_detector.leak_log[i].was_blocked = 0;
        tmxc_leak_detector.leak_log[i].leak_type = 0;
    }
    
    tmxc_uart_puts("[LEAK-DETECTOR] Leak log cleared\r\n");
}

void tmxc_leak_detector_reset_stats(void) {
    tmxc_leak_detector.packets_inspected = 0;
    tmxc_leak_detector.leaks_detected = 0;
    tmxc_leak_detector.leaks_blocked = 0;
}
