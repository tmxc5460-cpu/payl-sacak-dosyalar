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

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t stride;
    uint8_t* framebuffer;
    uint32_t buffer_count;
    uint32_t current_buffer;
    uint8_t* buffers[3];
    uint8_t initialized;
    uint32_t vsync_enabled;
    uint32_t triple_buffering;
    uint32_t zero_copy;
    uint64_t gpu_registers[64];
} tmxc_graphics_t;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t target_width;
    uint32_t target_height;
    uint32_t current_width;
    uint32_t current_height;
    uint32_t background_color;
    uint32_t foreground_color;
    uint8_t alpha;
    uint8_t target_alpha;
    uint8_t animation_active;
    uint8_t notification_type;
    char notification_text[64];
    uint64_t animation_start_time;
    uint32_t animation_duration_ms;
} tmxc_dynamic_island_t;

static tmxc_graphics_t tmxc_graphics;
static tmxc_dynamic_island_t tmxc_dynamic_island;
static uint8_t tmxc_vsync_bypass_enabled = 0;

static void tmxc_gpu_init(void) {
    for (int i = 0; i < 64; i++) {
        tmxc_graphics.gpu_registers[i] = 0;
    }
    
    uint32_t gpu_ctrl = tmxc_read32((volatile uint32_t*)TMXC_GPU_GP0_BASE);
    gpu_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_GPU_GP0_BASE, gpu_ctrl);
    
    gpu_ctrl = tmxc_read32((volatile uint32_t*)TMXC_GPU_GP0_BASE);
    gpu_ctrl |= (1 << 8);
    tmxc_write32((volatile uint32_t*)TMXC_GPU_GP0_BASE, gpu_ctrl);
    
    tmxc_timer_delay_ms(10);
    
    uint32_t v3d_ctrl = tmxc_read32((volatile uint32_t*)TMXC_GPU_V3D_BASE);
    v3d_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_GPU_V3D_BASE, v3d_ctrl);
    
    v3d_ctrl = tmxc_read32((volatile uint32_t*)TMXC_GPU_V3D_BASE);
    v3d_ctrl |= (1 << 8);
    tmxc_write32((volatile uint32_t*)TMXC_GPU_V3D_BASE, v3d_ctrl);
    
    tmxc_timer_delay_ms(10);
}

static void tmxc_display_init(void) {
    uint32_t lcd_ctrl = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL);
    lcd_ctrl &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL, lcd_ctrl);
    
    uint32_t timing0 = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_TIMING0);
    timing0 = (timing0 & ~0xFFFF) | 1920;
    timing0 = (timing0 & ~0xFFFF0000) | (20 << 16);
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_TIMING0, timing0);
    
    uint32_t timing1 = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_TIMING1);
    timing1 = (timing1 & ~0xFFFF) | 1080;
    timing1 = (timing1 & ~0xFFFF0000) | (10 << 16);
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_TIMING1, timing1);
    
    uint32_t timing2 = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_TIMING2);
    timing2 = (timing2 & ~0xFFFF) | 140;
    timing2 = (timing2 & ~0xFFFF0000) | (160 << 16);
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_TIMING2, timing2);
    
    uint32_t polarity = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_POLARITY);
    polarity |= (1 << 0);
    polarity |= (1 << 1);
    polarity |= (1 << 2);
    polarity |= (1 << 3);
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_POLARITY, polarity);
    
    uint32_t fifo_ctrl = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_FIFO_CTRL);
    fifo_ctrl |= (1 << 0);
    fifo_ctrl |= (1 << 8);
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_FIFO_CTRL, fifo_ctrl);
    
    lcd_ctrl = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL);
    lcd_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL, lcd_ctrl);
    
    tmxc_timer_delay_ms(50);
}

