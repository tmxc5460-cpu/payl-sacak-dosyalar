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

#ifndef TMXC_BEYAZ_KUS_AI_H
#define TMXC_BEYAZ_KUS_AI_H

#include "tmxc_kernel.h"

#define BEYAZ_KUS_MAX_SYSCALLS 1024
#define BEYAZ_KUS_HISTORY_SIZE 256
#define BEYAZ_KUS_THRESHOLD_HIGH 100
#define BEYAZ_KUS_THRESHOLD_MEDIUM 50
#define BEYAZ_KUS_WINDOW_MS 1000

typedef enum {
    BEYAZ_KUS_SYSCALL_READ = 0,
    BEYAZ_KUS_SYSCALL_WRITE,
    BEYAZ_KUS_SYSCALL_OPEN,
    BEYAZ_KUS_SYSCALL_CLOSE,
    BEYAZ_KUS_SYSCALL_MMAP,
    BEYAZ_KUS_SYSCALL_MPROTECT,
    BEYAZ_KUS_SYSCALL_EXECVE,
    BEYAZ_KUS_SYSCALL_FORK,
    BEYAZ_KUS_SYSCALL_CLONE,
    BEYAZ_KUS_SYSCALL_KILL,
    BEYAZ_KUS_SYSCALL_PTRACE,
    BEYAZ_KUS_SYSCALL_SOCKET,
    BEYAZ_KUS_SYSCALL_CONNECT,
    BEYAZ_KUS_SYSCALL_BIND,
    BEYAZ_KUS_SYSCALL_SENDTO,
    BEYAZ_KUS_SYSCALL_RECVFROM,
    BEYAZ_KUS_SYSCALL_IOCTL,
    BEYAZ_KUS_SYSCALL_TOTAL
} beyaz_kus_syscall_t;

typedef struct {
    uint32_t pid;
    beyaz_kus_syscall_t syscall_type;
    uint64_t timestamp;
    uint64_t address;
    uint64_t size;
    uint8_t anomaly_score;
} beyaz_kus_syscall_record_t;

typedef struct {
    uint32_t pid;
    uint64_t syscall_count[BEYAZ_KUS_SYSCALL_TOTAL];
    uint64_t total_syscalls;
    uint64_t first_seen;
    uint64_t last_seen;
    uint8_t suspicious;
    uint8_t blocked;
    uint32_t anomaly_count;
} beyaz_kus_process_profile_t;

typedef struct {
    beyaz_kus_syscall_record_t history[BEYAZ_KUS_HISTORY_SIZE];
    uint32_t history_index;
    uint32_t history_count;
    beyaz_kus_process_profile_t profiles[TMXC_MAX_PROCESSES];
    uint64_t total_syscalls_monitored;
    uint64_t anomalies_detected;
    uint64_t processes_killed;
    uint8_t monitoring_active;
    uint64_t last_analysis;
} beyaz_kus_ai_t;

typedef enum {
    BEYAZ_KUS_THREAT_NONE = 0,
    BEYAZ_KUS_THREAT_LOW,
    BEYAZ_KUS_THREAT_MEDIUM,
    BEYAZ_KUS_THREAT_HIGH,
    BEYAZ_KUS_THREAT_CRITICAL
} beyaz_kus_threat_level_t;

void tmxc_beyaz_kus_ai_init(void);
void tmxc_beyaz_kus_monitor_syscall(uint32_t pid, beyaz_kus_syscall_t syscall_type, uint64_t address, uint64_t size);
beyaz_kus_threat_level_t tmxc_beyaz_kus_analyze_process(uint32_t pid);
void tmxc_beyaz_kus_detect_buffer_overflow(uint32_t pid, uint64_t address, uint64_t size);
void tmxc_beyaz_kus_detect_privilege_escalation(uint32_t pid, beyaz_kus_syscall_t syscall_type);
void tmxc_beyaz_kus_kill_process(uint32_t pid);
void tmxc_beyaz_kus_block_process(uint32_t pid);
void tmxc_beyaz_kus_unblock_process(uint32_t pid);
void tmxc_beyaz_kus_periodic_analysis(void);
beyaz_kus_process_profile_t tmxc_beyaz_kus_get_process_profile(uint32_t pid);
uint64_t tmxc_beyaz_kus_get_total_anomalies(void);
uint64_t tmxc_beyaz_kus_get_processes_killed(void);

#endif
