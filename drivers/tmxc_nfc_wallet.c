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
#include "../kernel/tmxc_kernel.h"

typedef struct {
    uint8_t card_number[16];
    uint8_t expiry_month;
    uint8_t expiry_year;
    uint8_t cvv[3];
    uint8_t card_holder_name[32];
    uint8_t token[32];
    uint8_t is_virtual;
    uint8_t is_active;
} tmxc_virtual_card_t;

typedef struct {
    uint8_t initialized;
    uint8_t powered;
    uint8_t field_active;
    tmxc_virtual_card_t cards[8];
    uint32_t card_count;
    uint8_t current_card_index;
    uint8_t payment_in_progress;
    uint8_t encryption_enabled;
} tmxc_nfc_wallet_state_t;

static tmxc_nfc_wallet_state_t tmxc_nfc_wallet;

#define TMXC_NFC_BASE 0x90000000
#define TMXC_NFC_CTRL 0x00
#define TMXC_NFC_STATUS 0x04
#define TMXC_NFC_DATA 0x08
#define TMXC_NFC_IRQ 0x0C

#define TMXC_NFC_CMD_POWER_ON 0x01
#define TMXC_NFC_CMD_POWER_OFF 0x02
#define TMXC_NFC_CMD_FIELD_ON 0x03
#define TMXC_NFC_CMD_FIELD_OFF 0x04
#define TMXC_NFC_CMD_SEND 0x05
#define TMXC_NFC_CMD_RECV 0x06

extern void tmxc_aes256_encrypt(const uint8_t* plaintext, uint64_t plaintext_len, 
                                const uint8_t* key, uint8_t* ciphertext);
extern void tmxc_aes256_decrypt(const uint8_t* ciphertext, uint64_t ciphertext_len, 
                                const uint8_t* key, uint8_t* plaintext);

static uint8_t tee_encryption_key[32] = {
    0x4A, 0x7E, 0x25, 0x16, 0x38, 0xAE, 0xD2, 0xB6,
    0xCB, 0xF7, 0x25, 0x88, 0x19, 0xCF, 0x4F, 0x3D,
    0x4A, 0x7E, 0x25, 0x16, 0x38, 0xAE, 0xD2, 0xB6,
    0xCB, 0xF7, 0x25, 0x88, 0x19, 0xCF, 0x4F, 0x3D
};

void tmxc_nfc_wallet_init(void) {
    tmxc_uart_puts("[NFC-WALLET] Initializing NFC wallet driver...\r\n");
    
    tmxc_nfc_wallet.initialized = 0;
    tmxc_nfc_wallet.powered = 0;
    tmxc_nfc_wallet.field_active = 0;
    tmxc_nfc_wallet.card_count = 0;
    tmxc_nfc_wallet.current_card_index = 0;
    tmxc_nfc_wallet.payment_in_progress = 0;
    tmxc_nfc_wallet.encryption_enabled = 1;
    
    for (uint32_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 16; j++) {
            tmxc_nfc_wallet.cards[i].card_number[j] = 0;
        }
        tmxc_nfc_wallet.cards[i].expiry_month = 0;
        tmxc_nfc_wallet.cards[i].expiry_year = 0;
        for (uint8_t j = 0; j < 3; j++) {
            tmxc_nfc_wallet.cards[i].cvv[j] = 0;
        }
        for (uint8_t j = 0; j < 32; j++) {
            tmxc_nfc_wallet.cards[i].card_holder_name[j] = 0;
        }
        for (uint8_t j = 0; j < 32; j++) {
            tmxc_nfc_wallet.cards[i].token[j] = 0;
        }
        tmxc_nfc_wallet.cards[i].is_virtual = 0;
        tmxc_nfc_wallet.cards[i].is_active = 0;
    }
    
    uint32_t nfc_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_CTRL));
    nfc_ctrl |= TMXC_NFC_CMD_POWER_ON;
    tmxc_write32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_CTRL), nfc_ctrl);
    
    tmxc_timer_delay_ms(50);
    
    uint32_t nfc_status = tmxc_read32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_STATUS));
    
    if (nfc_status & (1 << 0)) {
        tmxc_nfc_wallet.powered = 1;
        tmxc_uart_puts("[NFC-WALLET] NFC controller powered on\r\n");
    } else {
        tmxc_uart_puts("[NFC-WALLET] NFC controller power on failed\r\n");
        return;
    }
    
    tmxc_nfc_wallet.initialized = 1;
    tmxc_uart_puts("[NFC-WALLET] NFC wallet initialization complete\r\n");
}

