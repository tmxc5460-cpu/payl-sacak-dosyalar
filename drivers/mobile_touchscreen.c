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
    uint32_t x;
    uint32_t y;
    uint32_t pressure;
    uint32_t gesture;
    uint64_t timestamp;
    uint8_t valid;
} tmxc_touch_point_t;

typedef struct {
    uint8_t i2c_addr;
    uint8_t initialized;
    uint8_t irq_enabled;
    uint32_t max_x;
    uint32_t max_y;
    uint32_t sample_rate;
    tmxc_touch_point_t current_point;
    tmxc_touch_point_t last_point;
    uint32_t touch_count;
    uint8_t multi_touch;
    uint8_t gesture_enabled;
} tmxc_touchscreen_t;

static tmxc_touchscreen_t tmxc_touchscreen;

static void tmxc_i2c_delay(void) {
    for (volatile int i = 0; i < 100; i++) {
        __asm__ volatile("nop");
    }
}

static void tmxc_i2c_start(void) {
    uint32_t gpio_reg = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE);
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_BASE, gpio_reg | (1 << 2));
    tmxc_i2c_delay();
    
    uint32_t i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg & ~(1 << 0));
    tmxc_i2c_delay();
}

static void tmxc_i2c_stop(void) {
    uint32_t i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg & ~(1 << 1));
    tmxc_i2c_delay();
    
    uint32_t gpio_reg = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE);
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_BASE, gpio_reg | (1 << 2));
    tmxc_i2c_delay();
}

static uint8_t tmxc_i2c_write_byte(uint8_t data) {
    for (int i = 7; i >= 0; i--) {
        uint32_t i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
        if (data & (1 << i)) {
            i2c_reg |= (1 << 1);
        } else {
            i2c_reg &= ~(1 << 1);
        }
        tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
        tmxc_i2c_delay();
        
        i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
        i2c_reg |= (1 << 0);
        tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
        tmxc_i2c_delay();
        
        i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
        i2c_reg &= ~(1 << 0);
        tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
        tmxc_i2c_delay();
    }
    
    uint32_t gpio_reg = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE);
    uint8_t ack = (gpio_reg & (1 << 3)) ? 1 : 0;
    
    uint32_t i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    tmxc_i2c_delay();
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    tmxc_i2c_delay();
    
    return ack;
}

static uint8_t tmxc_i2c_read_byte(uint8_t ack) {
    uint8_t data = 0;
    
    uint32_t gpio_reg = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE);
    gpio_reg |= (1 << 3);
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_BASE, gpio_reg);
    tmxc_i2c_delay();
    
    for (int i = 7; i >= 0; i--) {
        uint32_t i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
        i2c_reg |= (1 << 0);
        tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
        tmxc_i2c_delay();
        
        gpio_reg = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE);
        if (gpio_reg & (1 << 3)) {
            data |= (1 << i);
        }
        
        i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
        i2c_reg &= ~(1 << 0);
        tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
        tmxc_i2c_delay();
    }
    
    uint32_t i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    if (ack) {
        i2c_reg &= ~(1 << 1);
    } else {
        i2c_reg |= (1 << 1);
    }
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    tmxc_i2c_delay();
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    tmxc_i2c_delay();
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    tmxc_i2c_delay();
    
    return data;
}

static uint8_t tmxc_touch_i2c_write_reg(uint8_t reg, uint8_t value) {
    tmxc_i2c_start();
    
    uint8_t addr = (tmxc_touchscreen.i2c_addr << 1) | 0;
    if (!tmxc_i2c_write_byte(addr)) {
        tmxc_i2c_stop();
        return 0;
    }
    
    if (!tmxc_i2c_write_byte(reg)) {
        tmxc_i2c_stop();
        return 0;
    }
    
    if (!tmxc_i2c_write_byte(value)) {
        tmxc_i2c_stop();
        return 0;
    }
    
    tmxc_i2c_stop();
    return 1;
}

