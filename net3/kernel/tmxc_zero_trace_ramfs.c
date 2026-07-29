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
 */

#include "tmxc_zero_trace_ramfs.h"

static zerotrace_fs_t zerotrace_fs;

static int zerotrace_find_free_block(void) {
    for (uint32_t i = 0; i < ZEROTRACE_MAX_BLOCKS; i++) {
        if (zerotrace_fs.block_map[i] == 0) {
            return i;
        }
    }
    return -1;
}

static int zerotrace_find_file(const char* filename) {
    for (int i = 0; i < ZEROTRACE_MAX_FILES; i++) {
        if (zerotrace_fs.files[i].state == ZEROTRACE_FILE_ACTIVE) {
            int match = 1;
            for (int j = 0; j < ZEROTRACE_FILENAME_MAX; j++) {
                if (zerotrace_fs.files[i].filename[j] != filename[j]) {
                    match = 0;
                    break;
                }
                if (filename[j] == '\0') break;
            }
            if (match) {
                return i;
            }
        }
    }
    return -1;
}

static void zerotrace_secure_wipe_block(uint32_t block_index) {
    if (block_index >= ZEROTRACE_MAX_BLOCKS) return;
    
    uint8_t* block = &zerotrace_fs.data[block_index * ZEROTRACE_BLOCK_SIZE];
    
    for (int pass = 0; pass < 3; pass++) {
        for (size_t i = 0; i < ZEROTRACE_BLOCK_SIZE; i++) {
            block[i] = 0xFF;
        }
        
        for (size_t i = 0; i < ZEROTRACE_BLOCK_SIZE; i++) {
            block[i] = 0x00;
        }
        
        for (size_t i = 0; i < ZEROTRACE_BLOCK_SIZE; i++) {
            block[i] = (uint8_t)(i % 256);
        }
    }
    
    for (size_t i = 0; i < ZEROTRACE_BLOCK_SIZE; i++) {
        block[i] = 0;
    }
    
    __asm__ volatile("dsb sy");
    __asm__ volatile("isb");
}

void tmxc_zerotrace_ramfs_init(void) {
    tmxc_uart_puts("[ZEROTRACE] Initializing Zero-Trace RAM File System...\r\n");
    
    for (size_t i = 0; i < ZEROTRACE_FS_SIZE; i++) {
        zerotrace_fs.data[i] = 0;
    }
    
    for (uint32_t i = 0; i < ZEROTRACE_MAX_BLOCKS; i++) {
        zerotrace_fs.block_map[i] = 0;
    }
    
    for (int i = 0; i < ZEROTRACE_MAX_FILES; i++) {
        zerotrace_fs.files[i].state = ZEROTRACE_FILE_FREE;
        for (int j = 0; j < ZEROTRACE_FILENAME_MAX; j++) {
            zerotrace_fs.files[i].filename[j] = '\0';
        }
        zerotrace_fs.files[i].size = 0;
        zerotrace_fs.files[i].offset = 0;
        zerotrace_fs.files[i].pid = 0;
        zerotrace_fs.files[i].creation_time = 0;
        zerotrace_fs.files[i].last_access = 0;
        zerotrace_fs.files[i].read_only = 0;
        zerotrace_fs.files[i].encrypted = 0;
    }
    
    zerotrace_fs.total_used = 0;
    zerotrace_fs.total_free = ZEROTRACE_FS_SIZE;
    zerotrace_fs.active_files = 0;
    zerotrace_fs.fs_initialized = 1;
    zerotrace_fs.session_id = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[ZEROTRACE] RAM FS initialized: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = ZEROTRACE_FS_SIZE / (1024 * 1024);
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" MB\r\n");
    tmxc_uart_puts("[ZEROTRACE] Zero-trace mode enabled - no non-volatile writes\r\n");
}

