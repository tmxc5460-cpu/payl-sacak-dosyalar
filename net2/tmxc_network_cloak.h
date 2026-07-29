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
#ifndef TMXC_NETWORK_CLOAK_H
#define TMXC_NETWORK_CLOAK_H

#include "../kernel/tmxc_kernel.h"

typedef enum {
    TMXC_CLOAK_MODE_VISIBLE = 0,
    TMXC_CLOAK_MODE_GHOST = 1,
    TMXC_CLOAK_MODE_NULL_NODE = 2
} tmxc_cloak_mode_t;

void tmxc_network_cloak_init(void);
void tmxc_network_cloak_set_mode(tmxc_cloak_mode_t mode);
void tmxc_network_cloak_spoof_mac(const uint8_t* new_mac);
void tmxc_network_cloak_enable(uint8_t enable);
uint8_t tmxc_network_cloak_is_enabled(void);
tmxc_cloak_mode_t tmxc_network_cloak_get_mode(void);
uint64_t tmxc_network_cloak_get_packets_dropped(void);
void tmxc_network_cloak_cleanup(void);

#endif
