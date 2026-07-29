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
#ifndef TMXC_PHYSICAL_KEY_KILL_H
#define TMXC_PHYSICAL_KEY_KILL_H

#include <stdint.h>

void tmxc_physical_key_kill_init(void);
void tmxc_physical_key_kill_trigger(void);
void tmxc_physical_key_kill_restore(void);
void tmxc_physical_key_kill_enable(uint8_t enable);
uint8_t tmxc_physical_key_kill_is_enabled(void);
uint8_t tmxc_physical_key_kill_is_emergency_mode(void);
uint8_t tmxc_physical_key_kill_microphone_status(void);
uint8_t tmxc_physical_key_kill_camera_status(void);
uint8_t tmxc_physical_key_kill_gps_status(void);
uint8_t tmxc_physical_key_kill_modem_status(void);
uint32_t tmxc_physical_key_kill_get_trigger_count(void);
void tmxc_physical_key_kill_check_button(void);

#endif
