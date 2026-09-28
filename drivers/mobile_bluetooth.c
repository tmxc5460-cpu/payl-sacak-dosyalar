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
    uint8_t addr[6];
    char name[32];
    int8_t rssi;
    uint8_t device_class;
    uint8_t paired;
    uint8_t connected;
} tmxc_bluetooth_device_t;

typedef struct {
    uint8_t initialized;
    uint8_t powered;
    uint8_t scanning;
    uint8_t connected;
    tmxc_bluetooth_device_t devices[16];
    uint32_t device_count;
    tmxc_bluetooth_device_t* connected_device;
    uint8_t local_addr[6];
    uint8_t acl_enabled;
    uint8_t sco_enabled;
} tmxc_bluetooth_state_t;

static tmxc_bluetooth_state_t tmxc_bluetooth;

#define TMXC_BT_HCI_BASE 0x80000000
#define TMXC_BT_HCI_CMD 0x00
#define TMXC_BT_HCI_ACL 0x02
#define TMXC_BT_HCI_SCO 0x03
#define TMXC_BT_HCI_EVENT 0x04

#define TMXC_BT_OGF_LINK_CONTROL 0x01
#define TMXC_BT_OGF_LINK_POLICY 0x02
#define TMXC_BT_OGF_CTRL_BASEBAND 0x03
#define TMXC_BT_OGF_INFO_PARAM 0x04
#define TMXC_BT_OGF_STATUS_PARAM 0x05

#define TMXC_BT_OCF_RESET 0x0003
#define TMXC_BT_OCF_READ_BD_ADDR 0x0009
#define TMXC_BT_OCF_INQUIRY 0x0001
#define TMXC_BT_OCF_INQUIRY_CANCEL 0x0002
#define TMXC_BT_OCF_CREATE_CONNECTION 0x0005
#define TMXC_BT_OCF_DISCONNECT 0x0006
#define TMXC_BT_OCF_AUTH_REQUESTED 0x000F
#define TMXC_BT_OCF_REMOTE_NAME_REQ 0x0019

static void tmxc_bluetooth_hci_write(uint8_t* data, uint32_t len) {
    for (uint32_t i = 0; i < len; i++) {
        tmxc_write8((volatile uint8_t*)(TMXC_BT_HCI_BASE), data[i]);
    }
}

static uint32_t tmxc_bluetooth_hci_read(uint8_t* buffer, uint32_t max_len) {
    uint32_t len = 0;
    
    while (len < max_len) {
        uint32_t status = tmxc_read32((volatile uint32_t*)(TMXC_BT_HCI_BASE + 0x04));
        
        if (status & (1 << 0)) {
            buffer[len++] = tmxc_read8((volatile uint8_t*)(TMXC_BT_HCI_BASE + 0x08));
        } else {
            break;
        }
    }
    
    return len;
}

static int tmxc_bluetooth_send_hci_command(uint16_t ogf, uint16_t ocf, uint8_t* params, uint8_t param_len) {
    uint8_t cmd[256];
    uint32_t cmd_len = 3 + param_len;
    
    cmd[0] = (ocf & 0xFF);
    cmd[1] = ((ogf << 2) & 0xFC) | ((ocf >> 8) & 0x03);
    cmd[2] = param_len;
    
    for (uint8_t i = 0; i < param_len; i++) {
        cmd[3 + i] = params[i];
    }
    
    tmxc_bluetooth_hci_write(cmd, cmd_len);
    
    uint8_t event[64];
    uint32_t event_len = tmxc_bluetooth_hci_read(event, 64);
    
    if (event_len > 0 && event[0] == 0x0E) {
        return 0;
    }
    
    return -1;
}

