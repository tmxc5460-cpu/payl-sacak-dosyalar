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
#include "tmxc_kernel.h"

static tmxc_localization_t tmxc_localization;

static tmxc_localization_strings_t tmxc_strings_tr = {
    .welcome_message = "Hoş Geldiniz",
    .setup_wizard_title = "Kurulum Sihirbazı",
    .ecosystem_sync_title = "Ecosystem Senkronizasyonu",
    .ecosystem_sync_desc = "Eski cihazınızdaki verileri Mesh ağı üzerinden aktarın",
    .security_setup_title = "Güvenlik Kurulumu",
    .security_setup_desc = "FaceID, İris ve Kalp Atışı sensörlerini kalibre edin",
    .network_stealth_title = "Ağ Gizliliği",
    .network_stealth_desc = "İnternete görünmez mi girmek istiyorsunuz, yoksa tam hız mı?",
    .calibration_title = "Cihaz Kalibrasyonu",
    .calibration_desc = "Çevresel koşullara göre otomatik ayar",
    .complete_title = "Kurulum Tamamlandı",
    .complete_message = "TMXC OS artık kullanıma hazır",
    .next_button = "İleri",
    .back_button = "Geri",
    .skip_button = "Atla",
    .stealth_mode = "Gizli Mod",
    .full_speed = "Tam Hız",
    .balanced = "Dengeli"
};

static tmxc_localization_strings_t tmxc_strings_en = {
    .welcome_message = "Welcome",
    .setup_wizard_title = "Setup Wizard",
    .ecosystem_sync_title = "Ecosystem Sync",
    .ecosystem_sync_desc = "Transfer data from your old device via Mesh network",
    .security_setup_title = "Security Setup",
    .security_setup_desc = "Calibrate FaceID, Iris and Heart Rate sensors",
    .network_stealth_title = "Network Stealth",
    .network_stealth_desc = "Do you want to be invisible on the internet or full speed?",
    .calibration_title = "Device Calibration",
    .calibration_desc = "Automatic adjustment based on environmental conditions",
    .complete_title = "Setup Complete",
    .complete_message = "TMXC OS is ready to use",
    .next_button = "Next",
    .back_button = "Back",
    .skip_button = "Skip",
    .stealth_mode = "Stealth Mode",
    .full_speed = "Full Speed",
    .balanced = "Balanced"
};

static tmxc_localization_strings_t tmxc_strings_de = {
    .welcome_message = "Willkommen",
    .setup_wizard_title = "Einrichtungsassistent",
    .ecosystem_sync_title = "Ecosystem-Synchronisierung",
    .ecosystem_sync_desc = "Daten vom alten Gerät über Mesh-Netzwerk übertragen",
    .security_setup_title = "Sicherheitseinrichtung",
    .security_setup_desc = "FaceID-, Iris- und Herzfrequenzsensoren kalibrieren",
    .network_stealth_title = "Netzwerk-Tarnung",
    .network_stealth_desc = "Möchten Sie unsichtbar im Internet sein oder volle Geschwindigkeit?",
    .calibration_title = "Gerätekalibrierung",
    .calibration_desc = "Automatische Anpassung an Umweltbedingungen",
    .complete_title = "Einrichtung abgeschlossen",
    .complete_message = "TMXC OS ist einsatzbereit",
    .next_button = "Weiter",
    .back_button = "Zurück",
    .skip_button = "Überspringen",
    .stealth_mode = "Tarnmodus",
    .full_speed = "Volle Geschwindigkeit",
    .balanced = "Ausgewogen"
};

static tmxc_localization_strings_t tmxc_strings_fr = {
    .welcome_message = "Bienvenue",
    .setup_wizard_title = "Assistant de configuration",
    .ecosystem_sync_title = "Synchronisation Ecosystem",
    .ecosystem_sync_desc = "Transférer les données de l'ancien appareil via le réseau Mesh",
    .security_setup_title = "Configuration de sécurité",
    .security_setup_desc = "Calibrer les capteurs FaceID, Iris et fréquence cardiaque",
    .network_stealth_title = "Furtivité réseau",
    .network_stealth_desc = "Voulez-vous être invisible sur Internet ou à pleine vitesse?",
    .calibration_title = "Calibration de l'appareil",
    .calibration_desc = "Ajustement automatique en fonction des conditions environnementales",
    .complete_title = "Configuration terminée",
    .complete_message = "TMXC OS est prêt à l'emploi",
    .next_button = "Suivant",
    .back_button = "Retour",
    .skip_button = "Ignorer",
    .stealth_mode = "Mode furtif",
    .full_speed = "Vitesse maximale",
    .balanced = "Équilibré"
};

