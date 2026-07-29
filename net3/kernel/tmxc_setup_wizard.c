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
    uint8_t active;
    uint8_t current_step;
    uint8_t total_steps;
    uint8_t setup_completed;
    uint8_t blocking_mode;
    uint32_t selected_language_index;
    uint8_t ecosystem_sync_enabled;
    uint8_t security_calibration_completed;
    uint8_t network_stealth_mode;
    uint8_t device_calibration_completed;
    uint64_t setup_start_time;
} tmxc_setup_wizard_t;

static tmxc_setup_wizard_t tmxc_setup_wizard;

#define TMXC_SETUP_WIZARD_BASE 0xE4000000
#define TMXC_SETUP_WIZARD_CTRL 0x00
#define TMXC_SETUP_WIZARD_STATUS 0x04
#define TMXC_SETUP_WIZARD_STEP 0x08

#define TMXC_SETUP_STEP_LANGUAGE 0
#define TMXC_SETUP_STEP_ECOSYSTEM 1
#define TMXC_SETUP_STEP_SECURITY 2
#define TMXC_SETUP_STEP_NETWORK 3
#define TMXC_SETUP_STEP_CALIBRATION 4
#define TMXC_SETUP_STEP_COMPLETE 5

#define TMXC_NETWORK_MODE_STEALTH 0
#define TMXC_NETWORK_MODE_FULL_SPEED 1
#define TMXC_NETWORK_MODE_BALANCED 2

#define TMXC_COLOR_BG 0xFF1A1A2E
#define TMXC_COLOR_ACCENT 0xFFE94560
#define TMXC_COLOR_TEXT 0xFFFFFFFF
#define TMXC_COLOR_CARD 0xFF16213E
#define TMXC_COLOR_SUCCESS 0xFF4CAF50

extern const char* tmxc_localization_get_string(const char* key);
extern void tmxc_localization_set_language(const char* code);
extern tmxc_language_t* tmxc_localization_get_all_languages(void);
extern uint32_t tmxc_localization_get_language_count(void);

static void tmxc_setup_wizard_draw_header(void) {
    uint32_t header_height = 80;
    
    tmxc_graphics_rect(0, 0, 1280, header_height, TMXC_COLOR_BG);
    
    const char* title = tmxc_localization_get_string("setup");
    tmxc_graphics_text(50, 50, title, TMXC_COLOR_TEXT);
    
    uint32_t progress_width = (tmxc_setup_wizard.current_step * 100) / tmxc_setup_wizard.total_steps;
    tmxc_graphics_rect(0, header_height - 5, progress_width, 5, TMXC_COLOR_ACCENT);
}

static void tmxc_setup_wizard_draw_language_selection(void) {
    uint32_t y_offset = 120;
    
    const char* title = "Dil Seçimi / Language Selection";
    tmxc_graphics_text(50, y_offset, title, TMXC_COLOR_TEXT);
    y_offset += 50;
    
    tmxc_language_t* languages = tmxc_localization_get_all_languages();
    uint32_t lang_count = tmxc_localization_get_language_count();
    
    for (uint32_t i = 0; i < lang_count && i < 8; i++) {
        uint32_t card_y = y_offset + i * 70;
        
        uint32_t card_color = (i == tmxc_setup_wizard.selected_language_index) ? 
                             TMXC_COLOR_ACCENT : TMXC_COLOR_CARD;
        
        tmxc_graphics_rect(50, card_y, 1180, 60, card_color);
        
        uint32_t text_color = (i == tmxc_setup_wizard.selected_language_index) ? 
                             0xFF000000 : TMXC_COLOR_TEXT;
        
        tmxc_graphics_text(70, card_y + 35, languages[i].native_name, text_color);
        tmxc_graphics_text(400, card_y + 35, languages[i].name, text_color);
    }
}

static void tmxc_setup_wizard_draw_ecosystem_sync(void) {
    uint32_t y_offset = 120;
    
    const char* title = tmxc_localization_get_string("ecosystem");
    tmxc_graphics_text(50, y_offset, title, TMXC_COLOR_TEXT);
    y_offset += 50;
    
    const char* desc = tmxc_localization_get_string("ecosystem_desc");
    tmxc_graphics_text(50, y_offset, desc, TMXC_COLOR_TEXT);
    y_offset += 80;
    
    uint32_t sync_y = y_offset;
    uint32_t sync_color = tmxc_setup_wizard.ecosystem_sync_enabled ? 
                         TMXC_COLOR_SUCCESS : TMXC_COLOR_CARD;
    
    tmxc_graphics_rect(50, sync_y, 1180, 80, sync_color);
    
    const char* sync_text = tmxc_setup_wizard.ecosystem_sync_enabled ? 
                          "Ecosystem Sync: AKTİF" : "Ecosystem Sync: KAPALI";
    uint32_t sync_text_color = tmxc_setup_wizard.ecosystem_sync_enabled ? 
                              0xFF000000 : TMXC_COLOR_TEXT;
    
    tmxc_graphics_text(70, sync_y + 45, sync_text, sync_text_color);
}

