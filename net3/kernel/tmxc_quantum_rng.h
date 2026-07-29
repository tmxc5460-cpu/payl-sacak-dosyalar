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
#ifndef TMXC_QUANTUM_RNG_H
#define TMXC_QUANTUM_RNG_H

#include "tmxc_kernel.h"

#define TMXC_ENTROPY_POOL_SIZE 256
#define TMXC_ENTROPY_MIN_THRESHOLD 128

void tmxc_quantum_rng_init(void);
uint8_t tmxc_qrng_collect_thermal_noise(void);
uint8_t tmxc_qrng_collect_rf_noise(void);
void tmxc_qrng_add_entropy(const uint8_t* data, uint32_t size);
void tmxc_qrng_collect_hardware_noise(void);
uint8_t tmxc_qrng_get_random_byte(void);
uint32_t tmxc_qrng_get_random_uint32(void);
uint64_t tmxc_qrng_get_random_uint64(void);
void tmxc_qrng_fill_buffer(uint8_t* buffer, uint32_t size);
uint32_t tmxc_qrng_get_entropy_count(void);
uint64_t tmxc_qrng_get_total_generated(void);
void tmxc_quantum_rng_cleanup(void);

#endif
