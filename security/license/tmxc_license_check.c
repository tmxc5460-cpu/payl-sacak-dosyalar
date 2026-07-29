/*
.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

#define LICENSE_FILE "/tmxc_license.dat"
#define LICENSE_FILE_WINDOWS "C:\\tmxc_license.dat"
#define MAX_LICENSE_SIZE 4096
#define HARDWARE_ID_LENGTH 32

typedef struct {
    char hardware_id[HARDWARE_ID_LENGTH + 1];
    char license_key[65];
    char customer_name[256];
    time_t issue_date;
    time_t expiry_date;
    int license_type;
    unsigned char signature[32];
} LicenseData;

typedef enum {
    LICENSE_DEMO = 0,
    LICENSE_STANDARD = 1,
    LICENSE_PRO = 2,
    LICENSE_ENTERPRISE = 3
} LicenseType;

static LicenseData current_license;
static int license_valid = 0;
static int license_checked = 0;

char* get_hardware_id(void) {
    static char hardware_id[HARDWARE_ID_LENGTH + 1];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    char buffer[1024];
    
    snprintf(buffer, sizeof(buffer), "TMXC_OS-%ld-%d", time(NULL), rand());
    
    SHA256((unsigned char*)buffer, strlen(buffer), hash);
    
    for (int i = 0; i < HARDWARE_ID_LENGTH / 2; i++) {
        sprintf(&hardware_id[i * 2], "%02x", hash[i]);
    }
    hardware_id[HARDWARE_ID_LENGTH] = '\0';
    
    return hardware_id;
}

int verify_license_signature(LicenseData* license) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    char buffer[1024];
    
    snprintf(buffer, sizeof(buffer), "%s%s%s%ld%ld%d",
             license->hardware_id,
             license->license_key,
             license->customer_name,
             license->issue_date,
             license->expiry_date,
             license->license_type);
    
    SHA256((unsigned char*)buffer, strlen(buffer), hash);
    
    for (int i = 0; i < 32; i++) {
        if (hash[i] != license->signature[i]) {
            return 0;
        }
    }
    
    return 1;
}

int load_license_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return 0;
    }
    
    size_t read_size = fread(&current_license, 1, sizeof(LicenseData), file);
    fclose(file);
    
    if (read_size != sizeof(LicenseData)) {
        return 0;
    }
    
    return 1;
}

int validate_license(void) {
    char* current_hardware_id = get_hardware_id();
    time_t current_time = time(NULL);
    
    if (strcmp(current_license.hardware_id, current_hardware_id) != 0) {
        printf("LISANS HATASI: Donanım ID uyuşmazlığı!\n");
        printf("Beklenen: %s\n", current_license.hardware_id);
        printf("Mevcut: %s\n", current_hardware_id);
        return 0;
    }
    
    if (current_time > current_license.expiry_date) {
        printf("LISANS HATASI: Lisans süresi dolmuş!\n");
        return 0;
    }
    
    if (!verify_license_signature(&current_license)) {
        printf("LISANS HATASI: Lisans imzası geçersiz!\n");
        return 0;
    }
    
    return 1;
}

int tmxc_license_check(void) {
    if (license_checked) {
        return license_valid;
    }
    
    const char* license_file = LICENSE_FILE;
    FILE* test = fopen(LICENSE_FILE, "rb");
    if (!test) {
        license_file = LICENSE_FILE_WINDOWS;
    } else {
        fclose(test);
    }
    
    if (!load_license_file(license_file)) {
        printf("LISANS HATASI: Lisans dosyası bulunamadı veya okunamadı!\n");
        printf("Dosya: %s\n", license_file);
        license_checked = 1;
        license_valid = 0;
        return 0;
    }
    
    license_valid = validate_license();
    license_checked = 1;
    
    if (license_valid) {
        printf("LISANS BAŞARILI: %s için %s lisansı geçerli.\n",
               current_license.customer_name,
               current_license.license_key);
    } else {
        printf("LISANS HATASI: Lisans doğrulama başarısız!\n");
        printf("Sistem kilitleniyor...\n");
        exit(1);
    }
    
    return license_valid;
}

void tmxc_license_info(void) {
    if (!tmxc_license_check()) {
        return;
    }
    
    printf("\n=== TMXC_OS Lisans Bilgileri ===\n");
    printf("Müşteri: %s\n", current_license.customer_name);
    printf("Lisans Anahtarı: %s\n", current_license.license_key);
    printf("Donanım ID: %s\n", current_license.hardware_id);
    printf("Lisans Türü: %d\n", current_license.license_type);
    printf("Veriliş Tarihi: %s", ctime(&current_license.issue_date));
    printf("Son Kullanma Tarihi: %s", ctime(&current_license.expiry_date));
    printf("Durum: %s\n", license_valid ? "GEÇERLİ" : "GEÇERSİZ");
    printf("================================\n\n");
}

int tmxc_is_license_valid(void) {
    return tmxc_license_check();
}

const char* tmxc_get_license_type(void) {
    if (!tmxc_license_check()) {
        return "GEÇERSİZ";
    }
    
    switch (current_license.license_type) {
        case LICENSE_DEMO:
            return "DEMO";
        case LICENSE_STANDARD:
            return "STANDART";
        case LICENSE_PRO:
            return "PRO";
        case LICENSE_ENTERPRISE:
            return "ENTERPRISE";
        default:
            return "BİLİNMEYEN";
    }
}
