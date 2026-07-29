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
    uint32_t fps;
    uint8_t* frame_buffer;
    uint32_t buffer_size;
    uint32_t current_frame;
    uint8_t initialized;
    uint8_t streaming;
    uint8_t front_camera;
    uint32_t exposure;
    uint32_t gain;
    uint32_t white_balance;
    uint32_t focus_mode;
    uint32_t iso;
    uint8_t face_detection_enabled;
    uint8_t auto_focus_enabled;
    uint32_t zoom_factor;
    uint8_t zoom_enabled;
} tmxc_camera_t;

static tmxc_camera_t tmxc_camera;

static void tmxc_mipi_csi2_init(void) {
    uint32_t dphy_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_CSI2_DPHY_CTRL);
    dphy_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_CSI2_DPHY_CTRL, dphy_ctrl);
    
    dphy_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_CSI2_DPHY_CTRL);
    dphy_ctrl |= (1 << 8);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_CSI2_DPHY_CTRL, dphy_ctrl);
    
    tmxc_timer_delay_ms(10);
    
    uint32_t csi2_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_CSI2_CTRL);
    csi2_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_CSI2_CTRL, csi2_ctrl);
    
    csi2_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_CSI2_CTRL);
    csi2_ctrl |= (1 << 16);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_CSI2_CTRL, csi2_ctrl);
    
    tmxc_timer_delay_ms(10);
}

static void tmxc_mipi_csi2_configure(uint32_t lanes, uint32_t data_rate) {
    uint32_t dphy_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_CSI2_DPHY_CTRL);
    dphy_ctrl = (dphy_ctrl & ~0xF00) | ((lanes - 1) << 8);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_CSI2_DPHY_CTRL, dphy_ctrl);
    
    uint32_t csi2_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_CSI2_CTRL);
    csi2_ctrl = (csi2_ctrl & ~0xFFFF0000) | (data_rate << 16);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_CSI2_CTRL, csi2_ctrl);
}

static void tmxc_isp_init(void) {
    uint32_t isp_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE);
    isp_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE, isp_ctrl);
    
    isp_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE);
    isp_ctrl |= (1 << 8);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE, isp_ctrl);
    
    isp_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE);
    isp_ctrl |= (1 << 16);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE, isp_ctrl);
    
    tmxc_timer_delay_ms(10);
}

static void tmxc_isp_configure(uint32_t width, uint32_t height, uint32_t format) {
    uint32_t isp_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE);
    isp_ctrl = (isp_ctrl & ~0xFFFF) | width;
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE, isp_ctrl);
    
    isp_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE);
    isp_ctrl = (isp_ctrl & ~0xFFFF0000) | (height << 16);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE, isp_ctrl);
    
    isp_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE);
    isp_ctrl = (isp_ctrl & ~0xF00000000ULL) | (format << 32);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE, isp_ctrl);
}

static void tmxc_camera_i2c_write_reg(uint8_t reg, uint16_t value) {
    uint32_t i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg |= (1 << 15);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg = (i2c_reg & ~0xFF) | 0x20;
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg = (i2c_reg & ~0xFF00) | (reg << 8);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg = (i2c_reg & ~0xFFFF0000) | (value << 16);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    
    while ((tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE) & (1 << 0))) {
        ;
    }
}

static uint16_t tmxc_camera_i2c_read_reg(uint8_t reg) {
    uint32_t i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg |= (1 << 15);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg = (i2c_reg & ~0xFF) | 0x21;
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg = (i2c_reg & ~0xFF00) | (reg << 8);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    
    while ((tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE) & (1 << 0))) {
        ;
    }
    
    uint16_t value = (tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE) >> 16) & 0xFFFF;
    return value;
}