void tmxc_bluetooth_init(void) {
    tmxc_uart_puts("[BT] Initializing Bluetooth driver...\r\n");
    
    tmxc_bluetooth.initialized = 0;
    tmxc_bluetooth.powered = 0;
    tmxc_bluetooth.scanning = 0;
    tmxc_bluetooth.connected = 0;
    tmxc_bluetooth.device_count = 0;
    tmxc_bluetooth.connected_device = NULL;
    tmxc_bluetooth.acl_enabled = 1;
    tmxc_bluetooth.sco_enabled = 1;
    
    for (uint32_t i = 0; i < 16; i++) {
        for (uint8_t j = 0; j < 6; j++) {
            tmxc_bluetooth.devices[i].addr[j] = 0;
        }
        for (uint8_t j = 0; j < 32; j++) {
            tmxc_bluetooth.devices[i].name[j] = 0;
        }
        tmxc_bluetooth.devices[i].rssi = 0;
        tmxc_bluetooth.devices[i].device_class = 0;
        tmxc_bluetooth.devices[i].paired = 0;
        tmxc_bluetooth.devices[i].connected = 0;
    }
    
    for (uint8_t i = 0; i < 6; i++) {
        tmxc_bluetooth.local_addr[i] = 0;
    }
    
    uint32_t gpio_reg = tmxc_read32((volatile uint32_t*)TMXC_GPIO_BASE);
    gpio_reg |= (1 << 20);
    tmxc_write32((volatile uint32_t*)TMXC_GPIO_BASE, gpio_reg);
    
    tmxc_timer_delay_ms(100);
    
    int result = tmxc_bluetooth_send_hci_command(TMXC_BT_OGF_CTRL_BASEBAND, TMXC_BT_OCF_RESET, NULL, 0);
    
    if (result == 0) {
        tmxc_uart_puts("[BT] HCI reset successful\r\n");
        tmxc_bluetooth.powered = 1;
    } else {
        tmxc_uart_puts("[BT] HCI reset failed\r\n");
        return;
    }
    
    tmxc_timer_delay_ms(100);
    
    result = tmxc_bluetooth_send_hci_command(TMXC_BT_OGF_INFO_PARAM, TMXC_BT_OCF_READ_BD_ADDR, NULL, 0);
    
    if (result == 0) {
        uint8_t event[64];
        uint32_t event_len = tmxc_bluetooth_hci_read(event, 64);
        
        if (event_len >= 10 && event[0] == 0x0E) {
            for (uint8_t i = 0; i < 6; i++) {
                tmxc_bluetooth.local_addr[i] = event[7 + i];
            }
            
            tmxc_uart_puts("[BT] Local MAC: ");
            char hex_chars[] = "0123456789ABCDEF";
            for (int i = 5; i >= 0; i--) {
                tmxc_uart_putc(hex_chars[(tmxc_bluetooth.local_addr[i] >> 4) & 0x0F]);
                tmxc_uart_putc(hex_chars[tmxc_bluetooth.local_addr[i] & 0x0F]);
                if (i > 0) tmxc_uart_putc(':');
            }
            tmxc_uart_puts("\r\n");
        }
    }
    
    tmxc_bluetooth.initialized = 1;
    tmxc_uart_puts("[BT] Bluetooth initialization complete\r\n");
}

int tmxc_bluetooth_start_scan(void) {
    if (!tmxc_bluetooth.initialized || !tmxc_bluetooth.powered) {
        return -1;
    }
    
    if (tmxc_bluetooth.scanning) {
        return -2;
    }
    
    tmxc_uart_puts("[BT] Starting device scan...\r\n");
    
    uint8_t params[5];
    params[0] = 0x33;
    params[1] = 0x8B;
    params[2] = 0x9E;
    params[3] = 0x10;
    params[4] = 0x00;
    
    int result = tmxc_bluetooth_send_hci_command(TMXC_BT_OGF_LINK_CONTROL, TMXC_BT_OCF_INQUIRY, params, 5);
    
    if (result == 0) {
        tmxc_bluetooth.scanning = 1;
        tmxc_bluetooth.device_count = 0;
        return 0;
    }
    
    return -1;
}

int tmxc_bluetooth_stop_scan(void) {
    if (!tmxc_bluetooth.initialized) {
        return -1;
    }
    
    if (!tmxc_bluetooth.scanning) {
        return -2;
    }
    
    int result = tmxc_bluetooth_send_hci_command(TMXC_BT_OGF_LINK_CONTROL, TMXC_BT_OCF_INQUIRY_CANCEL, NULL, 0);
    
    if (result == 0) {
        tmxc_bluetooth.scanning = 0;
        tmxc_uart_puts("[BT] Scan stopped. Found ");
        char buffer[21];
        int pos = 20;
        buffer[pos] = '\0';
        uint64_t temp = tmxc_bluetooth.device_count;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts(" devices\r\n");
        return 0;
    }
    
    return -1;
}

uint32_t tmxc_bluetooth_get_devices(tmxc_bluetooth_device_t* devices, uint32_t max_count) {
    if (!tmxc_bluetooth.initialized || devices == NULL) {
        return 0;
    }
    
    uint32_t count = tmxc_bluetooth.device_count;
    if (count > max_count) {
        count = max_count;
    }
    
    for (uint32_t i = 0; i < count; i++) {
        for (uint8_t j = 0; j < 6; j++) {
            devices[i].addr[j] = tmxc_bluetooth.devices[i].addr[j];
        }
        for (uint8_t j = 0; j < 32; j++) {
            devices[i].name[j] = tmxc_bluetooth.devices[i].name[j];
        }
        devices[i].rssi = tmxc_bluetooth.devices[i].rssi;
        devices[i].device_class = tmxc_bluetooth.devices[i].device_class;
        devices[i].paired = tmxc_bluetooth.devices[i].paired;
        devices[i].connected = tmxc_bluetooth.devices[i].connected;
    }
    
    return count;
}