static void tmxc_framebuffer_init(void) {
    uint32_t buffer_size = tmxc_graphics.width * tmxc_graphics.height * 4;
    
    for (uint32_t i = 0; i < tmxc_graphics.buffer_count; i++) {
        tmxc_graphics.buffers[i] = (uint8_t*)tmxc_malloc(buffer_size);
        
        if (tmxc_graphics.buffers[i] != NULL) {
            for (uint32_t j = 0; j < buffer_size; j++) {
                tmxc_graphics.buffers[i][j] = 0;
            }
        }
    }
    
    tmxc_graphics.framebuffer = tmxc_graphics.buffers[0];
    
    uint32_t lcd_ctrl = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL);
    lcd_ctrl = (lcd_ctrl & 0xFFFFFFF0) | ((uint32_t)tmxc_graphics.framebuffer & 0xFFFFFFF0);
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL, lcd_ctrl);
}

static void tmxc_gpu_write_register(uint32_t reg, uint64_t value) {
    if (reg < 64) {
        tmxc_graphics.gpu_registers[reg] = value;
        
        uint32_t gpu_addr = TMXC_GPU_GP0_BASE + (reg * 8);
        tmxc_write32((volatile uint32_t*)gpu_addr, value & 0xFFFFFFFF);
        tmxc_write32((volatile uint32_t*)(gpu_addr + 4), (value >> 32) & 0xFFFFFFFF);
    }
}

static uint64_t tmxc_gpu_read_register(uint32_t reg) {
    if (reg < 64) {
        return tmxc_graphics.gpu_registers[reg];
    }
    return 0;
}

void tmxc_graphics_init(void) {
    tmxc_graphics.width = 1920;
    tmxc_graphics.height = 1080;
    tmxc_graphics.format = TMXC_GRAPHICS_FORMAT_ARGB8888;
    tmxc_graphics.stride = tmxc_graphics.width * 4;
    tmxc_graphics.framebuffer = NULL;
    tmxc_graphics.buffer_count = TMXC_GRAPHICS_TRIPLE_BUFFER;
    tmxc_graphics.current_buffer = 0;
    tmxc_graphics.initialized = 0;
    tmxc_graphics.vsync_enabled = 1;
    tmxc_graphics.triple_buffering = 1;
    tmxc_graphics.zero_copy = 1;
    
    for (int i = 0; i < 3; i++) {
        tmxc_graphics.buffers[i] = NULL;
    }
    
    for (int i = 0; i < 64; i++) {
        tmxc_graphics.gpu_registers[i] = 0;
    }
    
    tmxc_dynamic_island.x = (1920 - 200) / 2;
    tmxc_dynamic_island.y = 20;
    tmxc_dynamic_island.width = 200;
    tmxc_dynamic_island.height = 50;
    tmxc_dynamic_island.target_width = 200;
    tmxc_dynamic_island.target_height = 50;
    tmxc_dynamic_island.current_width = 200;
    tmxc_dynamic_island.current_height = 50;
    tmxc_dynamic_island.background_color = 0xFF000000;
    tmxc_dynamic_island.foreground_color = 0xFFFFFFFF;
    tmxc_dynamic_island.alpha = 255;
    tmxc_dynamic_island.target_alpha = 255;
    tmxc_dynamic_island.animation_active = 0;
    tmxc_dynamic_island.notification_type = 0;
    for (int i = 0; i < 64; i++) {
        tmxc_dynamic_island.notification_text[i] = 0;
    }
    tmxc_dynamic_island.animation_start_time = 0;
    tmxc_dynamic_island.animation_duration_ms = 300;
    
    tmxc_gpu_init();
    tmxc_display_init();
    tmxc_framebuffer_init();
    
    if (tmxc_graphics.framebuffer != NULL) {
        tmxc_graphics.initialized = 1;
    }
}

