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

#define TMXC_RECOVERY_FIRMWARE_BASE 0x80000000
#define TMXC_RECOVERY_FIRMWARE_SIZE (16 * 1024 * 1024)
#define TMXC_RECOVERY_RAM_BASE 0x80000000
#define TMXC_RECOVERY_CHECKSUM_OFFSET (TMXC_RECOVERY_FIRMWARE_SIZE - 4)
#define TMXC_RECOVERY_MAGIC 0x52435652
#define TMXC_RECOVERY_TRIGGER_GPIO 42

typedef enum {
    TMXC_RECOVERY_STATE_IDLE = 0,
    TMXC_RECOVERY_STATE_DETECTING = 1,
    TMXC_RECOVERY_STATE_LOADING = 2,
    TMXC_RECOVERY_STATE_VERIFYING = 3,
    TMXC_RECOVERY_STATE_RESTORING = 4,
    TMXC_RECOVERY_STATE_COMPLETE = 5,
    TMXC_RECOVERY_STATE_ERROR = 6
} tmxc_recovery_state_t;

typedef struct {
    uint32_t magic;
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t version_patch;
    uint32_t firmware_size;
    uint32_t checksum;
    uint64_t build_timestamp;
    uint8_t is_valid;
} tmxc_recovery_header_t;

typedef struct {
    tmxc_recovery_state_t state;
    tmxc_recovery_header_t header;
    uint64_t trigger_time;
    uint64_t load_start_time;
    uint64_t load_end_time;
    uint64_t restore_start_time;
    uint64_t restore_end_time;
    uint32_t bytes_loaded;
    uint32_t bytes_restored;
    uint8_t trigger_detected;
    uint8_t recovery_enabled;
    uint8_t auto_recovery;
} tmxc_hardware_recovery_t;

static tmxc_hardware_recovery_t tmxc_recovery;
static uint8_t tmxc_recovery_enabled = 1;

static uint32_t tmxc_calculate_checksum(const uint8_t* data, uint64_t size) {
    uint32_t checksum = 0;
    for (uint64_t i = 0; i < size; i++) {
        checksum += data[i];
        checksum = (checksum << 1) | (checksum >> 31);
    }
    return checksum;
}

void tmxc_hardware_recovery_init(void) {
    tmxc_recovery.state = TMXC_RECOVERY_STATE_IDLE;
    tmxc_recovery.trigger_time = 0;
    tmxc_recovery.load_start_time = 0;
    tmxc_recovery.load_end_time = 0;
    tmxc_recovery.restore_start_time = 0;
    tmxc_recovery.restore_end_time = 0;
    tmxc_recovery.bytes_loaded = 0;
    tmxc_recovery.bytes_restored = 0;
    tmxc_recovery.trigger_detected = 0;
    tmxc_recovery.recovery_enabled = 1;
    tmxc_recovery.auto_recovery = 1;
    
    tmxc_recovery.header.magic = 0;
    tmxc_recovery.header.version_major = 0;
    tmxc_recovery.header.version_minor = 0;
    tmxc_recovery.header.version_patch = 0;
    tmxc_recovery.header.firmware_size = 0;
    tmxc_recovery.header.checksum = 0;
    tmxc_recovery.header.build_timestamp = 0;
    tmxc_recovery.header.is_valid = 0;
    
    tmxc_uart_puts("[RECOVERY] Hardware Recovery Trigger initialized\r\n");
}

void tmxc_recovery_detect_trigger(void) {
    if (!tmxc_recovery_enabled) {
        return;
    }
    
    volatile uint64_t* gpio_reg = (volatile uint64_t*)(TMXC_GPIO_BASE + (TMXC_RECOVERY_TRIGGER_GPIO * 0x1000));
    uint64_t gpio_value = *gpio_reg;
    
    if ((gpio_value & 0x1) == 0x1) {
        tmxc_recovery.trigger_detected = 1;
        tmxc_recovery.trigger_time = tmxc_get_cycle_count();
        tmxc_recovery.state = TMXC_RECOVERY_STATE_DETECTING;
        
        tmxc_uart_puts("[RECOVERY] Hardware trigger detected\r\n");
        
        if (tmxc_recovery.auto_recovery) {
            tmxc_recovery_start();
        }
    }
}

