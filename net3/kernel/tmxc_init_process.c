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
    uint8_t first_boot;
    uint8_t setup_required;
    uint8_t blocking_mode;
    uint32_t boot_sequence_stage;
    uint64_t boot_start_time;
} tmxc_init_process_t;

static tmxc_init_process_t tmxc_init_process;

#define TMXC_INIT_PROCESS_BASE 0xE6000000
#define TMXC_INIT_PROCESS_CTRL 0x00
#define TMXC_INIT_PROCESS_STATUS 0x04
#define TMXC_INIT_PROCESS_STAGE 0x08

#define TMXC_BOOT_STAGE_BOOTLOADER 0
#define TMXC_BOOT_STAGE_ANIMATION 1
#define TMXC_BOOT_STAGE_SETUP 2
#define TMXC_BOOT_STAGE_CALIBRATION 3
#define TMXC_BOOT_STAGE_COMPLETE 4

extern void tmxc_boot_animation_init(void);
extern void tmxc_boot_animation_start(void);
extern void tmxc_boot_animation_update(void);
extern uint8_t tmxc_boot_animation_is_active(void);
extern uint8_t tmxc_boot_animation_is_complete(void);

extern void tmxc_localization_init(void);
extern void tmxc_setup_wizard_init(void);
extern void tmxc_setup_wizard_start(void);
extern void tmxc_setup_wizard_render(void);
extern uint8_t tmxc_setup_wizard_is_active(void);
extern uint8_t tmxc_setup_wizard_is_blocking_mode(void);
extern uint8_t tmxc_setup_wizard_is_completed(void);
extern void tmxc_setup_wizard_complete_setup(void);

extern void tmxc_device_calibration_init(void);
extern void tmxc_device_calibration_start(void);
extern void tmxc_device_calibration_calibrate(void);
extern uint8_t tmxc_device_calibration_is_completed(void);

static uint8_t tmxc_check_first_boot(void) {
    uint32_t boot_flag = tmxc_read32((volatile uint32_t*)(TMXC_INIT_PROCESS_BASE + TMXC_INIT_PROCESS_STATUS));
    
    if (boot_flag == 0xFFFFFFFF) {
        return 1;
    }
    
    return 0;
}

static void tmxc_mark_boot_completed(void) {
    tmxc_write32((volatile uint32_t*)(TMXC_INIT_PROCESS_BASE + TMXC_INIT_PROCESS_STATUS), 0x4D544C58);
}

void tmxc_init_process_init(void) {
    tmxc_uart_puts("[INIT-PROCESS] Initializing TMXC init process...\r\n");
    
    tmxc_init_process.initialized = 0;
    tmxc_init_process.first_boot = 0;
    tmxc_init_process.setup_required = 0;
    tmxc_init_process.blocking_mode = 0;
    tmxc_init_process.boot_sequence_stage = TMXC_BOOT_STAGE_BOOTLOADER;
    tmxc_init_process.boot_start_time = 0;
    
    uint32_t init_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_INIT_PROCESS_BASE + TMXC_INIT_PROCESS_CTRL));
    init_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)(TMXC_INIT_PROCESS_BASE + TMXC_INIT_PROCESS_CTRL), init_ctrl);
    
    tmxc_init_process.first_boot = tmxc_check_first_boot();
    
    if (tmxc_init_process.first_boot) {
        tmxc_init_process.setup_required = 1;
        tmxc_init_process.blocking_mode = 1;
        tmxc_uart_puts("[INIT-PROCESS] First boot detected - setup required\r\n");
    } else {
        tmxc_uart_puts("[INIT-PROCESS] Returning user detected - skipping setup\r\n");
    }
    
    tmxc_init_process.initialized = 1;
    tmxc_uart_puts("[INIT-PROCESS] TMXC init process initialized\r\n");
}

void tmxc_init_process_start(void) {
    if (!tmxc_init_process.initialized) {
        return;
    }
    
    tmxc_uart_puts("[INIT-PROCESS] Starting TMXC boot sequence...\r\n");
    
    tmxc_init_process.boot_start_time = tmxc_get_cycle_count();
    tmxc_init_process.boot_sequence_stage = TMXC_BOOT_STAGE_BOOTLOADER;
    
    tmxc_boot_animation_init();
    tmxc_localization_init();
    tmxc_setup_wizard_init();
    tmxc_device_calibration_init();
    
    tmxc_boot_animation_start();
    tmxc_init_process.boot_sequence_stage = TMXC_BOOT_STAGE_ANIMATION;
}

