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

static tmxc_security_callbacks_t tmxc_security_callbacks;

void tmxc_security_callbacks_init(void) {
    tmxc_uart_puts("[SECURITY-CALLBACKS] Initializing security callbacks...\r\n");
    
    tmxc_security_callbacks.on_leak_detected = NULL;
    tmxc_security_callbacks.on_threat_detected = NULL;
    tmxc_security_callbacks.on_lockdown_triggered = NULL;
    tmxc_security_callbacks.callbacks_enabled = 1;
    
    tmxc_uart_puts("[SECURITY-CALLBACKS] Security callbacks initialized\r\n");
}

void tmxc_security_callbacks_register_leak_handler(tmxc_security_callback_leak_detected handler) {
    tmxc_security_callbacks.on_leak_detected = handler;
    tmxc_uart_puts("[SECURITY-CALLBACKS] Leak handler registered\r\n");
}

void tmxc_security_callbacks_register_threat_handler(tmxc_security_callback_threat_detected handler) {
    tmxc_security_callbacks.on_threat_detected = handler;
    tmxc_uart_puts("[SECURITY-CALLBACKS] Threat handler registered\r\n");
}

void tmxc_security_callbacks_register_lockdown_handler(tmxc_security_callback_lockdown_triggered handler) {
    tmxc_security_callbacks.on_lockdown_triggered = handler;
    tmxc_uart_puts("[SECURITY-CALLBACKS] Lockdown handler registered\r\n");
}

void tmxc_security_callbacks_enable(uint8_t enable) {
    tmxc_security_callbacks.callbacks_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[SECURITY-CALLBACKS] Callbacks enabled\r\n");
    } else {
        tmxc_uart_puts("[SECURITY-CALLBACKS] Callbacks disabled\r\n");
    }
}

uint8_t tmxc_security_callbacks_are_enabled(void) {
    return tmxc_security_callbacks.callbacks_enabled;
}

void tmxc_security_notify_leak_detected(uint32_t dest_ip, uint16_t dest_port, uint32_t src_pid, uint8_t leak_type) {
    if (!tmxc_security_callbacks.callbacks_enabled) {
        return;
    }
    
    if (tmxc_security_callbacks.on_leak_detected != NULL) {
        tmxc_security_callbacks.on_leak_detected(dest_ip, dest_port, src_pid, leak_type);
    }
}

void tmxc_security_notify_threat_detected(uint64_t addr, uint32_t size, uint32_t owner_pid) {
    if (!tmxc_security_callbacks.callbacks_enabled) {
        return;
    }
    
    if (tmxc_security_callbacks.on_threat_detected != NULL) {
        tmxc_security_callbacks.on_threat_detected(addr, size, owner_pid);
    }
}

void tmxc_security_notify_lockdown_triggered(uint8_t reason) {
    if (!tmxc_security_callbacks.callbacks_enabled) {
        return;
    }
    
    if (tmxc_security_callbacks.on_lockdown_triggered != NULL) {
        tmxc_security_callbacks.on_lockdown_triggered(reason);
    }
}
