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

#define TMXC_ACTION_BUTTON_GPIO 45
#define TMXC_MICROPHONE_POWER_GPIO 50
#define TMXC_CAMERA_POWER_GPIO 51
#define TMXC_GPS_POWER_GPIO 52
#define TMXC_MODEM_POWER_GPIO 53

typedef struct {
    uint8_t hardware_disconnect_enabled;
    uint8_t microphone_active;
    uint8_t camera_active;
    uint8_t gps_active;
    uint8_t modem_active;
    uint64_t last_trigger_time;
    uint32_t trigger_count;
    uint8_t emergency_mode;
} tmxc_physical_key_kill_t;

static tmxc_physical_key_kill_t tmxc_key_kill;

static void tmxc_hardware_disconnect_microphone(uint8_t disconnect) {
    uint32_t gpio_value = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE + TMXC_MICROPHONE_POWER_GPIO);
    
    if (disconnect) {
        gpio_value &= ~(1 << TMXC_MICROPHONE_POWER_GPIO);
        tmxc_key_kill.microphone_active = 0;
    } else {
        gpio_value |= (1 << TMXC_MICROPHONE_POWER_GPIO);
        tmxc_key_kill.microphone_active = 1;
    }
    
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_BASE + TMXC_MICROPHONE_POWER_GPIO, gpio_value);
}

static void tmxc_hardware_disconnect_camera(uint8_t disconnect) {
    uint32_t gpio_value = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE + TMXC_CAMERA_POWER_GPIO);
    
    if (disconnect) {
        gpio_value &= ~(1 << TMXC_CAMERA_POWER_GPIO);
        tmxc_key_kill.camera_active = 0;
    } else {
        gpio_value |= (1 << TMXC_CAMERA_POWER_GPIO);
        tmxc_key_kill.camera_active = 1;
    }
    
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_BASE + TMXC_CAMERA_POWER_GPIO, gpio_value);
}

static void tmxc_hardware_disconnect_gps(uint8_t disconnect) {
    uint32_t gpio_value = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE + TMXC_GPS_POWER_GPIO);
    
    if (disconnect) {
        gpio_value &= ~(1 << TMXC_GPS_POWER_GPIO);
        tmxc_key_kill.gps_active = 0;
    } else {
        gpio_value |= (1 << TMXC_GPS_POWER_GPIO);
        tmxc_key_kill.gps_active = 1;
    }
    
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_BASE + TMXC_GPS_POWER_GPIO, gpio_value);
}

static void tmxc_hardware_disconnect_modem(uint8_t disconnect) {
    uint32_t gpio_value = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE + TMXC_MODEM_POWER_GPIO);
    
    if (disconnect) {
        gpio_value &= ~(1 << TMXC_MODEM_POWER_GPIO);
        tmxc_key_kill.modem_active = 0;
    } else {
        gpio_value |= (1 << TMXC_MODEM_POWER_GPIO);
        tmxc_key_kill.modem_active = 1;
    }
    
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_BASE + TMXC_MODEM_POWER_GPIO, gpio_value);
}

void tmxc_physical_key_kill_init(void) {
    tmxc_key_kill.hardware_disconnect_enabled = 1;
    tmxc_key_kill.microphone_active = 1;
    tmxc_key_kill.camera_active = 1;
    tmxc_key_kill.gps_active = 1;
    tmxc_key_kill.modem_active = 1;
    tmxc_key_kill.last_trigger_time = 0;
    tmxc_key_kill.trigger_count = 0;
    tmxc_key_kill.emergency_mode = 0;
    
    uint32_t gpio_ctrl = tmxc_read32((volatile uint32_t*)TMXC_GPIO_CTRL);
    gpio_ctrl |= (1 << TMXC_ACTION_BUTTON_GPIO);
    gpio_ctrl |= (1 << TMXC_MICROPHONE_POWER_GPIO);
    gpio_ctrl |= (1 << TMXC_CAMERA_POWER_GPIO);
    gpio_ctrl |= (1 << TMXC_GPS_POWER_GPIO);
    gpio_ctrl |= (1 << TMXC_MODEM_POWER_GPIO);
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_CTRL, gpio_ctrl);
    
    tmxc_uart_puts("[KEY-KILL] Physical Key-Kill initialized\r\n");
}

void tmxc_physical_key_kill_trigger(void) {
    if (!tmxc_key_kill.hardware_disconnect_enabled) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t time_since_last = current_time - tmxc_key_kill.last_trigger_time;
    
    if (time_since_last < 1000000000ULL) {
        return;
    }
    
    tmxc_key_kill.last_trigger_time = current_time;
    tmxc_key_kill.trigger_count++;
    tmxc_key_kill.emergency_mode = 1;
    
    tmxc_uart_puts("[KEY-KILL] EMERGENCY HARDWARE DISCONNECT TRIGGERED\r\n");
    
    tmxc_hardware_disconnect_microphone(1);
    tmxc_hardware_disconnect_camera(1);
    tmxc_hardware_disconnect_gps(1);
    tmxc_hardware_disconnect_modem(1);
    
    tmxc_uart_puts("[KEY-KILL] All sensors disconnected at hardware level\r\n");
}

void tmxc_physical_key_kill_restore(void) {
    if (!tmxc_key_kill.emergency_mode) {
        return;
    }
    
    tmxc_uart_puts("[KEY-KILL] Restoring hardware connections...\r\n");
    
    tmxc_hardware_disconnect_microphone(0);
    tmxc_hardware_disconnect_camera(0);
    tmxc_hardware_disconnect_gps(0);
    tmxc_hardware_disconnect_modem(0);
    
    tmxc_key_kill.emergency_mode = 0;
    
    tmxc_uart_puts("[KEY-KILL] Hardware connections restored\r\n");
}

void tmxc_physical_key_kill_enable(uint8_t enable) {
    tmxc_key_kill.hardware_disconnect_enabled = enable;
    tmxc_uart_puts("[KEY-KILL] Physical Key-Kill ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

uint8_t tmxc_physical_key_kill_is_enabled(void) {
    return tmxc_key_kill.hardware_disconnect_enabled;
}

uint8_t tmxc_physical_key_kill_is_emergency_mode(void) {
    return tmxc_key_kill.emergency_mode;
}

uint8_t tmxc_physical_key_kill_microphone_status(void) {
    return tmxc_key_kill.microphone_active;
}

uint8_t tmxc_physical_key_kill_camera_status(void) {
    return tmxc_key_kill.camera_active;
}

uint8_t tmxc_physical_key_kill_gps_status(void) {
    return tmxc_key_kill.gps_active;
}

uint8_t tmxc_physical_key_kill_modem_status(void) {
    return tmxc_key_kill.modem_active;
}

uint32_t tmxc_physical_key_kill_get_trigger_count(void) {
    return tmxc_key_kill.trigger_count;
}

void tmxc_physical_key_kill_check_button(void) {
    if (!tmxc_key_kill.hardware_disconnect_enabled) {
        return;
    }
    
    uint32_t gpio_value = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE + TMXC_ACTION_BUTTON_GPIO);
    
    if (gpio_value & (1 << TMXC_ACTION_BUTTON_GPIO)) {
        tmxc_physical_key_kill_trigger();
    }
}
