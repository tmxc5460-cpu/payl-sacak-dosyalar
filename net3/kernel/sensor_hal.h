/*
 * TMXC_OS - Custom Operating System
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * Unauthorized copying, distribution, or modification is prohibited.
 * 
 * For licensing inquiries, contact: license@tmxc-os.com
 */

#ifndef TMXC_SENSOR_HAL_H
#define TMXC_SENSOR_HAL_H

#include <stdint.h>

/*
 * ============================================================================
 * Sensor HAL Configuration
 * ============================================================================
 */

/**
 * @brief Maximum number of sensors
 */
#define TMXC_MAX_SENSORS 8

/**
 * @brief Sensor data buffer size
 */
#define TMXC_SENSOR_BUFFER_SIZE 256

/**
 * @brief Maximum sensor name length
 */
#define TMXC_SENSOR_NAME_LEN 32

/*
 * ============================================================================
 * Sensor Types
 * ============================================================================
 */

/**
 * @brief Sensor type enumeration
 */
typedef enum {
    TMXC_SENSOR_NONE = 0,
    TMXC_SENSOR_MAGNETOMETER = 0x01,
    TMXC_SENSOR_ACCELEROMETER = 0x02,
    TMXC_SENSOR_GYROSCOPE = 0x03,
    TMXC_SENSOR_PROXIMITY = 0x04,
    TMXC_SENSOR_LIGHT = 0x05,
    TMXC_SENSOR_BAROMETER = 0x06
} tmxc_sensor_type_t;

/*
 * ============================================================================
 * Sensor Data Structures
 * ============================================================================
 */

/**
 * @brief Sensor data structure
 */
typedef struct {
    float x, y, z;              /* Sensor readings (3-axis) */
    uint64_t timestamp;         /* Timestamp in microseconds */
    uint8_t sensor_type;        /* Sensor type */
    uint8_t accuracy;           /* Accuracy level (0-3) */
    uint8_t reserved[2];        /* Reserved for future use */
} tmxc_sensor_data_t;

/**
 * @brief Sensor configuration structure
 */
typedef struct {
    uint32_t sample_rate_hz;    /* Sample rate in Hz */
    uint8_t enabled;            /* Sensor enabled flag */
    uint8_t streaming;          /* Streaming mode flag */
    uint8_t calibrated;         /* Calibration status */
    uint8_t reserved[5];        /* Reserved for future use */
} tmxc_sensor_config_t;

/**
 * @brief Sensor calibration data
 */
typedef struct {
    float offset_x, offset_y, offset_z;    /* Offset values */
    float scale_x, scale_y, scale_z;       /* Scale factors */
    uint8_t valid;                         /* Calibration valid flag */
    uint8_t reserved[3];                   /* Reserved for future use */
} tmxc_sensor_calibration_t;

/*
 * ============================================================================
 * Sensor HAL Structures
 * ============================================================================
 */

/**
 * @brief Sensor driver interface
 */
typedef struct {
    int (*init)(void);                          /* Initialize sensor */
    int (*read)(tmxc_sensor_data_t* data);       /* Read sensor data */
    int (*start_stream)(uint32_t rate_hz);       /* Start streaming */
    int (*stop_stream)(void);                    /* Stop streaming */
    int (*calibrate)(void);                     /* Calibrate sensor */
    int (*get_status)(uint8_t* status);          /* Get sensor status */
    void (*cleanup)(void);                       /* Cleanup sensor */
} tmxc_sensor_driver_t;

/**
 * @brief Sensor HAL context
 */
typedef struct {
    tmxc_sensor_type_t sensor_type;
    char sensor_name[TMXC_SENSOR_NAME_LEN];
    tmxc_sensor_config_t config;
    tmxc_sensor_calibration_t calibration;
    tmxc_sensor_data_t buffer[TMXC_SENSOR_BUFFER_SIZE];
    uint32_t buffer_head;
    uint32_t buffer_tail;
    uint32_t buffer_count;
    uint8_t initialized;
    uint8_t driver_registered;
    tmxc_sensor_driver_t* driver;
} tmxc_sensor_hal_t;