int tmxc_nfc_wallet_add_card(const uint8_t* card_number, uint8_t expiry_month, uint8_t expiry_year,
                               const uint8_t* cvv, const char* card_holder_name) {
    if (!tmxc_nfc_wallet.initialized) {
        return -1;
    }
    
    if (tmxc_nfc_wallet.card_count >= 8) {
        return -2;
    }
    
    if (card_number == NULL || cvv == NULL || card_holder_name == NULL) {
        return -3;
    }
    
    uint32_t card_index = tmxc_nfc_wallet.card_count;
    
    for (uint8_t i = 0; i < 16; i++) {
        tmxc_nfc_wallet.cards[card_index].card_number[i] = card_number[i];
    }
    
    tmxc_nfc_wallet.cards[card_index].expiry_month = expiry_month;
    tmxc_nfc_wallet.cards[card_index].expiry_year = expiry_year;
    
    for (uint8_t i = 0; i < 3; i++) {
        tmxc_nfc_wallet.cards[card_index].cvv[i] = cvv[i];
    }
    
    for (uint8_t i = 0; i < 32 && card_holder_name[i] != '\0'; i++) {
        tmxc_nfc_wallet.cards[card_index].card_holder_name[i] = card_holder_name[i];
    }
    
    if (tmxc_nfc_wallet.encryption_enabled) {
        uint8_t card_data[64];
        for (uint8_t i = 0; i < 16; i++) {
            card_data[i] = card_number[i];
        }
        card_data[16] = expiry_month;
        card_data[17] = expiry_year;
        for (uint8_t i = 0; i < 3; i++) {
            card_data[18 + i] = cvv[i];
        }
        
        tmxc_aes256_encrypt(card_data, 21, tee_encryption_key, tmxc_nfc_wallet.cards[card_index].token);
        
        tmxc_nfc_wallet.cards[card_index].is_virtual = 1;
    } else {
        tmxc_nfc_wallet.cards[card_index].is_virtual = 0;
    }
    
    tmxc_nfc_wallet.cards[card_index].is_active = 1;
    tmxc_nfc_wallet.card_count++;
    
    tmxc_uart_puts("[NFC-WALLET] Card added (Virtual: ");
    if (tmxc_nfc_wallet.cards[card_index].is_virtual) {
        tmxc_uart_puts("Yes)\r\n");
    } else {
        tmxc_uart_puts("No)\r\n");
    }
    
    return 0;
}

int tmxc_nfc_wallet_select_card(uint32_t card_index) {
    if (!tmxc_nfc_wallet.initialized) {
        return -1;
    }
    
    if (card_index >= tmxc_nfc_wallet.card_count) {
        return -2;
    }
    
    tmxc_nfc_wallet.current_card_index = card_index;
    
    tmxc_uart_puts("[NFC-WALLET] Selected card index: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = card_index;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

int tmxc_nfc_wallet_activate_field(void) {
    if (!tmxc_nfc_wallet.initialized || !tmxc_nfc_wallet.powered) {
        return -1;
    }
    
    if (tmxc_nfc_wallet.field_active) {
        return -2;
    }
    
    uint32_t nfc_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_CTRL));
    nfc_ctrl |= TMXC_NFC_CMD_FIELD_ON;
    tmxc_write32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_CTRL), nfc_ctrl);
    
    tmxc_timer_delay_ms(10);
    
    uint32_t nfc_status = tmxc_read32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_STATUS));
    
    if (nfc_status & (1 << 1)) {
        tmxc_nfc_wallet.field_active = 1;
        tmxc_uart_puts("[NFC-WALLET] NFC field activated\r\n");
        return 0;
    }
    
    return -3;
}

int tmxc_nfc_wallet_deactivate_field(void) {
    if (!tmxc_nfc_wallet.initialized) {
        return -1;
    }
    
    if (!tmxc_nfc_wallet.field_active) {
        return -2;
    }
    
    uint32_t nfc_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_CTRL));
    nfc_ctrl |= TMXC_NFC_CMD_FIELD_OFF;
    tmxc_write32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_CTRL), nfc_ctrl);
    
    tmxc_timer_delay_ms(10);
    
    uint32_t nfc_status = tmxc_read32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_STATUS));
    
    if (!(nfc_status & (1 << 1))) {
        tmxc_nfc_wallet.field_active = 0;
        tmxc_uart_puts("[NFC-WALLET] NFC field deactivated\r\n");
        return 0;
    }
    
    return -3;
}

int tmxc_nfc_wallet_send_apdu(const uint8_t* apdu, uint32_t apdu_len, uint8_t* response, uint32_t max_resp_len) {
    if (!tmxc_nfc_wallet.initialized || !tmxc_nfc_wallet.field_active) {
        return -1;
    }
    
    if (apdu == NULL || apdu_len == 0) {
        return -2;
    }
    
    for (uint32_t i = 0; i < apdu_len; i++) {
        tmxc_write8((volatile uint8_t*)(TMXC_NFC_BASE + TMXC_NFC_DATA), apdu[i]);
    }
    
    uint32_t nfc_ctrl = tmxc_read32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_CTRL));
    nfc_ctrl |= TMXC_NFC_CMD_SEND;
    tmxc_write32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_CTRL), nfc_ctrl);
    
    tmxc_timer_delay_ms(20);
    
    uint32_t nfc_status = tmxc_read32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_STATUS));
    
    if (nfc_status & (1 << 2)) {
        uint32_t resp_len = tmxc_read32((volatile uint32_t*)(TMXC_NFC_BASE + TMXC_NFC_STATUS)) >> 16;
        
        if (resp_len > max_resp_len) {
            resp_len = max_resp_len;
        }
        
        for (uint32_t i = 0; i < resp_len; i++) {
            response[i] = tmxc_read8((volatile uint8_t*)(TMXC_NFC_BASE + TMXC_NFC_DATA));
        }
        
        tmxc_uart_puts("[NFC-WALLET] APDU sent, response received\r\n");
        return resp_len;
    }
    
    return -3;
}

