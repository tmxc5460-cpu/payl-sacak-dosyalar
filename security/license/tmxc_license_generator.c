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
#include <openssl/rand.h>

#define HARDWARE_ID_LENGTH 32
#define LICENSE_KEY_LENGTH 64

typedef struct {
    char hardware_id[HARDWARE_ID_LENGTH + 1];
    char license_key[LICENSE_KEY_LENGTH + 1];
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

char* generate_license_key(void) {
    static char license_key[LICENSE_KEY_LENGTH + 1];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    char buffer[1024];
    
    snprintf(buffer, sizeof(buffer), "TMXC_LICENSE_%ld_%d", time(NULL), rand());
    
    SHA256((unsigned char*)buffer, strlen(buffer), hash);
    
    for (int i = 0; i < LICENSE_KEY_LENGTH; i++) {
        license_key[i] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"[hash[i] % 36];
    }
    license_key[LICENSE_KEY_LENGTH] = '\0';
    
    return license_key;
}

void generate_signature(LicenseData* license) {
    char buffer[1024];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    snprintf(buffer, sizeof(buffer), "%s%s%s%ld%ld%d",
             license->hardware_id,
             license->license_key,
             license->customer_name,
             license->issue_date,
             license->expiry_date,
             license->license_type);
    
    SHA256((unsigned char*)buffer, strlen(buffer), hash);
    
    memcpy(license->signature, hash, 32);
}

LicenseData create_license(const char* hardware_id, const char* customer_name, int license_type) {
    LicenseData license;
    time_t current_time = time(NULL);
    
    strncpy(license.hardware_id, hardware_id, HARDWARE_ID_LENGTH);
    license.hardware_id[HARDWARE_ID_LENGTH] = '\0';
    
    strncpy(license.license_key, generate_license_key(), LICENSE_KEY_LENGTH);
    license.license_key[LICENSE_KEY_LENGTH] = '\0';
    
    strncpy(license.customer_name, customer_name, 255);
    license.customer_name[255] = '\0';
    
    license.issue_date = current_time;
    license.license_type = license_type;
    
    switch (license_type) {
        case LICENSE_DEMO:
            license.expiry_date = current_time + (30 * 24 * 60 * 60);
            break;
        case LICENSE_STANDARD:
            license.expiry_date = current_time + (365 * 24 * 60 * 60);
            break;
        case LICENSE_PRO:
            license.expiry_date = current_time + (10 * 365 * 24 * 60 * 60);
            break;
        case LICENSE_ENTERPRISE:
            license.expiry_date = current_time + (50 * 365 * 24 * 60 * 60);
            break;
        default:
            license.expiry_date = current_time + (365 * 24 * 60 * 60);
    }
    
    generate_signature(&license);
    
    return license;
}

int save_license_file(LicenseData* license, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return 0;
    }
    
    size_t written = fwrite(license, 1, sizeof(LicenseData), file);
    fclose(file);
    
    return written == sizeof(LicenseData);
}

void print_license_info(LicenseData* license) {
    printf("\n=== TMXC_OS Lisans Bilgileri ===\n");
    printf("Müşteri: %s\n", license->customer_name);
    printf("Lisans Anahtarı: %s\n", license->license_key);
    printf("Donanım ID: %s\n", license->hardware_id);
    printf("Lisans Türü: %d\n", license->license_type);
    printf("Veriliş Tarihi: %s", ctime(&license->issue_date));
    printf("Son Kullanma Tarihi: %s", ctime(&license->expiry_date));
    printf("================================\n\n");
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Kullanım: %s <donanım_id> <müşteri_adı> [lisans_türü]\n", argv[0]);
        printf("Lisans Türleri:\n");
        printf("  0 - DEMO (30 gün)\n");
        printf("  1 - STANDART (1 yıl)\n");
        printf("  2 - PRO (10 yıl)\n");
        printf("  3 - ENTERPRISE (50 yıl)\n");
        return 1;
    }
    
    char* hardware_id = argv[1];
    char* customer_name = argv[2];
    int license_type = LICENSE_STANDARD;
    
    if (argc >= 4) {
        license_type = atoi(argv[3]);
    }
    
    if (strlen(hardware_id) != HARDWARE_ID_LENGTH) {
        printf("HATA: Donanım ID %d karakter olmalıdır.\n", HARDWARE_ID_LENGTH);
        return 1;
    }
    
    LicenseData license = create_license(hardware_id, customer_name, license_type);
    print_license_info(&license);
    
    const char* filename = "tmxc_license.dat";
    if (save_license_file(&license, filename)) {
        printf("Lisans dosyası başarıyla oluşturuldu: %s\n", filename);
    } else {
        printf("HATA: Lisans dosyası oluşturulamadı!\n");
        return 1;
    }
    
    return 0;
}