/*
 * ============================================================================
 * Sensor HAL Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize Sensor HAL
 * 
 * Initializes the Sensor HAL subsystem.
 * Must be called before any sensor operations.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_init(void);

/**
 * @brief Register sensor driver
 * 
 * Registers a sensor driver for a specific sensor type.
 * 
 * @param sensor_type Sensor type
 * @param driver Pointer to sensor driver interface
 * @param name Sensor name (for logging)
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_register_driver(tmxc_sensor_type_t sensor_type, 
                                     tmxc_sensor_driver_t* driver,
                                     const char* name);

/**
 * @brief Initialize sensor
 * 
 * Initializes a specific sensor using its registered driver.
 * 
 * @param sensor_type Sensor type
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_init_sensor(tmxc_sensor_type_t sensor_type);

/**
 * @brief Read sensor data
 * 
 * Reads data from a sensor.
 * 
 * @param sensor_type Sensor type
 * @param data Pointer to sensor data structure
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_read(tmxc_sensor_type_t sensor_type, tmxc_sensor_data_t* data);

/**
 * @brief Start sensor streaming
 * 
 * Starts streaming mode for a sensor at a specified sample rate.
 * 
 * @param sensor_type Sensor type
 * @param rate_hz Sample rate in Hz
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_start_stream(tmxc_sensor_type_t sensor_type, uint32_t rate_hz);

/**
 * @brief Stop sensor streaming
 * 
 * Stops streaming mode for a sensor.
 * 
 * @param sensor_type Sensor type
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_stop_stream(tmxc_sensor_type_t sensor_type);

/**
 * @brief Calibrate sensor
 * 
 * Calibrates a sensor using its registered driver.
 * 
 * @param sensor_type Sensor type
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_calibrate(tmxc_sensor_type_t sensor_type);

/**
 * @brief Get sensor status
 * 
 * Gets the status of a sensor.
 * 
 * @param sensor_type Sensor type
 * @param status Pointer to status buffer
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_get_status(tmxc_sensor_type_t sensor_type, uint8_t* status);

/**
 * @brief Configure sensor
 * 
 * Configures a sensor with specific parameters.
 * 
 * @param sensor_type Sensor type
 * @param config Pointer to configuration structure
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_configure(tmxc_sensor_type_t sensor_type, 
                               const tmxc_sensor_config_t* config);

/**
 * @brief Get sensor configuration
 * 
 * Gets the current configuration of a sensor.
 * 
 * @param sensor_type Sensor type
 * @param config Pointer to configuration structure
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_get_config(tmxc_sensor_type_t sensor_type, 
                                tmxc_sensor_config_t* config);

/**
 * @brief Get sensor calibration data
 * 
 * Gets the calibration data for a sensor.
 * 
 * @param sensor_type Sensor type
 * @param calibration Pointer to calibration structure
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_get_calibration(tmxc_sensor_type_t sensor_type,
                                    tmxc_sensor_calibration_t* calibration);

/**
 * @brief Set sensor calibration data
 * 
 * Sets the calibration data for a sensor.
 * 
 * @param sensor_type Sensor type
 * @param calibration Pointer to calibration structure
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_set_calibration(tmxc_sensor_type_t sensor_type,
                                    const tmxc_sensor_calibration_t* calibration);

/**
 * @brief Sensor interrupt handler
 * 
 * Called by the interrupt controller when a sensor interrupt occurs.
 * Reads data from the sensor and stores it in the buffer.
 * 
 * @param sensor_type Sensor type
 */
void tmxc_sensor_hal_interrupt_handler(tmxc_sensor_type_t sensor_type);

/**
 * @brief Get buffered sensor data
 * 
 * Gets buffered sensor data from the circular buffer.
 * 
 * @param sensor_type Sensor type
 * @param data Pointer to sensor data structure
 * @return 0 on success, negative error code on failure (e.g., buffer empty)
 */
int tmxc_sensor_hal_get_buffered_data(tmxc_sensor_type_t sensor_type, 
                                       tmxc_sensor_data_t* data);

/**
 * @brief Get buffer count
 * 
 * Gets the number of buffered data samples for a sensor.
 * 
 * @param sensor_type Sensor type
 * @return uint32_t Number of buffered samples
 */
uint32_t tmxc_sensor_hal_get_buffer_count(tmxc_sensor_type_t sensor_type);

/**
 * @brief Clear sensor buffer
 * 
 * Clears the data buffer for a sensor.
 * 
 * @param sensor_type Sensor type
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_clear_buffer(tmxc_sensor_type_t sensor_type);

/**
 * @brief Cleanup Sensor HAL
 * 
 * Cleans up the Sensor HAL subsystem.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_sensor_hal_cleanup(void);

/**
 * @brief Get sensor type string
 * 
 * Returns a human-readable string for a sensor type.
 * 
 * @param sensor_type Sensor type
 * @return const char* Sensor type string
 */
const char* tmxc_sensor_hal_type_string(tmxc_sensor_type_t sensor_type);

#endif /* TMXC_SENSOR_HAL_H */
