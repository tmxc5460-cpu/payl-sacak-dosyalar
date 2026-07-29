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
    uint8_t animation_active;
    uint32_t current_frame;
    uint32_t total_frames;
    uint64_t animation_start_time;
    uint32_t animation_duration_ms;
    uint8_t phase;
    uint32_t screen_width;
    uint32_t screen_height;
    uint32_t center_x;
    uint32_t center_y;
} tmxc_boot_animation_t;

static tmxc_boot_animation_t tmxc_boot_animation;

#define TMXC_BOOT_ANIMATION_BASE 0xE3000000
#define TMXC_BOOT_ANIMATION_CTRL 0x00
#define TMXC_BOOT_ANIMATION_STATUS 0x04
#define TMXC_BOOT_ANIMATION_FRAME 0x08

#define TMXC_BOOT_PHASE_LETTERS_APPEAR 0
#define TMXC_BOOT_PHASE_LETTERS_MERGE 1
#define TMXC_BOOT_PHASE_LOCK_FORM 2
#define TMXC_BOOT_PHASE_WELCOME 3
#define TMXC_BOOT_PHASE_COMPLETE 4

#define TMXC_COLOR_BLACK 0xFF000000
#define TMXC_COLOR_WHITE 0xFFFFFFFF
#define TMXC_COLOR_GOLD 0xFFFFD700
#define TMXC_COLOR_SILVER 0xFFC0C0C0

typedef struct {
    int x;
    int y;
    int target_x;
    int target_y;
    float scale;
    float rotation;
    uint8_t visible;
    char letter;
} tmxc_letter_t;

static tmxc_letter_t tmxc_letters[4];

static void tmxc_boot_animation_init_letters(void) {
    int spacing = 80;
    int start_x = tmxc_boot_animation.center_x - (spacing * 3) / 2;
    
    tmxc_letters[0].letter = 'T';
    tmxc_letters[0].x = start_x;
    tmxc_letters[0].y = tmxc_boot_animation.center_y - 200;
    tmxc_letters[0].target_x = start_x;
    tmxc_letters[0].target_y = tmxc_boot_animation.center_y;
    tmxc_letters[0].scale = 0.5f;
    tmxc_letters[0].rotation = -45.0f;
    tmxc_letters[0].visible = 1;
    
    tmxc_letters[1].letter = 'M';
    tmxc_letters[1].x = start_x + spacing;
    tmxc_letters[1].y = tmxc_boot_animation.center_y - 200;
    tmxc_letters[1].target_x = start_x + spacing;
    tmxc_letters[1].target_y = tmxc_boot_animation.center_y;
    tmxc_letters[1].scale = 0.5f;
    tmxc_letters[1].rotation = 45.0f;
    tmxc_letters[1].visible = 1;
    
    tmxc_letters[2].letter = 'X';
    tmxc_letters[2].x = start_x + spacing * 2;
    tmxc_letters[2].y = tmxc_boot_animation.center_y - 200;
    tmxc_letters[2].target_x = start_x + spacing * 2;
    tmxc_letters[2].target_y = tmxc_boot_animation.center_y;
    tmxc_letters[2].scale = 0.5f;
    tmxc_letters[2].rotation = -30.0f;
    tmxc_letters[2].visible = 1;
    
    tmxc_letters[3].letter = 'C';
    tmxc_letters[3].x = start_x + spacing * 3;
    tmxc_letters[3].y = tmxc_boot_animation.center_y - 200;
    tmxc_letters[3].target_x = start_x + spacing * 3;
    tmxc_letters[3].target_y = tmxc_boot_animation.center_y;
    tmxc_letters[3].scale = 0.5f;
    tmxc_letters[3].rotation = 30.0f;
    tmxc_letters[3].visible = 1;
}

static void tmxc_boot_animation_draw_3d_letter(tmxc_letter_t* letter, uint32_t color) {
    if (!letter->visible) {
        return;
    }
    
    int size = (int)(60 * letter->scale);
    int x = letter->x;
    int y = letter->y;
    
    tmxc_graphics_rect(x - size/2, y - size/2, size, size, color);
    
    char letter_str[2] = {letter->letter, '\0'};
    tmxc_graphics_text(x - 10, y + 10, letter_str, TMXC_COLOR_BLACK);
}

