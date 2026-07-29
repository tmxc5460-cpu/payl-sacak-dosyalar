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

#include "tmxc_beyaz_kus_ai.h"

static beyaz_kus_ai_t beyaz_kus_ai;

static uint8_t beyaz_kus_calculate_anomaly_score(uint32_t pid, beyaz_kus_syscall_t syscall_type, uint64_t address, uint64_t size) {
    uint8_t score = 0;
    
    beyaz_kus_process_profile_t* profile = &beyaz_kus_ai.profiles[pid];
    
    uint64_t syscall_rate = profile->total_syscalls;
    uint64_t time_window = tmxc_get_cycle_count() - profile->first_seen;
    if (time_window > 0) {
        syscall_rate = (syscall_rate * 1000) / time_window;
    }
    
    if (syscall_rate > BEYAZ_KUS_THRESHOLD_HIGH) {
        score += 50;
    } else if (syscall_rate > BEYAZ_KUS_THRESHOLD_MEDIUM) {
        score += 25;
    }
    
    if (syscall_type == BEYAZ_KUS_SYSCALL_MPROTECT) {
        score += 30;
    }
    
    if (syscall_type == BEYAZ_KUS_SYSCALL_PTRACE) {
        score += 40;
    }
    
    if (syscall_type == BEYAZ_KUS_SYSCALL_EXECVE && profile->syscall_count[BEYAZ_KUS_SYSCALL_EXECVE] > 5) {
        score += 35;
    }
    
    if (size > 1024 * 1024) {
        score += 20;
    }
    
    if (address < 0x1000 || address > 0xFFFFFFFFFFFF0000ULL) {
        score += 45;
    }
    
    if (profile->anomaly_count > 10) {
        score += 30;
    }
    
    return score;
}

void tmxc_beyaz_kus_ai_init(void) {
    tmxc_uart_puts("[BEYAZ-KUS] Initializing Beyaz Kuş AI Syscall Anomaly Detector...\r\n");
    
    for (uint32_t i = 0; i < BEYAZ_KUS_HISTORY_SIZE; i++) {
        beyaz_kus_ai.history[i].pid = 0;
        beyaz_kus_ai.history[i].syscall_type = BEYAZ_KUS_SYSCALL_READ;
        beyaz_kus_ai.history[i].timestamp = 0;
        beyaz_kus_ai.history[i].address = 0;
        beyaz_kus_ai.history[i].size = 0;
        beyaz_kus_ai.history[i].anomaly_score = 0;
    }
    
    beyaz_kus_ai.history_index = 0;
    beyaz_kus_ai.history_count = 0;
    
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        beyaz_kus_ai.profiles[i].pid = i;
        for (int j = 0; j < BEYAZ_KUS_SYSCALL_TOTAL; j++) {
            beyaz_kus_ai.profiles[i].syscall_count[j] = 0;
        }
        beyaz_kus_ai.profiles[i].total_syscalls = 0;
        beyaz_kus_ai.profiles[i].first_seen = 0;
        beyaz_kus_ai.profiles[i].last_seen = 0;
        beyaz_kus_ai.profiles[i].suspicious = 0;
        beyaz_kus_ai.profiles[i].blocked = 0;
        beyaz_kus_ai.profiles[i].anomaly_count = 0;
    }
    
    beyaz_kus_ai.total_syscalls_monitored = 0;
    beyaz_kus_ai.anomalies_detected = 0;
    beyaz_kus_ai.processes_killed = 0;
    beyaz_kus_ai.monitoring_active = 1;
    beyaz_kus_ai.last_analysis = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[BEYAZ-KUS] AI anomaly detector initialized\r\n");
    tmxc_uart_puts("[BEYAZ-KUS] Monitoring system calls for buffer overflow and privilege escalation\r\n");
}

