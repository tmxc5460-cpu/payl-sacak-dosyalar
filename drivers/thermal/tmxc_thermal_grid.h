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
#ifndef TMXC_THERMAL_GRID_H
#define TMXC_THERMAL_GRID_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_THERMAL_GRID_SIZE 8
#define TMXC_THERMAL_HOT_THRESHOLD 70
#define TMXC_THERMAL_CRITICAL_THRESHOLD 85
#define TMXC_THERMAL_COLD_THRESHOLD 20

void tmxc_thermal_grid_init(void);
void tmxc_thermal_update_cell(uint8_t cell_index, double temperature);
void tmxc_thermal_redistribute_workload(void);
void tmxc_thermal_enable_throttling(void);
void tmxc_thermal_disable_throttling(void);
void tmxc_thermal_update(void);
double tmxc_thermal_get_max_temperature(void);
uint8_t tmxc_thermal_is_throttling(void);
void tmxc_thermal_cleanup(void);

#endif
