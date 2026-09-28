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
#include "tmxc_display_refresh.h"

static tmxc_display_refresh_t tmxc_refresh;

void tmxc_display_refresh_init(void) {
    tmxc_refresh.current_refresh_rate = TMXC_REFRESH_60HZ;
    tmxc_refresh.current_mode = TMXC_REFRESH_MODE_60HZ;
    tmxc_refresh.target_refresh_rate = TMXC_REFRESH_60HZ;
    tmxc_refresh.adaptive_enabled = 0;
    tmxc_refresh.frame_count = 0;
    tmxc_refresh.last_frame_time = 0;
    tmxc_refresh.actual_fps = 60;
    tmxc_refresh.vsync_enabled = 1;
    tmxc_refresh.refresh_initialized = 1;
    
    tmxc_uart_puts("[DISPLAY-REFRESH] Display refresh controller initialized\r\n");
}

void tmxc_display_refresh_set_mode(tmxc_refresh_mode_t mode) {
    if (!tmxc_refresh.refresh_initialized) {
        return;
    }
    
    tmxc_refresh.current_mode = mode;
    
    switch (mode) {
        case TMXC_REFRESH_MODE_60HZ:
            tmxc_display_refresh_set_rate(TMXC_REFRESH_60HZ);
            break;
        case TMXC_REFRESH_MODE_120HZ:
            tmxc_display_refresh_set_rate(TMXC_REFRESH_120HZ);
            break;
        case TMXC_REFRESH_MODE_160HZ:
            tmxc_display_refresh_set_rate(TMXC_REFRESH_160HZ);
            break;
        case TMXC_REFRESH_MODE_ADAPTIVE:
            tmxc_display_refresh_enable_adaptive(1);
            break;
    }
}

void tmxc_display_refresh_set_rate(uint32_t rate) {
    if (!tmxc_refresh.refresh_initialized) {
        return;
    }
    
    if (rate > TMXC_REFRESH_MAX) {
        rate = TMXC_REFRESH_MAX;
    }
    
    tmxc_refresh.target_refresh_rate = rate;
    tmxc_refresh.current_refresh_rate = rate;
    
    uint32_t timing_reg = TMXC_DISPLAY_BASE + 0x0008;
    uint32_t timing_value = (rate << 16) | (1000000000 / rate);
    __asm__ volatile("str %0, [%1]" : : "r"(timing_value), "r"(timing_reg));
    
    tmxc_uart_puts("[DISPLAY-REFRESH] Refresh rate set to ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = rate;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" Hz\r\n");
}

uint32_t tmxc_display_refresh_get_rate(void) {
    return tmxc_refresh.current_refresh_rate;
}

void tmxc_display_refresh_enable_adaptive(uint8_t enable) {
    if (!tmxc_refresh.refresh_initialized) {
        return;
    }
    
    tmxc_refresh.adaptive_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[DISPLAY-REFRESH] Adaptive refresh enabled\r\n");
    } else {
        tmxc_uart_puts("[DISPLAY-REFRESH] Adaptive refresh disabled\r\n");
    }
}

void tmxc_display_refresh_enable_vsync(uint8_t enable) {
    if (!tmxc_refresh.refresh_initialized) {
        return;
    }
    
    tmxc_refresh.vsync_enabled = enable;
}

void tmxc_display_refresh_update(void) {
    if (!tmxc_refresh.refresh_initialized) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    
    if (tmxc_refresh.last_frame_time != 0) {
        uint64_t frame_time_ns = (current_time - tmxc_refresh.last_frame_time) * 1000000000ULL / tmxc_get_frequency();
        
        if (frame_time_ns > 0) {
            tmxc_refresh.actual_fps = 1000000000ULL / frame_time_ns;
        }
    }
    
    tmxc_refresh.last_frame_time = current_time;
    tmxc_refresh.frame_count++;
    
    if (tmxc_refresh.adaptive_enabled) {
        if (tmxc_refresh.actual_fps < 55 && tmxc_refresh.current_refresh_rate > 60) {
            tmxc_display_refresh_set_rate(tmxc_refresh.current_refresh_rate / 2);
        } else if (tmxc_refresh.actual_fps > 115 && tmxc_refresh.current_refresh_rate < 120) {
            tmxc_display_refresh_set_rate(tmxc_refresh.current_refresh_rate * 2);
        }
    }
}

uint32_t tmxc_display_refresh_get_actual_fps(void) {
    return tmxc_refresh.actual_fps;
}