static void tmxc_camera_sensor_init(void) {
    tmxc_camera_i2c_write_reg(0x0100, 0x00);
    tmxc_timer_delay_ms(10);
    
    tmxc_camera_i2c_write_reg(0x0100, 0x01);
    tmxc_timer_delay_ms(50);
    
    tmxc_camera_i2c_write_reg(0x0103, 0x01);
    
    tmxc_camera_i2c_write_reg(0x0301, 0x05);
    tmxc_camera_i2c_write_reg(0x0302, 0x00);
    tmxc_camera_i2c_write_reg(0x0303, 0x00);
    tmxc_camera_i2c_write_reg(0x0304, 0x03);
    tmxc_camera_i2c_write_reg(0x0305, 0x01);
    tmxc_camera_i2c_write_reg(0x0306, 0x00);
    tmxc_camera_i2c_write_reg(0x0307, 0x39);
    
    tmxc_camera_i2c_write_reg(0x0308, 0x00);
    tmxc_camera_i2c_write_reg(0x0309, 0x08);
    tmxc_camera_i2c_write_reg(0x030A, 0x00);
    tmxc_camera_i2c_write_reg(0x030B, 0x04);
    
    tmxc_camera_i2c_write_reg(0x030C, 0x00);
    tmxc_camera_i2c_write_reg(0x030D, 0x04);
    tmxc_camera_i2c_write_reg(0x030E, 0x00);
    tmxc_camera_i2c_write_reg(0x030F, 0x02);
    
    tmxc_camera_i2c_write_reg(0x0310, 0x00);
    tmxc_camera_i2c_write_reg(0x0311, 0x02);
    tmxc_camera_i2c_write_reg(0x0312, 0x00);
    tmxc_camera_i2c_write_reg(0x0313, 0x01);
    
    tmxc_camera_i2c_write_reg(0x0314, 0x00);
    tmxc_camera_i2c_write_reg(0x0315, 0x01);
    tmxc_camera_i2c_write_reg(0x0316, 0x00);
    tmxc_camera_i2c_write_reg(0x0317, 0x01);
    
    tmxc_camera_i2c_write_reg(0x0318, 0x00);
    tmxc_camera_i2c_write_reg(0x0319, 0x01);
    tmxc_camera_i2c_write_reg(0x031A, 0x00);
    tmxc_camera_i2c_write_reg(0x031B, 0x01);
    
    tmxc_camera_i2c_write_reg(0x031C, 0x00);
    tmxc_camera_i2c_write_reg(0x031D, 0x01);
    tmxc_camera_i2c_write_reg(0x031E, 0x00);
    tmxc_camera_i2c_write_reg(0x031F, 0x01);
    
    tmxc_camera_i2c_write_reg(0x0114, 0x01);
    tmxc_camera_i2c_write_reg(0x0115, 0x10);
    
    tmxc_camera_i2c_write_reg(0x0202, 0x02);
    tmxc_camera_i2c_write_reg(0x0203, 0x80);
    
    tmxc_camera_i2c_write_reg(0x0204, 0x00);
    tmxc_camera_i2c_write_reg(0x0205, 0x10);
    
    tmxc_camera_i2c_write_reg(0x0207, 0x20);
    
    tmxc_camera_i2c_write_reg(0x0208, 0x02);
    tmxc_camera_i2c_write_reg(0x0209, 0x00);
    
    tmxc_camera_i2c_write_reg(0x020A, 0x01);
    tmxc_camera_i2c_write_reg(0x020B, 0x50);
    
    tmxc_camera_i2c_write_reg(0x020E, 0x01);
    tmxc_camera_i2c_write_reg(0x020F, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0210, 0x01);
    tmxc_camera_i2c_write_reg(0x0211, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0212, 0x01);
    tmxc_camera_i2c_write_reg(0x0213, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0214, 0x01);
    tmxc_camera_i2c_write_reg(0x0215, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0216, 0x01);
    tmxc_camera_i2c_write_reg(0x0217, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0218, 0x01);
    tmxc_camera_i2c_write_reg(0x0219, 0x00);
    
    tmxc_camera_i2c_write_reg(0x021A, 0x01);
    tmxc_camera_i2c_write_reg(0x021B, 0x00);
    
    tmxc_camera_i2c_write_reg(0x021C, 0x01);
    tmxc_camera_i2c_write_reg(0x021D, 0x00);
    
    tmxc_camera_i2c_write_reg(0x021E, 0x01);
    tmxc_camera_i2c_write_reg(0x021F, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0220, 0x01);
    tmxc_camera_i2c_write_reg(0x0221, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0222, 0x01);
    tmxc_camera_i2c_write_reg(0x0223, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0224, 0x01);
    tmxc_camera_i2c_write_reg(0x0225, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0226, 0x01);
    tmxc_camera_i2c_write_reg(0x0227, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0228, 0x01);
    tmxc_camera_i2c_write_reg(0x0229, 0x00);
    
    tmxc_camera_i2c_write_reg(0x022A, 0x01);
    tmxc_camera_i2c_write_reg(0x022B, 0x00);
    
    tmxc_camera_i2c_write_reg(0x022C, 0x01);
    tmxc_camera_i2c_write_reg(0x022D, 0x00);
    
    tmxc_camera_i2c_write_reg(0x022E, 0x01);
    tmxc_camera_i2c_write_reg(0x022F, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0230, 0x01);
    tmxc_camera_i2c_write_reg(0x0231, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0232, 0x01);
    tmxc_camera_i2c_write_reg(0x0233, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0234, 0x01);
    tmxc_camera_i2c_write_reg(0x0235, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0236, 0x01);
    tmxc_camera_i2c_write_reg(0x0237, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0238, 0x01);
    tmxc_camera_i2c_write_reg(0x0239, 0x00);
    
    tmxc_camera_i2c_write_reg(0x023A, 0x01);
    tmxc_camera_i2c_write_reg(0x023B, 0x00);
    
    tmxc_camera_i2c_write_reg(0x023C, 0x01);
    tmxc_camera_i2c_write_reg(0x023D, 0x00);
    
    tmxc_camera_i2c_write_reg(0x023E, 0x01);
    tmxc_camera_i2c_write_reg(0x023F, 0x00);
    
    tmxc_camera_i2c_write_reg(0x0100, 0x01);
    
    tmxc_timer_delay_ms(100);
}

