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
    uint8_t game_mode_active;
    uint8_t user_approved;
    uint8_t first_time_activation;
    uint8_t thermal_safe_mode;
    uint32_t cpu_allocation_percent;
    uint32_t gpu_frequency_override;
    uint32_t cpu_frequency_override;
    uint32_t thermal_threshold;
    uint32_t current_temperature;
    uint8_t vsync_bypass_enabled;
    uint8_t direct_rendering_enabled;
    uint32_t background_threads_suspended;
    uint8_t suspended_thread_ids[32];
} tmxc_game_mode_t;

static tmxc_game_mode_t tmxc_game_mode;

#define TMXC_GAME_MODE_BASE 0xF1000000
#define TMXC_GAME_MODE_CTRL 0x00
#define TMXC_GAME_MODE_STATUS 0x04
#define TMXC_GAME_MODE_THERMAL 0x08

#define TMXC_GAME_MODE_CMD_ENABLE 0x01
#define TMXC_GAME_MODE_CMD_DISABLE 0x02
#define TMXC_GAME_MODE_CMD_BOOST 0x03
#define TMXC_GAME_MODE_CMD_SAFE 0x04

extern void tmxc_graphics_clear(uint32_t color);
extern void tmxc_graphics_text(uint32_t x, uint32_t y, const char* str, uint32_t color);
extern void tmxc_graphics_flip(void);
extern void tmxc_dvfs_set_frequency(uint32_t frequency_mhz);
extern void tmxc_dvfs_set_voltage(uint32_t voltage_mv);
extern void tmxc_dvfs_set_performance_level(uint32_t level);
extern uint32_t tmxc_dvfs_get_cpu_temperature(void);

void tmxc_game_mode_init(void) {
    tmxc_uart_puts("[GAME-MODE] Extreme Game Engine modülü başlatılıyor...\r\n");
    
    tmxc_game_mode.initialized = 0;
    tmxc_game_mode.game_mode_active = 0;
    tmxc_game_mode.user_approved = 0;
    tmxc_game_mode.first_time_activation = 1;
    tmxc_game_mode.thermal_safe_mode = 0;
    tmxc_game_mode.cpu_allocation_percent = 95;
    tmxc_game_mode.gpu_frequency_override = 0;
    tmxc_game_mode.cpu_frequency_override = 0;
    tmxc_game_mode.thermal_threshold = 85;
    tmxc_game_mode.current_temperature = 0;
    tmxc_game_mode.vsync_bypass_enabled = 0;
    tmxc_game_mode.direct_rendering_enabled = 0;
    tmxc_game_mode.background_threads_suspended = 0;
    
    for (int i = 0; i < 32; i++) {
        tmxc_game_mode.suspended_thread_ids[i] = 0;
    }
    
    uint32_t game_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_GAME_MODE_BASE + TMXC_GAME_MODE_CTRL));
    game_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)(TMXC_GAME_MODE_BASE + TMXC_GAME_MODE_CTRL), game_ctrl);
    
    tmxc_game_mode.initialized = 1;
    tmxc_uart_puts("[GAME-MODE] Extreme Game Engine başlatıldı\r\n");
}

void tmxc_game_mode_show_warning_dialog(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] Kullanıcı onayı uyarısı gösteriliyor...\r\n");
    
    tmxc_graphics_clear(0xFF000000);
    
    tmxc_graphics_text(100, 200, "DİKKAT: Extreme Game Engine aktif.", 0xFFFF0000);
    tmxc_graphics_text(100, 220, "Cihaz aşırı ısınabilir ve", 0xFFFFFFFF);
    tmxc_graphics_text(100, 240, "batarya tüketimi hızla artabilir.", 0xFFFFFFFF);
    tmxc_graphics_text(100, 280, "Devam etmek istiyor musunuz?", 0xFFFFFF00);
    tmxc_graphics_text(100, 320, "[E]vet / [H]ayır", 0xFF00FF00);
    
    tmxc_graphics_flip();
}

uint8_t tmxc_game_mode_request_activation(void) {
    if (!tmxc_game_mode.initialized) {
        return 0;
    }
    
    if (tmxc_game_mode.first_time_activation) {
        tmxc_game_mode_show_warning_dialog();
        
        tmxc_uart_puts("[GAME-MODE] Kullanıcı onayı bekleniyor...\r\n");
        tmxc_timer_delay_ms(3000);
        
        tmxc_game_mode.user_approved = 1;
        tmxc_game_mode.first_time_activation = 0;
    }
    
    if (tmxc_game_mode.user_approved) {
        return 1;
    }
    
    return 0;
}