static uint8_t tmxc_touch_i2c_read_reg(uint8_t reg, uint8_t* value) {
    tmxc_i2c_start();
    
    uint8_t addr = (tmxc_touchscreen.i2c_addr << 1) | 0;
    if (!tmxc_i2c_write_byte(addr)) {
        tmxc_i2c_stop();
        return 0;
    }
    
    if (!tmxc_i2c_write_byte(reg)) {
        tmxc_i2c_stop();
        return 0;
    }
    
    tmxc_i2c_stop();
    
    tmxc_i2c_start();
    
    addr = (tmxc_touchscreen.i2c_addr << 1) | 1;
    if (!tmxc_i2c_write_byte(addr)) {
        tmxc_i2c_stop();
        return 0;
    }
    
    *value = tmxc_i2c_read_byte(0);
    tmxc_i2c_stop();
    
    return 1;
}

static uint8_t tmxc_touch_i2c_read_regs(uint8_t reg, uint8_t* buffer, uint32_t length) {
    tmxc_i2c_start();
    
    uint8_t addr = (tmxc_touchscreen.i2c_addr << 1) | 0;
    if (!tmxc_i2c_write_byte(addr)) {
        tmxc_i2c_stop();
        return 0;
    }
    
    if (!tmxc_i2c_write_byte(reg)) {
        tmxc_i2c_stop();
        return 0;
    }
    
    tmxc_i2c_stop();
    
    tmxc_i2c_start();
    
    addr = (tmxc_touchscreen.i2c_addr << 1) | 1;
    if (!tmxc_i2c_write_byte(addr)) {
        tmxc_i2c_stop();
        return 0;
    }
    
    for (uint32_t i = 0; i < length; i++) {
        buffer[i] = tmxc_i2c_read_byte(i < length - 1 ? 1 : 0);
    }
    
    tmxc_i2c_stop();
    return 1;
}

static void tmxc_touch_reset(void) {
    uint32_t gpio_reg = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE);
    gpio_reg &= ~(1 << 4);
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_BASE, gpio_reg);
    
    tmxc_timer_delay_ms(10);
    
    gpio_reg = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE);
    gpio_reg |= (1 << 4);
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_BASE, gpio_reg);
    
    tmxc_timer_delay_ms(100);
}

static uint8_t tmxc_touch_detect(void) {
    uint8_t device_id;
    if (tmxc_touch_i2c_read_reg(0x00, &device_id)) {
        if (device_id == 0x11 || device_id == 0x12 || device_id == 0x13 || device_id == 0x14) {
            return 1;
        }
    }
    return 0;
}

