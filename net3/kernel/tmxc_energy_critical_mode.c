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

#define TMXC_ENERGY_CRITICAL_THRESHOLD 1
#define TMXC_ENERGY_WARNING_THRESHOLD 5
#define TMXC_ENERGY_TARGET_HOURS 24
#define TMXC_ENERGY_TEXT_BUFFER_SIZE 4096
#define TMXC_ENERGY_MAX_MESSAGES 64

typedef enum {
    TMXC_ENERGY_MODE_NORMAL = 0,
    TMXC_ENERGY_MODE_POWER_SAVE = 1,
    TMXC_ENERGY_MODE_CRITICAL = 2
} tmxc_energy_mode_t;

typedef struct {
    uint8_t message[TMXC_ENERGY_TEXT_BUFFER_SIZE];
    uint32_t size;
    uint64_t timestamp;
    uint8_t is_from_ai;
} tmxc_text_message_t;

typedef struct {
    tmxc_energy_mode_t current_mode;
    uint8_t battery_level;
    uint64_t mode_switch_time;
    uint64_t critical_mode_start_time;
    uint32_t hours_remaining;
    uint8_t gui_disabled;
    uint8_t text_only_mode;
    uint8_t ai_text_only;
    uint32_t cpu_frequency;
    uint32_t screen_brightness;
    uint8_t network_disabled;
    uint8_t bluetooth_disabled;
    uint8_t wifi_disabled;
    uint8_t sensors_disabled;
    tmxc_text_message_t messages[TMXC_ENERGY_MAX_MESSAGES];
    uint32_t message_count;
} tmxc_energy_critical_t;

static tmxc_energy_critical_t tmxc_energy;
static uint8_t tmxc_energy_critical_enabled = 1;

void tmxc_energy_critical_mode_init(void) {
    tmxc_energy.current_mode = TMXC_ENERGY_MODE_NORMAL;
    tmxc_energy.battery_level = 100;
    tmxc_energy.mode_switch_time = 0;
    tmxc_energy.critical_mode_start_time = 0;
    tmxc_energy.hours_remaining = 0;
    tmxc_energy.gui_disabled = 0;
    tmxc_energy.text_only_mode = 0;
    tmxc_energy.ai_text_only = 0;
    tmxc_energy.cpu_frequency = 2400000;
    tmxc_energy.screen_brightness = 100;
    tmxc_energy.network_disabled = 0;
    tmxc_energy.bluetooth_disabled = 0;
    tmxc_energy.wifi_disabled = 0;
    tmxc_energy.sensors_disabled = 0;
    tmxc_energy.message_count = 0;
    
    for (uint32_t i = 0; i < TMXC_ENERGY_MAX_MESSAGES; i++) {
        tmxc_energy.messages[i].size = 0;
        tmxc_energy.messages[i].timestamp = 0;
        tmxc_energy.messages[i].is_from_ai = 0;
        for (uint32_t j = 0; j < TMXC_ENERGY_TEXT_BUFFER_SIZE; j++) {
            tmxc_energy.messages[i].message[j] = 0;
        }
    }
    
    tmxc_uart_puts("[ENERGY] Energy Critical Mode initialized\r\n");
}

void tmxc_energy_check_battery(void) {
    if (!tmxc_energy_critical_enabled) {
        return;
    }
    
    volatile uint64_t* pmu_reg = (volatile uint64_t*)TMXC_PMU_BASE;
    uint8_t battery_level = (uint8_t)(*pmu_reg & 0xFF);
    
    tmxc_energy.battery_level = battery_level;
    
    if (battery_level <= TMXC_ENERGY_CRITICAL_THRESHOLD && 
        tmxc_energy.current_mode != TMXC_ENERGY_MODE_CRITICAL) {
        tmxc_energy_enter_critical_mode();
    } else if (battery_level > TMXC_ENERGY_WARNING_THRESHOLD && 
               tmxc_energy.current_mode == TMXC_ENERGY_MODE_CRITICAL) {
        tmxc_energy_exit_critical_mode();
    }
}

void tmxc_energy_enter_critical_mode(void) {
    tmxc_uart_puts("[ENERGY] Entering Critical Mode...\r\n");
    
    tmxc_energy.current_mode = TMXC_ENERGY_MODE_CRITICAL;
    tmxc_energy.mode_switch_time = tmxc_get_cycle_count();
    tmxc_energy.critical_mode_start_time = tmxc_get_cycle_count();
    tmxc_energy.gui_disabled = 1;
    tmxc_energy.text_only_mode = 1;
    tmxc_energy.ai_text_only = 1;
    
    tmxc_energy_disable_gui();
    tmxc_energy_reduce_cpu_frequency();
    tmxc_energy_disable_screen();
    tmxc_energy_disable_peripherals();
    tmxc_energy_calculate_remaining_time();
    
    tmxc_uart_puts("[ENERGY] Critical Mode active - Text Only\r\n");
}

