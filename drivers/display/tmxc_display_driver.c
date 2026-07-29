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
#include "tmxc_display_driver.h"

static tmxc_display_driver_t tmxc_display;

void tmxc_display_driver_init(void) {
    tmxc_display.ctrl = 0;
    tmxc_display.status = 0;
    tmxc_display.timing = 0;
    tmxc_display.fifo_level = 0;
    tmxc_display.cursor_x = 0;
    tmxc_display.cursor_y = 0;
    tmxc_display.display_enabled = 0;
    tmxc_display.backlight_enabled = 0;
    tmxc_display.driver_initialized = 1;
    
    __asm__ volatile("msr daifset, #2");
    
    uint32_t ctrl_value = (1 << 0) | (1 << 8);
    __asm__ volatile("str %0, [%1]" : : "r"(ctrl_value), "r"(TMXC_DISPLAY_CTRL_REG));
    tmxc_display.ctrl = ctrl_value;
    
    uint32_t timing_value = (TMXC_DISPLAY_WIDTH << 16) | TMXC_DISPLAY_HEIGHT;
    __asm__ volatile("str %0, [%1]" : : "r"(timing_value), "r"(TMXC_DISPLAY_TIMING_REG));
    tmxc_display.timing = timing_value;
    
    __asm__ volatile("msr daifclr, #2");
    
    tmxc_uart_puts("[DISPLAY] Display driver initialized\r\n");
}

void tmxc_display_enable(uint8_t enable) {
    if (!tmxc_display.driver_initialized) {
        return;
    }
    
    uint32_t ctrl_value = tmxc_display.ctrl;
    if (enable) {
        ctrl_value |= (1 << 0);
        tmxc_display.display_enabled = 1;
    } else {
        ctrl_value &= ~(1 << 0);
        tmxc_display.display_enabled = 0;
    }
    
    __asm__ volatile("str %0, [%1]" : : "r"(ctrl_value), "r"(TMXC_DISPLAY_CTRL_REG));
    tmxc_display.ctrl = ctrl_value;
    
    if (enable) {
        tmxc_uart_puts("[DISPLAY] Display enabled\r\n");
    } else {
        tmxc_uart_puts("[DISPLAY] Display disabled\r\n");
    }
}

void tmxc_display_set_resolution(uint32_t width, uint32_t height) {
    if (!tmxc_display.driver_initialized) {
        return;
    }
    
    uint32_t timing_value = (width << 16) | height;
    __asm__ volatile("str %0, [%1]" : : "r"(timing_value), "r"(TMXC_DISPLAY_TIMING_REG));
    tmxc_display.timing = timing_value;
}

void tmxc_display_set_cursor(uint32_t x, uint32_t y) {
    if (!tmxc_display.driver_initialized) {
        return;
    }
    
    if (x >= TMXC_DISPLAY_WIDTH) x = TMXC_DISPLAY_WIDTH - 1;
    if (y >= TMXC_DISPLAY_HEIGHT) y = TMXC_DISPLAY_HEIGHT - 1;
    
    tmxc_display.cursor_x = x;
    tmxc_display.cursor_y = y;
    
    uint32_t cursor_value = (x << 16) | y;
    __asm__ volatile("str %0, [%1]" : : "r"(cursor_value), "r"(TMXC_DISPLAY_CURSOR_REG));
}

void tmxc_display_write_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!tmxc_display.driver_initialized || !tmxc_display.display_enabled) {
        return;
    }
    
    if (x >= TMXC_DISPLAY_WIDTH || y >= TMXC_DISPLAY_HEIGHT) {
        return;
    }
    
    uint32_t pixel_addr = TMXC_DISPLAY_BASE + 0x100000 + (y * TMXC_DISPLAY_WIDTH + x) * 4;
    __asm__ volatile("str %0, [%1]" : : "r"(color), "r"(pixel_addr));
}

uint32_t tmxc_display_read_pixel(uint32_t x, uint32_t y) {
    if (!tmxc_display.driver_initialized || !tmxc_display.display_enabled) {
        return 0;
    }
    
    if (x >= TMXC_DISPLAY_WIDTH || y >= TMXC_DISPLAY_HEIGHT) {
        return 0;
    }
    
    uint32_t pixel_addr = TMXC_DISPLAY_BASE + 0x100000 + (y * TMXC_DISPLAY_WIDTH + x) * 4;
    uint32_t color;
    __asm__ volatile("ldr %0, [%1]" : "=r"(color) : "r"(pixel_addr));
    
    return color;
}

void tmxc_display_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    if (!tmxc_display.driver_initialized || !tmxc_display.display_enabled) {
        return;
    }
    
    for (uint32_t py = y; py < y + height && py < TMXC_DISPLAY_HEIGHT; py++) {
        for (uint32_t px = x; px < x + width && px < TMXC_DISPLAY_WIDTH; px++) {
            tmxc_display_write_pixel(px, py, color);
        }
    }
}

void tmxc_display_clear(uint32_t color) {
    tmxc_display_fill_rect(0, 0, TMXC_DISPLAY_WIDTH, TMXC_DISPLAY_HEIGHT, color);
}

void tmxc_holographic_write_depth_layer(uint8_t layer, int16_t z_depth, int16_t parallax, uint8_t opacity) {
    if (!tmxc_display.driver_initialized) {
        return;
    }
    
    if (layer >= 16) {
        return;
    }
    
    uint32_t layer_base = TMXC_HOLO_DEPTH_LAYER_REG(layer);
    
    uint32_t z_value = (uint32_t)z_depth;
    __asm__ volatile("str %0, [%1]" : : "r"(z_value), "r"(layer_base + TMXC_HOLO_DEPTH_Z_REG));
    
    uint32_t parallax_value = (uint32_t)parallax;
    __asm__ volatile("str %0, [%1]" : : "r"(parallax_value), "r"(layer_base + TMXC_HOLO_DEPTH_PARALLAX_REG));
    
    uint32_t opacity_value = (uint32_t)opacity;
    __asm__ volatile("str %0, [%1]" : : "r"(opacity_value), "r"(layer_base + TMXC_HOLO_DEPTH_OPACITY_REG));
}

void tmxc_holographic_set_depth_offset(uint8_t layer, int16_t offset_x, int16_t offset_y) {
    if (!tmxc_display.driver_initialized) {
        return;
    }
    
    if (layer >= 16) {
        return;
    }
    
    uint32_t layer_base = TMXC_HOLO_DEPTH_LAYER_REG(layer);
    
    uint32_t offset_x_value = (uint32_t)offset_x;
    __asm__ volatile("str %0, [%1]" : : "r"(offset_x_value), "r"(layer_base + TMXC_HOLO_DEPTH_OFFSET_X_REG));
    
    uint32_t offset_y_value = (uint32_t)offset_y;
    __asm__ volatile("str %0, [%1]" : : "r"(offset_y_value), "r"(layer_base + TMXC_HOLO_DEPTH_OFFSET_Y_REG));
}

void tmxc_holographic_enable_depth_mapping(uint8_t enable) {
    if (!tmxc_display.driver_initialized) {
        return;
    }
    
    uint32_t holo_ctrl;
    __asm__ volatile("ldr %0, [%1]" : "=r"(holo_ctrl) : "r"(TMXC_DISPLAY_HOLOGRAPHIC_REG));
    
    if (enable) {
        holo_ctrl |= (1 << 0);
    } else {
        holo_ctrl &= ~(1 << 0);
    }
    
    __asm__ volatile("str %0, [%1]" : : "r"(holo_ctrl), "r"(TMXC_DISPLAY_HOLOGRAPHIC_REG));
}
