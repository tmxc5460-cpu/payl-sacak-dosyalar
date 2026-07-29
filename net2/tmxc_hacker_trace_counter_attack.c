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

#define TMXC_HACKER_TRACE_MAX_LOGS 100
#define TMXC_HACKER_TRACE_ASCII_ART_SIZE 512

typedef struct {
    uint64_t hacker_ip;
    uint8_t hacker_mac[6];
    uint64_t timestamp;
    uint8_t attack_type;
    uint32_t port;
    uint8_t location_revealed;
    char location_str[64];
} tmxc_hacker_log_t;

typedef struct {
    tmxc_hacker_log_t logs[TMXC_HACKER_TRACE_MAX_LOGS];
    uint32_t log_count;
    uint8_t counter_attack_enabled;
    uint8_t ascii_response_enabled;
    uint8_t location_reveal_enabled;
    uint64_t total_attacks_blocked;
} tmxc_hacker_trace_t;

static tmxc_hacker_trace_t tmxc_hacker_trace;

static const char* tmxc_ascii_middle_finger = 
    "    /\\  \n"
    "   /  \\ \n"
    "  |    |\n"
    "  |    |\n"
    "  |____|\n"
    "   |  |\n"
    "   |  |\n"
    "   |  |\n"
    "   |  |\n"
    "   |__|\n"
    "  /    \\\n"
    " /      \\\n";