void tmxc_energy_exit_critical_mode(void) {
    tmxc_uart_puts("[ENERGY] Exiting Critical Mode...\r\n");
    
    tmxc_energy.current_mode = TMXC_ENERGY_MODE_NORMAL;
    tmxc_energy.gui_disabled = 0;
    tmxc_energy.text_only_mode = 0;
    tmxc_energy.ai_text_only = 0;
    
    tmxc_energy_enable_gui();
    tmxc_energy_restore_cpu_frequency();
    tmxc_energy_enable_screen();
    tmxc_energy_enable_peripherals();
    
    tmxc_uart_puts("[ENERGY] Normal Mode restored\r\n");
}

void tmxc_energy_disable_gui(void) {
    tmxc_uart_puts("[ENERGY] Disabling GUI...\r\n");
    
    volatile uint64_t* display_reg = (volatile uint64_t*)TMXC_DISPLAY_BASE;
    *display_reg = 0x0;
    
    tmxc_energy.gui_disabled = 1;
}

void tmxc_energy_enable_gui(void) {
    tmxc_uart_puts("[ENERGY] Enabling GUI...\r\n");
    
    volatile uint64_t* display_reg = (volatile uint64_t*)TMXC_DISPLAY_BASE;
    *display_reg = 0x1;
    
    tmxc_energy.gui_disabled = 0;
}

void tmxc_energy_reduce_cpu_frequency(void) {
    tmxc_uart_puts("[ENERGY] Reducing CPU frequency...\r\n");
    
    tmxc_energy.cpu_frequency = 600000;
    
    for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
        uint64_t clk_reg = TMXC_CLOCK_CPU_CLK + (i * 0x100);
        volatile uint64_t* clk_ctrl = (volatile uint64_t*)clk_reg;
        *clk_ctrl = tmxc_energy.cpu_frequency;
    }
}

void tmxc_energy_restore_cpu_frequency(void) {
    tmxc_uart_puts("[ENERGY] Restoring CPU frequency...\r\n");
    
    tmxc_energy.cpu_frequency = 2400000;
    
    for (uint32_t i = 0; i < TMXC_MAX_CPUS; i++) {
        uint64_t clk_reg = TMXC_CLOCK_CPU_CLK + (i * 0x100);
        volatile uint64_t* clk_ctrl = (volatile uint64_t*)clk_reg;
        *clk_ctrl = tmxc_energy.cpu_frequency;
    }
}

void tmxc_energy_disable_screen(void) {
    tmxc_uart_puts("[ENERGY] Disabling screen...\r\n");
    
    tmxc_energy.screen_brightness = 0;
    
    volatile uint64_t* backlight_reg = (volatile uint64_t*)(TMXC_DISPLAY_BASE + 0x1000);
    *backlight_reg = 0x0;
}

void tmxc_energy_enable_screen(void) {
    tmxc_uart_puts("[ENERGY] Enabling screen...\r\n");
    
    tmxc_energy.screen_brightness = 100;
    
    volatile uint64_t* backlight_reg = (volatile uint64_t*)(TMXC_DISPLAY_BASE + 0x1000);
    *backlight_reg = 0x64;
}

void tmxc_energy_disable_peripherals(void) {
    tmxc_uart_puts("[ENERGY] Disabling peripherals...\r\n");
    
    tmxc_energy.network_disabled = 1;
    tmxc_energy.bluetooth_disabled = 1;
    tmxc_energy.wifi_disabled = 1;
    tmxc_energy.sensors_disabled = 1;
    
    volatile uint64_t* network_reg = (volatile uint64_t*)TMXC_NETWORK_BASE;
    *network_reg = 0x0;
    
    volatile uint64_t* bluetooth_reg = (volatile uint64_t*)TMXC_BLUETOOTH_BASE;
    *bluetooth_reg = 0x0;
    
    volatile uint64_t* sensor_reg = (volatile uint64_t*)TMXC_SENSOR_BASE;
    *sensor_reg = 0x0;
}