void tmxc_recovery_start(void) {
    if (!tmxc_recovery_enabled || !tmxc_recovery.trigger_detected) {
        return;
    }
    
    tmxc_recovery.state = TMXC_RECOVERY_STATE_LOADING;
    tmxc_recovery.load_start_time = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[RECOVERY] Starting recovery process...\r\n");
    
    tmxc_recovery_read_firmware_header();
    
    if (tmxc_recovery.header.is_valid) {
        tmxc_recovery_load_firmware();
        tmxc_recovery_verify_firmware();
        
        if (tmxc_recovery.state != TMXC_RECOVERY_STATE_ERROR) {
            tmxc_recovery_restore_firmware();
        }
    } else {
        tmxc_uart_puts("[RECOVERY] Invalid firmware header\r\n");
        tmxc_recovery.state = TMXC_RECOVERY_STATE_ERROR;
    }
}

void tmxc_recovery_read_firmware_header(void) {
    tmxc_uart_puts("[RECOVERY] Reading firmware header...\r\n");
    
    volatile uint32_t* firmware_base = (volatile uint32_t*)TMXC_RECOVERY_FIRMWARE_BASE;
    
    tmxc_recovery.header.magic = firmware_base[0];
    tmxc_recovery.header.version_major = firmware_base[1];
    tmxc_recovery.header.version_minor = firmware_base[2];
    tmxc_recovery.header.version_patch = firmware_base[3];
    tmxc_recovery.header.firmware_size = firmware_base[4];
    tmxc_recovery.header.checksum = firmware_base[TMXC_RECOVERY_CHECKSUM_OFFSET / 4];
    tmxc_recovery.header.build_timestamp = ((uint64_t)firmware_base[6] << 32) | firmware_base[5];
    
    if (tmxc_recovery.header.magic == TMXC_RECOVERY_MAGIC) {
        tmxc_recovery.header.is_valid = 1;
        
        tmxc_uart_puts("[RECOVERY] Firmware version: ");
        char buffer[21];
        int pos = 20;
        buffer[pos] = '\0';
        uint64_t temp = tmxc_recovery.header.version_major;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts(".");
        pos = 20;
        buffer[pos] = '\0';
        temp = tmxc_recovery.header.version_minor;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts(".");
        pos = 20;
        buffer[pos] = '\0';
        temp = tmxc_recovery.header.version_patch;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts("\r\n");
    } else {
        tmxc_recovery.header.is_valid = 0;
        tmxc_uart_puts("[RECOVERY] Invalid magic number\r\n");
    }
}

void tmxc_recovery_load_firmware(void) {
    if (!tmxc_recovery.header.is_valid) {
        return;
    }
    
    tmxc_uart_puts("[RECOVERY] Loading firmware to RAM...\r\n");
    
    volatile uint8_t* firmware_base = (volatile uint8_t*)TMXC_RECOVERY_FIRMWARE_BASE;
    uint8_t* ram_base = (uint8_t*)tmxc_malloc(tmxc_recovery.header.firmware_size);
    
    if (ram_base == NULL) {
        tmxc_uart_puts("[RECOVERY] Failed to allocate RAM\r\n");
        tmxc_recovery.state = TMXC_RECOVERY_STATE_ERROR;
        return;
    }
    
    uint32_t chunk_size = 4096;
    for (uint32_t offset = 0; offset < tmxc_recovery.header.firmware_size; offset += chunk_size) {
        uint32_t current_chunk = (offset + chunk_size) > tmxc_recovery.header.firmware_size ? 
                                (tmxc_recovery.header.firmware_size - offset) : chunk_size;
        
        for (uint32_t i = 0; i < current_chunk; i++) {
            ram_base[offset + i] = firmware_base[offset + i];
        }
        
        tmxc_recovery.bytes_loaded += current_chunk;
    }
    
    tmxc_recovery.load_end_time = tmxc_get_cycle_count();
    
    uint64_t frequency = tmxc_get_frequency();
    uint64_t load_time_ms = ((tmxc_recovery.load_end_time - tmxc_recovery.load_start_time) * 1000) / frequency;
    
    tmxc_uart_puts("[RECOVERY] Loaded ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = tmxc_recovery.bytes_loaded;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" bytes in ");
    pos = 20;
    buffer[pos] = '\0';
    temp = load_time_ms;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" ms\r\n");
    
    tmxc_free(ram_base);
}