static void tmxc_touch_init_hardware(void) {
    tmxc_touch_reset();
    
    tmxc_touch_i2c_write_reg(0x00, 0x00);
    
    tmxc_touch_i2c_write_reg(0x80, 0x01);
    tmxc_timer_delay_ms(10);
    
    tmxc_touch_i2c_write_reg(0x80, 0x00);
    tmxc_timer_delay_ms(10);
    
    tmxc_touch_i2c_write_reg(0x41, 0x04);
    
    tmxc_touch_i2c_write_reg(0x44, 0x01);
    
    tmxc_touch_i2c_write_reg(0x47, 0x04);
    
    tmxc_touch_i2c_write_reg(0x48, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x4A, 0x01);
    
    tmxc_touch_i2c_write_reg(0x4B, 0x00);
    
    tmxc_touch_i2c_write_reg(0x4D, 0x00);
    
    tmxc_touch_i2c_write_reg(0x4E, 0x01);
    
    tmxc_touch_i2c_write_reg(0x4F, 0x01);
    
    tmxc_touch_i2c_write_reg(0x50, 0x00);
    
    tmxc_touch_i2c_write_reg(0x51, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x52, 0x00);
    
    tmxc_touch_i2c_write_reg(0x53, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x54, 0x01);
    
    tmxc_touch_i2c_write_reg(0x55, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x56, 0x00);
    
    tmxc_touch_i2c_write_reg(0x57, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x58, 0x01);
    
    tmxc_touch_i2c_write_reg(0x59, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x5A, 0x00);
    
    tmxc_touch_i2c_write_reg(0x5B, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x5C, 0x01);
    
    tmxc_touch_i2c_write_reg(0x5D, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x5E, 0x00);
    
    tmxc_touch_i2c_write_reg(0x5F, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x60, 0x01);
    
    tmxc_touch_i2c_write_reg(0x61, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x62, 0x00);
    
    tmxc_touch_i2c_write_reg(0x63, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x64, 0x01);
    
    tmxc_touch_i2c_write_reg(0x65, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x66, 0x00);
    
    tmxc_touch_i2c_write_reg(0x67, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x68, 0x01);
    
    tmxc_touch_i2c_write_reg(0x69, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x6A, 0x00);
    
    tmxc_touch_i2c_write_reg(0x6B, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x6C, 0x01);
    
    tmxc_touch_i2c_write_reg(0x6D, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x6E, 0x00);
    
    tmxc_touch_i2c_write_reg(0x6F, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x70, 0x01);
    
    tmxc_touch_i2c_write_reg(0x71, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x72, 0x00);
    
    tmxc_touch_i2c_write_reg(0x73, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x74, 0x01);
    
    tmxc_touch_i2c_write_reg(0x75, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x76, 0x00);
    
    tmxc_touch_i2c_write_reg(0x77, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x78, 0x01);
    
    tmxc_touch_i2c_write_reg(0x79, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x7A, 0x00);
    
    tmxc_touch_i2c_write_reg(0x7B, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x7C, 0x01);
    
    tmxc_touch_i2c_write_reg(0x7D, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x7E, 0x00);
    
    tmxc_touch_i2c_write_reg(0x7F, 0x0F);
    
    tmxc_touch_i2c_write_reg(0x80, 0x40);
    
    tmxc_timer_delay_ms(50);
}

static void tmxc_touch_calibrate(void) {
    tmxc_touchscreen.max_x = 4095;
    tmxc_touchscreen.max_y = 4095;
    tmxc_touchscreen.sample_rate = 240;
}

void tmxc_touch_init(void) {
    tmxc_touchscreen.i2c_addr = TMXC_TOUCH_I2C_ADDR;
    tmxc_touchscreen.initialized = 0;
    tmxc_touchscreen.irq_enabled = 0;
    tmxc_touchscreen.max_x = 0;
    tmxc_touchscreen.max_y = 0;
    tmxc_touchscreen.sample_rate = 0;
    tmxc_touchscreen.touch_count = 0;
    tmxc_touchscreen.multi_touch = 0;
    tmxc_touchscreen.gesture_enabled = 0;
    
    tmxc_touchscreen.current_point.x = 0;
    tmxc_touchscreen.current_point.y = 0;
    tmxc_touchscreen.current_point.pressure = 0;
    tmxc_touchscreen.current_point.gesture = 0;
    tmxc_touchscreen.current_point.timestamp = 0;
    tmxc_touchscreen.current_point.valid = 0;
    
    tmxc_touchscreen.last_point.x = 0;
    tmxc_touchscreen.last_point.y = 0;
    tmxc_touchscreen.last_point.pressure = 0;
    tmxc_touchscreen.last_point.gesture = 0;
    tmxc_touchscreen.last_point.timestamp = 0;
    tmxc_touchscreen.last_point.valid = 0;
    
    uint32_t i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C0_BASE);
    i2c_reg |= (1 << 15);
    tmxc_write32((volatile uint32_t*)TMXC_I2C0_BASE, i2c_reg);
    
    uint32_t gpio_reg = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE);
    gpio_reg |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_BASE, gpio_reg);
    
    if (tmxc_touch_detect()) {
        tmxc_touch_init_hardware();
        tmxc_touch_calibrate();
        tmxc_touchscreen.initialized = 1;
        tmxc_touchscreen.irq_enabled = 1;
        tmxc_touchscreen.multi_touch = 1;
        tmxc_touchscreen.gesture_enabled = 1;
    }
}

