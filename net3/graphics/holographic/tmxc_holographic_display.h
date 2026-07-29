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
#ifndef TMXC_HOLOGRAPHIC_DISPLAY_H
#define TMXC_HOLOGRAPHIC_DISPLAY_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_HOLOGRAPHIC_WIDTH 1920
#define TMXC_HOLOGRAPHIC_HEIGHT 1080
#define TMXC_DEPTH_LAYERS 16
#define TMXC_PARALLAX_STRENGTH 10

typedef struct {
    int16_t x_offset;
    int16_t y_offset;
    int16_t z_depth;
    uint8_t layer_id;
    uint8_t opacity;
} tmxc_depth_layer_t;

typedef struct {
    float pitch;
    float roll;
    float yaw;
    uint64_t last_update_time;
    uint8_t gyroscope_active;
} tmxc_gyroscope_state_t;

typedef struct {
    uint32_t pixel_offset_x[TMXC_HOLOGRAPHIC_WIDTH];
    uint32_t pixel_offset_y[TMXC_HOLOGRAPHIC_HEIGHT];
    tmxc_depth_layer_t layers[TMXC_DEPTH_LAYERS];
    tmxc_gyroscope_state_t gyro_state;
    uint8_t holographic_enabled;
    uint8_t depth_mapping_enabled;
    uint8_t parallax_enabled;
    uint8_t holographic_initialized;
} tmxc_holographic_display_t;

void tmxc_holographic_display_init(void);
void tmxc_holographic_enable(uint8_t enable);
void tmxc_holographic_enable_depth_mapping(uint8_t enable);
void tmxc_holographic_enable_parallax(uint8_t enable);

void tmxc_holographic_update_gyroscope(void);
void tmxc_holographic_calculate_depth_offsets(void);
void tmxc_holographic_apply_parallax(void);

void tmxc_holographic_add_layer(uint8_t layer_id, int16_t z_depth, uint8_t opacity);
void tmxc_holographic_remove_layer(uint8_t layer_id);
tmxc_depth_layer_t* tmxc_holographic_get_layer(uint8_t layer_id);

void tmxc_holographic_render_frame(void);
void tmxc_holographic_set_parallax_strength(uint8_t strength);
uint8_t tmxc_holographic_get_parallax_strength(void);

#endif