static void tmxc_setup_wizard_draw_security_setup(void) {
    uint32_t y_offset = 120;
    
    const char* title = tmxc_localization_get_string("security");
    tmxc_graphics_text(50, y_offset, title, TMXC_COLOR_TEXT);
    y_offset += 50;
    
    const char* desc = tmxc_localization_get_string("security_desc");
    tmxc_graphics_text(50, y_offset, desc, TMXC_COLOR_TEXT);
    y_offset += 80;
    
    uint32_t faceid_y = y_offset;
    uint32_t faceid_color = TMXC_COLOR_CARD;
    
    tmxc_graphics_rect(50, faceid_y, 380, 100, faceid_color);
    tmxc_graphics_text(70, faceid_y + 30, "FaceID", TMXC_COLOR_TEXT);
    tmxc_graphics_text(70, faceid_y + 60, "Kalibrasyon Bekleniyor", 0xFFFFF200);
    
    uint32_t iris_y = y_offset;
    uint32_t iris_color = TMXC_COLOR_CARD;
    
    tmxc_graphics_rect(450, iris_y, 380, 100, iris_color);
    tmxc_graphics_text(470, iris_y + 30, "İris Tarama", TMXC_COLOR_TEXT);
    tmxc_graphics_text(470, iris_y + 60, "Kalibrasyon Bekleniyor", 0xFFFFF200);
    
    uint32_t heart_y = y_offset;
    uint32_t heart_color = TMXC_COLOR_CARD;
    
    tmxc_graphics_rect(850, heart_y, 380, 100, heart_color);
    tmxc_graphics_text(870, heart_y + 30, "Kalp Atışı", TMXC_COLOR_TEXT);
    tmxc_graphics_text(870, heart_y + 60, "Kalibrasyon Bekleniyor", 0xFFFFF200);
}

static void tmxc_setup_wizard_draw_network_stealth(void) {
    uint32_t y_offset = 120;
    
    const char* title = tmxc_localization_get_string("network");
    tmxc_graphics_text(50, y_offset, title, TMXC_COLOR_TEXT);
    y_offset += 50;
    
    const char* desc = tmxc_localization_get_string("network_desc");
    tmxc_graphics_text(50, y_offset, desc, TMXC_COLOR_TEXT);
    y_offset += 80;
    
    uint32_t stealth_y = y_offset;
    uint32_t stealth_color = (tmxc_setup_wizard.network_stealth_mode == TMXC_NETWORK_MODE_STEALTH) ? 
                            TMXC_COLOR_ACCENT : TMXC_COLOR_CARD;
    
    tmxc_graphics_rect(50, stealth_y, 380, 100, stealth_color);
    const char* stealth_text = tmxc_localization_get_string("stealth");
    uint32_t stealth_text_color = (tmxc_setup_wizard.network_stealth_mode == TMXC_NETWORK_MODE_STEALTH) ? 
                                 0xFF000000 : TMXC_COLOR_TEXT;
    tmxc_graphics_text(70, stealth_y + 50, stealth_text, stealth_text_color);
    
    uint32_t full_y = y_offset;
    uint32_t full_color = (tmxc_setup_wizard.network_stealth_mode == TMXC_NETWORK_MODE_FULL_SPEED) ? 
                         TMXC_COLOR_ACCENT : TMXC_COLOR_CARD;
    
    tmxc_graphics_rect(450, full_y, 380, 100, full_color);
    const char* full_text = tmxc_localization_get_string("full");
    uint32_t full_text_color = (tmxc_setup_wizard.network_stealth_mode == TMXC_NETWORK_MODE_FULL_SPEED) ? 
                              0xFF000000 : TMXC_COLOR_TEXT;
    tmxc_graphics_text(470, full_y + 50, full_text, full_text_color);
    
    uint32_t balanced_y = y_offset;
    uint32_t balanced_color = (tmxc_setup_wizard.network_stealth_mode == TMXC_NETWORK_MODE_BALANCED) ? 
                             TMXC_COLOR_ACCENT : TMXC_COLOR_CARD;
    
    tmxc_graphics_rect(850, balanced_y, 380, 100, balanced_color);
    const char* balanced_text = tmxc_localization_get_string("balanced");
    uint32_t balanced_text_color = (tmxc_setup_wizard.network_stealth_mode == TMXC_NETWORK_MODE_BALANCED) ? 
                                   0xFF000000 : TMXC_COLOR_TEXT;
    tmxc_graphics_text(870, balanced_y + 50, balanced_text, balanced_text_color);
}

