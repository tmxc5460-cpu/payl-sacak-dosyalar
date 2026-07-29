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
    uint8_t dvfs_enabled;
    uint32_t cpu_temperature;
    uint32_t current_frequency;
    uint32_t current_voltage;
    uint32_t target_frequency;
    uint32_t target_voltage;
    uint32_t thermal_threshold_high;
    uint32_t thermal_threshold_critical;
    uint8_t thermal_throttling_active;
    uint32_t performance_level;
} tmxc_dvfs_t;

static tmxc_dvfs_t tmxc_dvfs;

#define TMXC_DVFS_BASE 0xF0000000
#define TMXC_DVFS_CTRL 0x00
#define TMXC_DVFS_STATUS 0x04
#define TMXC_DVFS_FREQ 0x08
#define TMXC_DVFS_VOLT 0x0C
#define TMXC_DVFS_TEMP 0x10

#define TMXC_DVFS_CMD_ENABLE 0x01
#define TMXC_DVFS_CMD_DISABLE 0x02
#define TMXC_DVFS_CMD_SET_FREQ 0x03
#define TMXC_DVFS_CMD_SET_VOLT 0x04

void tmxc_dvfs_init(void) {
    tmxc_uart_puts("[DVFS] Initializing DVFS module...\r\n");
    
    tmxc_dvfs.initialized = 0;
    tmxc_dvfs.dvfs_enabled = 0;
    tmxc_dvfs.cpu_temperature = 0;
    tmxc_dvfs.current_frequency = 0;
    tmxc_dvfs.current_voltage = 0;
    tmxc_dvfs.target_frequency = 2400;
    tmxc_dvfs.target_voltage = 1100;
    tmxc_dvfs.thermal_threshold_high = 75;
    tmxc_dvfs.thermal_threshold_critical = 85;
    tmxc_dvfs.thermal_throttling_active = 0;
    tmxc_dvfs.performance_level = 3;
    
    uint32_t dvfs_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_CTRL));
    dvfs_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_CTRL), dvfs_ctrl);
    
    tmxc_dvfs.cpu_temperature = tmxc_read32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_TEMP));
    tmxc_dvfs.current_frequency = tmxc_read32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_FREQ));
    tmxc_dvfs.current_voltage = tmxc_read32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_VOLT));
    
    tmxc_dvfs.initialized = 1;
    tmxc_uart_puts("[DVFS] DVFS module initialized\r\n");
}

void tmxc_dvfs_enable(void) {
    if (!tmxc_dvfs.initialized) {
        return;
    }
    
    tmxc_uart_puts("[DVFS] Enabling DVFS...\r\n");
    
    uint32_t dvfs_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_CTRL));
    dvfs_ctrl |= TMXC_DVFS_CMD_ENABLE;
    tmxc_write32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_CTRL), dvfs_ctrl);
    
    tmxc_dvfs.dvfs_enabled = 1;
    
    tmxc_dvfs_set_frequency(tmxc_dvfs.target_frequency);
    tmxc_dvfs_set_voltage(tmxc_dvfs.target_voltage);
    
    tmxc_uart_puts("[DVFS] DVFS enabled\r\n");
}

void tmxc_dvfs_disable(void) {
    if (!tmxc_dvfs.initialized) {
        return;
    }
    
    uint32_t dvfs_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_CTRL));
    dvfs_ctrl |= TMXC_DVFS_CMD_DISABLE;
    tmxc_write32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_CTRL), dvfs_ctrl);
    
    tmxc_dvfs.dvfs_enabled = 0;
    
    tmxc_uart_puts("[DVFS] DVFS disabled\r\n");
}

void tmxc_dvfs_set_frequency(uint32_t frequency_mhz) {
    if (!tmxc_dvfs.initialized || !tmxc_dvfs.dvfs_enabled) {
        return;
    }
    
    if (frequency_mhz < 300) frequency_mhz = 300;
    if (frequency_mhz > 3000) frequency_mhz = 3000;
    
    tmxc_dvfs.target_frequency = frequency_mhz;
    
    uint32_t dvfs_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_CTRL));
    dvfs_ctrl |= TMXC_DVFS_CMD_SET_FREQ;
    tmxc_write32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_CTRL), dvfs_ctrl);
    
    tmxc_write32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_FREQ), frequency_mhz);
    
    tmxc_dvfs.current_frequency = frequency_mhz;
}

