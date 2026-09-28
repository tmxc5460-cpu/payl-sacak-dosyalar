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
    int16_t mag_x;
    int16_t mag_y;
    int16_t mag_z;
    int32_t heading;
    uint8_t initialized;
    uint8_t calibration_enabled;
    int16_t offset_x;
    int16_t offset_y;
    int16_t offset_z;
    uint8_t heading_valid;
} tmxc_compass_state_t;

static tmxc_compass_state_t tmxc_compass;

#define TMXC_COMPASS_I2C_ADDR 0x0C
#define TMXC_COMPASS_REG_X_LSB 0x00
#define TMXC_COMPASS_REG_X_MSB 0x01
#define TMXC_COMPASS_REG_Y_LSB 0x02
#define TMXC_COMPASS_REG_Y_MSB 0x03
#define TMXC_COMPASS_REG_Z_LSB 0x04
#define TMXC_COMPASS_REG_Z_MSB 0x05
#define TMXC_COMPASS_REG_CTRL 0x0A

static int32_t tmxc_atan2_fixed(int32_t y, int32_t x) {
    if (x == 0 && y == 0) {
        return 0;
    }
    
    int32_t abs_y = y < 0 ? -y : y;
    int32_t abs_x = x < 0 ? -x : x;
    
    int32_t angle;
    
    if (abs_y > abs_x) {
        angle = 90 - (int32_t)(((int64_t)x * 90) / (abs_y + (abs_y >> 1)));
    } else {
        angle = (int32_t)(((int64_t)y * 90) / (abs_x + (abs_x >> 1)));
    }
    
    if (x < 0) {
        if (y < 0) {
            angle = angle - 180;
        } else {
            angle = 180 - angle;
        }
    } else if (y < 0) {
        angle = -angle;
    }
    
    if (angle < 0) {
        angle += 360;
    }
    
    return angle;
}

static void tmxc_compass_i2c_write(uint8_t reg, uint8_t value) {
    uint32_t i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_reg |= (1 << 15);
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_reg &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_reg = (i2c_reg & ~0xFF) | TMXC_COMPASS_I2C_ADDR;
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_reg = (i2c_reg & ~0xFF00) | (reg << 8);
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_reg = (i2c_reg & ~0xFF0000) | (value << 16);
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_reg |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_reg);
    
    while ((tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE) & (1 << 0))) {
        ;
    }
}

static uint8_t tmxc_compass_i2c_read(uint8_t reg) {
    uint32_t i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_reg |= (1 << 15);
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_reg &= ~(1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_reg = (i2c_reg & ~0xFF) | (TMXC_COMPASS_I2C_ADDR | 0x01);
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_reg = (i2c_reg & ~0xFF00) | (reg << 8);
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_reg);
    
    i2c_reg = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_reg |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_reg);
    
    while ((tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE) & (1 << 0))) {
        ;
    }
    
    uint8_t value = (tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE) >> 16) & 0xFF;
    return value;
}

void tmxc_compass_init(void) {
    tmxc_uart_puts("[COMPASS] Initializing compass driver...\r\n");
    
    tmxc_compass.mag_x = 0;
    tmxc_compass.mag_y = 0;
    tmxc_compass.mag_z = 0;
    tmxc_compass.heading = 0;
    tmxc_compass.initialized = 0;
    tmxc_compass.calibration_enabled = 0;
    tmxc_compass.offset_x = 0;
    tmxc_compass.offset_y = 0;
    tmxc_compass.offset_z = 0;
    tmxc_compass.heading_valid = 0;
    
    uint32_t i2c_ctrl = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_ctrl |= (1 << 15);
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_ctrl);
    
    i2c_ctrl = tmxc_read32((volatile uint32_t*)TMXC_I2C1_BASE);
    i2c_ctrl = (i2c_ctrl & ~0x3F) | 0x14;
    tmxc_write32((volatile uint32_t*)TMXC_I2C1_BASE, i2c_ctrl);
    
    tmxc_timer_delay_ms(10);
    
    tmxc_compass_i2c_write(TMXC_COMPASS_REG_CTRL, 0x01);
    
    tmxc_timer_delay_ms(50);
    
    uint8_t who_am_i = tmxc_compass_i2c_read(0x0F);
    
    if (who_am_i == 0x48 || who_am_i == 0x49) {
        tmxc_uart_puts("[COMPASS] Magnetometer detected\r\n");
        tmxc_compass.initialized = 1;
    } else {
        tmxc_uart_puts("[COMPASS] Magnetometer not detected\r\n");
    }
    
    tmxc_uart_puts("[COMPASS] Compass initialization complete\r\n");
}