static void tmxc_hacker_trace_log_hacker(uint64_t ip, const uint8_t* mac, uint8_t attack_type, uint32_t port) {
    if (tmxc_hacker_trace.log_count >= TMXC_HACKER_TRACE_MAX_LOGS) {
        return;
    }
    
    uint32_t index = tmxc_hacker_trace.log_count;
    tmxc_hacker_trace.logs[index].hacker_ip = ip;
    
    for (int i = 0; i < 6; i++) {
        tmxc_hacker_trace.logs[index].hacker_mac[i] = mac[i];
    }
    
    tmxc_hacker_trace.logs[index].timestamp = tmxc_get_cycle_count();
    tmxc_hacker_trace.logs[index].attack_type = attack_type;
    tmxc_hacker_trace.logs[index].port = port;
    tmxc_hacker_trace.logs[index].location_revealed = 0;
    
    for (int i = 0; i < 64; i++) {
        tmxc_hacker_trace.logs[index].location_str[i] = 0;
    }
    
    tmxc_hacker_trace.log_count++;
    tmxc_hacker_trace.total_attacks_blocked++;
    
    tmxc_uart_puts("[HACKER-TRACE] Hacker logged - IP: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = ip;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

static void tmxc_hacker_trace_reveal_location(uint32_t log_index) {
    if (log_index >= tmxc_hacker_trace.log_count) {
        return;
    }
    
    uint64_t ip = tmxc_hacker_trace.logs[log_index].hacker_ip;
    
    uint8_t ip_bytes[4];
    ip_bytes[0] = (ip >> 24) & 0xFF;
    ip_bytes[1] = (ip >> 16) & 0xFF;
    ip_bytes[2] = (ip >> 8) & 0xFF;
    ip_bytes[3] = ip & 0xFF;
    
    if (ip_bytes[0] >= 1 && ip_bytes[0] <= 126) {
        tmxc_hacker_trace.logs[log_index].location_revealed = 1;
        const char* loc = "North America";
        for (int i = 0; i < 13 && i < 63; i++) {
            tmxc_hacker_trace.logs[log_index].location_str[i] = loc[i];
        }
    } else if (ip_bytes[0] >= 128 && ip_bytes[0] <= 191) {
        tmxc_hacker_trace.logs[log_index].location_revealed = 1;
        const char* loc = "Europe";
        for (int i = 0; i < 6 && i < 63; i++) {
            tmxc_hacker_trace.logs[log_index].location_str[i] = loc[i];
        }
    } else if (ip_bytes[0] >= 192 && ip_bytes[0] <= 223) {
        tmxc_hacker_trace.logs[log_index].location_revealed = 1;
        const char* loc = "Asia Pacific";
        for (int i = 0; i < 12 && i < 63; i++) {
            tmxc_hacker_trace.logs[log_index].location_str[i] = loc[i];
        }
    } else {
        tmxc_hacker_trace.logs[log_index].location_revealed = 1;
        const char* loc = "Unknown Region";
        for (int i = 0; i < 14 && i < 63; i++) {
            tmxc_hacker_trace.logs[log_index].location_str[i] = loc[i];
        }
    }
    
    tmxc_uart_puts("[HACKER-TRACE] Location revealed: ");
    tmxc_uart_puts(tmxc_hacker_trace.logs[log_index].location_str);
    tmxc_uart_puts("\r\n");
}

static void tmxc_hacker_trace_send_ascii_response(uint64_t dst_ip, uint16_t dst_port) {
    if (!tmxc_hacker_trace.ascii_response_enabled) {
        return;
    }
    
    tmxc_uart_puts("[HACKER-TRACE] Sending ASCII response to hacker\r\n");
    
    uint8_t response_packet[1024];
    uint32_t response_size = 0;
    
    const char* art = tmxc_ascii_middle_finger;
    while (*art && response_size < 1024) {
        response_packet[response_size++] = (uint8_t)*art;
        art++;
    }
    
    const char* msg = "\n\n[SECURITY ALERT] Your attack has been logged and traced.\n";
    while (*msg && response_size < 1024) {
        response_packet[response_size++] = (uint8_t)*msg;
        msg++;
    }
    
    tmxc_network_send(dst_ip, dst_port, response_packet, response_size);
    
    tmxc_uart_puts("[HACKER-TRACE] ASCII response sent\r\n");
}

void tmxc_hacker_trace_init(void) {
    for (uint32_t i = 0; i < TMXC_HACKER_TRACE_MAX_LOGS; i++) {
        tmxc_hacker_trace.logs[i].hacker_ip = 0;
        for (int j = 0; j < 6; j++) {
            tmxc_hacker_trace.logs[i].hacker_mac[j] = 0;
        }
        tmxc_hacker_trace.logs[i].timestamp = 0;
        tmxc_hacker_trace.logs[i].attack_type = 0;
        tmxc_hacker_trace.logs[i].port = 0;
        tmxc_hacker_trace.logs[i].location_revealed = 0;
        for (int j = 0; j < 64; j++) {
            tmxc_hacker_trace.logs[i].location_str[j] = 0;
        }
    }
    
    tmxc_hacker_trace.log_count = 0;
    tmxc_hacker_trace.counter_attack_enabled = 1;
    tmxc_hacker_trace.ascii_response_enabled = 1;
    tmxc_hacker_trace.location_reveal_enabled = 1;
    tmxc_hacker_trace.total_attacks_blocked = 0;
    
    tmxc_uart_puts("[HACKER-TRACE] Hacker Trace Counter-Attack initialized\r\n");
}

void tmxc_hacker_trace_detect_intrusion(uint64_t src_ip, const uint8_t* src_mac, uint16_t src_port, uint8_t attack_type) {
    if (!tmxc_hacker_trace.counter_attack_enabled) {
        return;
    }
    
    tmxc_hacker_trace_log_hacker(src_ip, src_mac, attack_type, src_port);
    
    if (tmxc_hacker_trace.location_reveal_enabled) {
        tmxc_hacker_trace_reveal_location(tmxc_hacker_trace.log_count - 1);
    }
    
    if (tmxc_hacker_trace.ascii_response_enabled) {
        tmxc_hacker_trace_send_ascii_response(src_ip, src_port);
    }
    
    tmxc_uart_puts("[HACKER-TRACE] Counter-attack executed\r\n");
}

void tmxc_hacker_trace_enable(uint8_t enable) {
    tmxc_hacker_trace.counter_attack_enabled = enable;
    tmxc_uart_puts("[HACKER-TRACE] Counter-Attack ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

void tmxc_hacker_trace_enable_ascii_response(uint8_t enable) {
    tmxc_hacker_trace.ascii_response_enabled = enable;
    tmxc_uart_puts("[HACKER-TRACE] ASCII response ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

void tmxc_hacker_trace_enable_location_reveal(uint8_t enable) {
    tmxc_hacker_trace.location_reveal_enabled = enable;
    tmxc_uart_puts("[HACKER-TRACE] Location reveal ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

uint8_t tmxc_hacker_trace_is_enabled(void) {
    return tmxc_hacker_trace.counter_attack_enabled;
}

uint32_t tmxc_hacker_trace_get_log_count(void) {
    return tmxc_hacker_trace.log_count;
}

tmxc_hacker_log_t* tmxc_hacker_trace_get_log(uint32_t index) {
    if (index >= tmxc_hacker_trace.log_count) {
        return NULL;
    }
    return &tmxc_hacker_trace.logs[index];
}

uint64_t tmxc_hacker_trace_get_total_attacks_blocked(void) {
    return tmxc_hacker_trace.total_attacks_blocked;
}

void tmxc_hacker_trace_clear_logs(void) {
    for (uint32_t i = 0; i < tmxc_hacker_trace.log_count; i++) {
        tmxc_hacker_trace.logs[i].hacker_ip = 0;
        for (int j = 0; j < 6; j++) {
            tmxc_hacker_trace.logs[i].hacker_mac[j] = 0;
        }
        tmxc_hacker_trace.logs[i].timestamp = 0;
        tmxc_hacker_trace.logs[i].attack_type = 0;
        tmxc_hacker_trace.logs[i].port = 0;
        tmxc_hacker_trace.logs[i].location_revealed = 0;
        for (int j = 0; j < 64; j++) {
            tmxc_hacker_trace.logs[i].location_str[j] = 0;
        }
    }
    tmxc_hacker_trace.log_count = 0;
    tmxc_uart_puts("[HACKER-TRACE] Logs cleared\r\n");
}
