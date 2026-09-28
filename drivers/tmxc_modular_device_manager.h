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
#ifndef TMXC_MODULAR_DEVICE_MANAGER_H
#define TMXC_MODULAR_DEVICE_MANAGER_H

#include "../kernel/tmxc_kernel.h"

#define TMXC_MDM_MAX_DEVICES 64
#define TMXC_MDM_MAX_DRIVERS 32
#define TMXC_MDM_DEVICE_NAME_LENGTH 64
#define TMXC_MDM_DRIVER_NAME_LENGTH 64
#define TMXC_MDM_MAX_RESOURCES 16

typedef enum {
    TMXC_DEVICE_TYPE_SENSOR = 0,
    TMXC_DEVICE_TYPE_CAMERA = 1,
    TMXC_DEVICE_TYPE_BATTERY = 2,
    TMXC_DEVICE_TYPE_DISPLAY = 3,
    TMXC_DEVICE_TYPE_AUDIO = 4,
    TMXC_DEVICE_TYPE_NETWORK = 5,
    TMXC_DEVICE_TYPE_STORAGE = 6,
    TMXC_DEVICE_TYPE_INPUT = 7,
    TMXC_DEVICE_TYPE_CUSTOM = 8
} tmxc_device_type_t;

typedef enum {
    TMXC_DRIVER_STATE_UNLOADED = 0,
    TMXC_DRIVER_STATE_LOADED = 1,
    TMXC_DRIVER_STATE_INITIALIZED = 2,
    TMXC_DRIVER_STATE_RUNNING = 3,
    TMXC_DRIVER_STATE_ERROR = 4
} tmxc_driver_state_t;

typedef struct {
    uint64_t base_address;
    uint64_t size;
    uint32_t irq;
    uint8_t type;
} tmxc_device_resource_t;

typedef struct {
    uint32_t device_id;
    char device_name[TMXC_MDM_DEVICE_NAME_LENGTH];
    tmxc_device_type_t device_type;
    uint64_t vendor_id;
    uint64_t product_id;
    uint8_t is_connected;
    uint8_t is_initialized;
    uint32_t driver_id;
    tmxc_device_resource_t resources[TMXC_MDM_MAX_RESOURCES];
    uint32_t resource_count;
    uint64_t detection_time;
} tmxc_device_t;

typedef struct {
    uint32_t driver_id;
    char driver_name[TMXC_MDM_DRIVER_NAME_LENGTH];
    tmxc_device_type_t supported_device_type;
    uint64_t vendor_id_mask;
    uint64_t product_id_mask;
    tmxc_driver_state_t state;
    void* driver_code;
    uint64_t driver_size;
    uint64_t load_time;
    uint32_t device_count;
} tmxc_driver_t;

void tmxc_modular_device_manager_init(void);
uint32_t tmxc_mdm_detect_devices(void);
void tmxc_mdm_detect_sensors(void);
void tmxc_mdm_detect_camera(void);
void tmxc_mdm_detect_battery(void);
void tmxc_mdm_detect_display(void);
void tmxc_mdm_detect_audio(void);
uint32_t tmxc_mdm_load_driver(uint32_t device_id);
uint8_t tmxc_mdm_initialize_device(uint32_t device_id);
void tmxc_mdm_unload_driver(uint32_t driver_id);
void tmxc_mdm_enable(uint8_t enable);
void tmxc_mdm_set_auto_detection(uint8_t enable);
tmxc_device_t* tmxc_mdm_get_devices(uint32_t* count);
tmxc_driver_t* tmxc_mdm_get_drivers(uint32_t* count);
void tmxc_mdm_cleanup(void);

#endif
