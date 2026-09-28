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
    uint32_t current_angle;
    uint32_t intensity;
    uint32_t mode;
    uint8_t enabled;
    uint8_t active;
    uint64_t last_update;
    uint32_t threshold_angle;
    uint32_t blur_strength;
    float darken_factor;
    uint32_t screen_width;
    uint32_t screen_height;
    uint8_t* original_buffer;
    uint8_t* processed_buffer;
    uint8_t* camera_buffer;
    uint8_t* blur_temp_buffer;
    uint32_t buffer_size;
    uint32_t camera_buffer_size;
    uint32_t camera_x;
    uint32_t camera_y;
    uint32_t gaze_x;
    uint32_t gaze_y;
    uint8_t user_detected;
    uint8_t registered_user;
} tmxc_stealthview_t;

static tmxc_stealthview_t tmxc_stealthview;

static int32_t tmxc_stealthview_calculate_angle(uint32_t gaze_x, uint32_t gaze_y, 
                                                  uint32_t screen_center_x, uint32_t screen_center_y) {
    int32_t dx = (int32_t)gaze_x - (int32_t)screen_center_x;
    int32_t dy = (int32_t)gaze_y - (int32_t)screen_center_y;
    
    if (dx == 0 && dy == 0) {
        return 0;
    }
    
    int32_t abs_dx = dx < 0 ? -dx : dx;
    int32_t abs_dy = dy < 0 ? -dy : dy;
    
    int32_t angle;
    
    if (abs_dx > abs_dy) {
        int32_t ratio = (abs_dy * 100) / abs_dx;
        if (dx > 0) {
            angle = (dy >= 0) ? ratio : (360 - ratio);
        } else {
            angle = (dy >= 0) ? (180 - ratio) : (180 + ratio);
        }
    } else {
        int32_t ratio = (abs_dx * 100) / abs_dy;
        if (dy > 0) {
            angle = (dx >= 0) ? (90 - ratio) : (90 + ratio);
        } else {
            angle = (dx >= 0) ? (270 + ratio) : (270 - ratio);
        }
    }
    
    if (angle < 0) {
        angle += 360;
    }
    if (angle >= 360) {
        angle -= 360;
    }
    
    return angle;
}

static uint32_t tmxc_stealthview_calculate_intensity(uint32_t angle, uint32_t threshold) {
    if (angle <= threshold) {
        return 0;
    }
    
    uint32_t excess_angle = angle - threshold;
    uint32_t max_excess = 90 - threshold;
    
    uint32_t intensity = (excess_angle * 100) / max_excess;
    if (intensity > 100) {
        intensity = 100;
    }
    
    return intensity;
}

static void tmxc_stealthview_anisotropic_blur(uint8_t* buffer, uint32_t width, uint32_t height, 
                                                uint32_t blur_strength, uint32_t angle) {
    if (buffer == NULL || width == 0 || height == 0 || blur_strength == 0) {
        return;
    }
    
    if (tmxc_stealthview.blur_temp_buffer == NULL) {
        return;
    }
    
    uint8_t* temp_buffer = tmxc_stealthview.blur_temp_buffer;
    
    int32_t step_x, step_y;
    
    if (angle >= 338 || angle < 23) {
        step_x = blur_strength;
        step_y = 0;
    } else if (angle >= 23 && angle < 68) {
        step_x = blur_strength;
        step_y = blur_strength / 2;
    } else if (angle >= 68 && angle < 113) {
        step_x = 0;
        step_y = blur_strength;
    } else if (angle >= 113 && angle < 158) {
        step_x = -blur_strength;
        step_y = blur_strength / 2;
    } else if (angle >= 158 && angle < 203) {
        step_x = -blur_strength;
        step_y = 0;
    } else if (angle >= 203 && angle < 248) {
        step_x = -blur_strength;
        step_y = -blur_strength / 2;
    } else if (angle >= 248 && angle < 293) {
        step_x = 0;
        step_y = -blur_strength;
    } else {
        step_x = blur_strength;
        step_y = -blur_strength / 2;
    }
    
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            uint32_t idx = (y * width + x) * 4;
            
            uint32_t r_sum = 0, g_sum = 0, b_sum = 0, a_sum = 0;
            uint32_t count = 0;
            
            for (int32_t offset = -(int32_t)blur_strength; offset <= (int32_t)blur_strength; offset++) {
                int32_t blur_x = x + (offset * step_x) / blur_strength;
                int32_t blur_y = y + (offset * step_y) / blur_strength;
                
                if (blur_x >= 0 && blur_x < (int32_t)width && 
                    blur_y >= 0 && blur_y < (int32_t)height) {
                    
                    uint32_t blur_idx = (blur_y * width + blur_x) * 4;
                    r_sum += buffer[blur_idx];
                    g_sum += buffer[blur_idx + 1];
                    b_sum += buffer[blur_idx + 2];
                    a_sum += buffer[blur_idx + 3];
                    count++;
                }
            }
            
            if (count > 0) {
                temp_buffer[idx] = r_sum / count;
                temp_buffer[idx + 1] = g_sum / count;
                temp_buffer[idx + 2] = b_sum / count;
                temp_buffer[idx + 3] = a_sum / count;
            }
        }
    }
    
    for (uint32_t i = 0; i < width * height * 4; i++) {
        buffer[i] = temp_buffer[i];
    }
}

