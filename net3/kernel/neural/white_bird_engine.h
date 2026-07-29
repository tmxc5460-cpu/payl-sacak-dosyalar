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
#ifndef TMXC_WHITE_BIRD_ENGINE_H
#define TMXC_WHITE_BIRD_ENGINE_H

#include "../tmxc_kernel.h"
#include "../../../drivers/tmxc_hal_config.h"

#define TMXC_WHITE_BIRD_WAKE_WORD "beyaz kuş"
#define TMXC_WHITE_BIRD_WAKE_WORD_EN "white bird"
#define TMXC_WHITE_BIRD_AUDIO_BUFFER_SIZE 4096
#define TMXC_WHITE_BIRD_SAMPLE_RATE 16000
#define TMXC_WHITE_BIRD_MFCC_FEATURES 13
#define TMXC_WHITE_BIRD_MODEL_SIZE 1024

typedef enum {
    TMXC_WHITE_BIRD_STATE_IDLE = 0,
    TMXC_WHITE_BIRD_STATE_LISTENING = 1,
    TMXC_WHITE_BIRD_STATE_PROCESSING = 2,
    TMXC_WHITE_BIRD_STATE_ACTIVE = 3,
    TMXC_WHITE_BIRD_STATE_ERROR = 4
} tmxc_white_bird_state_t;

typedef struct {
    int16_t audio_buffer[TMXC_WHITE_BIRD_AUDIO_BUFFER_SIZE];
    uint32_t buffer_index;
    float mfcc_features[TMXC_WHITE_BIRD_MFCC_FEATURES];
    float neural_weights[TMXC_WHITE_BIRD_MODEL_SIZE];
    float neural_biases[TMXC_WHITE_BIRD_MODEL_SIZE / 4];
    uint8_t wake_word_detected;
    uint8_t voice_command_active;
    char detected_command[128];
    uint64_t last_detection_time;
    tmxc_white_bird_state_t state;
    uint8_t initialized;
    uint8_t local_neural_enabled;
    float confidence_score;
    
    uint8_t hardware_detected;
    uint8_t use_npu;
    uint8_t use_gpu;
    uint8_t use_cpu_only;
    uint64_t cpu_frequency;
    uint64_t npu_frequency;
    uint64_t gpu_frequency;
    uint32_t cpu_cores;
} tmxc_white_bird_engine_t;

void tmxc_white_bird_engine_init(void);
void tmxc_white_bird_detect_hardware_capabilities(void);
void tmxc_white_bird_engine_enable_local_neural(uint8_t enable);
uint8_t tmxc_white_bird_engine_is_local_neural_enabled(void);

void tmxc_white_bird_engine_start_listening(void);
void tmxc_white_bird_engine_stop_listening(void);
void tmxc_white_bird_engine_process_audio(int16_t* audio_data, uint32_t samples);

uint8_t tmxc_white_bird_engine_is_wake_word_detected(void);
void tmxc_white_bird_engine_reset_detection(void);
const char* tmxc_white_bird_engine_get_detected_command(void);
float tmxc_white_bird_engine_get_confidence_score(void);

tmxc_white_bird_state_t tmxc_white_bird_engine_get_state(void);
void tmxc_white_bird_engine_set_state(tmxc_white_bird_state_t state);

void tmxc_white_bird_engine_train_wake_word(const int16_t* training_data, uint32_t samples);
void tmxc_white_bird_engine_load_model(const uint8_t* model_data, uint32_t size);
void tmxc_white_bird_engine_save_model(uint8_t* model_data, uint32_t* size);

void tmxc_white_bird_engine_cleanup(void);

#endif
