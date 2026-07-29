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
#include "../security/multi_account/tmxc_multi_account.h"

#define TMXC_SWITCH_TIME_MS 100

typedef struct {
    uint8_t initialized;
    uint8_t quick_switch_enabled;
    uint64_t last_switch_time;
    uint32_t switches_performed;
    uint8_t current_profile;
    uint8_t target_profile;
} tmxc_quick_switch_t;

static tmxc_quick_switch_t tmxc_qswitch;

void tmxc_quick_switch_init(void) {
    tmxc_qswitch.initialized = 0;
    tmxc_qswitch.quick_switch_enabled = 1;
    tmxc_qswitch.last_switch_time = 0;
    tmxc_qswitch.switches_performed = 0;
    tmxc_qswitch.current_profile = 0;
    tmxc_qswitch.target_profile = 0;
    
    tmxc_qswitch.initialized = 1;
    
    tmxc_uart_puts("[QUICK-SWITCH] Quick switch OS initialized\r\n");
}

void tmxc_quick_switch_profile(uint8_t profile_id) {
    if (!tmxc_qswitch.initialized || !tmxc_qswitch.quick_switch_enabled) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed_ms = (current_time - tmxc_qswitch.last_switch_time) * 1000 / tmxc_get_frequency();
    
    if (elapsed_ms < TMXC_SWITCH_TIME_MS) {
        return;
    }
    
    tmxc_qswitch.target_profile = profile_id;
    
    uint8_t auth_data[32] = {0};
    tmxc_switch_profile(profile_id, auth_data);
    
    tmxc_qswitch.current_profile = profile_id;
    tmxc_qswitch.last_switch_time = current_time;
    tmxc_qswitch.switches_performed++;
    
    tmxc_uart_puts("[QUICK-SWITCH] Profile switched in <100ms\r\n");
}

void tmxc_quick_switch_enable(uint8_t enable) {
    if (!tmxc_qswitch.initialized) {
        return;
    }
    
    tmxc_qswitch.quick_switch_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[QUICK-SWITCH] Quick switch enabled\r\n");
    } else {
        tmxc_uart_puts("[QUICK-SWITCH] Quick switch disabled\r\n");
    }
}

uint8_t tmxc_quick_switch_get_current_profile(void) {
    return tmxc_qswitch.current_profile;
}

uint32_t tmxc_quick_switch_get_switches_performed(void) {
    return tmxc_qswitch.switches_performed;
}

void tmxc_quick_switch_cleanup(void) {
    if (!tmxc_qswitch.initialized) {
        return;
    }
    
    tmxc_qswitch.quick_switch_enabled = 0;
    tmxc_qswitch.initialized = 0;
    
    tmxc_uart_puts("[QUICK-SWITCH] Quick switch OS cleaned up\r\n");
}