static void tmxc_setup_wizard_draw_calibration(void) {
    uint32_t y_offset = 120;
    
    const char* title = tmxc_localization_get_string("calibration");
    tmxc_graphics_text(50, y_offset, title, TMXC_COLOR_TEXT);
    y_offset += 50;
    
    const char* desc = tmxc_localization_get_string("calibration_desc");
    tmxc_graphics_text(50, y_offset, desc, TMXC_COLOR_TEXT);
    y_offset += 80;
    
    tmxc_graphics_rect(50, y_offset, 1180, 200, TMXC_COLOR_CARD);
    
    tmxc_graphics_text(70, y_offset + 30, "Çevresel Koşullar Analiz Ediliyor...", TMXC_COLOR_TEXT);
    tmxc_graphics_text(70, y_offset + 70, "Işık Seviyesi:", TMXC_COLOR_TEXT);
    tmxc_graphics_text(70, y_offset + 110, "Ortam Sıcaklığı:", TMXC_COLOR_TEXT);
    tmxc_graphics_text(70, y_offset + 150, "Kullanım Alışkanlığı:", TMXC_COLOR_TEXT);
}

static void tmxc_setup_wizard_draw_complete(void) {
    uint32_t y_offset = 200;
    
    const char* title = tmxc_localization_get_string("complete");
    tmxc_graphics_text(50, y_offset, title, TMXC_COLOR_SUCCESS);
    y_offset += 80;
    
    const char* message = tmxc_localization_get_string("complete_message");
    tmxc_graphics_text(50, y_offset, message, TMXC_COLOR_TEXT);
    y_offset += 80;
    
    tmxc_graphics_rect(50, y_offset, 1180, 100, TMXC_COLOR_SUCCESS);
    tmxc_graphics_text(70, y_offset + 55, "TMXC İmzası Uygulanıyor...", 0xFF000000);
}

static void tmxc_setup_wizard_draw_buttons(void) {
    uint32_t button_y = 650;
    
    const char* next_text = tmxc_localization_get_string("next");
    const char* back_text = tmxc_localization_get_string("back");
    const char* skip_text = tmxc_localization_get_string("skip");
    
    if (tmxc_setup_wizard.current_step > 0 && 
        tmxc_setup_wizard.current_step < TMXC_SETUP_STEP_COMPLETE) {
        tmxc_graphics_rect(50, button_y, 200, 60, TMXC_COLOR_CARD);
        tmxc_graphics_text(70, button_y + 35, back_text, TMXC_COLOR_TEXT);
    }
    
    if (tmxc_setup_wizard.current_step < TMXC_SETUP_STEP_COMPLETE) {
        tmxc_graphics_rect(1030, button_y, 200, 60, TMXC_COLOR_ACCENT);
        tmxc_graphics_text(1050, button_y + 35, next_text, 0xFF000000);
    } else {
        tmxc_graphics_rect(540, button_y, 200, 60, TMXC_COLOR_SUCCESS);
        tmxc_graphics_text(560, button_y + 35, "Başla", 0xFF000000);
    }
    
    if (tmxc_setup_wizard.current_step > TMXC_SETUP_STEP_LANGUAGE && 
        tmxc_setup_wizard.current_step < TMXC_SETUP_STEP_CALIBRATION) {
        tmxc_graphics_rect(540, button_y, 200, 60, TMXC_COLOR_CARD);
        tmxc_graphics_text(560, button_y + 35, skip_text, TMXC_COLOR_TEXT);
    }
}