void tmxc_beyaz_kus_monitor_syscall(uint32_t pid, beyaz_kus_syscall_t syscall_type, uint64_t address, uint64_t size) {
    if (!beyaz_kus_ai.monitoring_active) {
        return;
    }
    
    if (pid >= TMXC_MAX_PROCESSES) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    
    beyaz_kus_ai.history[beyaz_kus_ai.history_index].pid = pid;
    beyaz_kus_ai.history[beyaz_kus_ai.history_index].syscall_type = syscall_type;
    beyaz_kus_ai.history[beyaz_kus_ai.history_index].timestamp = current_time;
    beyaz_kus_ai.history[beyaz_kus_ai.history_index].address = address;
    beyaz_kus_ai.history[beyaz_kus_ai.history_index].size = size;
    
    uint8_t anomaly_score = beyaz_kus_calculate_anomaly_score(pid, syscall_type, address, size);
    beyaz_kus_ai.history[beyaz_kus_ai.history_index].anomaly_score = anomaly_score;
    
    beyaz_kus_ai.history_index = (beyaz_kus_ai.history_index + 1) % BEYAZ_KUS_HISTORY_SIZE;
    if (beyaz_kus_ai.history_count < BEYAZ_KUS_HISTORY_SIZE) {
        beyaz_kus_ai.history_count++;
    }
    
    beyaz_kus_ai.profiles[pid].pid = pid;
    beyaz_kus_ai.profiles[pid].syscall_count[syscall_type]++;
    beyaz_kus_ai.profiles[pid].total_syscalls++;
    
    if (beyaz_kus_ai.profiles[pid].first_seen == 0) {
        beyaz_kus_ai.profiles[pid].first_seen = current_time;
    }
    beyaz_kus_ai.profiles[pid].last_seen = current_time;
    
    beyaz_kus_ai.total_syscalls_monitored++;
    
    if (anomaly_score > 70) {
        beyaz_kus_ai.anomalies_detected++;
        beyaz_kus_ai.profiles[pid].anomaly_count++;
        
        tmxc_uart_puts("[BEYAZ-KUS] High anomaly score detected: ");
        char buffer[21];
        int pos = 20;
        buffer[pos] = '\0';
        uint64_t temp = anomaly_score;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts(" for PID: ");
        pos = 20;
        buffer[pos] = '\0';
        temp = pid;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts("\r\n");
        
        if (anomaly_score > 90) {
            tmxc_beyaz_kus_kill_process(pid);
        } else {
            beyaz_kus_ai.profiles[pid].suspicious = 1;
        }
    }
    
    tmxc_beyaz_kus_detect_buffer_overflow(pid, address, size);
    tmxc_beyaz_kus_detect_privilege_escalation(pid, syscall_type);
}

beyaz_kus_threat_level_t tmxc_beyaz_kus_analyze_process(uint32_t pid) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return BEYAZ_KUS_THREAT_NONE;
    }
    
    beyaz_kus_process_profile_t* profile = &beyaz_kus_ai.profiles[pid];
    
    if (profile->total_syscalls == 0) {
        return BEYAZ_KUS_THREAT_NONE;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed = current_time - profile->first_seen;
    if (elapsed == 0) elapsed = 1;
    
    uint64_t syscall_rate = (profile->total_syscalls * 1000) / elapsed;
    
    uint8_t threat_score = 0;
    
    if (syscall_rate > BEYAZ_KUS_THRESHOLD_HIGH) {
        threat_score += 30;
    } else if (syscall_rate > BEYAZ_KUS_THRESHOLD_MEDIUM) {
        threat_score += 15;
    }
    
    if (profile->anomaly_count > 20) {
        threat_score += 40;
    } else if (profile->anomaly_count > 10) {
        threat_score += 20;
    } else if (profile->anomaly_count > 5) {
        threat_score += 10;
    }
    
    if (profile->syscall_count[BEYAZ_KUS_SYSCALL_PTRACE] > 0) {
        threat_score += 25;
    }
    
    if (profile->syscall_count[BEYAZ_KUS_SYSCALL_MPROTECT] > 10) {
        threat_score += 20;
    }
    
    if (profile->suspicious) {
        threat_score += 15;
    }
    
    if (threat_score >= 80) {
        return BEYAZ_KUS_THREAT_CRITICAL;
    } else if (threat_score >= 60) {
        return BEYAZ_KUS_THREAT_HIGH;
    } else if (threat_score >= 40) {
        return BEYAZ_KUS_THREAT_MEDIUM;
    } else if (threat_score >= 20) {
        return BEYAZ_KUS_THREAT_LOW;
    }
    
    return BEYAZ_KUS_THREAT_NONE;
}

void tmxc_beyaz_kus_detect_buffer_overflow(uint32_t pid, uint64_t address, uint64_t size) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return;
    }
    
    uint8_t overflow_detected = 0;
    
    if (size > 10 * 1024 * 1024) {
        overflow_detected = 1;
    }
    
    if (address < 0x1000) {
        overflow_detected = 1;
    }
    
    if ((address + size) < address) {
        overflow_detected = 1;
    }
    
    if (overflow_detected) {
        beyaz_kus_ai.anomalies_detected++;
        beyaz_kus_ai.profiles[pid].anomaly_count++;
        
        tmxc_uart_puts("[BEYAZ-KUS] Buffer overflow attempt detected from PID: ");
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
        
        tmxc_beyaz_kus_kill_process(pid);
    }
}

