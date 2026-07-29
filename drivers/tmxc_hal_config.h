/*
 * TMXC_OS - Hardware-Agnostic Operating System
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * Universal Hardware Abstraction Layer (HAL) Configuration
 * Dynamically loads vendor-specific parameters at boot
 */

#ifndef TMXC_HAL_CONFIG_H
#define TMXC_HAL_CONFIG_H

#include "../kernel/tmxc_kernel.h"

#define TMXC_HAL_MAX_DEVICES 64
#define TMXC_HAL_CONFIG_PATH "/boot/hal_config.json"

typedef enum {
    TMXC_ARCH_ARM64 = 0,
    TMXC_ARCH_X86_64 = 1,
    TMXC_ARCH_RISCV64 = 2,
    TMXC_ARCH_UNKNOWN = 255
} tmxc_architecture_t;

typedef enum {
    TMXC_HAL_DISPLAY = 0,
    TMXC_HAL_POWER = 1,
    TMXC_HAL_NETWORK = 2,
    TMXC_HAL_AUDIO = 3,
    TMXC_HAL_CAMERA = 4,
    TMXC_HAL_SENSOR = 5,
    TMXC_HAL_STORAGE = 6
} tmxc_hal_component_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t refresh_rate;
    uint32_t color_depth;
    uint64_t base_address;
    uint32_t irq;
} tmxc_hal_display_config_t;

typedef struct {
    uint32_t battery_capacity;
    uint32_t charging_current;
    uint32_t voltage;
    uint64_t pmu_base_address;
    uint32_t pmu_irq;
} tmxc_hal_power_config_t;

typedef struct {
    uint32_t max_bandwidth;
    uint32_t supported_bands;
    uint64_t modem_base_address;
    uint32_t modem_irq;
} tmxc_hal_network_config_t;

typedef struct {
    tmxc_hal_display_config_t display;
    tmxc_hal_power_config_t power;
    tmxc_hal_network_config_t network;
    tmxc_architecture_t architecture;
    uint8_t has_npu;
    uint8_t has_gpu;
    uint32_t cpu_cores;
    uint64_t cpu_frequency;
    uint64_t npu_frequency;
    uint64_t gpu_frequency;
} tmxc_hal_config_t;

void tmxc_hal_config_init(void);
tmxc_hal_config_t* tmxc_hal_get_config(void);
tmxc_architecture_t tmxc_hal_detect_architecture(void);
uint8_t tmxc_hal_load_device_config(tmxc_hal_component_t component, void* config);
void tmxc_hal_detect_hardware_capabilities(void);

#endif