void tmxc_camera_init(void) {
    tmxc_camera.width = 1920;
    tmxc_camera.height = 1080;
    tmxc_camera.format = 0;
    tmxc_camera.fps = 30;
    tmxc_camera.frame_buffer = NULL;
    tmxc_camera.buffer_size = 0;
    tmxc_camera.current_frame = 0;
    tmxc_camera.initialized = 0;
    tmxc_camera.streaming = 0;
    tmxc_camera.front_camera = 1;
    tmxc_camera.exposure = 100;
    tmxc_camera.gain = 100;
    tmxc_camera.white_balance = 5000;
    tmxc_camera.focus_mode = 0;
    tmxc_camera.iso = 100;
    tmxc_camera.face_detection_enabled = 0;
    tmxc_camera.auto_focus_enabled = 0;
    tmxc_camera.zoom_factor = 10;
    tmxc_camera.zoom_enabled = 0;
    
    tmxc_camera.buffer_size = tmxc_camera.width * tmxc_camera.height * 2;
    tmxc_camera.frame_buffer = (uint8_t*)tmxc_malloc(tmxc_camera.buffer_size);
    
    if (tmxc_camera.frame_buffer != NULL) {
        tmxc_mipi_csi2_init();
        tmxc_mipi_csi2_configure(4, 800);
        tmxc_isp_init();
        tmxc_isp_configure(tmxc_camera.width, tmxc_camera.height, tmxc_camera.format);
        tmxc_camera_sensor_init();
        tmxc_camera.initialized = 1;
    }
}

void tmxc_camera_set_resolution(uint32_t width, uint32_t height) {
    if (width < 320) width = 320;
    if (width > 3840) width = 3840;
    if (height < 240) height = 240;
    if (height > 2160) height = 2160;
    
    tmxc_camera.width = width;
    tmxc_camera.height = height;
    
    tmxc_camera.buffer_size = width * height * 2;
    
    if (tmxc_camera.frame_buffer != NULL) {
        tmxc_free(tmxc_camera.frame_buffer);
    }
    tmxc_camera.frame_buffer = (uint8_t*)tmxc_malloc(tmxc_camera.buffer_size);
    
    tmxc_isp_configure(width, height, tmxc_camera.format);
}