void tmxc_graphics_set_resolution(uint32_t width, uint32_t height) {
    if (width < 320) width = 320;
    if (width > 3840) width = 3840;
    if (height < 240) height = 240;
    if (height > 2160) height = 2160;
    
    tmxc_graphics.width = width;
    tmxc_graphics.height = height;
    tmxc_graphics.stride = width * 4;
    
    uint32_t timing0 = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_TIMING0);
    timing0 = (timing0 & ~0xFFFF) | width;
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_TIMING0, timing0);
    
    uint32_t timing1 = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_TIMING1);
    timing1 = (timing1 & ~0xFFFF) | height;
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_TIMING1, timing1);
    
    uint32_t buffer_size = width * height * 4;
    
    for (uint32_t i = 0; i < tmxc_graphics.buffer_count; i++) {
        if (tmxc_graphics.buffers[i] != NULL) {
            tmxc_free(tmxc_graphics.buffers[i]);
        }
        tmxc_graphics.buffers[i] = (uint8_t*)tmxc_malloc(buffer_size);
        
        if (tmxc_graphics.buffers[i] != NULL) {
            for (uint32_t j = 0; j < buffer_size; j++) {
                tmxc_graphics.buffers[i][j] = 0;
            }
        }
    }
    
    tmxc_graphics.framebuffer = tmxc_graphics.buffers[0];
    
    uint32_t lcd_ctrl = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL);
    lcd_ctrl = (lcd_ctrl & 0xFFFFFFF0) | ((uint32_t)tmxc_graphics.framebuffer & 0xFFFFFFF0);
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL, lcd_ctrl);
}

void tmxc_graphics_get_resolution(uint32_t* width, uint32_t* height) {
    if (width != NULL) {
        *width = tmxc_graphics.width;
    }
    if (height != NULL) {
        *height = tmxc_graphics.height;
    }
}

void tmxc_graphics_set_format(uint32_t format) {
    tmxc_graphics.format = format;
    
    uint32_t bytes_per_pixel = 4;
    if (format == TMXC_GRAPHICS_FORMAT_RGB565) {
        bytes_per_pixel = 2;
    } else if (format == TMXC_GRAPHICS_FORMAT_XRGB8888) {
        bytes_per_pixel = 4;
    } else if (format == TMXC_GRAPHICS_FORMAT_ARGB8888) {
        bytes_per_pixel = 4;
    }
    
    tmxc_graphics.stride = tmxc_graphics.width * bytes_per_pixel;
}

uint32_t tmxc_graphics_get_format(void) {
    return tmxc_graphics.format;
}

void tmxc_graphics_clear(uint32_t color) {
    if (!tmxc_graphics.initialized || tmxc_graphics.framebuffer == NULL) {
        return;
    }
    
    uint32_t buffer_size = tmxc_graphics.width * tmxc_graphics.height * 4;
    
    for (uint32_t i = 0; i < buffer_size; i += 4) {
        tmxc_graphics.framebuffer[i] = color & 0xFF;
        tmxc_graphics.framebuffer[i + 1] = (color >> 8) & 0xFF;
        tmxc_graphics.framebuffer[i + 2] = (color >> 16) & 0xFF;
        tmxc_graphics.framebuffer[i + 3] = (color >> 24) & 0xFF;
    }
}

void tmxc_graphics_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!tmxc_graphics.initialized || tmxc_graphics.framebuffer == NULL) {
        return;
    }
    
    if (x >= tmxc_graphics.width || y >= tmxc_graphics.height) {
        return;
    }
    
    uint32_t idx = (y * tmxc_graphics.width + x) * 4;
    
    tmxc_graphics.framebuffer[idx] = color & 0xFF;
    tmxc_graphics.framebuffer[idx + 1] = (color >> 8) & 0xFF;
    tmxc_graphics.framebuffer[idx + 2] = (color >> 16) & 0xFF;
    tmxc_graphics.framebuffer[idx + 3] = (color >> 24) & 0xFF;
}

