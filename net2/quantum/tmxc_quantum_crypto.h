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
#ifndef TMXC_QUANTUM_CRYPTO_H
#define TMXC_QUANTUM_CRYPTO_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_QUANTUM_KEY_SIZE 256
#define TMXC_POST_QUANTUM_ALGORITHM_KYBER 1
#define TMXC_POST_QUANTUM_ALGORITHM_DILITHIUM 2
#define TMXC_POST_QUANTUM_ALGORITHM_NTRU 3

typedef struct {
    uint8_t public_key[TMXC_QUANTUM_KEY_SIZE];
    uint8_t private_key[TMXC_QUANTUM_KEY_SIZE];
    uint8_t shared_secret[TMXC_QUANTUM_KEY_SIZE];
    uint8_t key_initialized;
    uint32_t algorithm;
} tmxc_quantum_keypair_t;

typedef struct {
    uint8_t ciphertext[TMXC_QUANTUM_KEY_SIZE * 2];
    uint8_t signature[TMXC_QUANTUM_KEY_SIZE];
    uint64_t ciphertext_size;
    uint64_t signature_size;
} tmxc_quantum_encrypted_t;

typedef struct {
    uint64_t qkd_key_exchange_count;
    uint64_t qkd_key_bits_exchanged;
    uint64_t qkd_error_rate;
    uint8_t qkd_enabled;
    uint64_t quantum_random_bytes_generated;
} tmxc_quantum_stats_t;

void tmxc_quantum_crypto_init(void);
int tmxc_quantum_generate_keypair(tmxc_quantum_keypair_t* keypair, uint32_t algorithm);
int tmxc_quantum_key_exchange(tmxc_quantum_keypair_t* my_keypair, const uint8_t* peer_public_key, uint8_t* shared_secret);
int tmxc_quantum_encrypt(const uint8_t* plaintext, uint64_t plaintext_size, const uint8_t* key, tmxc_quantum_encrypted_t* encrypted);
int tmxc_quantum_decrypt(const tmxc_quantum_encrypted_t* encrypted, const uint8_t* key, uint8_t* plaintext);
int tmxc_quantum_sign(const uint8_t* message, uint64_t message_size, const tmxc_quantum_keypair_t* keypair, uint8_t* signature);
int tmxc_quantum_verify(const uint8_t* message, uint64_t message_size, const uint8_t* public_key, const uint8_t* signature);

uint8_t tmxc_quantum_get_random_byte(void);
void tmxc_quantum_get_random_bytes(uint8_t* buffer, uint64_t size);

void tmxc_quantum_qkd_init(void);
int tmxc_quantum_qkd_exchange_key(uint8_t* shared_key, uint64_t key_size);
uint64_t tmxc_quantum_qkd_get_error_rate(void);

tmxc_quantum_stats_t tmxc_quantum_get_stats(void);

#endif