static tmxc_localization_strings_t tmxc_strings_es = {
    .welcome_message = "Bienvenido",
    .setup_wizard_title = "Asistente de configuración",
    .ecosystem_sync_title = "Sincronización Ecosystem",
    .ecosystem_sync_desc = "Transfiere datos del dispositivo antiguo a través de la red Mesh",
    .security_setup_title = "Configuración de seguridad",
    .security_setup_desc = "Calibrar sensores FaceID, Iris y frecuencia cardíaca",
    .network_stealth_title = "Sigilo de red",
    .network_stealth_desc = "¿Quieres ser invisible en internet o a máxima velocidad?",
    .calibration_title = "Calibración del dispositivo",
    .calibration_desc = "Ajuste automático según condiciones ambientales",
    .complete_title = "Configuración completa",
    .complete_message = "TMXC OS está listo para usar",
    .next_button = "Siguiente",
    .back_button = "Atrás",
    .skip_button = "Omitir",
    .stealth_mode = "Modo sigilo",
    .full_speed = "Velocidad máxima",
    .balanced = "Equilibrado"
};

static tmxc_localization_strings_t tmxc_strings_ja = {
    .welcome_message = "ようこそ",
    .setup_wizard_title = "セットアップウィザード",
    .ecosystem_sync_title = "エコシステム同期",
    .ecosystem_sync_desc = "Meshネットワーク経由で古いデバイスからデータを転送",
    .security_setup_title = "セキュリティ設定",
    .security_setup_desc = "FaceID、虹彩、心拍センサーをキャリブレーション",
    .network_stealth_title = "ネットワークステルス",
    .network_stealth_desc = "インターネットで不可視にしますか、それともフルスピード?",
    .calibration_title = "デバイスキャリブレーション",
    .calibration_desc = "環境条件に基づく自動調整",
    .complete_title = "セットアップ完了",
    .complete_message = "TMXC OSの使用準備ができました",
    .next_button = "次へ",
    .back_button = "戻る",
    .skip_button = "スキップ",
    .stealth_mode = "ステルスモード",
    .full_speed = "フルスピード",
    .balanced = "バランス"
};

static tmxc_localization_strings_t tmxc_strings_zh = {
    .welcome_message = "欢迎",
    .setup_wizard_title = "设置向导",
    .ecosystem_sync_title = "生态系统同步",
    .ecosystem_sync_desc = "通过Mesh网络从旧设备传输数据",
    .security_setup_title = "安全设置",
    .security_setup_desc = "校准FaceID、虹膜和心率传感器",
    .network_stealth_title = "网络隐身",
    .network_stealth_desc = "您想在互联网上隐身还是全速?",
    .calibration_title = "设备校准",
    .calibration_desc = "根据环境条件自动调整",
    .complete_title = "设置完成",
    .complete_message = "TMXC OS已准备就绪",
    .next_button = "下一步",
    .back_button = "返回",
    .skip_button = "跳过",
    .stealth_mode = "隐身模式",
    .full_speed = "全速",
    .balanced = "平衡"
};

static tmxc_localization_strings_t tmxc_strings_ar = {
    .welcome_message = "مرحباً",
    .setup_wizard_title = "معالج الإعداد",
    .ecosystem_sync_title = "مزامنة النظام البيئي",
    .ecosystem_sync_desc = "نقل البيانات من الجهاز القديم عبر شبكة Mesh",
    .security_setup_title = "إعداد الأمان",
    .security_setup_desc = "معايرة مستشعرات FaceID والقزحية ومعدل ضربات القلب",
    .network_stealth_title = "التخفي الشبكي",
    .network_stealth_desc = "هل تريد أن تكون غير مرئي على الإنترنت أو بسرعة كاملة?",
    .calibration_title = "معايرة الجهاز",
    .calibration_desc = "تعديل تلقائي بناءً على الظروف البيئية",
    .complete_title = "اكتمل الإعداد",
    .complete_message = "TMXC OS جاهز للاستخدام",
    .next_button = "التالي",
    .back_button = "رجوع",
    .skip_button = "تخطي",
    .stealth_mode = "وضع التخفي",
    .full_speed = "السرعة الكاملة",
    .balanced = "متوازن"
};

