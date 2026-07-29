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
#include "../../kernel/tmxc_kernel.h"

#define TMXC_DECOY_MAX_FILES 50
#define TMXC_DECOY_MAX_APPS 20

typedef struct {
    char filename[256];
    uint64_t file_size;
    uint8_t is_fake;
} tmxc_decoy_file_t;

typedef struct {
    char app_name[64];
    uint8_t is_fake;
} tmxc_decoy_app_t;

typedef struct {
    tmxc_decoy_file_t files[TMXC_DECOY_MAX_FILES];
    tmxc_decoy_app_t apps[TMXC_DECOY_MAX_APPS];
    uint32_t file_count;
    uint32_t app_count;
    uint8_t initialized;
    uint8_t decoy_mode_active;
    uint8_t wrong_password_detected;
} tmxc_decoy_os_t;

static tmxc_decoy_os_t tmxc_decoy;

void tmxc_decoy_os_init(void) {
    tmxc_decoy.initialized = 0;
    tmxc_decoy.decoy_mode_active = 0;
    tmxc_decoy.wrong_password_detected = 0;
    tmxc_decoy.file_count = 0;
    tmxc_decoy.app_count = 0;
    
    for (uint32_t i = 0; i < TMXC_DECOY_MAX_FILES; i++) {
        for (int j = 0; j < 256; j++) {
            tmxc_decoy.files[i].filename[j] = 0;
        }
        tmxc_decoy.files[i].file_size = 0;
        tmxc_decoy.files[i].is_fake = 0;
    }
    
    for (uint32_t i = 0; i < TMXC_DECOY_MAX_APPS; i++) {
        for (int j = 0; j < 64; j++) {
            tmxc_decoy.apps[i].app_name[j] = 0;
        }
        tmxc_decoy.apps[i].is_fake = 0;
    }
    
    tmxc_decoy.initialized = 1;
    
    tmxc_uart_puts("[DECOY-OS] Decoy OS initialized\r\n");
}

void tmxc_decoy_add_fake_file(const char* filename, uint64_t size) {
    if (!tmxc_decoy.initialized || filename == NULL || tmxc_decoy.file_count >= TMXC_DECOY_MAX_FILES) {
        return;
    }
    
    uint32_t index = tmxc_decoy.file_count;
    
    for (int j = 0; j < 256 && filename[j] != 0; j++) {
        tmxc_decoy.files[index].filename[j] = filename[j];
    }
    
    tmxc_decoy.files[index].file_size = size;
    tmxc_decoy.files[index].is_fake = 1;
    
    tmxc_decoy.file_count++;
    
    tmxc_uart_puts("[DECOY-OS] Fake file added\r\n");
}

void tmxc_decoy_add_fake_app(const char* app_name) {
    if (!tmxc_decoy.initialized || app_name == NULL || tmxc_decoy.app_count >= TMXC_DECOY_MAX_APPS) {
        return;
    }
    
    uint32_t index = tmxc_decoy.app_count;
    
    for (int j = 0; j < 64 && app_name[j] != 0; j++) {
        tmxc_decoy.apps[index].app_name[j] = app_name[j];
    }
    
    tmxc_decoy.apps[index].is_fake = 1;
    
    tmxc_decoy.app_count++;
    
    tmxc_uart_puts("[DECOY-OS] Fake app added\r\n");
}

void tmxc_decoy_activate_on_wrong_password(const uint8_t* attempted_password) {
    if (!tmxc_decoy.initialized || attempted_password == NULL) {
        return;
    }
    
    tmxc_decoy.wrong_password_detected = 1;
    tmxc_decoy.decoy_mode_active = 1;
    
    tmxc_uart_puts("[DECOY-OS] Wrong password detected - Decoy mode activated\r\n");
}

void tmxc_decoy_deactivate(void) {
    if (!tmxc_decoy.initialized) {
        return;
    }
    
    tmxc_decoy.decoy_mode_active = 0;
    tmxc_decoy.wrong_password_detected = 0;
    
    tmxc_uart_puts("[DECOY-OS] Decoy mode deactivated\r\n");
}

uint8_t tmxc_decoy_is_active(void) {
    return tmxc_decoy.decoy_mode_active;
}

uint32_t tmxc_decoy_get_fake_file_count(void) {
    return tmxc_decoy.file_count;
}

uint32_t tmxc_decoy_get_fake_app_count(void) {
    return tmxc_decoy.app_count;
}

void tmxc_decoy_os_cleanup(void) {
    if (!tmxc_decoy.initialized) {
        return;
    }
    
    tmxc_decoy_deactivate();
    tmxc_decoy.initialized = 0;
    
    tmxc_uart_puts("[DECOY-OS] Decoy OS cleaned up\r\n");
}