static void tmxc_stealthview_darken(uint8_t* buffer, uint32_t width, uint32_t height, 
                                     float darken_factor) {
    if (buffer == NULL || width == 0 || height == 0 || darken_factor >= 1.0f) {
        return;
    }
    
    for (uint32_t i = 0; i < width * height * 4; i += 4) {
        buffer[i] = (uint8_t)(buffer[i] * darken_factor);
        buffer[i + 1] = (uint8_t)(buffer[i + 1] * darken_factor);
        buffer[i + 2] = (uint8_t)(buffer[i + 2] * darken_factor);
    }
}

static void tmxc_stealthview_subpixel_polarization(uint8_t* buffer, uint32_t width, uint32_t height, 
                                                   uint32_t angle, uint32_t intensity) {
    if (buffer == NULL || width == 0 || height == 0) {
        return;
    }
    
    uint32_t polarization_factor = intensity / 2;
    
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            uint32_t idx = (y * width + x) * 4;
            
            uint8_t subpixel_phase = (x + y) % 3;
            uint32_t phase_shift = (angle + subpixel_phase * 120) % 2;
            
            if (phase_shift == 0) {
                buffer[idx] = (uint8_t)((buffer[idx] * (200 - polarization_factor)) / 200);
                buffer[idx + 1] = (uint8_t)((buffer[idx + 1] * (200 - (polarization_factor * 3 / 5))) / 200);
                buffer[idx + 2] = (uint8_t)((buffer[idx + 2] * (200 - (polarization_factor * 7 / 10))) / 200);
            } else {
                buffer[idx] = (uint8_t)((buffer[idx] * (200 - (polarization_factor * 7 / 10))) / 200);
                buffer[idx + 1] = (uint8_t)((buffer[idx + 1] * (200 - (polarization_factor * 3 / 5))) / 200);
                buffer[idx + 2] = (uint8_t)((buffer[idx + 2] * (200 - polarization_factor)) / 200);
            }
        }
    }
}

static void tmxc_stealthview_detect_gaze(void) {
    if (tmxc_stealthview.camera_buffer == NULL) {
        tmxc_stealthview.user_detected = 0;
        return;
    }
    
    int capture_result = tmxc_camera_capture(tmxc_stealthview.camera_buffer, tmxc_stealthview.camera_buffer_size);
    
    if (capture_result != 0) {
        tmxc_stealthview.user_detected = 0;
        return;
    }
    
    uint32_t face_count = 0;
    uint32_t best_face_x = 0;
    uint32_t best_face_y = 0;
    uint32_t best_face_size = 0;
    
    for (uint32_t y = 100; y < 980; y += 40) {
        for (uint32_t x = 100; x < 1820; x += 40) {
            uint32_t brightness = 0;
            for (uint32_t dy = 0; dy < 10; dy++) {
                for (uint32_t dx = 0; dx < 10; dx++) {
                    uint32_t pixel_idx = ((y + dy) * 1920 + (x + dx)) * 2;
                    brightness += tmxc_stealthview.camera_buffer[pixel_idx];
                }
            }
            brightness /= 100;
            
            if (brightness > 80 && brightness < 180) {
                uint32_t face_size = 0;
                for (uint32_t fy = y - 30; fy < y + 30 && fy < 1080; fy++) {
                    for (uint32_t fx = x - 30; fx < x + 30 && fx < 1920; fx++) {
                        uint32_t pixel_idx = (fy * 1920 + fx) * 2;
                        uint32_t pixel_brightness = tmxc_stealthview.camera_buffer[pixel_idx];
                        if (pixel_brightness > 80 && pixel_brightness < 180) {
                            face_size++;
                        }
                    }
                }
                
                if (face_size > 300 && face_size > best_face_size) {
                    best_face_size = face_size;
                    best_face_x = x;
                    best_face_y = y;
                    face_count++;
                }
            }
        }
    }
    
    if (face_count > 0 && best_face_size > 600) {
        tmxc_stealthview.user_detected = 1;
        tmxc_stealthview.gaze_x = best_face_x;
        tmxc_stealthview.gaze_y = best_face_y;
    } else {
        tmxc_stealthview.user_detected = 0;
    }
}