void tmxc_setup_wizard_init(void) {
    tmxc_uart_puts("[SETUP-WIZARD] Initializing setup wizard...\r\n");
    
    tmxc_setup_wizard.initialized = 0;
    tmxc_setup_wizard.active = 0;
    tmxc_setup_wizard.current_step = TMXC_SETUP_STEP_LANGUAGE;
    tmxc_setup_wizard.total_steps = 6;
    tmxc_setup_wizard.setup_completed = 0;
    tmxc_setup_wizard.blocking_mode = 1;
    tmxc_setup_wizard.selected_language_index = 0;
    tmxc_setup_wizard.ecosystem_sync_enabled = 0;
    tmxc_setup_wizard.security_calibration_completed = 0;
    tmxc_setup_wizard.network_stealth_mode = TMXC_NETWORK_MODE_BALANCED;
    tmxc_setup_wizard.device_calibration_completed = 0;
    tmxc_setup_wizard.setup_start_time = 0;
    
    uint32_t wizard_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_SETUP_WIZARD_BASE + TMXC_SETUP_WIZARD_CTRL));
    wizard_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)(TMXC_SETUP_WIZARD_BASE + TMXC_SETUP_WIZARD_CTRL), wizard_ctrl);
    
    tmxc_setup_wizard.initialized = 1;
    tmxc_uart_puts("[SETUP-WIZARD] Setup wizard initialized with blocking mode\r\n");
}

void tmxc_setup_wizard_start(void) {
    if (!tmxc_setup_wizard.initialized) {
        return;
    }
    
    tmxc_uart_puts("[SETUP-WIZARD] Starting setup wizard...\r\n");
    
    tmxc_setup_wizard.active = 1;
    tmxc_setup_wizard.current_step = TMXC_SETUP_STEP_LANGUAGE;
    tmxc_setup_wizard.setup_start_time = tmxc_get_cycle_count();
    
    tmxc_graphics_clear(TMXC_COLOR_BG);
}

void tmxc_setup_wizard_stop(void) {
    if (!tmxc_setup_wizard.initialized) {
        return;
    }
    
    tmxc_setup_wizard.active = 0;
    tmxc_uart_puts("[SETUP-WIZARD] Setup wizard stopped\r\n");
}

uint8_t tmxc_setup_wizard_is_active(void) {
    return tmxc_setup_wizard.active;
}

uint8_t tmxc_setup_wizard_is_blocking_mode(void) {
    return tmxc_setup_wizard.blocking_mode;
}

uint8_t tmxc_setup_wizard_is_completed(void) {
    return tmxc_setup_wizard.setup_completed;
}

void tmxc_setup_wizard_next_step(void) {
    if (!tmxc_setup_wizard.active) {
        return;
    }
    
    if (tmxc_setup_wizard.current_step < tmxc_setup_wizard.total_steps - 1) {
        tmxc_setup_wizard.current_step++;
        tmxc_uart_puts("[SETUP-WIZARD] Advanced to step ");
        char buffer[21];
        int pos = 20;
        buffer[pos] = '\0';
        uint64_t temp = tmxc_setup_wizard.current_step;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts("\r\n");
    } else {
        tmxc_setup_wizard.complete_setup();
    }
}

void tmxc_setup_wizard_previous_step(void) {
    if (!tmxc_setup_wizard.active) {
        return;
    }
    
    if (tmxc_setup_wizard.current_step > 0) {
        tmxc_setup_wizard.current_step--;
    }
}

