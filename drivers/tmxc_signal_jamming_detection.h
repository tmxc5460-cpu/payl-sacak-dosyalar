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
#ifndef TMXC_SIGNAL_JAMMING_DETECTION_H
#define TMXC_SIGNAL_JAMMING_DETECTION_H

#include <stdint.h>

#define TMXC_JAMMING_DETECTION_THRESHOLD 80
#define TMXC_JAMMING_ALERT_COOLDOWN_MS 5000
#define TMXC_SIGNAL_HISTORY_SIZE 100

void tmxc_signal_jamming_init(void);
void tmxc_signal_jamming_monitor(void);
void tmxc_signal_jamming_enable(uint8_t enable);
void tmxc_signal_jamming_enable_auto_countermeasures(uint8_t enable);
uint8_t tmxc_signal_jamming_is_enabled(void);
uint8_t tmxc_signal_jamming_is_jamming_detected(void);
uint8_t tmxc_signal_jamming_is_stingray_detected(void);
uint8_t tmxc_signal_jamming_is_imsi_catcher_detected(void);
int32_t tmxc_signal_jamming_get_current_signal(void);
int32_t tmxc_signal_jamming_get_baseline_signal(void);
uint32_t tmxc_signal_jamming_get_alert_count(void);
void tmxc_signal_jamming_reset_detection(void);
void tmxc_signal_jamming_set_threshold(uint8_t threshold);

#endif