static void tmxc_boot_animation_draw_lock_form(void) {
    uint32_t lock_width = 120;
    uint32_t lock_height = 160;
    uint32_t lock_x = tmxc_boot_animation.center_x - lock_width / 2;
    uint32_t lock_y = tmxc_boot_animation.center_y - lock_height / 2;
    
    tmxc_graphics_rect(lock_x, lock_y, lock_width, lock_height, TMXC_COLOR_GOLD);
    
    uint32_t shackle_width = 40;
    uint32_t shackle_height = 60;
    uint32_t shackle_x = tmxc_boot_animation.center_x - shackle_width / 2;
    uint32_t shackle_y = lock_y - shackle_height + 20;
    
    tmxc_graphics_rect(shackle_x, shackle_y, shackle_width, shackle_height, TMXC_COLOR_SILVER);
    
    uint32_t keyhole_width = 20;
    uint32_t keyhole_height = 30;
    uint32_t keyhole_x = tmxc_boot_animation.center_x - keyhole_width / 2;
    uint32_t keyhole_y = lock_y + lock_height / 2 - keyhole_height / 2;
    
    tmxc_graphics_rect(keyhole_x, keyhole_y, keyhole_width, keyhole_height, TMXC_COLOR_BLACK);
}

static void tmxc_boot_animation_draw_welcome(void) {
    const char* welcome = "Hoş Geldiniz";
    
    uint32_t text_x = tmxc_boot_animation.center_x - 150;
    uint32_t text_y = tmxc_boot_animation.center_y + 120;
    
    tmxc_graphics_text(text_x, text_y, welcome, TMXC_COLOR_WHITE);
}

static void tmxc_boot_animation_update_phase_letters_appear(float progress) {
    for (int i = 0; i < 4; i++) {
        float letter_progress = (float)i / 4.0f;
        if (progress >= letter_progress) {
            float local_progress = (progress - letter_progress) / (1.0f - letter_progress);
            
            tmxc_letters[i].y = tmxc_letters[i].y + (tmxc_letters[i].target_y - tmxc_letters[i].y) * local_progress * 0.1f;
            tmxc_letters[i].scale = 0.5f + (1.0f - 0.5f) * local_progress;
            tmxc_letters[i].rotation = tmxc_letters[i].rotation * (1.0f - local_progress);
        }
    }
}

static void tmxc_boot_animation_update_phase_letters_merge(float progress) {
    int merge_x = tmxc_boot_animation.center_x;
    int merge_y = tmxc_boot_animation.center_y;
    
    for (int i = 0; i < 4; i++) {
        tmxc_letters[i].x = tmxc_letters[i].x + (merge_x - tmxc_letters[i].x) * progress * 0.05f;
        tmxc_letters[i].y = tmxc_letters[i].y + (merge_y - tmxc_letters[i].y) * progress * 0.05f;
        tmxc_letters[i].scale = 1.0f + (0.3f - 1.0f) * progress;
    }
}

static void tmxc_boot_animation_update_phase_lock_form(float progress) {
    for (int i = 0; i < 4; i++) {
        tmxc_letters[i].scale = 0.3f * (1.0f - progress);
        if (progress > 0.8f) {
            tmxc_letters[i].visible = 0;
        }
    }
}

static void tmxc_boot_animation_update_phase_welcome(float progress) {
}

void tmxc_boot_animation_init(void) {
    tmxc_uart_puts("[BOOT-ANIMATION] Initializing TMXC signature reveal...\r\n");
    
    tmxc_boot_animation.initialized = 0;
    tmxc_boot_animation.animation_active = 0;
    tmxc_boot_animation.current_frame = 0;
    tmxc_boot_animation.total_frames = 300;
    tmxc_boot_animation.animation_start_time = 0;
    tmxc_boot_animation.animation_duration_ms = 5000;
    tmxc_boot_animation.phase = TMXC_BOOT_PHASE_LETTERS_APPEAR;
    tmxc_boot_animation.screen_width = 1280;
    tmxc_boot_animation.screen_height = 720;
    tmxc_boot_animation.center_x = tmxc_boot_animation.screen_width / 2;
    tmxc_boot_animation.center_y = tmxc_boot_animation.screen_height / 2;
    
    tmxc_boot_animation_init_letters();
    
    uint32_t anim_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_BOOT_ANIMATION_BASE + TMXC_BOOT_ANIMATION_CTRL));
    anim_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)(TMXC_BOOT_ANIMATION_BASE + TMXC_BOOT_ANIMATION_CTRL), anim_ctrl);
    
    tmxc_boot_animation.initialized = 1;
    tmxc_uart_puts("[BOOT-ANIMATION] TMXC signature reveal initialized\r\n");
}