int32_t tmxc_compass_read_heading(void) {
    if (!tmxc_compass.initialized) {
        return -1;
    }
    
    uint8_t x_lsb = tmxc_compass_i2c_read(TMXC_COMPASS_REG_X_LSB);
    uint8_t x_msb = tmxc_compass_i2c_read(TMXC_COMPASS_REG_X_MSB);
    uint8_t y_lsb = tmxc_compass_i2c_read(TMXC_COMPASS_REG_Y_LSB);
    uint8_t y_msb = tmxc_compass_i2c_read(TMXC_COMPASS_REG_Y_MSB);
    uint8_t z_lsb = tmxc_compass_i2c_read(TMXC_COMPASS_REG_Z_LSB);
    uint8_t z_msb = tmxc_compass_i2c_read(TMXC_COMPASS_REG_Z_MSB);
    
    int16_t raw_x = (int16_t)((x_msb << 8) | x_lsb);
    int16_t raw_y = (int16_t)((y_msb << 8) | y_lsb);
    int16_t raw_z = (int16_t)((z_msb << 8) | z_lsb);
    
    tmxc_compass.mag_x = raw_x - tmxc_compass.offset_x;
    tmxc_compass.mag_y = raw_y - tmxc_compass.offset_y;
    tmxc_compass.mag_z = raw_z - tmxc_compass.offset_z;
    
    tmxc_compass.heading = tmxc_atan2_fixed(tmxc_compass.mag_y, tmxc_compass.mag_x);
    
    tmxc_compass.heading_valid = 1;
    
    return tmxc_compass.heading;
}

void tmxc_compass_read_raw(int16_t* x, int16_t* y, int16_t* z) {
    if (!tmxc_compass.initialized) {
        return;
    }
    
    uint8_t x_lsb = tmxc_compass_i2c_read(TMXC_COMPASS_REG_X_LSB);
    uint8_t x_msb = tmxc_compass_i2c_read(TMXC_COMPASS_REG_X_MSB);
    uint8_t y_lsb = tmxc_compass_i2c_read(TMXC_COMPASS_REG_Y_LSB);
    uint8_t y_msb = tmxc_compass_i2c_read(TMXC_COMPASS_REG_Y_MSB);
    uint8_t z_lsb = tmxc_compass_i2c_read(TMXC_COMPASS_REG_Z_LSB);
    uint8_t z_msb = tmxc_compass_i2c_read(TMXC_COMPASS_REG_Z_MSB);
    
    int16_t raw_x = (int16_t)((x_msb << 8) | x_lsb);
    int16_t raw_y = (int16_t)((y_msb << 8) | y_lsb);
    int16_t raw_z = (int16_t)((z_msb << 8) | z_lsb);
    
    if (x != NULL) *x = raw_x;
    if (y != NULL) *y = raw_y;
    if (z != NULL) *z = raw_z;
}

void tmxc_compass_calibrate(void) {
    if (!tmxc_compass.initialized) {
        return;
    }
    
    tmxc_uart_puts("[COMPASS] Starting calibration...\r\n");
    
    int32_t min_x = 32767;
    int32_t max_x = -32768;
    int32_t min_y = 32767;
    int32_t max_y = -32768;
    int32_t min_z = 32767;
    int32_t max_z = -32768;
    
    for (int i = 0; i < 100; i++) {
        int16_t x, y, z;
        tmxc_compass_read_raw(&x, &y, &z);
        
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
        if (z < min_z) min_z = z;
        if (z > max_z) max_z = z;
        
        tmxc_timer_delay_ms(20);
    }
    
    tmxc_compass.offset_x = (int16_t)((max_x + min_x) / 2);
    tmxc_compass.offset_y = (int16_t)((max_y + min_y) / 2);
    tmxc_compass.offset_z = (int16_t)((max_z + min_z) / 2);
    
    tmxc_compass.calibration_enabled = 1;
    
    tmxc_uart_puts("[COMPASS] Calibration complete\r\n");
    tmxc_uart_puts("[COMPASS] Offsets: X=");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    int64_t temp = tmxc_compass.offset_x;
    if (temp < 0) {
        temp = -temp;
        tmxc_uart_putc('-');
    }
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" Y=");
    pos = 20;
    buffer[pos] = '\0';
    temp = tmxc_compass.offset_y;
    if (temp < 0) {
        temp = -temp;
        tmxc_uart_putc('-');
    }
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" Z=");
    pos = 20;
    buffer[pos] = '\0';
    temp = tmxc_compass.offset_z;
    if (temp < 0) {
        temp = -temp;
        tmxc_uart_putc('-');
    }
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

void tmxc_compass_set_offsets(int16_t x, int16_t y, int16_t z) {
    tmxc_compass.offset_x = x;
    tmxc_compass.offset_y = y;
    tmxc_compass.offset_z = z;
    tmxc_compass.calibration_enabled = 1;
}

void tmxc_compass_get_offsets(int16_t* x, int16_t* y, int16_t* z) {
    if (x != NULL) *x = tmxc_compass.offset_x;
    if (y != NULL) *y = tmxc_compass.offset_y;
    if (z != NULL) *z = tmxc_compass.offset_z;
}

uint8_t tmxc_compass_is_calibrated(void) {
    return tmxc_compass.calibration_enabled;
}

uint8_t tmxc_compass_is_heading_valid(void) {
    return tmxc_compass.heading_valid;
}

const char* tmxc_compass_get_direction_string(void) {
    int32_t heading = tmxc_compass.heading;
    
    if (heading >= 338 || heading < 23) {
        return "Kuzey";
    } else if (heading >= 23 && heading < 68) {
        return "Kuzeydoğu";
    } else if (heading >= 68 && heading < 113) {
        return "Doğu";
    } else if (heading >= 113 && heading < 158) {
        return "Güneydoğu";
    } else if (heading >= 158 && heading < 203) {
        return "Güney";
    } else if (heading >= 203 && heading < 248) {
        return "Güneybatı";
    } else if (heading >= 248 && heading < 293) {
        return "Batı";
    } else {
        return "Kuzeybatı";
    }
}