static void tmxc_stealthview_update_display_controller(void) {
    uint32_t lcd_ctrl = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL);
    
    if (tmxc_stealthview.active && tmxc_stealthview.intensity > 50) {
        lcd_ctrl |= (1 << 16);
        lcd_ctrl |= (1 << 17);
    } else {
        lcd_ctrl &= ~(1 << 16);
        lcd_ctrl &= ~(1 << 17);
    }
    
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL, lcd_ctrl);
    
    uint32_t lcd_ctrl2 = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL2);
    
    uint32_t backlight_level = 100 - (tmxc_stealthview.intensity / 2);
    if (backlight_level < 10) {
        backlight_level = 10;
    }
    
    lcd_ctrl2 = (lcd_ctrl2 & ~0xFF) | backlight_level;
    tmxc_write32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL2, lcd_ctrl2);
}

void tmxc_stealthview_init(void) {
    tmxc_stealthview.current_angle = 0;
    tmxc_stealthview.intensity = 0;
    tmxc_stealthview.mode = 0;
    tmxc_stealthview.enabled = 0;
    tmxc_stealthview.active = 0;
    tmxc_stealthview.last_update = 0;
    tmxc_stealthview.threshold_angle = TMXC_STEALTHVIEW_ANGLE_THRESHOLD;
    tmxc_stealthview.blur_strength = TMXC_STEALTHVIEW_BLUR_STRENGTH;
    tmxc_stealthview.darken_factor = TMXC_STEALTHVIEW_DARKEN_FACTOR;
    tmxc_stealthview.screen_width = 1920;
    tmxc_stealthview.screen_height = 1080;
    tmxc_stealthview.original_buffer = NULL;
    tmxc_stealthview.processed_buffer = NULL;
    tmxc_stealthview.camera_buffer = NULL;
    tmxc_stealthview.blur_temp_buffer = NULL;
    tmxc_stealthview.buffer_size = 0;
    tmxc_stealthview.camera_buffer_size = 0;
    tmxc_stealthview.camera_x = 0;
    tmxc_stealthview.camera_y = 0;
    tmxc_stealthview.gaze_x = 960;
    tmxc_stealthview.gaze_y = 540;
    tmxc_stealthview.user_detected = 0;
    tmxc_stealthview.registered_user = 0;
    
    tmxc_stealthview.buffer_size = tmxc_stealthview.screen_width * tmxc_stealthview.screen_height * 4;
    tmxc_stealthview.camera_buffer_size = tmxc_stealthview.screen_width * tmxc_stealthview.screen_height * 2;
    
    tmxc_stealthview.original_buffer = (uint8_t*)tmxc_malloc(tmxc_stealthview.buffer_size);
    tmxc_stealthview.processed_buffer = (uint8_t*)tmxc_malloc(tmxc_stealthview.buffer_size);
    tmxc_stealthview.camera_buffer = (uint8_t*)tmxc_malloc(tmxc_stealthview.camera_buffer_size);
    tmxc_stealthview.blur_temp_buffer = (uint8_t*)tmxc_malloc(tmxc_stealthview.buffer_size);
    
    if (tmxc_stealthview.original_buffer != NULL && tmxc_stealthview.processed_buffer != NULL &&
        tmxc_stealthview.camera_buffer != NULL && tmxc_stealthview.blur_temp_buffer != NULL) {
        tmxc_stealthview.enabled = 1;
    }
}

void tmxc_stealthview_enable(void) {
    if (!tmxc_stealthview.enabled) {
        return;
    }
    
    tmxc_stealthview.active = 1;
    tmxc_stealthview.registered_user = 1;
}

void tmxc_stealthview_disable(void) {
    tmxc_stealthview.active = 0;
    tmxc_stealthview.intensity = 0;
    tmxc_stealthview.current_angle = 0;
    
    tmxc_stealthview_update_display_controller();
}