void tmxc_boot_animation_start(void) {
    if (!tmxc_boot_animation.initialized) {
        return;
    }
    
    tmxc_uart_puts("[BOOT-ANIMATION] Starting signature reveal animation...\r\n");
    
    tmxc_boot_animation.animation_active = 1;
    tmxc_boot_animation.current_frame = 0;
    tmxc_boot_animation.animation_start_time = tmxc_get_cycle_count();
    tmxc_boot_animation.phase = TMXC_BOOT_PHASE_LETTERS_APPEAR;
    
    tmxc_boot_animation_init_letters();
    
    tmxc_graphics_clear(TMXC_COLOR_BLACK);
}

void tmxc_boot_animation_stop(void) {
    if (!tmxc_boot_animation.initialized) {
        return;
    }
    
    tmxc_boot_animation.animation_active = 0;
    tmxc_uart_puts("[BOOT-ANIMATION] Signature reveal animation stopped\r\n");
}

uint8_t tmxc_boot_animation_is_active(void) {
    return tmxc_boot_animation.animation_active;
}

void tmxc_boot_animation_update(void) {
    if (!tmxc_boot_animation.initialized || !tmxc_boot_animation.animation_active) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed_cycles = current_time - tmxc_boot_animation.animation_start_time;
    uint64_t elapsed_ms = (elapsed_cycles * 1000) / tmxc_get_frequency();
    
    float total_progress = (float)elapsed_ms / (float)tmxc_boot_animation.animation_duration_ms;
    
    if (total_progress >= 1.0f) {
        tmxc_boot_animation.phase = TMXC_BOOT_PHASE_COMPLETE;
        tmxc_boot_animation.animation_active = 0;
        tmxc_uart_puts("[BOOT-ANIMATION] Signature reveal animation complete\r\n");
        return;
    }
    
    tmxc_boot_animation.current_frame++;
    
    tmxc_graphics_clear(TMXC_COLOR_BLACK);
    
    if (total_progress < 0.25f) {
        tmxc_boot_animation.phase = TMXC_BOOT_PHASE_LETTERS_APPEAR;
        float phase_progress = total_progress / 0.25f;
        tmxc_boot_animation_update_phase_letters_appear(phase_progress);
        
        for (int i = 0; i < 4; i++) {
            tmxc_boot_animation_draw_3d_letter(&tmxc_letters[i], TMXC_COLOR_GOLD);
        }
    } else if (total_progress < 0.50f) {
        tmxc_boot_animation.phase = TMXC_BOOT_PHASE_LETTERS_MERGE;
        float phase_progress = (total_progress - 0.25f) / 0.25f;
        tmxc_boot_animation_update_phase_letters_merge(phase_progress);
        
        for (int i = 0; i < 4; i++) {
            tmxc_boot_animation_draw_3d_letter(&tmxc_letters[i], TMXC_COLOR_GOLD);
        }
    } else if (total_progress < 0.75f) {
        tmxc_boot_animation.phase = TMXC_BOOT_PHASE_LOCK_FORM;
        float phase_progress = (total_progress - 0.50f) / 0.25f;
        tmxc_boot_animation_update_phase_lock_form(phase_progress);
        
        for (int i = 0; i < 4; i++) {
            tmxc_boot_animation_draw_3d_letter(&tmxc_letters[i], TMXC_COLOR_GOLD);
        }
        
        if (phase_progress > 0.5f) {
            tmxc_boot_animation_draw_lock_form();
        }
    } else {
        tmxc_boot_animation.phase = TMXC_BOOT_PHASE_WELCOME;
        float phase_progress = (total_progress - 0.75f) / 0.25f;
        tmxc_boot_animation_update_phase_welcome(phase_progress);
        
        tmxc_boot_animation_draw_lock_form();
        tmxc_boot_animation_draw_welcome();
    }
    
    tmxc_graphics_flip();
}

void tmxc_boot_animation_set_duration(uint32_t duration_ms) {
    tmxc_boot_animation.animation_duration_ms = duration_ms;
}

uint8_t tmxc_boot_animation_is_complete(void) {
    return tmxc_boot_animation.phase == TMXC_BOOT_PHASE_COMPLETE;
}
