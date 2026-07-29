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

#ifndef TMXC_ZERO_TRACE_RAMFS_H
#define TMXC_ZERO_TRACE_RAMFS_H

#include "tmxc_kernel.h"

#define ZEROTRACE_MAX_FILES 128
#define ZEROTRACE_MAX_FILE_SIZE (1024 * 1024)  // 1MB per file
#define ZEROTRACE_FS_SIZE (64 * 1024 * 1024)   // 64MB total RAM filesystem
#define ZEROTRACE_BLOCK_SIZE 4096
#define ZEROTRACE_MAX_BLOCKS (ZEROTRACE_FS_SIZE / ZEROTRACE_BLOCK_SIZE)
#define ZEROTRACE_FILENAME_MAX 64

typedef enum {
    ZEROTRACE_FILE_FREE = 0,
    ZEROTRACE_FILE_ACTIVE,
    ZEROTRACE_FILE_DELETED
} zerotrace_file_state_t;

typedef struct {
    char filename[ZEROTRACE_FILENAME_MAX];
    uint64_t size;
    uint64_t offset;
    uint32_t pid;
    uint64_t creation_time;
    uint64_t last_access;
    zerotrace_file_state_t state;
    uint8_t read_only;
    uint8_t encrypted;
} zerotrace_file_t;

typedef struct {
    uint8_t data[ZEROTRACE_FS_SIZE];
    uint8_t block_map[ZEROTRACE_MAX_BLOCKS];
    zerotrace_file_t files[ZEROTRACE_MAX_FILES];
    uint64_t total_used;
    uint64_t total_free;
    uint32_t active_files;
    uint8_t fs_initialized;
    uint64_t session_id;
} zerotrace_fs_t;

typedef struct {
    uint32_t pid;
    uint64_t bytes_read;
    uint64_t bytes_written;
    uint32_t file_count;
    uint64_t last_activity;
} zerotrace_process_stats_t;

void tmxc_zerotrace_ramfs_init(void);
int tmxc_zerotrace_create_file(const char* filename, uint32_t pid, uint64_t size);
int tmxc_zerotrace_delete_file(const char* filename, uint32_t pid);
int tmxc_zerotrace_write_file(const char* filename, const uint8_t* data, uint64_t offset, size_t length);
int tmxc_zerotrace_read_file(const char* filename, uint8_t* data, uint64_t offset, size_t length);
int tmxc_zerotrace_truncate_file(const char* filename, uint64_t new_size);
void tmxc_zerotrace_garbage_collect(void);
void tmxc_zerotrace_session_cleanup(uint32_t pid);
void tmxc_zerotrace_secure_wipe(void);
uint64_t tmxc_zerotrace_get_free_space(void);
uint64_t tmxc_zerotrace_get_used_space(void);
zerotrace_process_stats_t tmxc_zerotrace_get_process_stats(uint32_t pid);
void tmxc_zerotrace_process_isolation(uint32_t pid);

#endif