void tmxc_setup_wizard_complete_setup(void) {
    if (!tmxc_setup_wizard.active) {
        return;
    }
    
    tmxc_uart_puts("[SETUP-WIZARD] Setup completed!\r\n");
    
    tmxc_setup_wizard.setup_completed = 1;
    tmxc_setup_wizard.active = 0;
    tmxc_setup_wizard.blocking_mode = 0;
    
    uint64_t setup_duration = tmxc_get_cycle_count() - tmxc_setup_wizard.setup_start_time;
    uint64_t setup_ms = (setup_duration * 1000) / tmxc_get_frequency();
    
    tmxc_uart_puts("[SETUP-WIZARD] Setup completed in ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = setup_ms;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" ms\r\n");
}

void tmxc_setup_wizard_render(void) {
    if (!tmxc_setup_wizard.initialized || !tmxc_setup_wizard.active) {
        return;
    }
    
    tmxc_graphics_clear(TMXC_COLOR_BG);
    
    tmxc_setup_wizard_draw_header();
    
    switch (tmxc_setup_wizard.current_step) {
        case TMXC_SETUP_STEP_LANGUAGE:
            tmxc_setup_wizard_draw_language_selection();
            break;
        case TMXC_SETUP_STEP_ECOSYSTEM:
            tmxc_setup_wizard_draw_ecosystem_sync();
            break;
        case TMXC_SETUP_STEP_SECURITY:
            tmxc_setup_wizard_draw_security_setup();
            break;
        case TMXC_SETUP_STEP_NETWORK:
            tmxc_setup_wizard_draw_network_stealth();
            break;
        case TMXC_SETUP_STEP_CALIBRATION:
            tmxc_setup_wizard_draw_calibration();
            break;
        case TMXC_SETUP_STEP_COMPLETE:
            tmxc_setup_wizard_draw_complete();
            break;
    }
    
    tmxc_setup_wizard_draw_buttons();
    
    tmxc_graphics_flip();
}

void tmxc_setup_wizard_handle_touch(tmxc_touch_event_t* event) {
    if (!tmxc_setup_wizard.initialized || !tmxc_setup_wizard.active) {
        return;
    }
    
    uint32_t x = event->x;
    uint32_t y = event->y;
    uint32_t button_y = 650;
    
    if (y >= button_y && y <= button_y + 60) {
        if (x >= 1030 && x <= 1230) {
            tmxc_setup_wizard_next_step();
            return;
        }
        
        if (x >= 50 && x <= 250 && tmxc_setup_wizard.current_step > 0) {
            tmxc_setup_wizard_previous_step();
            return;
        }
        
        if (x >= 540 && x <= 740) {
            if (tmxc_setup_wizard.current_step == TMXC_SETUP_STEP_COMPLETE) {
                tmxc_setup_wizard.complete_setup();
            } else if (tmxc_setup_wizard.current_step > TMXC_SETUP_STEP_LANGUAGE && 
                      tmxc_setup_wizard.current_step < TMXC_SETUP_STEP_CALIBRATION) {
                tmxc_setup_wizard.current_step++;
            }
            return;
        }
    }
    
    if (tmxc_setup_wizard.current_step == TMXC_SETUP_STEP_LANGUAGE) {
        uint32_t y_offset = 170;
        for (uint32_t i = 0; i < 8; i++) {
            uint32_t card_y = y_offset + i * 70;
            if (y >= card_y && y <= card_y + 60 && x >= 50 && x <= 1230) {
                tmxc_setup_wizard.selected_language_index = i;
                
                tmxc_language_t* languages = tmxc_localization_get_all_languages();
                if (languages != NULL) {
                    tmxc_localization_set_language(languages[i].code);
                }
                return;
            }
        }
    }
    
    if (tmxc_setup_wizard.current_step == TMXC_SETUP_STEP_ECOSYSTEM) {
        uint32_t sync_y = 250;
        if (y >= sync_y && y <= sync_y + 80 && x >= 50 && x <= 1230) {
            tmxc_setup_wizard.ecosystem_sync_enabled = !tmxc_setup_wizard.ecosystem_sync_enabled;
            return;
        }
    }
    
    if (tmxc_setup_wizard.current_step == TMXC_SETUP_STEP_NETWORK) {
        uint32_t y_offset = 250;
        
        if (y >= y_offset && y <= y_offset + 100 && x >= 50 && x <= 430) {
            tmxc_setup_wizard.network_stealth_mode = TMXC_NETWORK_MODE_STEALTH;
            return;
        }
        
        if (y >= y_offset && y <= y_offset + 100 && x >= 450 && x <= 830) {
            tmxc_setup_wizard.network_stealth_mode = TMXC_NETWORK_MODE_FULL_SPEED;
            return;
        }
        
        if (y >= y_offset && y <= y_offset + 100 && x >= 850 && x <= 1230) {
            tmxc_setup_wizard.network_stealth_mode = TMXC_NETWORK_MODE_BALANCED;
            return;
        }
    }
}

void tmxc_setup_wizard_set_language(uint32_t index) {
    if (index < tmxc_localization_get_language_count()) {
        tmxc_setup_wizard.selected_language_index = index;
        
        tmxc_language_t* languages = tmxc_localization_get_all_languages();
        if (languages != NULL) {
            tmxc_localization_set_language(languages[index].code);
        }
    }
}

void tmxc_setup_wizard_enable_ecosystem_sync(uint8_t enable) {
    tmxc_setup_wizard.ecosystem_sync_enabled = enable;
}

void tmxc_setup_wizard_set_network_mode(uint8_t mode) {
    tmxc_setup_wizard.network_stealth_mode = mode;
}

uint8_t tmxc_setup_wizard_get_network_mode(void) {
    return tmxc_setup_wizard.network_stealth_mode;
}