int tmxc_nfc_wallet_process_payment(uint32_t amount_cents) {
    if (!tmxc_nfc_wallet.initialized) {
        return -1;
    }
    
    if (tmxc_nfc_wallet.card_count == 0) {
        return -2;
    }
    
    if (tmxc_nfc_wallet.payment_in_progress) {
        return -3;
    }
    
    tmxc_nfc_wallet.payment_in_progress = 1;
    
    tmxc_uart_puts("[NFC-WALLET] Processing payment: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = amount_cents;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" cents\r\n");
    
    uint8_t apdu_command[32];
    uint8_t apdu_len = 0;
    
    apdu_command[apdu_len++] = 0x00;
    apdu_command[apdu_len++] = 0xA4;
    apdu_command[apdu_len++] = 0x04;
    apdu_command[apdu_len++] = 0x00;
    apdu_command[apdu_len++] = 0x0A;
    
    tmxc_virtual_card_t* card = &tmxc_nfc_wallet.cards[tmxc_nfc_wallet.current_card_index];
    
    if (card->is_virtual) {
        for (uint8_t i = 0; i < 32; i++) {
            apdu_command[apdu_len++] = card->token[i];
        }
    } else {
        for (uint8_t i = 0; i < 16; i++) {
            apdu_command[apdu_len++] = card->card_number[i];
        }
    }
    
    apdu_command[apdu_len++] = (amount_cents >> 24) & 0xFF;
    apdu_command[apdu_len++] = (amount_cents >> 16) & 0xFF;
    apdu_command[apdu_len++] = (amount_cents >> 8) & 0xFF;
    apdu_command[apdu_len++] = amount_cents & 0xFF;
    
    uint8_t response[64];
    int result = tmxc_nfc_wallet_send_apdu(apdu_command, apdu_len, response, 64);
    
    if (result >= 2 && response[result - 2] == 0x90 && response[result - 1] == 0x00) {
        tmxc_uart_puts("[NFC-WALLET] Payment successful\r\n");
        tmxc_nfc_wallet.payment_in_progress = 0;
        return 0;
    } else {
        tmxc_uart_puts("[NFC-WALLET] Payment failed\r\n");
        tmxc_nfc_wallet.payment_in_progress = 0;
        return -4;
    }
}

int tmxc_nfc_wallet_remove_card(uint32_t card_index) {
    if (!tmxc_nfc_wallet.initialized) {
        return -1;
    }
    
    if (card_index >= tmxc_nfc_wallet.card_count) {
        return -2;
    }
    
    for (uint8_t i = 0; i < 16; i++) {
        tmxc_nfc_wallet.cards[card_index].card_number[i] = 0;
    }
    tmxc_nfc_wallet.cards[card_index].expiry_month = 0;
    tmxc_nfc_wallet.cards[card_index].expiry_year = 0;
    for (uint8_t i = 0; i < 3; i++) {
        tmxc_nfc_wallet.cards[card_index].cvv[i] = 0;
    }
    for (uint8_t i = 0; i < 32; i++) {
        tmxc_nfc_wallet.cards[card_index].card_holder_name[i] = 0;
    }
    for (uint8_t i = 0; i < 32; i++) {
        tmxc_nfc_wallet.cards[card_index].token[i] = 0;
    }
    tmxc_nfc_wallet.cards[card_index].is_virtual = 0;
    tmxc_nfc_wallet.cards[card_index].is_active = 0;
    
    tmxc_nfc_wallet.card_count--;
    
    tmxc_uart_puts("[NFC-WALLET] Card removed\r\n");
    
    return 0;
}

uint32_t tmxc_nfc_wallet_get_card_count(void) {
    return tmxc_nfc_wallet.card_count;
}

tmxc_virtual_card_t* tmxc_nfc_wallet_get_card(uint32_t card_index) {
    if (card_index >= tmxc_nfc_wallet.card_count) {
        return NULL;
    }
    
    return &tmxc_nfc_wallet.cards[card_index];
}

void tmxc_nfc_wallet_set_encryption(uint8_t enable) {
    tmxc_nfc_wallet.encryption_enabled = enable;
}

uint8_t tmxc_nfc_wallet_is_encryption_enabled(void) {
    return tmxc_nfc_wallet.encryption_enabled;
}

uint8_t tmxc_nfc_wallet_is_payment_in_progress(void) {
    return tmxc_nfc_wallet.payment_in_progress;
}