void tmxc_graphics_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    if (!tmxc_graphics.initialized || tmxc_graphics.framebuffer == NULL) {
        return;
    }
    
    if (x >= tmxc_graphics.width || y >= tmxc_graphics.height) {
        return;
    }
    
    if (x + w > tmxc_graphics.width) {
        w = tmxc_graphics.width - x;
    }
    
    if (y + h > tmxc_graphics.height) {
        h = tmxc_graphics.height - y;
    }
    
    for (uint32_t py = y; py < y + h; py++) {
        for (uint32_t px = x; px < x + w; px++) {
            uint32_t idx = (py * tmxc_graphics.width + px) * 4;
            
            tmxc_graphics.framebuffer[idx] = color & 0xFF;
            tmxc_graphics.framebuffer[idx + 1] = (color >> 8) & 0xFF;
            tmxc_graphics.framebuffer[idx + 2] = (color >> 16) & 0xFF;
            tmxc_graphics.framebuffer[idx + 3] = (color >> 24) & 0xFF;
        }
    }
}

static uint8_t tmxc_font_get_char_width(char c) {
    if (c >= 32 && c <= 126) {
        return 8;
    }
    return 8;
}

static void tmxc_font_draw_char(uint32_t x, uint32_t y, char c, uint32_t color) {
    static const uint8_t font_data[95][8] = {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00},
        {0x00, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x6C, 0xFE, 0x6C, 0x6C, 0xFE, 0x6C, 0x00},
        {0x18, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00},
        {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00},
        {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00},
        {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00},
        {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00},
        {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00},
        {0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x00},
        {0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00},
        {0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00},
        {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x6C, 0x38, 0x00},
        {0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00},
        {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00},
        {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00},
        {0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x66, 0x00},
        {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},
        {0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00},
        {0x3C, 0x66, 0x66, 0x66, 0x66, 0x6E, 0x3C, 0x06},
        {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x66, 0x00},
        {0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00},
        {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00},
        {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},
        {0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00},
        {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},
        {0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00},
        {0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00},
        {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00},
        {0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C, 0x00},
        {0x0C, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00},
        {0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00},
        {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},
        {0x00, 0x18, 0x18, 0x7E, 0x18, 0x18, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30},
        {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00},
        {0x00, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x00},
        {0x3C, 0x66, 0x6C, 0x60, 0x6C, 0x66, 0x3C, 0x00},
        {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00},
        {0x3C, 0x66, 0x06, 0x0C, 0x30, 0x60, 0x7E, 0x00},
        {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00},
        {0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x0C, 0x00},
        {0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00},
        {0x3C, 0x66, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00},
        {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00},
        {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00},
        {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00},
        {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00},
        {0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30},
        {0x0C, 0x18, 0x30, 0x60, 0x30, 0x18, 0x0C, 0x00},
        {0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00},
        {0x30, 0x18, 0x0C, 0x06, 0x0C, 0x18, 0x30, 0x00},
        {0x3C, 0x66, 0x0C, 0x18, 0x18, 0x00, 0x18, 0x00},
        {0x3C, 0x66, 0x6E, 0x6E, 0x60, 0x62, 0x3C, 0x00},
        {0x3C, 0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x00},
        {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00},
        {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00},
        {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00},
        {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00},
        {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x60, 0x00},
        {0x3C, 0x66, 0x60, 0x6E, 0x66, 0x66, 0x3C, 0x00},
        {0x66, 0x66, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00},
        {0x7C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7C, 0x00},
        {0x3E, 0x0C, 0x0C, 0x0C, 0x0C, 0x6C, 0x38, 0x00},
        {0x66, 0x6C, 0x78, 0x70, 0x78, 0x6C, 0x66, 0x00},
        {0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00},
        {0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00},
        {0x66, 0x76, 0x7E, 0x7E, 0x6E, 0x66, 0x66, 0x00},
        {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},
        {0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00},
        {0x3C, 0x66, 0x66, 0x66, 0x6E, 0x3C, 0x06},
        {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x66, 0x00},
        {0x3C, 0x66, 0x60, 0x3C, 0x06, 0x66, 0x3C, 0x00},
        {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00},
        {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},
        {0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x00},
        {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},
        {0x66, 0x66, 0x3C, 0x18, 0x3C, 0x66, 0x66, 0x00},
        {0x66, 0x66, 0x66, 0x3C, 0x18, 0x18, 0x18, 0x00},
        {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x7E, 0x00},
        {0x3C, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3C, 0x00},
        {0x18, 0x3C, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}
    };
    
    if (c < 32 || c > 126) {
        c = 32;
    }
    
    uint8_t char_idx = c - 32;
    
    for (uint8_t row = 0; row < 8; row++) {
        uint8_t font_row = font_data[char_idx][row];
        
        for (uint8_t col = 0; col < 8; col++) {
            if (font_row & (1 << (7 - col))) {
                tmxc_graphics_pixel(x + col, y + row, color);
            }
        }
    }
}