void tmxc_energy_enable_peripherals(void) {
    tmxc_uart_puts("[ENERGY] Enabling peripherals...\r\n");
    
    tmxc_energy.network_disabled = 0;
    tmxc_energy.bluetooth_disabled = 0;
    tmxc_energy.wifi_disabled = 0;
    tmxc_energy.sensors_disabled = 0;
    
    volatile uint64_t* network_reg = (volatile uint64_t*)TMXC_NETWORK_BASE;
    *network_reg = 0x1;
    
    volatile uint64_t* bluetooth_reg = (volatile uint64_t*)TMXC_BLUETOOTH_BASE;
    *bluetooth_reg = 0x1;
    
    volatile uint64_t* sensor_reg = (volatile uint64_t*)TMXC_SENSOR_BASE;
    *sensor_reg = 0x1;
}

void tmxc_energy_calculate_remaining_time(void) {
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed_cycles = current_time - tmxc_energy.critical_mode_start_time;
    uint64_t frequency = tmxc_get_frequency();
    uint64_t elapsed_hours = (elapsed_cycles / frequency) / 3600;
    
    tmxc_energy.hours_remaining = TMXC_ENERGY_TARGET_HOURS - (uint32_t)elapsed_hours;
    
    if (elapsed_hours > TMXC_ENERGY_TARGET_HOURS) {
        tmxc_energy.hours_remaining = 0;
    }
    
    tmxc_uart_puts("[ENERGY] Estimated remaining time: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = tmxc_energy.hours_remaining;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" hours\r\n");
}

void tmxc_energy_add_text_message(const char* message, uint8_t is_from_ai) {
    if (!tmxc_energy.text_only_mode) {
        return;
    }
    
    if (tmxc_energy.message_count >= TMXC_ENERGY_MAX_MESSAGES) {
        return;
    }
    
    uint32_t index = tmxc_energy.message_count;
    
    for (uint32_t i = 0; i < TMXC_ENERGY_TEXT_BUFFER_SIZE - 1 && message[i] != '\0'; i++) {
        tmxc_energy.messages[index].message[i] = message[i];
    }
    tmxc_energy.messages[index].message[TMXC_ENERGY_TEXT_BUFFER_SIZE - 1] = '\0';
    
    tmxc_energy.messages[index].size = 0;
    while (tmxc_energy.messages[index].message[tmxc_energy.messages[index].size] != '\0' && 
           tmxc_energy.messages[index].size < TMXC_ENERGY_TEXT_BUFFER_SIZE) {
        tmxc_energy.messages[index].size++;
    }
    
    tmxc_energy.messages[index].timestamp = tmxc_get_cycle_count();
    tmxc_energy.messages[index].is_from_ai = is_from_ai;
    
    tmxc_energy.message_count++;
    
    tmxc_uart_puts("[ENERGY] Text message: ");
    tmxc_uart_puts(message);
    tmxc_uart_puts("\r\n");
}

void tmxc_energy_display_text_messages(void) {
    if (!tmxc_energy.text_only_mode) {
        return;
    }
    
    tmxc_uart_puts("\r\n=== ENERGY CRITICAL MODE ===\r\n");
    tmxc_uart_puts("Battery: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = tmxc_energy.battery_level;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("%\r\n");
    
    tmxc_uart_puts("Remaining: ");
    pos = 20;
    buffer[pos] = '\0';
    temp = tmxc_energy.hours_remaining;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" hours\r\n");
    
    tmxc_uart_puts("=== MESSAGES ===\r\n");
    
    for (uint32_t i = 0; i < tmxc_energy.message_count; i++) {
        if (tmxc_energy.messages[i].is_from_ai) {
            tmxc_uart_puts("[AI] ");
        } else {
            tmxc_uart_puts("[SYS] ");
        }
        tmxc_uart_puts((char*)tmxc_energy.messages[i].message);
        tmxc_uart_puts("\r\n");
    }
    
    tmxc_uart_puts("========================\r\n\r\n");
}

void tmxc_energy_enable(uint8_t enable) {
    tmxc_energy_critical_enabled = enable;
    tmxc_uart_puts("[ENERGY] Energy Critical Mode ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

tmxc_energy_mode_t tmxc_energy_get_mode(void) {
    return tmxc_energy.current_mode;
}

uint8_t tmxc_energy_get_battery_level(void) {
    return tmxc_energy.battery_level;
}

uint8_t tmxc_energy_is_text_only_mode(void) {
    return tmxc_energy.text_only_mode;
}

tmxc_energy_critical_t* tmxc_energy_get_status(void) {
    return &tmxc_energy;
}

void tmxc_energy_cleanup(void) {
    if (tmxc_energy.current_mode == TMXC_ENERGY_MODE_CRITICAL) {
        tmxc_energy_exit_critical_mode();
    }
    
    tmxc_uart_puts("[ENERGY] Energy Critical Mode cleaned up\r\n");
}