int tmxc_touch_read(tmxc_touch_event_t* event) {
    if (!tmxc_touchscreen.initialized || event == NULL) {
        return -1;
    }
    
    uint8_t touch_data[6];
    if (!tmxc_touch_i2c_read_regs(0x03, touch_data, 6)) {
        return -2;
    }
    
    uint16_t raw_x = ((uint16_t)touch_data[0] << 8) | touch_data[1];
    uint16_t raw_y = ((uint16_t)touch_data[2] << 8) | touch_data[3];
    uint8_t pressure = touch_data[4];
    uint8_t gesture = touch_data[5];
    
    if (raw_x == 0 && raw_y == 0 && pressure == 0) {
        tmxc_touchscreen.current_point.valid = 0;
        event->x = 0;
        event->y = 0;
        event->pressure = 0;
        event->gesture = 0;
        event->timestamp = tmxc_get_cycle_count();
        return 0;
    }
    
    tmxc_touchscreen.current_point.x = (raw_x * 1920) / tmxc_touchscreen.max_x;
    tmxc_touchscreen.current_point.y = (raw_y * 1080) / tmxc_touchscreen.max_y;
    tmxc_touchscreen.current_point.pressure = pressure;
    tmxc_touchscreen.current_point.gesture = gesture;
    tmxc_touchscreen.current_point.timestamp = tmxc_get_cycle_count();
    tmxc_touchscreen.current_point.valid = 1;
    
    event->x = tmxc_touchscreen.current_point.x;
    event->y = tmxc_touchscreen.current_point.y;
    event->pressure = tmxc_touchscreen.current_point.pressure;
    event->gesture = tmxc_touchscreen.current_point.gesture;
    event->timestamp = tmxc_touchscreen.current_point.timestamp;
    
    tmxc_touchscreen.last_point = tmxc_touchscreen.current_point;
    tmxc_touchscreen.touch_count++;
    
    return 1;
}

void tmxc_touch_set_sample_rate(uint32_t rate) {
    if (rate < 60) rate = 60;
    if (rate > 480) rate = 480;
    
    tmxc_touchscreen.sample_rate = rate;
    
    uint8_t rate_reg = (rate - 60) / 30;
    tmxc_touch_i2c_write_reg(0x44, rate_reg);
}

uint32_t tmxc_touch_get_sample_rate(void) {
    return tmxc_touchscreen.sample_rate;
}

void tmxc_touch_enable_multi_touch(uint8_t enable) {
    tmxc_touchscreen.multi_touch = enable;
    
    if (enable) {
        tmxc_touch_i2c_write_reg(0x41, 0x04);
    } else {
        tmxc_touch_i2c_write_reg(0x41, 0x01);
    }
}

void tmxc_touch_enable_gesture(uint8_t enable) {
    tmxc_touchscreen.gesture_enabled = enable;
    
    if (enable) {
        tmxc_touch_i2c_write_reg(0x47, 0x04);
    } else {
        tmxc_touch_i2c_write_reg(0x47, 0x00);
    }
}

uint8_t tmxc_touch_is_pressed(void) {
    return tmxc_touchscreen.current_point.valid;
}

void tmxc_touch_get_position(uint32_t* x, uint32_t* y) {
    if (x != NULL) {
        *x = tmxc_touchscreen.current_point.x;
    }
    if (y != NULL) {
        *y = tmxc_touchscreen.current_point.y;
    }
}

uint32_t tmxc_touch_get_pressure(void) {
    return tmxc_touchscreen.current_point.pressure;
}

uint32_t tmxc_touch_get_gesture(void) {
    return tmxc_touchscreen.current_point.gesture;
}
