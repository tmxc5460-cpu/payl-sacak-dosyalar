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
#include "tmxc_biometric_encryption.h"

static tmxc_biometric_data_t tmxc_biometric_data;
static tmxc_biometric_encryption_t tmxc_biometric_encryption;

void tmxc_biometric_encryption_init(void) {
    for (int i = 0; i < TMXC_BIOMETRIC_DATA_SIZE; i++) {
        tmxc_biometric_data.face_data[i] = 0;
        tmxc_biometric_data.iris_data[i] = 0;
        tmxc_biometric_data.voice_data[i] = 0;
        tmxc_biometric_data.heartbeat_data[i] = 0;
    }
    
    tmxc_biometric_data.face_enrolled = 0;
    tmxc_biometric_data.iris_enrolled = 0;
    tmxc_biometric_data.voice_enrolled = 0;
    tmxc_biometric_data.heartbeat_enrolled = 0;
    tmxc_biometric_data.enrollment_timestamp = 0;
    
    for (int i = 0; i < 64; i++) {
        tmxc_biometric_encryption.encryption_key[i] = 0;
        tmxc_biometric_encryption.biometric_hash[i] = 0;
    }
    
    tmxc_biometric_encryption.encryption_enabled = 1;
    tmxc_biometric_encryption.multi_factor_required = 1;
    tmxc_biometric_encryption.enrolled_factors = 0;
    tmxc_biometric_encryption.last_authentication_time = 0;
    tmxc_biometric_encryption.authentication_status = 0;
    
    tmxc_uart_puts("[BIOMETRIC] Biometric encryption initialized\r\n");
}

int tmxc_biometric_enroll_face(const uint8_t* face_data) {
    if (face_data == NULL) {
        return -1;
    }
    
    for (int i = 0; i < TMXC_BIOMETRIC_DATA_SIZE; i++) {
        tmxc_biometric_data.face_data[i] = face_data[i];
    }
    
    tmxc_biometric_data.face_enrolled = 1;
    tmxc_biometric_encryption.enrolled_factors++;
    tmxc_biometric_data.enrollment_timestamp = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[BIOMETRIC] Face enrolled successfully\r\n");
    
    return 0;
}

int tmxc_biometric_enroll_iris(const uint8_t* iris_data) {
    if (iris_data == NULL) {
        return -1;
    }
    
    for (int i = 0; i < TMXC_BIOMETRIC_DATA_SIZE; i++) {
        tmxc_biometric_data.iris_data[i] = iris_data[i];
    }
    
    tmxc_biometric_data.iris_enrolled = 1;
    tmxc_biometric_encryption.enrolled_factors++;
    tmxc_biometric_data.enrollment_timestamp = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[BIOMETRIC] Iris enrolled successfully\r\n");
    
    return 0;
}

int tmxc_biometric_enroll_voice(const uint8_t* voice_data) {
    if (voice_data == NULL) {
        return -1;
    }
    
    for (int i = 0; i < TMXC_BIOMETRIC_DATA_SIZE; i++) {
        tmxc_biometric_data.voice_data[i] = voice_data[i];
    }
    
    tmxc_biometric_data.voice_enrolled = 1;
    tmxc_biometric_encryption.enrolled_factors++;
    tmxc_biometric_data.enrollment_timestamp = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[BIOMETRIC] Voice enrolled successfully\r\n");
    
    return 0;
}

int tmxc_biometric_enroll_heartbeat(const uint8_t* heartbeat_data) {
    if (heartbeat_data == NULL) {
        return -1;
    }
    
    for (int i = 0; i < TMXC_BIOMETRIC_DATA_SIZE; i++) {
        tmxc_biometric_data.heartbeat_data[i] = heartbeat_data[i];
    }
    
    tmxc_biometric_data.heartbeat_enrolled = 1;
    tmxc_biometric_encryption.enrolled_factors++;
    tmxc_biometric_data.enrollment_timestamp = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[BIOMETRIC] Heartbeat enrolled successfully\r\n");
    
    return 0;
}