void tmxc_dvfs_set_voltage(uint32_t voltage_mv) {
    if (!tmxc_dvfs.initialized || !tmxc_dvfs.dvfs_enabled) {
        return;
    }
    
    if (voltage_mv < 800) voltage_mv = 800;
    if (voltage_mv > 1300) voltage_mv = 1300;
    
    tmxc_dvfs.target_voltage = voltage_mv;
    
    uint32_t dvfs_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_CTRL));
    dvfs_ctrl |= TMXC_DVFS_CMD_SET_VOLT;
    tmxc_write32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_CTRL), dvfs_ctrl);
    
    tmxc_write32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_VOLT), voltage_mv);
    
    tmxc_dvfs.current_voltage = voltage_mv;
}

void tmxc_dvfs_update_temperature(void) {
    if (!tmxc_dvfs.initialized) {
        return;
    }
    
    tmxc_dvfs.cpu_temperature = tmxc_read32((volatile uint32_t*)(TMXC_DVFS_BASE + TMXC_DVFS_TEMP));
}

void tmxc_dvfs_cool_down(void) {
    if (!tmxc_dvfs.initialized || !tmxc_dvfs.dvfs_enabled) {
        return;
    }
    
    tmxc_dvfs_update_temperature();
    
    if (tmxc_dvfs.cpu_temperature >= tmxc_dvfs.thermal_threshold_critical) {
        tmxc_uart_puts("[DVFS] Critical temperature detected - emergency cool down\r\n");
        
        tmxc_dvfs_set_frequency(600);
        tmxc_dvfs_set_voltage(900);
        
        tmxc_dvfs.thermal_throttling_active = 1;
        tmxc_dvfs.performance_level = 0;
        
    } else if (tmxc_dvfs.cpu_temperature >= tmxc_dvfs.thermal_threshold_high) {
        tmxc_uart_puts("[DVFS] High temperature detected - throttling\r\n");
        
        tmxc_dvfs_set_frequency(1200);
        tmxc_dvfs_set_voltage(1000);
        
        tmxc_dvfs.thermal_throttling_active = 1;
        tmxc_dvfs.performance_level = 1;
        
    } else if (tmxc_dvfs.thermal_throttling_active && tmxc_dvfs.cpu_temperature < tmxc_dvfs.thermal_threshold_high - 10) {
        tmxc_uart_puts("[DVFS] Temperature normal - restoring performance\r\n");
        
        tmxc_dvfs_set_frequency(tmxc_dvfs.target_frequency);
        tmxc_dvfs_set_voltage(tmxc_dvfs.target_voltage);
        
        tmxc_dvfs.thermal_throttling_active = 0;
        tmxc_dvfs.performance_level = 3;
    }
}

void tmxc_dvfs_set_performance_level(uint32_t level) {
    if (!tmxc_dvfs.initialized || !tmxc_dvfs.dvfs_enabled) {
        return;
    }
    
    if (level > 3) level = 3;
    
    tmxc_dvfs.performance_level = level;
    
    switch (level) {
        case 0:
            tmxc_dvfs_set_frequency(600);
            tmxc_dvfs_set_voltage(900);
            break;
        case 1:
            tmxc_dvfs_set_frequency(1200);
            tmxc_dvfs_set_voltage(1000);
            break;
        case 2:
            tmxc_dvfs_set_frequency(1800);
            tmxc_dvfs_set_voltage(1050);
            break;
        case 3:
            tmxc_dvfs_set_frequency(2400);
            tmxc_dvfs_set_voltage(1100);
            break;
    }
}

void tmxc_dvfs_set_thermal_thresholds(uint32_t high, uint32_t critical) {
    tmxc_dvfs.thermal_threshold_high = high;
    tmxc_dvfs.thermal_threshold_critical = critical;
}

uint32_t tmxc_dvfs_get_cpu_temperature(void) {
    return tmxc_dvfs.cpu_temperature;
}

uint32_t tmxc_dvfs_get_current_frequency(void) {
    return tmxc_dvfs.current_frequency;
}

uint32_t tmxc_dvfs_get_current_voltage(void) {
    return tmxc_dvfs.current_voltage;
}

uint8_t tmxc_dvfs_is_thermal_throttling_active(void) {
    return tmxc_dvfs.thermal_throttling_active;
}

uint32_t tmxc_dvfs_get_performance_level(void) {
    return tmxc_dvfs.performance_level;
}

uint8_t tmxc_dvfs_is_enabled(void) {
    return tmxc_dvfs.dvfs_enabled;
}