void tmxc_graphics_text(uint32_t x, uint32_t y, const char* str, uint32_t color) {
    if (!tmxc_graphics.initialized || str == NULL) {
        return;
    }
    
    uint32_t current_x = x;
    uint32_t current_y = y;
    
    while (*str != '\0') {
        if (*str == '\n') {
            current_x = x;
            current_y += 10;
        } else if (*str == '\r') {
            current_x = x;
        } else {
            tmxc_font_draw_char(current_x, current_y, *str, color);
            current_x += tmxc_font_get_char_width(*str);
        }
        
        str++;
    }
}

void tmxc_graphics_flip(void) {
    if (!tmxc_graphics.initialized) {
        return;
    }
    
    if (tmxc_graphics.vsync_enabled && !tmxc_vsync_bypass_enabled) {
        uint32_t lcd_stat = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL);
        
        while (!(lcd_stat & (1 << 31))) {
            lcd_stat = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL);
            tmxc_timer_delay_ms(1);
        }
    }
    
    if (tmxc_graphics.triple_buffering) {
        tmxc_graphics.current_buffer = (tmxc_graphics.current_buffer + 1) % tmxc_graphics.buffer_count;
        tmxc_graphics.framebuffer = tmxc_graphics.buffers[tmxc_graphics.current_buffer];
        
        uint32_t lcd_ctrl = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL);
        lcd_ctrl = (lcd_ctrl & 0xFFFFFFF0) | ((uint32_t)tmxc_graphics.framebuffer & 0xFFFFFFF0);
        tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL, lcd_ctrl);
    }
}

void tmxc_graphics_enable_vsync_bypass(uint8_t enable) {
    tmxc_vsync_bypass_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[GRAPHICS] VSync bypass aktif - Sınırsız FPS!\r\n");
    } else {
        tmxc_uart_puts("[GRAPHICS] VSync bypass kapatıldı\r\n");
    }
}

uint8_t tmxc_graphics_is_vsync_bypass_enabled(void) {
    return tmxc_vsync_bypass_enabled;
}

void tmxc_graphics_enable_vsync(uint8_t enable) {
    tmxc_graphics.vsync_enabled = enable;
    
    uint32_t lcd_ctrl = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL);
    if (enable) {
        lcd_ctrl |= (1 << 8);
    } else {
        lcd_ctrl &= ~(1 << 8);
    }
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL, lcd_ctrl);
}

uint8_t tmxc_graphics_is_vsync_enabled(void) {
    return tmxc_graphics.vsync_enabled;
}

void tmxc_graphics_enable_triple_buffering(uint8_t enable) {
    tmxc_graphics.triple_buffering = enable;
    
    if (enable) {
        tmxc_graphics.buffer_count = TMXC_GRAPHICS_TRIPLE_BUFFER;
    } else {
        tmxc_graphics.buffer_count = TMXC_GRAPHICS_DOUBLE_BUFFER;
    }
}

uint8_t tmxc_graphics_is_triple_buffering_enabled(void) {
    return tmxc_graphics.triple_buffering;
}

void tmxc_graphics_enable_zero_copy(uint8_t enable) {
    tmxc_graphics.zero_copy = enable;
}

uint8_t tmxc_graphics_is_zero_copy_enabled(void) {
    return tmxc_graphics.zero_copy;
}

uint8_t* tmxc_graphics_get_framebuffer(void) {
    return tmxc_graphics.framebuffer;
}