int tmxc_zerotrace_create_file(const char* filename, uint32_t pid, uint64_t size) {
    if (!zerotrace_fs.fs_initialized) {
        tmxc_uart_puts("[ZEROTRACE] FS not initialized\r\n");
        return -1;
    }
    
    if (size > ZEROTRACE_MAX_FILE_SIZE) {
        tmxc_uart_puts("[ZEROTRACE] File size exceeds maximum\r\n");
        return -1;
    }
    
    int file_index = zerotrace_find_file(filename);
    if (file_index >= 0) {
        tmxc_uart_puts("[ZEROTRACE] File already exists\r\n");
        return -1;
    }
    
    uint32_t blocks_needed = (size + ZEROTRACE_BLOCK_SIZE - 1) / ZEROTRACE_BLOCK_SIZE;
    
    if (blocks_needed > ZEROTRACE_MAX_BLOCKS) {
        tmxc_uart_puts("[ZEROTRACE] Not enough blocks available\r\n");
        return -1;
    }
    
    int free_slot = -1;
    for (int i = 0; i < ZEROTRACE_MAX_FILES; i++) {
        if (zerotrace_fs.files[i].state == ZEROTRACE_FILE_FREE) {
            free_slot = i;
            break;
        }
    }
    
    if (free_slot < 0) {
        tmxc_uart_puts("[ZEROTRACE] No free file slots\r\n");
        return -1;
    }
    
    uint64_t file_offset = 0;
    for (uint32_t i = 0; i < blocks_needed; i++) {
        int block_index = zerotrace_find_free_block();
        if (block_index < 0) {
            tmxc_uart_puts("[ZEROTRACE] Block allocation failed\r\n");
            return -1;
        }
        zerotrace_fs.block_map[block_index] = 1;
        if (i == 0) {
            file_offset = block_index * ZEROTRACE_BLOCK_SIZE;
        }
    }
    
    for (int i = 0; i < ZEROTRACE_FILENAME_MAX && filename[i] != '\0'; i++) {
        zerotrace_fs.files[free_slot].filename[i] = filename[i];
    }
    zerotrace_fs.files[free_slot].filename[ZEROTRACE_FILENAME_MAX - 1] = '\0';
    
    zerotrace_fs.files[free_slot].size = size;
    zerotrace_fs.files[free_slot].offset = file_offset;
    zerotrace_fs.files[free_slot].pid = pid;
    zerotrace_fs.files[free_slot].creation_time = tmxc_get_cycle_count();
    zerotrace_fs.files[free_slot].last_access = tmxc_get_cycle_count();
    zerotrace_fs.files[free_slot].state = ZEROTRACE_FILE_ACTIVE;
    zerotrace_fs.files[free_slot].read_only = 0;
    zerotrace_fs.files[free_slot].encrypted = 1;
    
    zerotrace_fs.total_used += size;
    zerotrace_fs.total_free -= size;
    zerotrace_fs.active_files++;
    
    tmxc_uart_puts("[ZEROTRACE] File created: ");
    tmxc_uart_puts(filename);
    tmxc_uart_puts("\r\n");
    
    return free_slot;
}

