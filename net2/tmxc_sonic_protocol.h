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
#ifndef TMXC_SONIC_PROTOCOL_H
#define TMXC_SONIC_PROTOCOL_H

#include "../kernel/tmxc_kernel.h"

#define TMXC_SONIC_MIN_FREQ 18000
#define TMXC_SONIC_MAX_FREQ 22000
#define TMXC_SONIC_SAMPLE_RATE 44100
#define TMXC_SONIC_BUFFER_SIZE 4096
#define TMXC_SONIC_MAX_PACKET_SIZE 1024
#define TMXC_SONIC_PREAMBLE_LENGTH 32
#define TMXC_SONIC_ENCRYPTION_KEY_LENGTH 32

typedef enum {
    TMXC_SONIC_STATE_IDLE = 0,
    TMXC_SONIC_STATE_TRANSMITTING = 1,
    TMXC_SONIC_STATE_RECEIVING = 2,
    TMXC_SONIC_STATE_SYNCING = 3
} tmxc_sonic_state_t;

typedef struct {
    uint8_t data[TMXC_SONIC_MAX_PACKET_SIZE];
    uint32_t size;
    uint32_t sequence_number;
    uint8_t encryption_key[TMXC_SONIC_ENCRYPTION_KEY_LENGTH];
    uint8_t is_encrypted;
    uint32_t checksum;
} tmxc_sonic_packet_t;

void tmxc_sonic_protocol_init(void);
uint8_t tmxc_sonic_transmit_packet(const uint8_t* data, uint32_t size, uint8_t encrypt);
uint8_t tmxc_sonic_receive_packet(uint8_t* buffer, uint32_t buffer_size, uint32_t* actual_size);
void tmxc_sonic_set_frequency(uint32_t frequency);
void tmxc_sonic_enable(uint8_t enable);
tmxc_sonic_state_t tmxc_sonic_get_state(void);
uint64_t tmxc_sonic_get_bytes_transmitted(void);
uint64_t tmxc_sonic_get_bytes_received(void);
void tmxc_sonic_cleanup(void);

#endif