void tmxc_camera_get_resolution(uint32_t* width, uint32_t* height) {
    if (width != NULL) {
        *width = tmxc_camera.width;
    }
    if (height != NULL) {
        *height = tmxc_camera.height;
    }
}

void tmxc_camera_set_fps(uint32_t fps) {
    if (fps < 1) fps = 1;
    if (fps > 120) fps = 120;
    
    tmxc_camera.fps = fps;
    
    uint32_t csi2_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_CSI2_CTRL);
    csi2_ctrl = (csi2_ctrl & ~0xFF) | fps;
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_CSI2_CTRL, csi2_ctrl);
}

uint32_t tmxc_camera_get_fps(void) {
    return tmxc_camera.fps;
}

void tmxc_camera_set_format(uint32_t format) {
    tmxc_camera.format = format;
    
    if (format == 0) {
        tmxc_camera.buffer_size = tmxc_camera.width * tmxc_camera.height * 2;
    } else if (format == 1) {
        tmxc_camera.buffer_size = tmxc_camera.width * tmxc_camera.height * 3;
    } else if (format == 2) {
        tmxc_camera.buffer_size = tmxc_camera.width * tmxc_camera.height * 4;
    }
    
    if (tmxc_camera.frame_buffer != NULL) {
        tmxc_free(tmxc_camera.frame_buffer);
    }
    tmxc_camera.frame_buffer = (uint8_t*)tmxc_malloc(tmxc_camera.buffer_size);
    
    tmxc_isp_configure(tmxc_camera.width, tmxc_camera.height, format);
}

uint32_t tmxc_camera_get_format(void) {
    return tmxc_camera.format;
}

int tmxc_camera_capture(uint8_t* buffer, size_t size) {
    if (!tmxc_camera.initialized || buffer == NULL || size == 0) {
        return -1;
    }
    
    if (size < tmxc_camera.buffer_size) {
        return -2;
    }
    
    uint32_t csi2_stat = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_CSI2_STAT);
    
    while (!(csi2_stat & (1 << 0))) {
        csi2_stat = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_CSI2_STAT);
        tmxc_timer_delay_ms(1);
    }
    
    uint8_t* temp_buffer = (uint8_t*)tmxc_malloc(tmxc_camera.buffer_size);
    
    if (temp_buffer == NULL) {
        return -3;
    }
    
    for (uint32_t i = 0; i < tmxc_camera.buffer_size; i += 4) {
        uint32_t pixel_data = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE + 0x1000);
        
        if (i + 3 < tmxc_camera.buffer_size) {
            temp_buffer[i] = pixel_data & 0xFF;
            temp_buffer[i+1] = (pixel_data >> 8) & 0xFF;
            temp_buffer[i+2] = (pixel_data >> 16) & 0xFF;
            temp_buffer[i+3] = (pixel_data >> 24) & 0xFF;
        } else {
            for (uint32_t j = 0; j < 4 && i + j < tmxc_camera.buffer_size; j++) {
                temp_buffer[i+j] = (pixel_data >> (j * 8)) & 0xFF;
            }
        }
    }
    
    if (tmxc_camera.zoom_enabled && tmxc_camera.zoom_factor > 10) {
        uint32_t bytes_per_pixel = 2;
        if (tmxc_camera.format == 1) bytes_per_pixel = 3;
        else if (tmxc_camera.format == 2) bytes_per_pixel = 4;
        
        tmxc_camera_crop_and_scale(temp_buffer, tmxc_camera.width, tmxc_camera.height,
                                    buffer, tmxc_camera.width, tmxc_camera.height,
                                    tmxc_camera.zoom_factor, bytes_per_pixel);
    } else {
        for (uint32_t i = 0; i < tmxc_camera.buffer_size; i++) {
            buffer[i] = temp_buffer[i];
        }
    }
    
    tmxc_free(temp_buffer);
    
    tmxc_camera.current_frame++;
    
    return 0;
}