uint32_t tmxc_graphics_get_framebuffer_size(void) {
    return tmxc_graphics.width * tmxc_graphics.height * 4;
}

static uint32_t tmxc_alpha_blend(uint32_t src, uint32_t dst, uint8_t alpha) {
    uint8_t src_a = (src >> 24) & 0xFF;
    uint8_t src_r = (src >> 16) & 0xFF;
    uint8_t src_g = (src >> 8) & 0xFF;
    uint8_t src_b = src & 0xFF;
    
    uint8_t dst_a = (dst >> 24) & 0xFF;
    uint8_t dst_r = (dst >> 16) & 0xFF;
    uint8_t dst_g = (dst >> 8) & 0xFF;
    uint8_t dst_b = dst & 0xFF;
    
    uint8_t out_a = (src_a * alpha + dst_a * (255 - alpha)) / 255;
    uint8_t out_r = (src_r * alpha + dst_r * (255 - alpha)) / 255;
    uint8_t out_g = (src_g * alpha + dst_g * (255 - alpha)) / 255;
    uint8_t out_b = (src_b * alpha + dst_b * (255 - alpha)) / 255;
    
    return (out_a << 24) | (out_r << 16) | (out_g << 8) | out_b;
}

static uint32_t tmxc_ease_out_cubic(uint32_t t) {
    uint32_t t_inv = 1000 - t;
    return 1000 - ((t_inv * t_inv * t_inv) / 1000000);
}

void tmxc_ui_dynamic_island_show_notification(uint8_t type, const char* text, uint32_t duration_ms) {
    if (!tmxc_graphics.initialized) {
        return;
    }
    
    tmxc_dynamic_island.notification_type = type;
    tmxc_dynamic_island.animation_active = 1;
    tmxc_dynamic_island.animation_start_time = tmxc_get_cycle_count();
    tmxc_dynamic_island.animation_duration_ms = duration_ms;
    tmxc_dynamic_island.current_width = tmxc_dynamic_island.width;
    tmxc_dynamic_island.current_height = tmxc_dynamic_island.height;
    tmxc_dynamic_island.target_width = 400;
    tmxc_dynamic_island.target_height = 80;
    tmxc_dynamic_island.alpha = 0;
    tmxc_dynamic_island.target_alpha = 255;
    
    switch (type) {
        case 0:
            tmxc_dynamic_island.background_color = 0xFF1A1A1A;
            tmxc_dynamic_island.foreground_color = 0xFFFFFFFF;
            break;
        case 1:
            tmxc_dynamic_island.background_color = 0xFF00FF00;
            tmxc_dynamic_island.foreground_color = 0xFF000000;
            break;
        case 2:
            tmxc_dynamic_island.background_color = 0xFFFF0000;
            tmxc_dynamic_island.foreground_color = 0xFFFFFFFF;
            break;
        case 3:
            tmxc_dynamic_island.background_color = 0xFF0000FF;
            tmxc_dynamic_island.foreground_color = 0xFFFFFFFF;
            break;
        default:
            tmxc_dynamic_island.background_color = 0xFF1A1A1A;
            tmxc_dynamic_island.foreground_color = 0xFFFFFFFF;
            break;
    }
    
    for (int i = 0; i < 64 && text != NULL && text[i] != '\0'; i++) {
        tmxc_dynamic_island.notification_text[i] = text[i];
    }
    tmxc_dynamic_island.notification_text[63] = '\0';
}

void tmxc_ui_dynamic_island_hide(void) {
    if (!tmxc_graphics.initialized) {
        return;
    }
    
    tmxc_dynamic_island.animation_active = 1;
    tmxc_dynamic_island.animation_start_time = tmxc_get_cycle_count();
    tmxc_dynamic_island.animation_duration_ms = 300;
    tmxc_dynamic_island.target_width = 200;
    tmxc_dynamic_island.target_height = 50;
    tmxc_dynamic_island.target_alpha = 0;
}