int tmxc_bluetooth_pair_device(const char* mac_addr) {
    if (!tmxc_bluetooth.initialized || mac_addr == NULL) {
        return -1;
    }
    
    tmxc_uart_puts("[BT] Pairing device: ");
    tmxc_uart_puts(mac_addr);
    tmxc_uart_puts("\r\n");
    
    uint8_t bd_addr[6];
    uint32_t addr_pos = 0;
    uint32_t byte_pos = 0;
    uint8_t byte_val = 0;
    
    for (uint32_t i = 0; mac_addr[i] != '\0' && byte_pos < 6; i++) {
        char c = mac_addr[i];
        
        if (c >= '0' && c <= '9') {
            byte_val = byte_val * 16 + (c - '0');
        } else if (c >= 'A' && c <= 'F') {
            byte_val = byte_val * 16 + (c - 'A' + 10);
        } else if (c >= 'a' && c <= 'f') {
            byte_val = byte_val * 16 + (c - 'a' + 10);
        } else if (c == ':') {
            bd_addr[byte_pos++] = byte_val;
            byte_val = 0;
        }
        
        if (addr_pos % 2 == 1 && c != ':') {
            bd_addr[byte_pos++] = byte_val;
            byte_val = 0;
        }
        addr_pos++;
    }
    
    uint8_t params[7];
    params[0] = 0x01;
    for (uint8_t i = 0; i < 6; i++) {
        params[1 + i] = bd_addr[i];
    }
    
    int result = tmxc_bluetooth_send_hci_command(TMXC_BT_OGF_LINK_CONTROL, TMXC_BT_OCF_AUTH_REQUESTED, params, 7);
    
    if (result == 0) {
        for (uint32_t i = 0; i < 16; i++) {
            uint8_t match = 1;
            for (uint8_t j = 0; j < 6; j++) {
                if (tmxc_bluetooth.devices[i].addr[j] != bd_addr[j]) {
                    match = 0;
                    break;
                }
            }
            
            if (match) {
                tmxc_bluetooth.devices[i].paired = 1;
                break;
            }
        }
        
        tmxc_uart_puts("[BT] Pairing successful\r\n");
        return 0;
    }
    
    tmxc_uart_puts("[BT] Pairing failed\r\n");
    return -1;
}

int tmxc_bluetooth_connect(const char* mac_addr) {
    if (!tmxc_bluetooth.initialized || mac_addr == NULL) {
        return -1;
    }
    
    if (tmxc_bluetooth.connected) {
        return -2;
    }
    
    tmxc_uart_puts("[BT] Connecting to: ");
    tmxc_uart_puts(mac_addr);
    tmxc_uart_puts("\r\n");
    
    uint8_t bd_addr[6];
    uint32_t addr_pos = 0;
    uint32_t byte_pos = 0;
    uint8_t byte_val = 0;
    
    for (uint32_t i = 0; mac_addr[i] != '\0' && byte_pos < 6; i++) {
        char c = mac_addr[i];
        
        if (c >= '0' && c <= '9') {
            byte_val = byte_val * 16 + (c - '0');
        } else if (c >= 'A' && c <= 'F') {
            byte_val = byte_val * 16 + (c - 'A' + 10);
        } else if (c >= 'a' && c <= 'f') {
            byte_val = byte_val * 16 + (c - 'a' + 10);
        } else if (c == ':') {
            bd_addr[byte_pos++] = byte_val;
            byte_val = 0;
        }
        
        if (addr_pos % 2 == 1 && c != ':') {
            bd_addr[byte_pos++] = byte_val;
            byte_val = 0;
        }
        addr_pos++;
    }
    
    uint8_t params[13];
    for (uint8_t i = 0; i < 6; i++) {
        params[i] = bd_addr[5 - i];
    }
    params[6] = 0x18;
    params[7] = 0xCC;
    params[8] = 0x01;
    params[9] = 0x00;
    params[10] = 0x00;
    params[11] = 0x00;
    params[12] = 0x00;
    
    int result = tmxc_bluetooth_send_hci_command(TMXC_BT_OGF_LINK_CONTROL, TMXC_BT_OCF_CREATE_CONNECTION, params, 13);
    
    if (result == 0) {
        for (uint32_t i = 0; i < 16; i++) {
            uint8_t match = 1;
            for (uint8_t j = 0; j < 6; j++) {
                if (tmxc_bluetooth.devices[i].addr[j] != bd_addr[j]) {
                    match = 0;
                    break;
                }
            }
            
            if (match) {
                tmxc_bluetooth.devices[i].connected = 1;
                tmxc_bluetooth.connected_device = &tmxc_bluetooth.devices[i];
                break;
            }
        }
        
        tmxc_bluetooth.connected = 1;
        tmxc_uart_puts("[BT] Connection successful\r\n");
        return 0;
    }
    
    tmxc_uart_puts("[BT] Connection failed\r\n");
    return -1;
}