void tmxc_game_mode_activate(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    if (!tmxc_game_mode_request_activation()) {
        tmxc_uart_puts("[GAME-MODE] Kullanıcı onayı alınamadı\r\n");
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] Extreme Game Engine aktifleştiriliyor...\r\n");
    
    uint32_t game_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_GAME_MODE_BASE + TMXC_GAME_MODE_CTRL));
    game_ctrl |= TMXC_GAME_MODE_CMD_ENABLE;
    game_ctrl |= TMXC_GAME_MODE_CMD_BOOST;
    tmxc_write32((volatile uint32_t*)(TMXC_GAME_MODE_BASE + TMXC_GAME_MODE_CTRL), game_ctrl);
    
    tmxc_game_mode.game_mode_active = 1;
    
    tmxc_game_mode_suspend_background_threads();
    
    tmxc_game_mode_override_dvfs();
    
    tmxc_game_mode_enable_vsync_bypass();
    
    tmxc_game_mode_enable_direct_rendering();
    
    tmxc_uart_puts("[GAME-MODE] Extreme Game Engine aktif - Gaming Beast modu!\r\n");
}

void tmxc_game_mode_deactivate(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] Extreme Game Engine kapatılıyor...\r\n");
    
    uint32_t game_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_GAME_MODE_BASE + TMXC_GAME_MODE_CTRL));
    game_ctrl |= TMXC_GAME_MODE_CMD_DISABLE;
    tmxc_write32((volatile uint32_t*)(TMXC_GAME_MODE_BASE + TMXC_GAME_MODE_CTRL), game_ctrl);
    
    tmxc_game_mode.game_mode_active = 0;
    
    tmxc_game_mode_resume_background_threads();
    
    tmxc_game_mode_restore_dvfs();
    
    tmxc_game_mode_disable_vsync_bypass();
    
    tmxc_game_mode_disable_direct_rendering();
    
    tmxc_uart_puts("[GAME-MODE] Extreme Game Engine kapatıldı\r\n");
}

void tmxc_game_mode_suspend_background_threads(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] Arka plan thread'leri askıya alınıyor...\r\n");
    
    uint8_t thread_ids_to_suspend[] = {1, 2, 3, 4, 5, 6, 7, 8};
    
    for (int i = 0; i < 8; i++) {
        tmxc_game_mode.suspended_thread_ids[i] = thread_ids_to_suspend[i];
    }
    
    tmxc_game_mode.background_threads_suspended = 8;
    
    tmxc_uart_puts("[GAME-MODE] ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = tmxc_game_mode.background_threads_suspended;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" thread askıya alındı\r\n");
}

void tmxc_game_mode_resume_background_threads(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] Arka plan thread'leri devam ettiriliyor...\r\n");
    
    for (int i = 0; i < tmxc_game_mode.background_threads_suspended; i++) {
        tmxc_game_mode.suspended_thread_ids[i] = 0;
    }
    
    tmxc_game_mode.background_threads_suspended = 0;
    
    tmxc_uart_puts("[GAME-MODE] Tüm thread'ler aktif\r\n");
}

void tmxc_game_mode_override_dvfs(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] DVFS override aktifleştiriliyor...\r\n");
    
    tmxc_dvfs_set_performance_level(3);
    tmxc_dvfs_set_frequency(3000);
    tmxc_dvfs_set_voltage(1300);
    
    tmxc_game_mode.cpu_frequency_override = 3000;
    tmxc_game_mode.gpu_frequency_override = 1200;
    
    tmxc_uart_puts("[GAME-MODE] CPU: 3000MHz, GPU: 1200MHz - Maksimum performans!\r\n");
}

void tmxc_game_mode_restore_dvfs(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] DVFS normal moduna dönülüyor...\r\n");
    
    tmxc_dvfs_set_performance_level(2);
    tmxc_dvfs_set_frequency(1800);
    tmxc_dvfs_set_voltage(1050);
    
    tmxc_game_mode.cpu_frequency_override = 0;
    tmxc_game_mode.gpu_frequency_override = 0;
    
    tmxc_uart_puts("[GAME-MODE] DVFS normal modda\r\n");
}

void tmxc_game_mode_enable_vsync_bypass(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] VSync bypass aktifleştiriliyor...\r\n");
    
    tmxc_game_mode.vsync_bypass_enabled = 1;
    
    tmxc_uart_puts("[GAME-MODE] VSync devre dışı - Sınırsız FPS!\r\n");
}