tmxc_biometric_auth_result_t tmxc_biometric_authenticate(const uint8_t* face_data, const uint8_t* iris_data, 
                                                         const uint8_t* voice_data, const uint8_t* heartbeat_data) {
    tmxc_biometric_auth_result_t result;
    result.face_match_score = 0;
    result.iris_match_score = 0;
    result.voice_match_score = 0;
    result.heartbeat_match_score = 0;
    result.overall_match_score = 0;
    result.authentication_success = 0;
    result.authentication_time_ms = 0;
    
    uint64_t start_time = tmxc_get_cycle_count();
    
    uint8_t factors_matched = 0;
    uint8_t factors_required = tmxc_biometric_encryption.multi_factor_required ? 2 : 1;
    
    if (face_data != NULL && tmxc_biometric_data.face_enrolled) {
        uint32_t match_count = 0;
        for (int i = 0; i < TMXC_BIOMETRIC_DATA_SIZE; i++) {
            if (face_data[i] == tmxc_biometric_data.face_data[i]) {
                match_count++;
            }
        }
        result.face_match_score = (match_count * 100) / TMXC_BIOMETRIC_DATA_SIZE;
        if (result.face_match_score > 80) {
            factors_matched++;
        }
    }
    
    if (iris_data != NULL && tmxc_biometric_data.iris_enrolled) {
        uint32_t match_count = 0;
        for (int i = 0; i < TMXC_BIOMETRIC_DATA_SIZE; i++) {
            if (iris_data[i] == tmxc_biometric_data.iris_data[i]) {
                match_count++;
            }
        }
        result.iris_match_score = (match_count * 100) / TMXC_BIOMETRIC_DATA_SIZE;
        if (result.iris_match_score > 80) {
            factors_matched++;
        }
    }
    
    if (voice_data != NULL && tmxc_biometric_data.voice_enrolled) {
        uint32_t match_count = 0;
        for (int i = 0; i < TMXC_BIOMETRIC_DATA_SIZE; i++) {
            if (voice_data[i] == tmxc_biometric_data.voice_data[i]) {
                match_count++;
            }
        }
        result.voice_match_score = (match_count * 100) / TMXC_BIOMETRIC_DATA_SIZE;
        if (result.voice_match_score > 80) {
            factors_matched++;
        }
    }
    
    if (heartbeat_data != NULL && tmxc_biometric_data.heartbeat_enrolled) {
        uint32_t match_count = 0;
        for (int i = 0; i < TMXC_BIOMETRIC_DATA_SIZE; i++) {
            if (heartbeat_data[i] == tmxc_biometric_data.heartbeat_data[i]) {
                match_count++;
            }
        }
        result.heartbeat_match_score = (match_count * 100) / TMXC_BIOMETRIC_DATA_SIZE;
        if (result.heartbeat_match_score > 80) {
            factors_matched++;
        }
    }
    
    result.overall_match_score = (result.face_match_score + result.iris_match_score + 
                                   result.voice_match_score + result.heartbeat_match_score) / 4;
    
    if (factors_matched >= factors_required && result.overall_match_score > 70) {
        result.authentication_success = 1;
        tmxc_biometric_encryption.authentication_status = 1;
        tmxc_biometric_encryption.last_authentication_time = tmxc_get_cycle_count();
        
        for (int i = 0; i < 64; i++) {
            tmxc_biometric_encryption.biometric_hash[i] = tmxc_quantum_get_random_byte();
        }
        
        tmxc_uart_puts("[BIOMETRIC] Authentication successful\r\n");
    } else {
        tmxc_biometric_encryption.authentication_status = 0;
        tmxc_uart_puts("[BIOMETRIC] Authentication failed\r\n");
    }
    
    uint64_t end_time = tmxc_get_cycle_count();
    result.authentication_time_ms = (end_time - start_time) * 1000 / tmxc_get_frequency();
    
    return result;
}

void tmxc_biometric_encryption_enable(uint8_t enable) {
    tmxc_biometric_encryption.encryption_enabled = enable;
}

void tmxc_biometric_set_multi_factor(uint8_t required) {
    tmxc_biometric_encryption.multi_factor_required = required;
}

uint8_t tmxc_biometric_get_enrolled_factors(void) {
    return tmxc_biometric_encryption.enrolled_factors;
}

int tmxc_biometric_encrypt_data(const uint8_t* plaintext, uint64_t size, uint8_t* ciphertext) {
    if (!tmxc_biometric_encryption.encryption_enabled || plaintext == NULL || ciphertext == NULL || size == 0) {
        return -1;
    }
    
    if (!tmxc_biometric_encryption.authentication_status) {
        return -2;
    }
    
    for (uint64_t i = 0; i < size; i++) {
        ciphertext[i] = plaintext[i] ^ tmxc_biometric_encryption.biometric_hash[i % 64];
    }
    
    return 0;
}

int tmxc_biometric_decrypt_data(const uint8_t* ciphertext, uint64_t size, uint8_t* plaintext) {
    if (!tmxc_biometric_encryption.encryption_enabled || ciphertext == NULL || plaintext == NULL || size == 0) {
        return -1;
    }
    
    if (!tmxc_biometric_encryption.authentication_status) {
        return -2;
    }
    
    for (uint64_t i = 0; i < size; i++) {
        plaintext[i] = ciphertext[i] ^ tmxc_biometric_encryption.biometric_hash[i % 64];
    }
    
    return 0;
}

uint8_t tmxc_biometric_is_authenticated(void) {
    return tmxc_biometric_encryption.authentication_status;
}

uint64_t tmxc_biometric_get_last_auth_time(void) {
    return tmxc_biometric_encryption.last_authentication_time;
}