void tmxc_stealthview_update(void) {
    if (!tmxc_stealthview.enabled || !tmxc_stealthview.active) {
        return;
    }
    
    if (tmxc_stealthview.original_buffer == NULL || tmxc_stealthview.processed_buffer == NULL) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    if (current_time - tmxc_stealthview.last_update < 1000000) {
        return;
    }
    tmxc_stealthview.last_update = current_time;
    
    tmxc_stealthview_detect_gaze();
    
    if (!tmxc_stealthview.user_detected) {
        tmxc_stealthview.intensity = 100;
        tmxc_stealthview.current_angle = 90;
    } else {
        uint32_t screen_center_x = tmxc_stealthview.screen_width / 2;
        uint32_t screen_center_y = tmxc_stealthview.screen_height / 2;
        
        tmxc_stealthview.current_angle = tmxc_stealthview_calculate_angle(
            tmxc_stealthview.gaze_x, tmxc_stealthview.gaze_y,
            screen_center_x, screen_center_y
        );
        
        tmxc_stealthview.intensity = tmxc_stealthview_calculate_intensity(
            tmxc_stealthview.current_angle,
            tmxc_stealthview.threshold_angle
        );
    }
    
    uint32_t display_ctrl = tmxc_read32((volatile uint32_t*)TMXC_DISPLAY_LCD_CTRL);
    uint32_t framebuffer_addr = display_ctrl & 0xFFFFFFF0;
    
    if (framebuffer_addr != 0) {
        uint8_t* framebuffer = (uint8_t*)framebuffer_addr;
        
        for (uint32_t i = 0; i < tmxc_stealthview.buffer_size; i++) {
            tmxc_stealthview.original_buffer[i] = framebuffer[i];
        }
        
        if (tmxc_stealthview.current_angle > tmxc_stealthview.threshold_angle) {
            for (uint32_t i = 0; i < tmxc_stealthview.buffer_size; i++) {
                tmxc_stealthview.processed_buffer[i] = tmxc_stealthview.original_buffer[i];
            }
            
            tmxc_stealthview_darken(tmxc_stealthview.processed_buffer,
                                   tmxc_stealthview.screen_width,
                                   tmxc_stealthview.screen_height,
                                   tmxc_stealthview.darken_factor);
            
            if (tmxc_stealthview.mode == 1) {
                tmxc_stealthview_anisotropic_blur(tmxc_stealthview.processed_buffer,
                                                  tmxc_stealthview.screen_width,
                                                  tmxc_stealthview.screen_height,
                                                  tmxc_stealthview.blur_strength,
                                                  tmxc_stealthview.current_angle);
            } else if (tmxc_stealthview.mode == 2) {
                tmxc_stealthview_subpixel_polarization(tmxc_stealthview.processed_buffer,
                                                       tmxc_stealthview.screen_width,
                                                       tmxc_stealthview.screen_height,
                                                       tmxc_stealthview.current_angle,
                                                       tmxc_stealthview.intensity);
            }
            
            for (uint32_t i = 0; i < tmxc_stealthview.buffer_size; i++) {
                framebuffer[i] = tmxc_stealthview.processed_buffer[i];
            }
        } else {
            for (uint32_t i = 0; i < tmxc_stealthview.buffer_size; i++) {
                framebuffer[i] = tmxc_stealthview.original_buffer[i];
            }
        }
    }
    
    tmxc_stealthview_update_display_controller();
}

void tmxc_stealthview_set_threshold_angle(uint32_t angle) {
    if (angle < 10) angle = 10;
    if (angle > 60) angle = 60;
    
    tmxc_stealthview.threshold_angle = angle;
}

uint32_t tmxc_stealthview_get_threshold_angle(void) {
    return tmxc_stealthview.threshold_angle;
}

void tmxc_stealthview_set_blur_strength(uint32_t strength) {
    if (strength < 1) strength = 1;
    if (strength > 20) strength = 20;
    
    tmxc_stealthview.blur_strength = strength;
}

uint32_t tmxc_stealthview_get_blur_strength(void) {
    return tmxc_stealthview.blur_strength;
}

void tmxc_stealthview_set_darken_factor(float factor) {
    if (factor < 0.0f) factor = 0.0f;
    if (factor > 1.0f) factor = 1.0f;
    
    tmxc_stealthview.darken_factor = factor;
}

float tmxc_stealthview_get_darken_factor(void) {
    return tmxc_stealthview.darken_factor;
}

void tmxc_stealthview_set_mode(uint32_t mode) {
    if (mode > 2) mode = 0;
    
    tmxc_stealthview.mode = mode;
}

uint32_t tmxc_stealthview_get_mode(void) {
    return tmxc_stealthview.mode;
}

uint8_t tmxc_stealthview_is_enabled(void) {
    return tmxc_stealthview.enabled;
}

uint8_t tmxc_stealthview_is_active(void) {
    return tmxc_stealthview.active;
}

uint32_t tmxc_stealthview_get_current_angle(void) {
    return tmxc_stealthview.current_angle;
}

uint32_t tmxc_stealthview_get_intensity(void) {
    return tmxc_stealthview.intensity;
}

uint8_t tmxc_stealthview_is_user_detected(void) {
    return tmxc_stealthview.user_detected;
}

void tmxc_stealthview_get_gaze_position(uint32_t* x, uint32_t* y) {
    if (x != NULL) {
        *x = tmxc_stealthview.gaze_x;
    }
    if (y != NULL) {
        *y = tmxc_stealthview.gaze_y;
    }
}
