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
#include "tmxc_holographic_display.h"

static tmxc_holographic_display_t tmxc_holographic;

void tmxc_holographic_display_init(void) {
    for (uint32_t i = 0; i < TMXC_HOLOGRAPHIC_WIDTH; i++) {
        tmxc_holographic.pixel_offset_x[i] = 0;
    }
    for (uint32_t i = 0; i < TMXC_HOLOGRAPHIC_HEIGHT; i++) {
        tmxc_holographic.pixel_offset_y[i] = 0;
    }
    
    for (int i = 0; i < TMXC_DEPTH_LAYERS; i++) {
        tmxc_holographic.layers[i].x_offset = 0;
        tmxc_holographic.layers[i].y_offset = 0;
        tmxc_holographic.layers[i].z_depth = i * 10;
        tmxc_holographic.layers[i].layer_id = i;
        tmxc_holographic.layers[i].opacity = 255;
    }
    
    tmxc_holographic.gyro_state.pitch = 0.0f;
    tmxc_holographic.gyro_state.roll = 0.0f;
    tmxc_holographic.gyro_state.yaw = 0.0f;
    tmxc_holographic.gyro_state.last_update_time = 0;
    tmxc_holographic.gyro_state.gyroscope_active = 1;
    
    tmxc_holographic.holographic_enabled = 1;
    tmxc_holographic.depth_mapping_enabled = 1;
    tmxc_holographic.parallax_enabled = 1;
    tmxc_holographic.holographic_initialized = 1;
    
    tmxc_uart_puts("[HOLOGRAPHIC] Holographic Depth-Display initialized\r\n");
}

void tmxc_holographic_enable(uint8_t enable) {
    tmxc_holographic.holographic_enabled = enable;
    if (enable) {
        tmxc_uart_puts("[HOLOGRAPHIC] Holographic display enabled\r\n");
    } else {
        tmxc_uart_puts("[HOLOGRAPHIC] Holographic display disabled\r\n");
    }
}

void tmxc_holographic_enable_depth_mapping(uint8_t enable) {
    tmxc_holographic.depth_mapping_enabled = enable;
}

void tmxc_holographic_enable_parallax(uint8_t enable) {
    tmxc_holographic.parallax_enabled = enable;
}

void tmxc_holographic_update_gyroscope(void) {
    if (!tmxc_holographic.holographic_initialized || !tmxc_holographic.gyro_state.gyroscope_active) {
        return;
    }
    
    int16_t gyro_x = tmxc_env_get_accel_x();
    int16_t gyro_y = tmxc_env_get_accel_y();
    int16_t gyro_z = tmxc_env_get_accel_z();
    
    tmxc_holographic.gyro_state.pitch = (float)gyro_x / 1000.0f;
    tmxc_holographic.gyro_state.roll = (float)gyro_y / 1000.0f;
    tmxc_holographic.gyro_state.yaw = (float)gyro_z / 1000.0f;
    
    tmxc_holographic.gyro_state.last_update_time = tmxc_get_cycle_count();
}

void tmxc_holographic_calculate_depth_offsets(void) {
    if (!tmxc_holographic.holographic_enabled || !tmxc_holographic.depth_mapping_enabled) {
        return;
    }
    
    tmxc_holographic_update_gyroscope();
    
    float pitch = tmxc_holographic.gyro_state.pitch;
    float roll = tmxc_holographic.gyro_state.roll;
    
    for (uint32_t i = 0; i < TMXC_HOLOGRAPHIC_WIDTH; i++) {
        int32_t offset = (int32_t)(pitch * TMXC_PARALLAX_STRENGTH);
        tmxc_holographic.pixel_offset_x[i] = offset;
    }
    
    for (uint32_t i = 0; i < TMXC_HOLOGRAPHIC_HEIGHT; i++) {
        int32_t offset = (int32_t)(roll * TMXC_PARALLAX_STRENGTH);
        tmxc_holographic.pixel_offset_y[i] = offset;
    }
}

void tmxc_holographic_apply_parallax(void) {
    if (!tmxc_holographic.holographic_enabled || !tmxc_holographic.parallax_enabled) {
        return;
    }
    
    tmxc_holographic_calculate_depth_offsets();
    
    for (int i = 0; i < TMXC_DEPTH_LAYERS; i++) {
        float depth_factor = (float)tmxc_holographic.layers[i].z_depth / 100.0f;
        
        tmxc_holographic.layers[i].x_offset = (int16_t)(tmxc_holographic.gyro_state.pitch * TMXC_PARALLAX_STRENGTH * depth_factor);
        tmxc_holographic.layers[i].y_offset = (int16_t)(tmxc_holographic.gyro_state.roll * TMXC_PARALLAX_STRENGTH * depth_factor);
    }
}

void tmxc_holographic_add_layer(uint8_t layer_id, int16_t z_depth, uint8_t opacity) {
    if (layer_id >= TMXC_DEPTH_LAYERS) {
        return;
    }
    
    tmxc_holographic.layers[layer_id].layer_id = layer_id;
    tmxc_holographic.layers[layer_id].z_depth = z_depth;
    tmxc_holographic.layers[layer_id].opacity = opacity;
}

void tmxc_holographic_remove_layer(uint8_t layer_id) {
    if (layer_id >= TMXC_DEPTH_LAYERS) {
        return;
    }
    
    tmxc_holographic.layers[layer_id].z_depth = 0;
    tmxc_holographic.layers[layer_id].opacity = 0;
}

tmxc_depth_layer_t* tmxc_holographic_get_layer(uint8_t layer_id) {
    if (layer_id >= TMXC_DEPTH_LAYERS) {
        return NULL;
    }
    
    return &tmxc_holographic.layers[layer_id];
}

void tmxc_holographic_render_frame(void) {
    if (!tmxc_holographic.holographic_enabled) {
        return;
    }
    
    tmxc_holographic_apply_parallax();
}

void tmxc_holographic_set_parallax_strength(uint8_t strength) {
    if (strength > 20) {
        strength = 20;
    }
}

uint8_t tmxc_holographic_get_parallax_strength(void) {
    return TMXC_PARALLAX_STRENGTH;
}