int tmxc_zerotrace_delete_file(const char* filename, uint32_t pid) {
    if (!zerotrace_fs.fs_initialized) {
        return -1;
    }
    
    int file_index = zerotrace_find_file(filename);
    if (file_index < 0) {
        tmxc_uart_puts("[ZEROTRACE] File not found\r\n");
        return -1;
    }
    
    if (zerotrace_fs.files[file_index].pid != pid) {
        tmxc_uart_puts("[ZEROTRACE] Permission denied\r\n");
        return -1;
    }
    
    uint32_t blocks_used = (zerotrace_fs.files[file_index].size + ZEROTRACE_BLOCK_SIZE - 1) / ZEROTRACE_BLOCK_SIZE;
    uint32_t start_block = zerotrace_fs.files[file_index].offset / ZEROTRACE_BLOCK_SIZE;
    
    for (uint32_t i = 0; i < blocks_used; i++) {
        zerotrace_secure_wipe_block(start_block + i);
        zerotrace_fs.block_map[start_block + i] = 0;
    }
    
    zerotrace_fs.total_used -= zerotrace_fs.files[file_index].size;
    zerotrace_fs.total_free += zerotrace_fs.files[file_index].size;
    zerotrace_fs.active_files--;
    
    zerotrace_fs.files[file_index].state = ZEROTRACE_FILE_DELETED;
    for (int i = 0; i < ZEROTRACE_FILENAME_MAX; i++) {
        zerotrace_fs.files[file_index].filename[i] = '\0';
    }
    zerotrace_fs.files[file_index].size = 0;
    zerotrace_fs.files[file_index].offset = 0;
    zerotrace_fs.files[file_index].pid = 0;
    
    tmxc_uart_puts("[ZEROTRACE] File deleted and securely wiped: ");
    tmxc_uart_puts(filename);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

int tmxc_zerotrace_write_file(const char* filename, const uint8_t* data, uint64_t offset, size_t length) {
    if (!zerotrace_fs.fs_initialized) {
        return -1;
    }
    
    int file_index = zerotrace_find_file(filename);
    if (file_index < 0) {
        tmxc_uart_puts("[ZEROTRACE] File not found\r\n");
        return -1;
    }
    
    if (zerotrace_fs.files[file_index].read_only) {
        tmxc_uart_puts("[ZEROTRACE] File is read-only\r\n");
        return -1;
    }
    
    if (offset + length > zerotrace_fs.files[file_index].size) {
        tmxc_uart_puts("[ZEROTRACE] Write exceeds file size\r\n");
        return -1;
    }
    
    uint64_t file_offset = zerotrace_fs.files[file_index].offset + offset;
    
    for (size_t i = 0; i < length; i++) {
        zerotrace_fs.data[file_offset + i] = data[i];
    }
    
    zerotrace_fs.files[file_index].last_access = tmxc_get_cycle_count();
    
    __asm__ volatile("dsb sy");
    
    return 0;
}

int tmxc_zerotrace_read_file(const char* filename, uint8_t* data, uint64_t offset, size_t length) {
    if (!zerotrace_fs.fs_initialized) {
        return -1;
    }
    
    int file_index = zerotrace_find_file(filename);
    if (file_index < 0) {
        tmxc_uart_puts("[ZEROTRACE] File not found\r\n");
        return -1;
    }
    
    if (offset + length > zerotrace_fs.files[file_index].size) {
        tmxc_uart_puts("[ZEROTRACE] Read exceeds file size\r\n");
        return -1;
    }
    
    uint64_t file_offset = zerotrace_fs.files[file_index].offset + offset;
    
    for (size_t i = 0; i < length; i++) {
        data[i] = zerotrace_fs.data[file_offset + i];
    }
    
    zerotrace_fs.files[file_index].last_access = tmxc_get_cycle_count();
    
    return 0;
}

int tmxc_zerotrace_truncate_file(const char* filename, uint64_t new_size) {
    if (!zerotrace_fs.fs_initialized) {
        return -1;
    }
    
    int file_index = zerotrace_find_file(filename);
    if (file_index < 0) {
        return -1;
    }
    
    if (new_size > ZEROTRACE_MAX_FILE_SIZE) {
        return -1;
    }
    
    uint64_t old_size = zerotrace_fs.files[file_index].size;
    uint32_t old_blocks = (old_size + ZEROTRACE_BLOCK_SIZE - 1) / ZEROTRACE_BLOCK_SIZE;
    uint32_t new_blocks = (new_size + ZEROTRACE_BLOCK_SIZE - 1) / ZEROTRACE_BLOCK_SIZE;
    
    if (new_blocks < old_blocks) {
        uint32_t start_block = zerotrace_fs.files[file_index].offset / ZEROTRACE_BLOCK_SIZE;
        for (uint32_t i = new_blocks; i < old_blocks; i++) {
            zerotrace_secure_wipe_block(start_block + i);
            zerotrace_fs.block_map[start_block + i] = 0;
        }
    }
    
    zerotrace_fs.total_used = zerotrace_fs.total_used - old_size + new_size;
    zerotrace_fs.total_free = zerotrace_fs.total_free + old_size - new_size;
    zerotrace_fs.files[file_index].size = new_size;
    zerotrace_fs.files[file_index].last_access = tmxc_get_cycle_count();
    
    return 0;
}

void tmxc_zerotrace_garbage_collect(void) {
    if (!zerotrace_fs.fs_initialized) {
        return;
    }
    
    tmxc_uart_puts("[ZEROTRACE] Running garbage collection...\r\n");
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t timeout = tmxc_get_frequency() * 300;  // 5 minutes
    
    for (int i = 0; i < ZEROTRACE_MAX_FILES; i++) {
        if (zerotrace_fs.files[i].state == ZEROTRACE_FILE_ACTIVE) {
            if (current_time - zerotrace_fs.files[i].last_access > timeout) {
                tmxc_uart_puts("[ZEROTRACE] Collecting stale file: ");
                tmxc_uart_puts(zerotrace_fs.files[i].filename);
                tmxc_uart_puts("\r\n");
                tmxc_zerotrace_delete_file(zerotrace_fs.files[i].filename, zerotrace_fs.files[i].pid);
            }
        }
    }
    
    tmxc_uart_puts("[ZEROTRACE] Garbage collection complete\r\n");
}

void tmxc_zerotrace_session_cleanup(uint32_t pid) {
    if (!zerotrace_fs.fs_initialized) {
        return;
    }
    
    tmxc_uart_puts("[ZEROTRACE] Cleaning up session for PID: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = pid;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
    
    for (int i = 0; i < ZEROTRACE_MAX_FILES; i++) {
        if (zerotrace_fs.files[i].state == ZEROTRACE_FILE_ACTIVE && zerotrace_fs.files[i].pid == pid) {
            tmxc_zerotrace_delete_file(zerotrace_fs.files[i].filename, pid);
        }
    }
}

void tmxc_zerotrace_secure_wipe(void) {
    tmxc_uart_puts("[ZEROTRACE] Performing full secure wipe...\r\n");
    
    for (uint32_t i = 0; i < ZEROTRACE_MAX_BLOCKS; i++) {
        zerotrace_secure_wipe_block(i);
    }
    
    for (int i = 0; i < ZEROTRACE_MAX_FILES; i++) {
        zerotrace_fs.files[i].state = ZEROTRACE_FILE_FREE;
        for (int j = 0; j < ZEROTRACE_FILENAME_MAX; j++) {
            zerotrace_fs.files[i].filename[j] = '\0';
        }
        zerotrace_fs.files[i].size = 0;
        zerotrace_fs.files[i].offset = 0;
        zerotrace_fs.files[i].pid = 0;
    }
    
    zerotrace_fs.total_used = 0;
    zerotrace_fs.total_free = ZEROTRACE_FS_SIZE;
    zerotrace_fs.active_files = 0;
    
    tmxc_uart_puts("[ZEROTRACE] Secure wipe complete\r\n");
}

uint64_t tmxc_zerotrace_get_free_space(void) {
    return zerotrace_fs.total_free;
}

uint64_t tmxc_zerotrace_get_used_space(void) {
    return zerotrace_fs.total_used;
}

zerotrace_process_stats_t tmxc_zerotrace_get_process_stats(uint32_t pid) {
    zerotrace_process_stats_t stats;
    stats.pid = pid;
    stats.bytes_read = 0;
    stats.bytes_written = 0;
    stats.file_count = 0;
    stats.last_activity = 0;
    
    for (int i = 0; i < ZEROTRACE_MAX_FILES; i++) {
        if (zerotrace_fs.files[i].state == ZEROTRACE_FILE_ACTIVE && zerotrace_fs.files[i].pid == pid) {
            stats.file_count++;
            stats.bytes_written += zerotrace_fs.files[i].size;
            if (zerotrace_fs.files[i].last_access > stats.last_activity) {
                stats.last_activity = zerotrace_fs.files[i].last_access;
            }
        }
    }
    
    return stats;
}

void tmxc_zerotrace_process_isolation(uint32_t pid) {
    tmxc_uart_puts("[ZEROTRACE] Enforcing process isolation for PID: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = pid;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
    
    for (int i = 0; i < ZEROTRACE_MAX_FILES; i++) {
        if (zerotrace_fs.files[i].state == ZEROTRACE_FILE_ACTIVE && zerotrace_fs.files[i].pid != pid) {
            zerotrace_fs.files[i].read_only = 1;
        }
    }
}
