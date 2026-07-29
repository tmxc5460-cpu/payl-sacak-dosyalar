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
    uint8_t initialized;
    uint8_t promesh_connection_lost;
    uint8_t ecosystem_connection_lost;
    uint8_t bluetooth_connection_lost;
    uint8_t modem_connection_lost;
    uint64_t last_promesh_heartbeat;
    uint64_t last_ecosystem_heartbeat;
    uint64_t last_bluetooth_heartbeat;
    uint64_t last_modem_heartbeat;
    uint32_t fail_count;
    uint8_t critical_failure;
    uint8_t recovery_mode;
} tmxc_failsafe_state_t;

static tmxc_failsafe_state_t tmxc_failsafe;

extern uint8_t tmxc_promesh_is_connected(void);
extern int tmxc_promesh_disconnect(void);
extern int tmxc_promesh_connect(const uint8_t* device_id);

extern uint8_t tmxc_ecosystem_is_peer_connected(void);
extern int tmxc_ecosystem_disconnect(void);

extern uint8_t tmxc_bluetooth_is_connected(void);
extern int tmxc_bluetooth_disconnect(void);

extern uint8_t tmxc_modem_is_call_active(void);
extern int tmxc_modem_hangup(void);

void tmxc_failsafe_init(void) {
    tmxc_uart_puts("[FAILSAFE] Initializing fail-safe mechanisms...\r\n");
    
    tmxc_failsafe.initialized = 0;
    tmxc_failsafe.promesh_connection_lost = 0;
    tmxc_failsafe.ecosystem_connection_lost = 0;
    tmxc_failsafe.bluetooth_connection_lost = 0;
    tmxc_failsafe.modem_connection_lost = 0;
    tmxc_failsafe.last_promesh_heartbeat = 0;
    tmxc_failsafe.last_ecosystem_heartbeat = 0;
    tmxc_failsafe.last_bluetooth_heartbeat = 0;
    tmxc_failsafe.last_modem_heartbeat = 0;
    tmxc_failsafe.fail_count = 0;
    tmxc_failsafe.critical_failure = 0;
    tmxc_failsafe.recovery_mode = 0;
    
    uint64_t current_time = tmxc_get_cycle_count();
    tmxc_failsafe.last_promesh_heartbeat = current_time;
    tmxc_failsafe.last_ecosystem_heartbeat = current_time;
    tmxc_failsafe.last_bluetooth_heartbeat = current_time;
    tmxc_failsafe.last_modem_heartbeat = current_time;
    
    tmxc_failsafe.initialized = 1;
    tmxc_uart_puts("[FAILSAFE] Fail-safe mechanisms initialized\r\n");
}

void tmxc_failsafe_update_heartbeat(uint8_t service) {
    if (!tmxc_failsafe.initialized) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    
    switch (service) {
        case 0:
            tmxc_failsafe.last_promesh_heartbeat = current_time;
            break;
        case 1:
            tmxc_failsafe.last_ecosystem_heartbeat = current_time;
            break;
        case 2:
            tmxc_failsafe.last_bluetooth_heartbeat = current_time;
            break;
        case 3:
            tmxc_failsafe.last_modem_heartbeat = current_time;
            break;
        default:
            break;
    }
}

void tmxc_failsafe_check_connections(void) {
    if (!tmxc_failsafe.initialized) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t timeout_cycles = (5 * tmxc_get_frequency());
    
    if (tmxc_promesh_is_connected()) {
        if (current_time - tmxc_failsafe.last_promesh_heartbeat > timeout_cycles) {
            tmxc_uart_puts("[FAILSAFE] ProMesh connection lost - initiating recovery\r\n");
            
            tmxc_promesh_disconnect();
            tmxc_failsafe.promesh_connection_lost = 1;
            tmxc_failsafe.fail_count++;
            
            if (tmxc_failsafe.fail_count > 5) {
                tmxc_failsafe.critical_failure = 1;
            }
        }
    }
    
    if (tmxc_ecosystem_is_peer_connected()) {
        if (current_time - tmxc_failsafe.last_ecosystem_heartbeat > timeout_cycles) {
            tmxc_uart_puts("[FAILSAFE] Ecosystem connection lost - initiating recovery\r\n");
            
            tmxc_ecosystem_disconnect();
            tmxc_failsafe.ecosystem_connection_lost = 1;
            tmxc_failsafe.fail_count++;
        }
    }
    
    if (tmxc_bluetooth_is_connected()) {
        if (current_time - tmxc_failsafe.last_bluetooth_heartbeat > timeout_cycles) {
            tmxc_uart_puts("[FAILSAFE] Bluetooth connection lost - initiating recovery\r\n");
            
            tmxc_bluetooth_disconnect();
            tmxc_failsafe.bluetooth_connection_lost = 1;
            tmxc_failsafe.fail_count++;
        }
    }
    
    if (tmxc_modem_is_call_active()) {
        if (current_time - tmxc_failsafe.last_modem_heartbeat > (timeout_cycles * 2)) {
            tmxc_uart_puts("[FAILSAFE] Modem call lost - initiating recovery\r\n");
            
            tmxc_modem_hangup();
            tmxc_failsafe.modem_connection_lost = 1;
            tmxc_failsafe.fail_count++;
        }
    }
}

int tmxc_failsafe_attempt_recovery(uint8_t service) {
    if (!tmxc_failsafe.initialized) {
        return -1;
    }
    
    tmxc_uart_puts("[FAILSAFE] Attempting recovery for service ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = service;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
    
    tmxc_failsafe.recovery_mode = 1;
    
    switch (service) {
        case 0:
            tmxc_failsafe.promesh_connection_lost = 0;
            tmxc_uart_puts("[FAILSAFE] ProMesh recovery initiated\r\n");
            break;
        case 1:
            tmxc_failsafe.ecosystem_connection_lost = 0;
            tmxc_uart_puts("[FAILSAFE] Ecosystem recovery initiated\r\n");
            break;
        case 2:
            tmxc_failsafe.bluetooth_connection_lost = 0;
            tmxc_uart_puts("[FAILSAFE] Bluetooth recovery initiated\r\n");
            break;
        case 3:
            tmxc_failsafe.modem_connection_lost = 0;
            tmxc_uart_puts("[FAILSAFE] Modem recovery initiated\r\n");
            break;
        default:
            tmxc_failsafe.recovery_mode = 0;
            return -2;
    }
    
    tmxc_failsafe.recovery_mode = 0;
    tmxc_failsafe.fail_count = 0;
    
    return 0;
}

void tmxc_failsafe_reset_fail_count(void) {
    tmxc_failsafe.fail_count = 0;
    tmxc_failsafe.critical_failure = 0;
    
    tmxc_uart_puts("[FAILSAFE] Fail count reset\r\n");
}

uint32_t tmxc_failsafe_get_fail_count(void) {
    return tmxc_failsafe.fail_count;
}

uint8_t tmxc_failsafe_is_critical_failure(void) {
    return tmxc_failsafe.critical_failure;
}

uint8_t tmxc_failsafe_is_recovery_mode(void) {
    return tmxc_failsafe.recovery_mode;
}

uint8_t tmxc_failsafe_is_service_healthy(uint8_t service) {
    switch (service) {
        case 0:
            return !tmxc_failsafe.promesh_connection_lost;
        case 1:
            return !tmxc_failsafe.ecosystem_connection_lost;
        case 2:
            return !tmxc_failsafe.bluetooth_connection_lost;
        case 3:
            return !tmxc_failsafe.modem_connection_lost;
        default:
            return 0;
    }
}
