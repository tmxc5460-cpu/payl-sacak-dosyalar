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
#ifndef TMXC_BIOMETRIC_ENCRYPTION_H
#define TMXC_BIOMETRIC_ENCRYPTION_H

#include "../../kernel/tmxc_kernel.h"

#define TMXC_BIOMETRIC_FACE_ID 1
#define TMXC_BIOMETRIC_IRIS_ID 2
#define TMXC_BIOMETRIC_VOICE_ID 3
#define TMXC_BIOMETRIC_HEARTBEAT_ID 4
#define TMXC_BIOMETRIC_DATA_SIZE 512

typedef struct {
    uint8_t face_data[TMXC_BIOMETRIC_DATA_SIZE];
    uint8_t iris_data[TMXC_BIOMETRIC_DATA_SIZE];
    uint8_t voice_data[TMXC_BIOMETRIC_DATA_SIZE];
    uint8_t heartbeat_data[TMXC_BIOMETRIC_DATA_SIZE];
    uint8_t face_enrolled;
    uint8_t iris_enrolled;
    uint8_t voice_enrolled;
    uint8_t heartbeat_enrolled;
    uint64_t enrollment_timestamp;
} tmxc_biometric_data_t;

typedef struct {
    uint8_t encryption_key[64];
    uint8_t biometric_hash[64];
    uint8_t encryption_enabled;
    uint8_t multi_factor_required;
    uint8_t enrolled_factors;
    uint64_t last_authentication_time;
    uint8_t authentication_status;
} tmxc_biometric_encryption_t;

typedef struct {
    uint8_t face_match_score;
    uint8_t iris_match_score;
    uint8_t voice_match_score;
    uint8_t heartbeat_match_score;
    uint8_t overall_match_score;
    uint8_t authentication_success;
    uint64_t authentication_time_ms;
} tmxc_biometric_auth_result_t;

void tmxc_biometric_encryption_init(void);

int tmxc_biometric_enroll_face(const uint8_t* face_data);
int tmxc_biometric_enroll_iris(const uint8_t* iris_data);
int tmxc_biometric_enroll_voice(const uint8_t* voice_data);
int tmxc_biometric_enroll_heartbeat(const uint8_t* heartbeat_data);

tmxc_biometric_auth_result_t tmxc_biometric_authenticate(const uint8_t* face_data, const uint8_t* iris_data, 
                                                         const uint8_t* voice_data, const uint8_t* heartbeat_data);

void tmxc_biometric_encryption_enable(uint8_t enable);
void tmxc_biometric_set_multi_factor(uint8_t required);
uint8_t tmxc_biometric_get_enrolled_factors(void);

int tmxc_biometric_encrypt_data(const uint8_t* plaintext, uint64_t size, uint8_t* ciphertext);
int tmxc_biometric_decrypt_data(const uint8_t* ciphertext, uint64_t size, uint8_t* plaintext);

uint8_t tmxc_biometric_is_authenticated(void);
uint64_t tmxc_biometric_get_last_auth_time(void);

#endif