void tmxc_ui_dynamic_island_anim(void) {
    if (!tmxc_graphics.initialized || !tmxc_dynamic_island.animation_active) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed_cycles = current_time - tmxc_dynamic_island.animation_start_time;
    uint64_t elapsed_ms = (elapsed_cycles * 1000) / tmxc_get_frequency();
    
    if (elapsed_ms >= tmxc_dynamic_island.animation_duration_ms) {
        elapsed_ms = tmxc_dynamic_island.animation_duration_ms;
        tmxc_dynamic_island.animation_active = 0;
    }
    
    uint32_t progress = (elapsed_ms * 1000) / tmxc_dynamic_island.animation_duration_ms;
    uint32_t eased_progress = tmxc_ease_out_cubic(progress);
    
    if (tmxc_dynamic_island.target_alpha > tmxc_dynamic_island.alpha) {
        tmxc_dynamic_island.alpha = (tmxc_dynamic_island.target_alpha * eased_progress) / 1000;
    } else {
        tmxc_dynamic_island.alpha = tmxc_dynamic_island.target_alpha + ((255 - tmxc_dynamic_island.target_alpha) * (1000 - eased_progress)) / 1000;
    }
    
    if (tmxc_dynamic_island.target_width > tmxc_dynamic_island.current_width) {
        tmxc_dynamic_island.current_width = tmxc_dynamic_island.width + ((tmxc_dynamic_island.target_width - tmxc_dynamic_island.width) * eased_progress) / 1000;
    } else {
        tmxc_dynamic_island.current_width = tmxc_dynamic_island.target_width + ((tmxc_dynamic_island.width - tmxc_dynamic_island.target_width) * (1000 - eased_progress)) / 1000;
    }
    
    if (tmxc_dynamic_island.target_height > tmxc_dynamic_island.current_height) {
        tmxc_dynamic_island.current_height = tmxc_dynamic_island.height + ((tmxc_dynamic_island.target_height - tmxc_dynamic_island.height) * eased_progress) / 1000;
    } else {
        tmxc_dynamic_island.current_height = tmxc_dynamic_island.target_height + ((tmxc_dynamic_island.height - tmxc_dynamic_island.target_height) * (1000 - eased_progress)) / 1000;
    }
    
    uint32_t island_x = tmxc_dynamic_island.x + (tmxc_dynamic_island.width - tmxc_dynamic_island.current_width) / 2;
    uint32_t island_y = tmxc_dynamic_island.y + (tmxc_dynamic_island.height - tmxc_dynamic_island.current_height) / 2;
    
    for (uint32_t y = island_y; y < island_y + tmxc_dynamic_island.current_height; y++) {
        for (uint32_t x = island_x; x < island_x + tmxc_dynamic_island.current_width; x++) {
            if (x >= tmxc_graphics.width || y >= tmxc_graphics.height) {
                continue;
            }
            
            uint32_t idx = (y * tmxc_graphics.width + x) * 4;
            uint32_t bg_color = tmxc_dynamic_island.background_color;
            uint32_t dst_color = (tmxc_graphics.framebuffer[idx + 3] << 24) | 
                               (tmxc_graphics.framebuffer[idx + 2] << 16) |
                               (tmxc_graphics.framebuffer[idx + 1] << 8) |
                               tmxc_graphics.framebuffer[idx];
            
            uint32_t blended = tmxc_alpha_blend(bg_color, dst_color, tmxc_dynamic_island.alpha);
            
            tmxc_graphics.framebuffer[idx] = blended & 0xFF;
            tmxc_graphics.framebuffer[idx + 1] = (blended >> 8) & 0xFF;
            tmxc_graphics.framebuffer[idx + 2] = (blended >> 16) & 0xFF;
            tmxc_graphics.framebuffer[idx + 3] = (blended >> 24) & 0xFF;
        }
    }
    
    if (tmxc_dynamic_island.notification_text[0] != '\0') {
        uint32_t text_x = island_x + 20;
        uint32_t text_y = island_y + tmxc_dynamic_island.current_height / 2 + 4;
        
        for (int i = 0; tmxc_dynamic_island.notification_text[i] != '\0' && i < 64; i++) {
            char c = tmxc_dynamic_island.notification_text[i];
            
            if (c >= 32 && c <= 126) {
                for (uint8_t row = 0; row < 8; row++) {
                    uint8_t font_row = 0;
                    
                    if (c >= '0' && c <= '9') {
                        static const uint8_t digits[10][8] = {
                            {0x3C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3C, 0x00},
                            {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00},
                            {0x3C, 0x66, 0x06, 0x0C, 0x30, 0x60, 0x7E, 0x00},
                            {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00},
                            {0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x0C, 0x00},
                            {0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00},
                            {0x3C, 0x66, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00},
                            {0x7E, 0x06, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00},
                            {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00},
                            {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x0C, 0x38, 0x00}
                        };
                        if (c >= '0') font_row = digits[c - '0'][row];
                    } else if (c >= 'A' && c <= 'Z') {
                        font_row = 0x66;
                    } else if (c >= 'a' && c <= 'z') {
                        font_row = 0x66;
                    } else if (c == ' ') {
                        font_row = 0x00;
                    } else {
                        font_row = 0x66;
                    }
                    
                    for (uint8_t col = 0; col < 8; col++) {
                        if (font_row & (1 << (7 - col))) {
                            uint32_t px = text_x + i * 9 + col;
                            uint32_t py = text_y + row;
                            
                            if (px < tmxc_graphics.width && py < tmxc_graphics.height) {
                                uint32_t pidx = (py * tmxc_graphics.width + px) * 4;
                                uint32_t fg_color = tmxc_dynamic_island.foreground_color;
                                uint32_t dst = (tmxc_graphics.framebuffer[pidx + 3] << 24) | 
                                             (tmxc_graphics.framebuffer[pidx + 2] << 16) |
                                             (tmxc_graphics.framebuffer[pidx + 1] << 8) |
                                             tmxc_graphics.framebuffer[pidx];
                                
                                uint32_t blended = tmxc_alpha_blend(fg_color, dst, tmxc_dynamic_island.alpha);
                                
                                tmxc_graphics.framebuffer[pidx] = blended & 0xFF;
                                tmxc_graphics.framebuffer[pidx + 1] = (blended >> 8) & 0xFF;
                                tmxc_graphics.framebuffer[pidx + 2] = (blended >> 16) & 0xFF;
                                tmxc_graphics.framebuffer[pidx + 3] = (blended >> 24) & 0xFF;
                            }
                        }
                    }
                }
            }
        }
    }
}