void tmxc_beyaz_kus_detect_privilege_escalation(uint32_t pid, beyaz_kus_syscall_t syscall_type) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return;
    }
    
    uint8_t escalation_detected = 0;
    
    if (syscall_type == BEYAZ_KUS_SYSCALL_PTRACE) {
        escalation_detected = 1;
    }
    
    if (syscall_type == BEYAZ_KUS_SYSCALL_MPROTECT) {
        if (beyaz_kus_ai.profiles[pid].syscall_count[BEYAZ_KUS_SYSCALL_MPROTECT] > 5) {
            escalation_detected = 1;
        }
    }
    
    if (syscall_type == BEYAZ_KUS_SYSCALL_KILL && pid < 100) {
        escalation_detected = 1;
    }
    
    if (escalation_detected) {
        beyaz_kus_ai.anomalies_detected++;
        beyaz_kus_ai.profiles[pid].anomaly_count++;
        
        tmxc_uart_puts("[BEYAZ-KUS] Privilege escalation attempt detected from PID: ");
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
        
        beyaz_kus_threat_level_t threat = tmxc_beyaz_kus_analyze_process(pid);
        if (threat >= BEYAZ_KUS_THREAT_HIGH) {
            tmxc_beyaz_kus_kill_process(pid);
        } else {
            beyaz_kus_ai.profiles[pid].suspicious = 1;
        }
    }
}

void tmxc_beyaz_kus_kill_process(uint32_t pid) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return;
    }
    
    tmxc_uart_puts("[BEYAZ-KUS] KILLING MALICIOUS PROCESS PID: ");
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
    
    beyaz_kus_ai.processes_killed++;
    beyaz_kus_ai.profiles[pid].blocked = 1;
    
    extern void tmxc_process_kill(uint32_t pid);
    tmxc_process_kill(pid);
}

void tmxc_beyaz_kus_block_process(uint32_t pid) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return;
    }
    
    beyaz_kus_ai.profiles[pid].blocked = 1;
    
    tmxc_uart_puts("[BEYAZ-KUS] Process blocked: ");
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
}

void tmxc_beyaz_kus_unblock_process(uint32_t pid) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return;
    }
    
    beyaz_kus_ai.profiles[pid].blocked = 0;
    beyaz_kus_ai.profiles[pid].suspicious = 0;
    beyaz_kus_ai.profiles[pid].anomaly_count = 0;
}

void tmxc_beyaz_kus_periodic_analysis(void) {
    if (!beyaz_kus_ai.monitoring_active) {
        return;
    }
    
    uint64_t current_time = tmxc_get_cycle_count();
    uint64_t elapsed_ms = (current_time - beyaz_kus_ai.last_analysis) / tmxc_get_frequency();
    
    if (elapsed_ms < BEYAZ_KUS_WINDOW_MS) {
        return;
    }
    
    tmxc_uart_puts("[BEYAZ-KUS] Running periodic analysis...\r\n");
    
    uint32_t critical_threats = 0;
    uint32_t high_threats = 0;
    
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (beyaz_kus_ai.profiles[i].total_syscalls > 0) {
            beyaz_kus_threat_level_t threat = tmxc_beyaz_kus_analyze_process(i);
            
            if (threat == BEYAZ_KUS_THREAT_CRITICAL) {
                critical_threats++;
                tmxc_beyaz_kus_kill_process(i);
            } else if (threat == BEYAZ_KUS_THREAT_HIGH) {
                high_threats++;
                beyaz_kus_ai.profiles[i].suspicious = 1;
            }
        }
    }
    
    if (critical_threats > 0 || high_threats > 0) {
        tmxc_uart_puts("[BEYAZ-KUS] Threats detected - Critical: ");
        char buffer[21];
        int pos = 20;
        buffer[pos] = '\0';
        uint64_t temp = critical_threats;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts(", High: ");
        pos = 20;
        buffer[pos] = '\0';
        temp = high_threats;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts("\r\n");
    }
    
    beyaz_kus_ai.last_analysis = current_time;
}

beyaz_kus_process_profile_t tmxc_beyaz_kus_get_process_profile(uint32_t pid) {
    if (pid >= TMXC_MAX_PROCESSES) {
        beyaz_kus_process_profile_t empty = {0};
        return empty;
    }
    return beyaz_kus_ai.profiles[pid];
}

uint64_t tmxc_beyaz_kus_get_total_anomalies(void) {
    return beyaz_kus_ai.anomalies_detected;
}

uint64_t tmxc_beyaz_kus_get_processes_killed(void) {
    return beyaz_kus_ai.processes_killed;
}
