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
#ifndef TMXC_HARDWARE_TEST_H
#define TMXC_HARDWARE_TEST_H

#include "tmxc_kernel.h"

#define TMXC_HW_TEST_COMPONENTS 32

typedef enum {
    TMXC_HW_TEST_PASS = 0,
    TMXC_HW_TEST_FAIL = 1,
    TMXC_HW_TEST_WARNING = 2,
    TMXC_HW_TEST_SKIP = 3
} tmxc_hw_test_result_t;

void tmxc_hardware_test_init(void);
void tmxc_hw_test_component(const char* component_name, uint8_t (*test_func)(void));
void tmxc_hw_run_boot_tests(void);
void tmxc_hw_enable_boot_test(uint8_t enable);
uint32_t tmxc_hw_get_pass_count(void);
uint32_t tmxc_hw_get_fail_count(void);
void tmxc_hardware_test_cleanup(void);

uint8_t tmxc_hw_test_cpu(void);
uint8_t tmxc_hw_test_memory(void);
uint8_t tmxc_hw_test_gpu(void);
uint8_t tmxc_hw_test_display(void);
uint8_t tmxc_hw_test_camera(void);
uint8_t tmxc_hw_test_audio(void);
uint8_t tmxc_hw_test_sensors(void);
uint8_t tmxc_hw_test_battery(void);
uint8_t tmxc_hw_test_network(void);
uint8_t tmxc_hw_test_bluetooth(void);
uint8_t tmxc_hw_test_gps(void);
uint8_t tmxc_hw_test_storage(void);

#endif