uint8_t tmxc_ui_dynamic_island_is_animating(void) {
    return tmxc_dynamic_island.animation_active;
}

static uint64_t tmxc_last_frame_time = 0;
static uint32_t tmxc_frame_time_ms = 16;

void tmxc_graphics_optimize_60fps(void) {
    uint64_t current_time = tmxc_get_cycle_count();
    
    if (tmxc_last_frame_time != 0) {
        uint64_t elapsed_cycles = current_time - tmxc_last_frame_time;
        uint64_t elapsed_ms = (elapsed_cycles * 1000) / tmxc_get_frequency();
        
        if (elapsed_ms < tmxc_frame_time_ms) {
            uint64_t remaining_cycles = ((tmxc_frame_time_ms - elapsed_ms) * tmxc_get_frequency()) / 1000;
            
            while ((tmxc_get_cycle_count() - current_time) < remaining_cycles) {
                tmxc_wfi();
            }
        }
    }
    
    tmxc_last_frame_time = tmxc_get_cycle_count();
}

uint32_t tmxc_graphics_get_fps(void) {
    if (tmxc_last_frame_time == 0) {
        return 0;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed_cycles = current_time - tmxc_last_frame_time;
    uint64_t elapsed_ms = (elapsed_cycles * 1000) / tmxc_get_frequency();
    
    if (elapsed_ms == 0) {
        return 60;
    }
    
    return (1000 / elapsed_ms);
}
