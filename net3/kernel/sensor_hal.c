/*
 * TMXC_OS - Custom Operating System
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * Unauthorized copying, distribution, or modification is prohibited.
 * 
 * For licensing inquiries, contact: license@tmxc-os.com
 */

#include "sensor_hal.h"
#include "uart.h"
#include "mmio.h"

/*
 * ============================================================================
 * Sensor HAL Context
 * ============================================================================
 */

static tmxc_sensor_hal_t tmxc_sensors[TMXC_MAX_SENSORS];
static uint8_t tmxc_sensor_hal_initialized = 0;

/*
 * ============================================================================
 * Utility Functions
 * ============================================================================
 */

/**
 * @brief String length calculation
 */
static uint32_t tmxc_strlen(const char* str) {
    uint32_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/**
 * @brief String copy
 */
static void tmxc_strcpy(char* dst, const char* src) {
    while (*src != '\0') {
        *dst++ = *src++;
    }
    *dst = '\0';
}

/**
 * @brief Memory copy
 */
static void tmxc_memcpy(void* dst, const void* src, uint32_t len) {
    uint8_t* d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;
    while (len--) {
        *d++ = *s++;
    }
}

/**
 * @brief Memory set
 */
static void tmxc_memset(void* ptr, uint8_t value, uint32_t len) {
    uint8_t* p = (uint8_t*)ptr;
    while (len--) {
        *p++ = value;
    }
}

/**
 * @brief Convert a 64-bit value to decimal string
 */
static void tmxc_print_dec(uint64_t value) {
    if (value == 0) {
        tmxc_uart_putc('0');
        return;
    }
    
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    
    while (value > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (value % 10);
        value /= 10;
    }
    
    tmxc_uart_puts(&buffer[pos]);
}

/**
 * @brief Convert a 32-bit value to decimal string
 */
static void tmxc_print_dec32(uint32_t value) {
    tmxc_print_dec((uint64_t)value);
}

/*
 * ============================================================================
 * Sensor HAL Implementation
 * ============================================================================
 */

/**
 * @brief Initialize Sensor HAL
 */
int tmxc_sensor_hal_init(void) {
    /*
     * Initialize all sensor contexts
     */
    for (int i = 0; i < TMXC_MAX_SENSORS; i++) {
        tmxc_memset(&tmxc_sensors[i], 0, sizeof(tmxc_sensor_hal_t));
        tmxc_sensors[i].sensor_type = TMXC_SENSOR_NONE;
        tmxc_sensors[i].buffer_head = 0;
        tmxc_sensors[i].buffer_tail = 0;
        tmxc_sensors[i].buffer_count = 0;
        tmxc_sensors[i].initialized = 0;
        tmxc_sensors[i].driver_registered = 0;
        tmxc_sensors[i].driver = NULL;
    }
    
    tmxc_sensor_hal_initialized = 1;
    
    tmxc_uart_puts("[SENSOR_HAL] Sensor HAL initialized\r\n");
    
    return 0;
}

/**
 * @brief Find sensor by type
 */
static int tmxc_sensor_hal_find_sensor(tmxc_sensor_type_t sensor_type) {
    for (int i = 0; i < TMXC_MAX_SENSORS; i++) {
        if (tmxc_sensors[i].sensor_type == sensor_type && tmxc_sensors[i].initialized) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Find free sensor slot
 */
static int tmxc_sensor_hal_find_free_slot(void) {
    for (int i = 0; i < TMXC_MAX_SENSORS; i++) {
        if (tmxc_sensors[i].sensor_type == TMXC_SENSOR_NONE) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Register sensor driver
 */
int tmxc_sensor_hal_register_driver(tmxc_sensor_type_t sensor_type, 
                                     tmxc_sensor_driver_t* driver,
                                     const char* name) {
    if (!tmxc_sensor_hal_initialized || driver == NULL) {
        return -1;
    }
    
    /*
     * Check if sensor already registered
     */
    int existing = tmxc_sensor_hal_find_sensor(sensor_type);
    if (existing >= 0) {
        tmxc_uart_puts("[SENSOR_HAL] Sensor already registered: ");
        tmxc_uart_puts(name);
        tmxc_uart_puts("\r\n");
        return -2;
    }
    
    /*
     * Find free slot
     */
    int slot = tmxc_sensor_hal_find_free_slot();
    if (slot < 0) {
        tmxc_uart_puts("[SENSOR_HAL] No free sensor slots\r\n");
        return -3;
    }
    
    /*
     * Register sensor
     */
    tmxc_sensors[slot].sensor_type = sensor_type;
    tmxc_sensors[slot].driver = driver;
    tmxc_sensors[slot].driver_registered = 1;
    
    if (name != NULL) {
        tmxc_strcpy(tmxc_sensors[slot].sensor_name, name);
    }
    
    tmxc_uart_puts("[SENSOR_HAL] Driver registered: ");
    tmxc_uart_puts(name);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Initialize sensor
 */
int tmxc_sensor_hal_init_sensor(tmxc_sensor_type_t sensor_type) {
    if (!tmxc_sensor_hal_initialized) {
        return -1;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        tmxc_uart_puts("[SENSOR_HAL] Sensor not found\r\n");
        return -2;
    }
    
    /*
     * Check if driver registered
     */
    if (!tmxc_sensors[slot].driver_registered || tmxc_sensors[slot].driver == NULL) {
        tmxc_uart_puts("[SENSOR_HAL] No driver registered\r\n");
        return -3;
    }
    
    /*
     * Call driver init
     */
    if (tmxc_sensors[slot].driver->init != NULL) {
        int result = tmxc_sensors[slot].driver->init();
        if (result != 0) {
            tmxc_uart_puts("[SENSOR_HAL] Driver init failed\r\n");
            return result;
        }
    }
    
    /*
     * Mark sensor as initialized
     */
    tmxc_sensors[slot].initialized = 1;
    tmxc_sensors[slot].config.sample_rate_hz = 0;
    tmxc_sensors[slot].config.enabled = 0;
    tmxc_sensors[slot].config.streaming = 0;
    tmxc_sensors[slot].config.calibrated = 0;
    
    tmxc_uart_puts("[SENSOR_HAL] Sensor initialized: ");
    tmxc_uart_puts(tmxc_sensor_hal_type_string(sensor_type));
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Read sensor data
 */
int tmxc_sensor_hal_read(tmxc_sensor_type_t sensor_type, tmxc_sensor_data_t* data) {
    if (!tmxc_sensor_hal_initialized || data == NULL) {
        return -1;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return -2;
    }
    
    /*
     * Check if sensor initialized
     */
    if (!tmxc_sensors[slot].initialized) {
        return -3;
    }
    
    /*
     * Call driver read
     */
    if (tmxc_sensors[slot].driver->read != NULL) {
        int result = tmxc_sensors[slot].driver->read(data);
        if (result != 0) {
            return result;
        }
    }
    
    /*
     * Set sensor type and timestamp
     */
    data->sensor_type = sensor_type;
    
    /*
     * Get timestamp (placeholder - use system timer)
     */
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(data->timestamp));
    
    return 0;
}

/**
 * @brief Start sensor streaming
 */
int tmxc_sensor_hal_start_stream(tmxc_sensor_type_t sensor_type, uint32_t rate_hz) {
    if (!tmxc_sensor_hal_initialized) {
        return -1;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return -2;
    }
    
    /*
     * Check if sensor initialized
     */
    if (!tmxc_sensors[slot].initialized) {
        return -3;
    }
    
    /*
     * Call driver start_stream
     */
    if (tmxc_sensors[slot].driver->start_stream != NULL) {
        int result = tmxc_sensors[slot].driver->start_stream(rate_hz);
        if (result != 0) {
            return result;
        }
    }
    
    /*
     * Update configuration
     */
    tmxc_sensors[slot].config.sample_rate_hz = rate_hz;
    tmxc_sensors[slot].config.streaming = 1;
    tmxc_sensors[slot].config.enabled = 1;
    
    /*
     * Clear buffer
     */
    tmxc_sensor_hal_clear_buffer(sensor_type);
    
    tmxc_uart_puts("[SENSOR_HAL] Streaming started: ");
    tmxc_uart_puts(tmxc_sensor_hal_type_string(sensor_type));
    tmxc_uart_puts(" at ");
    tmxc_print_dec32(rate_hz);
    tmxc_uart_puts(" Hz\r\n");
    
    return 0;
}

/**
 * @brief Stop sensor streaming
 */
int tmxc_sensor_hal_stop_stream(tmxc_sensor_type_t sensor_type) {
    if (!tmxc_sensor_hal_initialized) {
        return -1;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return -2;
    }
    
    /*
     * Call driver stop_stream
     */
    if (tmxc_sensors[slot].driver->stop_stream != NULL) {
        int result = tmxc_sensors[slot].driver->stop_stream();
        if (result != 0) {
            return result;
        }
    }
    
    /*
     * Update configuration
     */
    tmxc_sensors[slot].config.streaming = 0;
    tmxc_sensors[slot].config.enabled = 0;
    
    tmxc_uart_puts("[SENSOR_HAL] Streaming stopped: ");
    tmxc_uart_puts(tmxc_sensor_hal_type_string(sensor_type));
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Calibrate sensor
 */
int tmxc_sensor_hal_calibrate(tmxc_sensor_type_t sensor_type) {
    if (!tmxc_sensor_hal_initialized) {
        return -1;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return -2;
    }
    
    /*
     * Call driver calibrate
     */
    if (tmxc_sensors[slot].driver->calibrate != NULL) {
        int result = tmxc_sensors[slot].driver->calibrate();
        if (result != 0) {
            return result;
        }
    }
    
    /*
     * Mark as calibrated
     */
    tmxc_sensors[slot].config.calibrated = 1;
    tmxc_sensors[slot].calibration.valid = 1;
    
    tmxc_uart_puts("[SENSOR_HAL] Sensor calibrated: ");
    tmxc_uart_puts(tmxc_sensor_hal_type_string(sensor_type));
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Get sensor status
 */
int tmxc_sensor_hal_get_status(tmxc_sensor_type_t sensor_type, uint8_t* status) {
    if (!tmxc_sensor_hal_initialized || status == NULL) {
        return -1;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return -2;
    }
    
    /*
     * Call driver get_status
     */
    if (tmxc_sensors[slot].driver->get_status != NULL) {
        int result = tmxc_sensors[slot].driver->get_status(status);
        if (result != 0) {
            return result;
        }
    } else {
        /*
         * Default status based on configuration
         */
        *status = 0;
        if (tmxc_sensors[slot].config.enabled) {
            *status |= 0x01;
        }
        if (tmxc_sensors[slot].config.streaming) {
            *status |= 0x02;
        }
        if (tmxc_sensors[slot].config.calibrated) {
            *status |= 0x04;
        }
    }
    
    return 0;
}

/**
 * @brief Configure sensor
 */
int tmxc_sensor_hal_configure(tmxc_sensor_type_t sensor_type, 
                               const tmxc_sensor_config_t* config) {
    if (!tmxc_sensor_hal_initialized || config == NULL) {
        return -1;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return -2;
    }
    
    /*
     * Update configuration
     */
    tmxc_memcpy(&tmxc_sensors[slot].config, config, sizeof(tmxc_sensor_config_t));
    
    return 0;
}

/**
 * @brief Get sensor configuration
 */
int tmxc_sensor_hal_get_config(tmxc_sensor_type_t sensor_type, 
                                tmxc_sensor_config_t* config) {
    if (!tmxc_sensor_hal_initialized || config == NULL) {
        return -1;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return -2;
    }
    
    /*
     * Copy configuration
     */
    tmxc_memcpy(config, &tmxc_sensors[slot].config, sizeof(tmxc_sensor_config_t));
    
    return 0;
}

/**
 * @brief Get sensor calibration data
 */
int tmxc_sensor_hal_get_calibration(tmxc_sensor_type_t sensor_type,
                                    tmxc_sensor_calibration_t* calibration) {
    if (!tmxc_sensor_hal_initialized || calibration == NULL) {
        return -1;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return -2;
    }
    
    /*
     * Copy calibration data
     */
    tmxc_memcpy(calibration, &tmxc_sensors[slot].calibration, sizeof(tmxc_sensor_calibration_t));
    
    return 0;
}

/**
 * @brief Set sensor calibration data
 */
int tmxc_sensor_hal_set_calibration(tmxc_sensor_type_t sensor_type,
                                    const tmxc_sensor_calibration_t* calibration) {
    if (!tmxc_sensor_hal_initialized || calibration == NULL) {
        return -1;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return -2;
    }
    
    /*
     * Copy calibration data
     */
    tmxc_memcpy(&tmxc_sensors[slot].calibration, calibration, sizeof(tmxc_sensor_calibration_t));
    
    return 0;
}

/**
 * @brief Sensor interrupt handler
 */
void tmxc_sensor_hal_interrupt_handler(tmxc_sensor_type_t sensor_type) {
    if (!tmxc_sensor_hal_initialized) {
        return;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return;
    }
    
    /*
     * Check if streaming
     */
    if (!tmxc_sensors[slot].config.streaming) {
        return;
    }
    
    /*
     * Read sensor data
     */
    tmxc_sensor_data_t data;
    if (tmxc_sensor_hal_read(sensor_type, &data) != 0) {
        return;
    }
    
    /*
     * Store in circular buffer
     */
    uint32_t next_head = (tmxc_sensors[slot].buffer_head + 1) % TMXC_SENSOR_BUFFER_SIZE;
    
    /*
     * Check if buffer full
     */
    if (next_head == tmxc_sensors[slot].buffer_tail) {
        /*
         * Buffer full, discard oldest data
         */
        tmxc_sensors[slot].buffer_tail = (tmxc_sensors[slot].buffer_tail + 1) % TMXC_SENSOR_BUFFER_SIZE;
    } else {
        tmxc_sensors[slot].buffer_count++;
    }
    
    /*
     * Store data
     */
    tmxc_memcpy(&tmxc_sensors[slot].buffer[tmxc_sensors[slot].buffer_head], 
                &data, sizeof(tmxc_sensor_data_t));
    tmxc_sensors[slot].buffer_head = next_head;
}

/**
 * @brief Get buffered sensor data
 */
int tmxc_sensor_hal_get_buffered_data(tmxc_sensor_type_t sensor_type, 
                                       tmxc_sensor_data_t* data) {
    if (!tmxc_sensor_hal_initialized || data == NULL) {
        return -1;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return -2;
    }
    
    /*
     * Check if buffer empty
     */
    if (tmxc_sensors[slot].buffer_head == tmxc_sensors[slot].buffer_tail) {
        return -3;
    }
    
    /*
     * Get data from buffer
     */
    tmxc_memcpy(data, &tmxc_sensors[slot].buffer[tmxc_sensors[slot].buffer_tail], 
                sizeof(tmxc_sensor_data_t));
    
    /*
     * Update tail
     */
    tmxc_sensors[slot].buffer_tail = (tmxc_sensors[slot].buffer_tail + 1) % TMXC_SENSOR_BUFFER_SIZE;
    tmxc_sensors[slot].buffer_count--;
    
    return 0;
}

/**
 * @brief Get buffer count
 */
uint32_t tmxc_sensor_hal_get_buffer_count(tmxc_sensor_type_t sensor_type) {
    if (!tmxc_sensor_hal_initialized) {
        return 0;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return 0;
    }
    
    return tmxc_sensors[slot].buffer_count;
}

/**
 * @brief Clear sensor buffer
 */
int tmxc_sensor_hal_clear_buffer(tmxc_sensor_type_t sensor_type) {
    if (!tmxc_sensor_hal_initialized) {
        return -1;
    }
    
    /*
     * Find sensor
     */
    int slot = tmxc_sensor_hal_find_sensor(sensor_type);
    if (slot < 0) {
        return -2;
    }
    
    /*
     * Clear buffer
     */
    tmxc_sensors[slot].buffer_head = 0;
    tmxc_sensors[slot].buffer_tail = 0;
    tmxc_sensors[slot].buffer_count = 0;
    
    return 0;
}

/**
 * @brief Cleanup Sensor HAL
 */
int tmxc_sensor_hal_cleanup(void) {
    if (!tmxc_sensor_hal_initialized) {
        return -1;
    }
    
    /*
     * Cleanup all sensors
     */
    for (int i = 0; i < TMXC_MAX_SENSORS; i++) {
        if (tmxc_sensors[i].initialized && tmxc_sensors[i].driver != NULL) {
            if (tmxc_sensors[i].driver->cleanup != NULL) {
                tmxc_sensors[i].driver->cleanup();
            }
        }
    }
    
    tmxc_sensor_hal_initialized = 0;
    
    tmxc_uart_puts("[SENSOR_HAL] Sensor HAL cleaned up\r\n");
    
    return 0;
}

/**
 * @brief Get sensor type string
 */
const char* tmxc_sensor_hal_type_string(tmxc_sensor_type_t sensor_type) {
    switch (sensor_type) {
        case TMXC_SENSOR_NONE:
            return "None";
        case TMXC_SENSOR_MAGNETOMETER:
            return "Magnetometer";
        case TMXC_SENSOR_ACCELEROMETER:
            return "Accelerometer";
        case TMXC_SENSOR_GYROSCOPE:
            return "Gyroscope";
        case TMXC_SENSOR_PROXIMITY:
            return "Proximity";
        case TMXC_SENSOR_LIGHT:
            return "Light";
        case TMXC_SENSOR_BAROMETER:
            return "Barometer";
        default:
            return "Unknown";
    }
}