void tmxc_recovery_verify_firmware(void) {
    if (!tmxc_recovery.header.is_valid) {
        return;
    }
    
    tmxc_uart_puts("[RECOVERY] Verifying firmware checksum...\r\n");
    
    tmxc_recovery.state = TMXC_RECOVERY_STATE_VERIFYING;
    
    volatile uint8_t* firmware_base = (volatile uint8_t*)TMXC_RECOVERY_FIRMWARE_BASE;
    
    uint32_t calculated_checksum = tmxc_calculate_checksum(
        (uint8_t*)firmware_base,
        tmxc_recovery.header.firmware_size - 4
    );
    
    if (calculated_checksum == tmxc_recovery.header.checksum) {
        tmxc_uart_puts("[RECOVERY] Checksum valid\r\n");
    } else {
        tmxc_uart_puts("[RECOVERY] Checksum mismatch\r\n");
        tmxc_recovery.state = TMXC_RECOVERY_STATE_ERROR;
    }
}

void tmxc_recovery_restore_firmware(void) {
    if (tmxc_recovery.state == TMXC_RECOVERY_STATE_ERROR) {
        return;
    }
    
    tmxc_uart_puts("[RECOVERY] Restoring firmware...\r\n");
    
    tmxc_recovery.state = TMXC_RECOVERY_STATE_RESTORING;
    tmxc_recovery.restore_start_time = tmxc_get_cycle_count();
    
    volatile uint8_t* firmware_base = (volatile uint8_t*)TMXC_RECOVERY_FIRMWARE_BASE;
    uint8_t* ram_base = (uint8_t*)TMXC_RECOVERY_RAM_BASE;
    
    uint32_t chunk_size = 4096;
    for (uint32_t offset = 0; offset < tmxc_recovery.header.firmware_size; offset += chunk_size) {
        uint32_t current_chunk = (offset + chunk_size) > tmxc_recovery.header.firmware_size ? 
                                (tmxc_recovery.header.firmware_size - offset) : chunk_size;
        
        for (uint32_t i = 0; i < current_chunk; i++) {
            ram_base[offset + i] = firmware_base[offset + i];
        }
        
        tmxc_recovery.bytes_restored += current_chunk;
    }
    
    tmxc_recovery.restore_end_time = tmxc_get_cycle_count();
    
    uint64_t frequency = tmxc_get_frequency();
    uint64_t restore_time_ms = ((tmxc_recovery.restore_end_time - tmxc_recovery.restore_start_time) * 1000) / frequency;
    
    tmxc_uart_puts("[RECOVERY] Restored ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = tmxc_recovery.bytes_restored;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" bytes in ");
    pos = 20;
    buffer[pos] = '\0';
    temp = restore_time_ms;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" ms\r\n");
    
    tmxc_recovery.state = TMXC_RECOVERY_STATE_COMPLETE;
    tmxc_uart_puts("[RECOVERY] Recovery complete\r\n");
}

void tmxc_recovery_manual_trigger(void) {
    tmxc_recovery.trigger_detected = 1;
    tmxc_recovery.trigger_time = tmxc_get_cycle_count();
    tmxc_uart_puts("[RECOVERY] Manual trigger activated\r\n");
    tmxc_recovery_start();
}

void tmxc_recovery_enable(uint8_t enable) {
    tmxc_recovery_enabled = enable;
    tmxc_recovery.recovery_enabled = enable;
    tmxc_uart_puts("[RECOVERY] Hardware Recovery ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

void tmxc_recovery_set_auto_recovery(uint8_t enable) {
    tmxc_recovery.auto_recovery = enable;
    tmxc_uart_puts("[RECOVERY] Auto Recovery ");
    tmxc_uart_puts(enable ? "enabled" : "disabled");
    tmxc_uart_puts("\r\n");
}

tmxc_recovery_state_t tmxc_recovery_get_state(void) {
    return tmxc_recovery.state;
}

tmxc_recovery_header_t* tmxc_recovery_get_header(void) {
    return &tmxc_recovery.header;
}

uint8_t tmxc_recovery_is_trigger_detected(void) {
    return tmxc_recovery.trigger_detected;
}

void tmxc_recovery_reset_trigger(void) {
    tmxc_recovery.trigger_detected = 0;
    tmxc_recovery.state = TMXC_RECOVERY_STATE_IDLE;
    tmxc_uart_puts("[RECOVERY] Trigger reset\r\n");
}

void tmxc_hardware_recovery_cleanup(void) {
    tmxc_recovery.state = TMXC_RECOVERY_STATE_IDLE;
    tmxc_recovery.trigger_detected = 0;
    
    tmxc_uart_puts("[RECOVERY] Hardware Recovery cleaned up\r\n");
}
