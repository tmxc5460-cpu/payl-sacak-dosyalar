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
#ifndef TMXC_NEURAL_SYNC_H
#define TMXC_NEURAL_SYNC_H

#include "../tmxc_kernel.h"

#define TMXC_NEURAL_LEARNING_DAYS 7
#define TMXC_BEHAVIOR_MAX_ENTRIES 1000
#define TMXC_MESSAGE_DRAFT_MAX 256
#define TMXC_UI_PREFERENCE_MAX 32

typedef enum {
    TMXC_BEHAVIOR_APP_OPEN = 0,
    TMXC_BEHAVIOR_APP_CLOSE = 1,
    TMXC_BEHAVIOR_MESSAGE_SEND = 2,
    TMXC_BEHAVIOR_CALL_MAKE = 3,
    TMXC_BEHAVIOR_SETTING_CHANGE = 4,
    TMXC_BEHAVIOR_TIME_SLOT = 5,
    TMXC_BEHAVIOR_LOCATION = 6
} tmxc_behavior_type_t;

void tmxc_neural_sync_init(void);
void tmxc_neural_log_behavior(tmxc_behavior_type_t type, const char* app_name, const char* setting_name, uint8_t setting_value);
void tmxc_neural_learn_message_pattern(const char* recipient, const char* message);
char* tmxc_neural_suggest_reply(const char* recipient);
void tmxc_neural_learn_ui_preference(const char* setting_name, uint8_t value);
uint8_t tmxc_neural_get_preferred_setting(const char* setting_name);
void tmxc_neural_analyze_time_patterns(void);
char* tmxc_neural_predict_app_for_hour(uint8_t hour);
void tmxc_neural_complete_learning(void);
void tmxc_neural_enable_learning(uint8_t enable);
void tmxc_neural_enable_auto_reply(uint8_t enable);
void tmxc_neural_enable_ui_adaptation(uint8_t enable);
uint8_t tmxc_neural_is_learning_complete(void);
uint32_t tmxc_neural_get_behavior_count(void);
void tmxc_neural_sync_cleanup(void);

#endif