void tmxc_init_process_update(void) {
    if (!tmxc_init_process.initialized) {
        return;
    }
    
    switch (tmxc_init_process.boot_sequence_stage) {
        case TMXC_BOOT_STAGE_ANIMATION:
            if (tmxc_boot_animation_is_active()) {
                tmxc_boot_animation_update();
            } else if (tmxc_boot_animation_is_complete()) {
                tmxc_uart_puts("[INIT-PROCESS] Boot animation complete\r\n");
                
                if (tmxc_init_process.setup_required) {
                    tmxc_setup_wizard_start();
                    tmxc_init_process.boot_sequence_stage = TMXC_BOOT_STAGE_SETUP;
                    tmxc_uart_puts("[INIT-PROCESS] Entering setup wizard\r\n");
                } else {
                    tmxc_device_calibration_start();
                    tmxc_device_calibration_calibrate();
                    tmxc_init_process.boot_sequence_stage = TMXC_BOOT_STAGE_COMPLETE;
                    tmxc_uart_puts("[INIT-PROCESS] Boot sequence complete\r\n");
                }
            }
            break;
            
        case TMXC_BOOT_STAGE_SETUP:
            if (tmxc_setup_wizard_is_active()) {
                tmxc_setup_wizard_render();
            } else if (tmxc_setup_wizard_is_completed()) {
                tmxc_uart_puts("[INIT-PROCESS] Setup wizard complete\r\n");
                
                tmxc_device_calibration_start();
                tmxc_device_calibration_calibrate();
                tmxc_init_process.boot_sequence_stage = TMXC_BOOT_STAGE_CALIBRATION;
                tmxc_uart_puts("[INIT-PROCESS] Starting device calibration\r\n");
            }
            break;
            
        case TMXC_BOOT_STAGE_CALIBRATION:
            if (tmxc_device_calibration_is_completed()) {
                tmxc_uart_puts("[INIT-PROCESS] Device calibration complete\r\n");
                
                tmxc_init_process.boot_sequence_stage = TMXC_BOOT_STAGE_COMPLETE;
                tmxc_init_process.blocking_mode = 0;
                
                if (tmxc_init_process.first_boot) {
                    tmxc_mark_boot_completed();
                    tmxc_uart_puts("[INIT-PROCESS] First boot marked as complete\r\n");
                }
                
                uint64_t boot_duration = tmxc_get_cycle_count() - tmxc_init_process.boot_start_time;
                uint64_t boot_ms = (boot_duration * 1000) / tmxc_get_frequency();
                
                tmxc_uart_puts("[INIT-PROCESS] TMXC boot sequence completed in ");
                char buffer[21];
                int pos = 20;
                buffer[pos] = '\0';
                uint64_t temp = boot_ms;
                while (temp > 0 && pos > 0) {
                    pos--;
                    buffer[pos] = '0' + (temp % 10);
                    temp /= 10;
                }
                tmxc_uart_puts(&buffer[pos]);
                tmxc_uart_puts(" ms\r\n");
            }
            break;
            
        case TMXC_BOOT_STAGE_COMPLETE:
            break;
            
        default:
            break;
    }
}

uint8_t tmxc_init_process_is_blocking_mode(void) {
    return tmxc_init_process.blocking_mode;
}

uint8_t tmxc_init_process_is_first_boot(void) {
    return tmxc_init_process.first_boot;
}

uint8_t tmxc_init_process_is_setup_required(void) {
    return tmxc_init_process.setup_required;
}

uint8_t tmxc_init_process_is_boot_complete(void) {
    return tmxc_init_process.boot_sequence_stage == TMXC_BOOT_STAGE_COMPLETE;
}

void tmxc_init_process_force_setup(void) {
    if (!tmxc_init_process.initialized) {
        return;
    }
    
    tmxc_uart_puts("[INIT-PROCESS] Forcing setup wizard...\r\n");
    
    tmxc_init_process.setup_required = 1;
    tmxc_init_process.blocking_mode = 1;
    tmxc_init_process.boot_sequence_stage = TMXC_BOOT_STAGE_SETUP;
    
    tmxc_setup_wizard_start();
}

void tmxc_init_process_skip_setup(void) {
    if (!tmxc_init_process.initialized) {
        return;
    }
    
    tmxc_uart_puts("[INIT-PROCESS] Skipping setup wizard...\r\n");
    
    tmxc_init_process.setup_required = 0;
    tmxc_init_process.blocking_mode = 0;
    tmxc_init_process.boot_sequence_stage = TMXC_BOOT_STAGE_CALIBRATION;
    
    tmxc_device_calibration_start();
    tmxc_device_calibration_calibrate();
}

uint32_t tmxc_init_process_get_boot_stage(void) {
    return tmxc_init_process.boot_sequence_stage;
}
