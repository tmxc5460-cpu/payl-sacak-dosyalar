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
#include "../../kernel/tmxc_kernel.h"

#define TMXC_REFRESH_MIN_HZ 1
#define TMXC_REFRESH_MAX_HZ 160

typedef enum {
    TMXC_REFRESH_MODE_STATIC = 0,
    TMXC_REFRESH_MODE_DYNAMIC = 1,
    TMXC_REFRESH_MODE_GAME = 2
} tmxc_refresh_mode_t;

typedef struct {
    uint32_t current_hz;
    uint32_t target_hz;
    tmxc_refresh_mode_t mode;
    uint8_t initialized;
    uint8_t dynamic_refresh_enabled;
    uint64_t last_update_time;
    uint32_t frame_count;
    uint64_t frame_time_accumulator;
} tmxc_dynamic_refresh_t;

static tmxc_dynamic_refresh_t tmxc_refresh;

void tmxc_dynamic_refresh_init(void) {
    tmxc_refresh.initialized = 0;
    tmxc_refresh.current_hz = 60;
    tmxc_refresh.target_hz = 60;
    tmxc_refresh.mode = TMXC_REFRESH_MODE_DYNAMIC;
    tmxc_refresh.dynamic_refresh_enabled = 1;
    tmxc_refresh.last_update_time = tmxc_get_cycle_count();
    tmxc_refresh.frame_count = 0;
    tmxc_refresh.frame_time_accumulator = 0;
    
    tmxc_refresh.initialized = 1;
    
    tmxc_uart_puts("[DYNAMIC-REFRESH] Dynamic refresh rate initialized\r\n");
}

void tmxc_dynamic_refresh_set_hz(uint32_t hz) {
    if (!tmxc_refresh.initialized) {
        return;
    }
    
    if (hz < TMXC_REFRESH_MIN_HZ) {
        hz = TMXC_REFRESH_MIN_HZ;
    }
    if (hz > TMXC_REFRESH_MAX_HZ) {
        hz = TMXC_REFRESH_MAX_HZ;
    }
    
    tmxc_refresh.target_hz = hz;
    
    tmxc_uart_puts("[DYNAMIC-REFRESH] Target refresh rate: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = hz;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" Hz\r\n");
}

void tmxc_dynamic_refresh_update(void) {
    if (!tmxc_refresh.initialized || !tmxc_dynamic_refresh_enabled) {
        return;
    }
    
    if (tmxc_refresh.current_hz != tmxc_refresh.target_hz) {
        if (tmxc_refresh.current_hz < tmxc_refresh.target_hz) {
            tmxc_refresh.current_hz += 5;
            if (tmxc_refresh.current_hz > tmxc_refresh.target_hz) {
                tmxc_refresh.current_hz = tmxc_refresh.target_hz;
            }
        } else {
            tmxc_refresh.current_hz -= 5;
            if (tmxc_refresh.current_hz < tmxc_refresh.target_hz) {
                tmxc_refresh.current_hz = tmxc_refresh.target_hz;
            }
        }
    }
}

void tmxc_dynamic_refresh_auto_adjust(uint32_t fps) {
    if (!tmxc_refresh.initialized || !tmxc_refresh.dynamic_refresh_enabled || tmxc_refresh.mode != TMXC_REFRESH_MODE_DYNAMIC) {
        return;
    }
    
    if (fps < 30) {
        tmxc_dynamic_refresh_set_hz(30);
    } else if (fps < 60) {
        tmxc_dynamic_refresh_set_hz(60);
    } else if (fps < 120) {
        tmxc_dynamic_refresh_set_hz(90);
    } else {
        tmxc_dynamic_refresh_set_hz(120);
    }
}

void tmxc_dynamic_refresh_set_mode(tmxc_refresh_mode_t mode) {
    if (!tmxc_refresh.initialized) {
        return;
    }
    
    tmxc_refresh.mode = mode;
    
    switch (mode) {
        case TMXC_REFRESH_MODE_STATIC:
            tmxc_dynamic_refresh_set_hz(60);
            break;
        case TMXC_REFRESH_MODE_DYNAMIC:
            tmxc_dynamic_refresh_set_hz(60);
            break;
        case TMXC_REFRESH_MODE_GAME:
            tmxc_dynamic_refresh_set_hz(120);
            break;
    }
    
    tmxc_uart_puts("[DYNAMIC-REFRESH] Mode set\r\n");
}

void tmxc_dynamic_refresh_enable(uint8_t enable) {
    if (!tmxc_refresh.initialized) {
        return;
    }
    
    tmxc_refresh.dynamic_refresh_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[DYNAMIC-REFRESH] Dynamic refresh enabled\r\n");
    } else {
        tmxc_uart_puts("[DYNAMIC-REFRESH] Dynamic refresh disabled\r\n");
    }
}

uint32_t tmxc_dynamic_refresh_get_current_hz(void) {
    return tmxc_refresh.current_hz;
}

uint32_t tmxc_dynamic_refresh_get_target_hz(void) {
    return tmxc_refresh.target_hz;
}

void tmxc_dynamic_refresh_cleanup(void) {
    if (!tmxc_refresh.initialized) {
        return;
    }
    
    tmxc_dynamic_refresh_set_hz(60);
    tmxc_refresh.dynamic_refresh_enabled = 0;
    tmxc_refresh.initialized = 0;
    
    tmxc_uart_puts("[DYNAMIC-REFRESH] Dynamic refresh cleaned up\r\n");
}