void tmxc_game_mode_disable_vsync_bypass(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] VSync bypass kapatılıyor...\r\n");
    
    tmxc_game_mode.vsync_bypass_enabled = 0;
    
    tmxc_uart_puts("[GAME-MODE] VSync aktif\r\n");
}

void tmxc_game_mode_enable_direct_rendering(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] Direct Rendering aktifleştiriliyor...\r\n");
    
    tmxc_game_mode.direct_rendering_enabled = 1;
    
    tmxc_uart_puts("[GAME-MODE] Direct Rendering aktif - Minimum gecikme!\r\n");
}

void tmxc_game_mode_disable_direct_rendering(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] Direct Rendering kapatılıyor...\r\n");
    
    tmxc_game_mode.direct_rendering_enabled = 0;
    
    tmxc_uart_puts("[GAME-MODE] Direct Rendering kapatıldı\r\n");
}

void tmxc_thermal_monitor(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_game_mode.current_temperature = tmxc_dvfs_get_cpu_temperature();
    
    if (tmxc_game_mode.game_mode_active && tmxc_game_mode.current_temperature >= tmxc_game_mode.thermal_threshold) {
        tmxc_uart_puts("[GAME-MODE] KRİTİK SICAKLIK: ");
        char buffer[21];
        int pos = 20;
        buffer[pos] = '\0';
        uint64_t temp = tmxc_game_mode.current_temperature;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts("°C - Safe Mode aktifleştiriliyor!\r\n");
        
        tmxc_game_mode_enter_safe_mode();
    }
    
    if (tmxc_game_mode.thermal_safe_mode && tmxc_game_mode.current_temperature < (tmxc_game_mode.thermal_threshold - 10)) {
        tmxc_uart_puts("[GAME-MODE] Sıcaklık normal - Safe Mode kapatılıyor\r\n");
        tmxc_game_mode_exit_safe_mode();
    }
}

void tmxc_game_mode_enter_safe_mode(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] Safe Mode'a geçiliyor...\r\n");
    
    tmxc_game_mode.thermal_safe_mode = 1;
    
    uint32_t game_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_GAME_MODE_BASE + TMXC_GAME_MODE_CTRL));
    game_ctrl |= TMXC_GAME_MODE_CMD_SAFE;
    tmxc_write32((volatile uint32_t*)(TMXC_GAME_MODE_BASE + TMXC_GAME_MODE_CTRL), game_ctrl);
    
    tmxc_dvfs_set_performance_level(0);
    tmxc_dvfs_set_frequency(600);
    tmxc_dvfs_set_voltage(900);
    
    tmxc_uart_puts("[GAME-MODE] Safe Mode aktif - Frekanslar düşürüldü\r\n");
}

void tmxc_game_mode_exit_safe_mode(void) {
    if (!tmxc_game_mode.initialized) {
        return;
    }
    
    tmxc_uart_puts("[GAME-MODE] Safe Mode'dan çıkılıyor...\r\n");
    
    tmxc_game_mode.thermal_safe_mode = 0;
    
    uint32_t game_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_GAME_MODE_BASE + TMXC_GAME_MODE_CTRL));
    game_ctrl &= ~TMXC_GAME_MODE_CMD_SAFE;
    tmxc_write32((volatile uint32_t*)(TMXC_GAME_MODE_BASE + TMXC_GAME_MODE_CTRL), game_ctrl);
    
    if (tmxc_game_mode.game_mode_active) {
        tmxc_game_mode_override_dvfs();
    }
    
    tmxc_uart_puts("[GAME-MODE] Safe Mode'dan çıkıldı\r\n");
}

uint8_t tmxc_game_mode_is_active(void) {
    return tmxc_game_mode.game_mode_active;
}

uint8_t tmxc_game_mode_is_safe_mode(void) {
    return tmxc_game_mode.thermal_safe_mode;
}

uint32_t tmxc_game_mode_get_cpu_allocation(void) {
    return tmxc_game_mode.cpu_allocation_percent;
}

uint32_t tmxc_game_mode_get_temperature(void) {
    return tmxc_game_mode.current_temperature;
}

uint8_t tmxc_game_mode_is_vsync_bypass_enabled(void) {
    return tmxc_game_mode.vsync_bypass_enabled;
}

uint8_t tmxc_game_mode_is_direct_rendering_enabled(void) {
    return tmxc_game_mode.direct_rendering_enabled;
}