int tmxc_bluetooth_disconnect(void) {
    if (!tmxc_bluetooth.initialized) {
        return -1;
    }
    
    if (!tmxc_bluetooth.connected) {
        return -2;
    }
    
    uint8_t params[3];
    params[0] = 0x01;
    params[1] = 0x00;
    params[2] = 0x13;
    
    int result = tmxc_bluetooth_send_hci_command(TMXC_BT_OGF_LINK_CONTROL, TMXC_BT_OCF_DISCONNECT, params, 3);
    
    if (result == 0) {
        if (tmxc_bluetooth.connected_device != NULL) {
            tmxc_bluetooth.connected_device->connected = 0;
            tmxc_bluetooth.connected_device = NULL;
        }
        
        tmxc_bluetooth.connected = 0;
        tmxc_uart_puts("[BT] Disconnected\r\n");
        return 0;
    }
    
    return -1;
}

int tmxc_bluetooth_send_data(uint8_t* data, uint32_t size) {
    if (!tmxc_bluetooth.initialized || !tmxc_bluetooth.connected) {
        return -1;
    }
    
    if (data == NULL || size == 0) {
        return -2;
    }
    
    if (tmxc_bluetooth.connected_device == NULL) {
        return -3;
    }
    
    uint32_t max_acl_size = 1024;
    uint32_t handle = 0x0001;
    
    for (uint32_t offset = 0; offset < size; offset += max_acl_size) {
        uint32_t chunk_size = size - offset;
        if (chunk_size > max_acl_size) {
            chunk_size = max_acl_size;
        }
        
        uint8_t acl[chunk_size + 4];
        acl[0] = handle & 0xFF;
        acl[1] = ((handle >> 8) & 0x0F) | (0x00 << 4);
        acl[2] = chunk_size & 0xFF;
        acl[3] = (chunk_size >> 8) & 0xFF;
        
        for (uint32_t i = 0; i < chunk_size; i++) {
            acl[4 + i] = data[offset + i];
        }
        
        tmxc_bluetooth_hci_write(acl, chunk_size + 4);
    }
    
    tmxc_uart_puts("[BT] Data sent: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = size;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" bytes\r\n");
    
    return 0;
}

int tmxc_bluetooth_receive_data(uint8_t* buffer, uint32_t max_size) {
    if (!tmxc_bluetooth.initialized || !tmxc_bluetooth.connected) {
        return -1;
    }
    
    if (buffer == NULL || max_size == 0) {
        return -2;
    }
    
    uint8_t acl_header[4];
    uint32_t header_len = tmxc_bluetooth_hci_read(acl_header, 4);
    
    if (header_len < 4) {
        return 0;
    }
    
    uint16_t data_len = acl_header[2] | (acl_header[3] << 8);
    
    if (data_len > max_size) {
        data_len = max_size;
    }
    
    uint32_t received = tmxc_bluetooth_hci_read(buffer, data_len);
    
    if (received > 0) {
        tmxc_uart_puts("[BT] Data received: ");
        char buf[21];
        int p = 20;
        buf[p] = '\0';
        uint64_t t = received;
        while (t > 0 && p > 0) {
            p--;
            buf[p] = '0' + (t % 10);
            t /= 10;
        }
        tmxc_uart_puts(&buf[p]);
        tmxc_uart_puts(" bytes\r\n");
    }
    
    return received;
}

uint8_t tmxc_bluetooth_is_initialized(void) {
    return tmxc_bluetooth.initialized;
}

uint8_t tmxc_bluetooth_is_powered(void) {
    return tmxc_bluetooth.powered;
}

uint8_t tmxc_bluetooth_is_connected(void) {
    return tmxc_bluetooth.connected;
}

uint8_t tmxc_bluetooth_is_scanning(void) {
    return tmxc_bluetooth.scanning;
}

const char* tmxc_bluetooth_get_local_addr(void) {
    static char addr_str[18];
    char hex_chars[] = "0123456789ABCDEF";
    
    for (int i = 0; i < 6; i++) {
        addr_str[i * 3] = hex_chars[(tmxc_bluetooth.local_addr[i] >> 4) & 0x0F];
        addr_str[i * 3 + 1] = hex_chars[tmxc_bluetooth.local_addr[i] & 0x0F];
        if (i < 5) {
            addr_str[i * 3 + 2] = ':';
        }
    }
    addr_str[17] = '\0';
    
    return addr_str;
}
