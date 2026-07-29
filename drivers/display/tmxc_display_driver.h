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
#ifndef TMXC_DISPLAY_DRIVER_H
#define TMXC_DISPLAY_DRIVER_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_DISPLAY_BASE 0x1A000000
#define TMXC_DISPLAY_WIDTH 1920
#define TMXC_DISPLAY_HEIGHT 1080
#define TMXC_DISPLAY_BPP 32

#define TMXC_DISPLAY_CTRL_REG (TMXC_DISPLAY_BASE + 0x0000)
#define TMXC_DISPLAY_STATUS_REG (TMXC_DISPLAY_BASE + 0x0004)
#define TMXC_DISPLAY_TIMING_REG (TMXC_DISPLAY_BASE + 0x0008)
#define TMXC_DISPLAY_FIFO_REG (TMXC_DISPLAY_BASE + 0x000C)
#define TMXC_DISPLAY_CURSOR_REG (TMXC_DISPLAY_BASE + 0x0010)
#define TMXC_DISPLAY_LAYER_REG (TMXC_DISPLAY_BASE + 0x0014)
#define TMXC_DISPLAY_GAMMA_REG (TMXC_DISPLAY_BASE + 0x0018)
#define TMXC_DISPLAY_HOLOGRAPHIC_REG (TMXC_DISPLAY_BASE + 0x001C)

#define TMXC_HOLO_DEPTH_BASE (TMXC_DISPLAY_BASE + 0x2000)
#define TMXC_HOLO_DEPTH_LAYER_REG(layer) (TMXC_HOLO_DEPTH_BASE + (layer * 0x100))
#define TMXC_HOLO_DEPTH_Z_REG (TMXC_HOLO_DEPTH_BASE + 0x0000)
#define TMXC_HOLO_DEPTH_PARALLAX_REG (TMXC_HOLO_DEPTH_BASE + 0x0004)
#define TMXC_HOLO_DEPTH_OPACITY_REG (TMXC_HOLO_DEPTH_BASE + 0x0008)
#define TMXC_HOLO_DEPTH_OFFSET_X_REG (TMXC_HOLO_DEPTH_BASE + 0x000C)
#define TMXC_HOLO_DEPTH_OFFSET_Y_REG (TMXC_HOLO_DEPTH_BASE + 0x0010)

typedef struct {
    uint32_t ctrl;
    uint32_t status;
    uint32_t timing;
    uint32_t fifo_level;
    uint32_t cursor_x;
    uint32_t cursor_y;
    uint8_t display_enabled;
    uint8_t backlight_enabled;
    uint8_t driver_initialized;
} tmxc_display_driver_t;

void tmxc_display_driver_init(void);
void tmxc_display_enable(uint8_t enable);
void tmxc_display_set_resolution(uint32_t width, uint32_t height);
void tmxc_display_set_cursor(uint32_t x, uint32_t y);
void tmxc_display_write_pixel(uint32_t x, uint32_t y, uint32_t color);
uint32_t tmxc_display_read_pixel(uint32_t x, uint32_t y);
void tmxc_display_fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void tmxc_display_clear(uint32_t color);

void tmxc_holographic_write_depth_layer(uint8_t layer, int16_t z_depth, int16_t parallax, uint8_t opacity);
void tmxc_holographic_set_depth_offset(uint8_t layer, int16_t offset_x, int16_t offset_y);
void tmxc_holographic_enable_depth_mapping(uint8_t enable);

#endif
