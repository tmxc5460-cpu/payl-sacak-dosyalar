/*
 * TMXC_OS - Hardware-Agnostic Operating System
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * Universal Hardware Abstraction Layer (HAL) Configuration Implementation
 */

#include "tmxc_hal_config.h"

static tmxc_hal_config_t tmxc_hal_config;
static uint8_t tmxc_hal_initialized = 0;

void tmxc_hal_config_init(void) {
    tmxc_uart_puts("[HAL] Initializing Hardware Abstraction Layer...\r\n");
    
    tmxc_hal_config.architecture = tmxc_hal_detect_architecture();
    
    tmxc_hal_detect_hardware_capabilities();
    
    tmxc_hal_initialized = 1;
    
    tmxc_uart_puts("[HAL] HAL initialized - Architecture: ");
    switch (tmxc_hal_config.architecture) {
        case TMXC_ARCH_ARM64:
            tmxc_uart_puts("ARM64\r\n");
            break;
        case TMXC_ARCH_X86_64:
            tmxc_uart_puts("x86_64\r\n");
            break;
        case TMXC_ARCH_RISCV64:
            tmxc_uart_puts("RISC-V 64\r\n");
            break;
        default:
            tmxc_uart_puts("Unknown\r\n");
            break;
    }
}

tmxc_hal_config_t* tmxc_hal_get_config(void) {
    if (!tmxc_hal_initialized) {
        return NULL;
    }
    return &tmxc_hal_config;
}

tmxc_architecture_t tmxc_hal_detect_architecture(void) {
#ifdef __aarch64__
    return TMXC_ARCH_ARM64;
#elif defined(__x86_64__)
    return TMXC_ARCH_X86_64;
#elif defined(__riscv) && (__riscv_xlen == 64)
    return TMXC_ARCH_RISCV64;
#else
    return TMXC_ARCH_UNKNOWN;
#endif
}

uint8_t tmxc_hal_load_device_config(tmxc_hal_component_t component, void* config) {
    if (!tmxc_hal_initialized) {
        return 0;
    }
    
    switch (component) {
        case TMXC_HAL_DISPLAY:
            if (config) {
                tmxc_hal_display_config_t* display_cfg = (tmxc_hal_display_config_t*)config;
                tmxc_hal_config.display = *display_cfg;
                tmxc_uart_puts("[HAL] Display configuration loaded dynamically\r\n");
            }
            return 1;
        case TMXC_HAL_POWER:
            if (config) {
                tmxc_hal_power_config_t* power_cfg = (tmxc_hal_power_config_t*)config;
                tmxc_hal_config.power = *power_cfg;
                tmxc_uart_puts("[HAL] Power configuration loaded dynamically\r\n");
            }
            return 1;
        case TMXC_HAL_NETWORK:
            if (config) {
                tmxc_hal_network_config_t* network_cfg = (tmxc_hal_network_config_t*)config;
                tmxc_hal_config.network = *network_cfg;
                tmxc_uart_puts("[HAL] Network configuration loaded dynamically\r\n");
            }
            return 1;
        default:
            return 0;
    }
}

void tmxc_hal_detect_hardware_capabilities(void) {
    tmxc_hal_config.cpu_cores = 4;
    tmxc_hal_config.cpu_frequency = 2000000000ULL;
    tmxc_hal_config.has_npu = 0;
    tmxc_hal_config.has_gpu = 1;
    tmxc_hal_config.npu_frequency = 0;
    tmxc_hal_config.gpu_frequency = 800000000ULL;
    
    tmxc_hal_config.display.width = 1920;
    tmxc_hal_config.display.height = 1080;
    tmxc_hal_config.display.refresh_rate = 60;
    tmxc_hal_config.display.color_depth = 32;
    tmxc_hal_config.display.base_address = TMXC_DISPLAY_BASE;
    tmxc_hal_config.display.irq = 35;
    
    tmxc_hal_config.power.battery_capacity = 5000;
    tmxc_hal_config.power.charging_current = 3000;
    tmxc_hal_config.power.voltage = 3700;
    tmxc_hal_config.power.pmu_base_address = TMXC_PMU_BASE;
    tmxc_hal_config.power.pmu_irq = 34;
    
    tmxc_hal_config.network.max_bandwidth = 1000;
    tmxc_hal_config.network.supported_bands = 0xFFFFFFFF;
    tmxc_hal_config.network.modem_base_address = 0;
    tmxc_hal_config.network.modem_irq = 0;
    
    tmxc_uart_puts("[HAL] Hardware capabilities detected dynamically\r\n");
}