void tmxc_camera_start_stream(void) {
    if (!tmxc_camera.initialized) {
        return;
    }
    
    uint32_t csi2_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_CSI2_CTRL);
    csi2_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_CSI2_CTRL, csi2_ctrl);
    
    uint32_t isp_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE);
    isp_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE, isp_ctrl);
    
    tmxc_camera.streaming = 1;
}

void tmxc_camera_stop_stream(void) {
    if (!tmxc_camera.initialized) {
        return;
    }
    
    uint32_t csi2_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_CSI2_CTRL);
    csi2_ctrl &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_CSI2_CTRL, csi2_ctrl);
    
    uint32_t isp_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE);
    isp_ctrl &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE, isp_ctrl);
    
    tmxc_camera.streaming = 0;
}

uint8_t tmxc_camera_is_streaming(void) {
    return tmxc_camera.streaming;
}

void tmxc_camera_set_exposure(uint32_t exposure) {
    if (exposure < 1) exposure = 1;
    if (exposure > 10000) exposure = 10000;
    
    tmxc_camera.exposure = exposure;
    
    tmxc_camera_i2c_write_reg(0x0200, exposure & 0xFF);
    tmxc_camera_i2c_write_reg(0x0201, (exposure >> 8) & 0xFF);
}

uint32_t tmxc_camera_get_exposure(void) {
    return tmxc_camera.exposure;
}

void tmxc_camera_set_gain(uint32_t gain) {
    if (gain < 1) gain = 1;
    if (gain > 1000) gain = 1000;
    
    tmxc_camera.gain = gain;
    
    tmxc_camera_i2c_write_reg(0x0204, gain & 0xFF);
    tmxc_camera_i2c_write_reg(0x0205, (gain >> 8) & 0xFF);
}

uint32_t tmxc_camera_get_gain(void) {
    return tmxc_camera.gain;
}

void tmxc_camera_set_white_balance(uint32_t kelvin) {
    if (kelvin < 2000) kelvin = 2000;
    if (kelvin > 10000) kelvin = 10000;
    
    tmxc_camera.white_balance = kelvin;
    
    uint32_t r_gain = (kelvin * 256) / 5000;
    uint32_t b_gain = (5000 * 256) / kelvin;
    
    tmxc_camera_i2c_write_reg(0x0220, r_gain & 0xFF);
    tmxc_camera_i2c_write_reg(0x0221, (r_gain >> 8) & 0xFF);
    tmxc_camera_i2c_write_reg(0x0222, b_gain & 0xFF);
    tmxc_camera_i2c_write_reg(0x0223, (b_gain >> 8) & 0xFF);
}

uint32_t tmxc_camera_get_white_balance(void) {
    return tmxc_camera.white_balance;
}

void tmxc_camera_set_focus_mode(uint32_t mode) {
    tmxc_camera.focus_mode = mode;
    
    if (mode == 0) {
        tmxc_camera.auto_focus_enabled = 0;
    } else if (mode == 1) {
        tmxc_camera.auto_focus_enabled = 1;
    }
    
    tmxc_camera_i2c_write_reg(0x0240, mode);
}

uint32_t tmxc_camera_get_focus_mode(void) {
    return tmxc_camera.focus_mode;
}

void tmxc_camera_set_iso(uint32_t iso) {
    if (iso < 50) iso = 50;
    if (iso > 6400) iso = 6400;
    
    tmxc_camera.iso = iso;
    
    uint32_t gain = (iso * 100) / 100;
    tmxc_camera_set_gain(gain);
}

uint32_t tmxc_camera_get_iso(void) {
    return tmxc_camera.iso;
}

void tmxc_camera_enable_face_detection(uint8_t enable) {
    tmxc_camera.face_detection_enabled = enable;
    
    uint32_t isp_ctrl = tmxc_read32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE);
    if (enable) {
        isp_ctrl |= (1 << 24);
    } else {
        isp_ctrl &= ~(1 << 24);
    }
    tmxc_write32((volatile uint32_t*)TMXC_CAMERA_ISP_BASE, isp_ctrl);
}