void tmxc_localization_init(void) {
    tmxc_uart_puts("[LOCALIZATION] Initializing localization system...\r\n");
    
    tmxc_localization.language_count = 0;
    tmxc_localization.current_language_index = 0;
    tmxc_localization.initialized = 0;
    tmxc_localization.instant_translation_enabled = 0;
    
    tmxc_localization_add_language("tr", "Turkish", "Türkçe", 0);
    tmxc_localization_add_language("en", "English", "English", 0);
    tmxc_localization_add_language("de", "German", "Deutsch", 0);
    tmxc_localization_add_language("fr", "French", "Français", 0);
    tmxc_localization_add_language("es", "Spanish", "Español", 0);
    tmxc_localization_add_language("ja", "Japanese", "日本語", 0);
    tmxc_localization_add_language("zh", "Chinese", "中文", 0);
    tmxc_localization_add_language("ar", "Arabic", "العربية", 1);
    
    tmxc_localization.initialized = 1;
    tmxc_uart_puts("[LOCALIZATION] Localization system initialized with ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = tmxc_localization.language_count;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" languages\r\n");
}

void tmxc_localization_add_language(const char* code, const char* name, const char* native_name, uint8_t rtl) {
    if (tmxc_localization.language_count >= TMXC_MAX_LANGUAGES) {
        return;
    }
    
    uint32_t index = tmxc_localization.language_count;
    
    for (int i = 0; i < TMXC_LANGUAGE_CODE_LENGTH && code[i]; i++) {
        tmxc_localization.languages[index].code[i] = code[i];
    }
    tmxc_localization.languages[index].code[TMXC_LANGUAGE_CODE_LENGTH - 1] = '\0';
    
    for (int i = 0; i < 32 && name[i]; i++) {
        tmxc_localization.languages[index].name[i] = name[i];
    }
    tmxc_localization.languages[index].name[31] = '\0';
    
    for (int i = 0; i < 32 && native_name[i]; i++) {
        tmxc_localization.languages[index].native_name[i] = native_name[i];
    }
    tmxc_localization.languages[index].native_name[31] = '\0';
    
    tmxc_localization.languages[index].rtl = rtl;
    
    tmxc_localization.language_count++;
}

void tmxc_localization_set_language(const char* code) {
    for (uint32_t i = 0; i < tmxc_localization.language_count; i++) {
        uint8_t match = 1;
        for (int j = 0; j < TMXC_LANGUAGE_CODE_LENGTH; j++) {
            if (tmxc_localization.languages[i].code[j] != code[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            tmxc_localization.current_language_index = i;
            tmxc_uart_puts("[LOCALIZATION] Language set to: ");
            tmxc_uart_puts(tmxc_localization.languages[i].native_name);
            tmxc_uart_puts("\r\n");
            return;
        }
    }
}

tmxc_language_t* tmxc_localization_get_current_language(void) {
    if (!tmxc_localization.initialized) {
        return NULL;
    }
    
    return &tmxc_localization.languages[tmxc_localization.current_language_index];
}

tmxc_language_t* tmxc_localization_get_language_by_code(const char* code) {
    if (!tmxc_localization.initialized) {
        return NULL;
    }
    
    for (uint32_t i = 0; i < tmxc_localization.language_count; i++) {
        uint8_t match = 1;
        for (int j = 0; j < TMXC_LANGUAGE_CODE_LENGTH; j++) {
            if (tmxc_localization.languages[i].code[j] != code[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            return &tmxc_localization.languages[i];
        }
    }
    
    return NULL;
}

tmxc_language_t* tmxc_localization_get_all_languages(void) {
    if (!tmxc_localization.initialized) {
        return NULL;
    }
    
    return tmxc_localization.languages;
}

uint32_t tmxc_localization_get_language_count(void) {
    return tmxc_localization.language_count;
}

void tmxc_localization_enable_instant_translation(uint8_t enable) {
    tmxc_localization.instant_translation_enabled = enable;
    
    if (enable) {
        tmxc_uart_puts("[LOCALIZATION] Instant translation enabled\r\n");
    } else {
        tmxc_uart_puts("[LOCALIZATION] Instant translation disabled\r\n");
    }
}

uint8_t tmxc_localization_is_instant_translation_enabled(void) {
    return tmxc_localization.instant_translation_enabled;
}

const char* tmxc_localization_get_string(const char* key) {
    if (!tmxc_localization.initialized) {
        return key;
    }
    
    const char* code = tmxc_localization.languages[tmxc_localization.current_language_index].code;
    
    tmxc_localization_strings_t* strings = NULL;
    
    if (code[0] == 't' && code[1] == 'r') {
        strings = &tmxc_strings_tr;
    } else if (code[0] == 'e' && code[1] == 'n') {
        strings = &tmxc_strings_en;
    } else if (code[0] == 'd' && code[1] == 'e') {
        strings = &tmxc_strings_de;
    } else if (code[0] == 'f' && code[1] == 'r') {
        strings = &tmxc_strings_fr;
    } else if (code[0] == 'e' && code[1] == 's') {
        strings = &tmxc_strings_es;
    } else if (code[0] == 'j' && code[1] == 'a') {
        strings = &tmxc_strings_ja;
    } else if (code[0] == 'z' && code[1] == 'h') {
        strings = &tmxc_strings_zh;
    } else if (code[0] == 'a' && code[1] == 'r') {
        strings = &tmxc_strings_ar;
    } else {
        strings = &tmxc_strings_en;
    }
    
    if (strings == NULL) {
        return key;
    }
    
    if (key[0] == 'w' && key[1] == 'e') {
        return strings->welcome_message;
    } else if (key[0] == 's' && key[1] == 'e') {
        return strings->setup_wizard_title;
    } else if (key[0] == 'e' && key[1] == 'c') {
        return strings->ecosystem_sync_title;
    } else if (key[0] == 's' && key[1] == 'e' && key[2] == 'c') {
        return strings->security_setup_title;
    } else if (key[0] == 'n' && key[1] == 'e') {
        return strings->network_stealth_title;
    } else if (key[0] == 'c' && key[1] == 'a') {
        return strings->calibration_title;
    } else if (key[0] == 'c' && key[1] == 'o') {
        return strings->complete_title;
    } else if (key[0] == 'n' && key[1] == 'e' && key[2] == 'x') {
        return strings->next_button;
    } else if (key[0] == 'b' && key[1] == 'a') {
        return strings->back_button;
    } else if (key[0] == 's' && key[1] == 'k') {
        return strings->skip_button;
    }
    
    return key;
}