uint8_t tmxc_camera_is_face_detection_enabled(void) {
    return tmxc_camera.face_detection_enabled;
}

void tmxc_camera_select_camera(uint8_t front) {
    tmxc_camera.front_camera = front;
    
    uint32_t gpio_reg = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE);
    if (front) {
        gpio_reg |= (1 << 10);
    } else {
        gpio_reg &= ~(1 << 10);
    }
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_BASE, gpio_reg);
    
    tmxc_timer_delay_ms(50);
}

uint8_t tmxc_camera_is_front_camera(void) {
    return tmxc_camera.front_camera;
}

static void tmxc_camera_nearest_neighbor_scale(const uint8_t* src, uint32_t src_w, uint32_t src_h,
                                                uint8_t* dst, uint32_t dst_w, uint32_t dst_h,
                                                uint32_t bytes_per_pixel) {
    uint32_t x_ratio = ((src_w << 16) / dst_w) + 1;
    uint32_t y_ratio = ((src_h << 16) / dst_h) + 1;
    
    for (uint32_t y = 0; y < dst_h; y++) {
        uint32_t y2 = ((y * y_ratio) >> 16);
        
        for (uint32_t x = 0; x < dst_w; x++) {
            uint32_t x2 = ((x * x_ratio) >> 16);
            
            uint32_t src_idx = (y2 * src_w + x2) * bytes_per_pixel;
            uint32_t dst_idx = (y * dst_w + x) * bytes_per_pixel;
            
            for (uint32_t b = 0; b < bytes_per_pixel; b++) {
                dst[dst_idx + b] = src[src_idx + b];
            }
        }
    }
}

static void tmxc_camera_crop_and_scale(const uint8_t* src, uint32_t src_w, uint32_t src_h,
                                       uint8_t* dst, uint32_t dst_w, uint32_t dst_h,
                                       uint32_t zoom_factor, uint32_t bytes_per_pixel) {
    uint32_t crop_w = (src_w * 10) / zoom_factor;
    uint32_t crop_h = (src_h * 10) / zoom_factor;
    
    if (crop_w < dst_w) crop_w = dst_w;
    if (crop_h < dst_h) crop_h = dst_h;
    
    uint32_t crop_x = (src_w - crop_w) / 2;
    uint32_t crop_y = (src_h - crop_h) / 2;
    
    uint8_t* cropped_buffer = (uint8_t*)tmxc_malloc(crop_w * crop_h * bytes_per_pixel);
    
    if (cropped_buffer == NULL) {
        return;
    }
    
    for (uint32_t y = 0; y < crop_h; y++) {
        for (uint32_t x = 0; x < crop_w; x++) {
            uint32_t src_idx = ((crop_y + y) * src_w + (crop_x + x)) * bytes_per_pixel;
            uint32_t crop_idx = (y * crop_w + x) * bytes_per_pixel;
            
            for (uint32_t b = 0; b < bytes_per_pixel; b++) {
                cropped_buffer[crop_idx + b] = src[src_idx + b];
            }
        }
    }
    
    tmxc_camera_nearest_neighbor_scale(cropped_buffer, crop_w, crop_h,
                                        dst, dst_w, dst_h, bytes_per_pixel);
    
    tmxc_free(cropped_buffer);
}

void tmxc_camera_set_zoom(uint32_t zoom_factor) {
    if (zoom_factor < 10) zoom_factor = 10;
    if (zoom_factor > 100) zoom_factor = 100;
    
    tmxc_camera.zoom_factor = zoom_factor;
    tmxc_camera.zoom_enabled = (zoom_factor > 10) ? 1 : 0;
    
    tmxc_uart_puts("[CAMERA] Zoom factor set to: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = zoom_factor;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

uint32_t tmxc_camera_get_zoom(void) {
    return tmxc_camera.zoom_factor;
}

void tmxc_camera_enable_zoom(uint8_t enable) {
    tmxc_camera.zoom_enabled = enable;
}

uint8_t tmxc_camera_is_zoom_enabled(void) {
    return tmxc_camera.zoom_enabled;
}
