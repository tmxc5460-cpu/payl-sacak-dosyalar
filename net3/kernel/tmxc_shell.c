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

typedef struct {
    char command[64];
    char args[256];
    uint8_t active;
} tmxc_shell_state_t;

static tmxc_shell_state_t tmxc_shell;

extern void tmxc_uart_puts(const char* str);
extern char tmxc_uart_getc(void);
extern uint64_t tmxc_get_uptime(void);
extern tmxc_memory_info_t tmxc_get_memory_info(void);
extern tmxc_battery_info_t tmxc_get_battery_info(void);
extern tmxc_cpu_info_t tmxc_get_cpu_info(void);

static void tmxc_shell_print_prompt(void) {
    tmxc_uart_puts("\r\nTMXC-OS# ");
}

static void tmxc_shell_parse_command(const char* input, char* command, char* args) {
    uint32_t i = 0;
    uint32_t cmd_pos = 0;
    uint32_t arg_pos = 0;
    uint8_t in_args = 0;
    
    while (input[i] != '\0' && i < 320) {
        if (input[i] == ' ' || input[i] == '\t') {
            if (!in_args) {
                in_args = 1;
            }
        } else if (input[i] == '\r' || input[i] == '\n') {
            break;
        } else {
            if (!in_args) {
                if (cmd_pos < 63) {
                    command[cmd_pos++] = input[i];
                }
            } else {
                if (arg_pos < 255) {
                    args[arg_pos++] = input[i];
                }
            }
        }
        i++;
    }
    
    command[cmd_pos] = '\0';
    args[arg_pos] = '\0';
}

static void tmxc_shell_cmd_help(void) {
    tmxc_uart_puts("\r\nAvailable commands:\r\n");
    tmxc_uart_puts("  ps          - List all processes\r\n");
    tmxc_uart_puts("  free        - Show memory usage\r\n");
    tmxc_uart_puts("  uptime      - Show system uptime\r\n");
    tmxc_uart_puts("  kill <pid>  - Terminate a process\r\n");
    tmxc_uart_puts("  battery     - Show battery information\r\n");
    tmxc_uart_puts("  cpu         - Show CPU information\r\n");
    tmxc_uart_puts("  memguard    - Show memory guard status\r\n");
    tmxc_uart_puts("  deadlock    - Show deadlock detection status\r\n");
    tmxc_uart_puts("  power       - Show power management status\r\n");
    tmxc_uart_puts("  help        - Show this help message\r\n");
    tmxc_uart_puts("  clear       - Clear screen\r\n");
    tmxc_uart_puts("  reboot      - Reboot the system\r\n");
    tmxc_uart_puts("  shutdown    - Shutdown the system\r\n");
}

static void tmxc_shell_cmd_ps(void) {
    tmxc_uart_puts("\r\nProcess List:\r\n");
    tmxc_uart_puts("PID   PPID  State     Priority\r\n");
    tmxc_uart_puts("---------------------------\r\n");
    
    extern tmxc_process_t tmxc_scheduler_processes[];
    extern uint32_t tmxc_scheduler_process_count;
    
    for (uint32_t i = 0; i < 256; i++) {
        if (tmxc_scheduler_processes[i].pid != 0) {
            char buffer[21];
            int pos = 20;
            buffer[pos] = '\0';
            uint64_t temp = tmxc_scheduler_processes[i].pid;
            while (temp > 0 && pos > 0) {
                pos--;
                buffer[pos] = '0' + (temp % 10);
                temp /= 10;
            }
            tmxc_uart_puts(&buffer[pos]);
            tmxc_uart_puts("     ");
            
            pos = 20;
            buffer[pos] = '\0';
            temp = tmxc_scheduler_processes[i].ppid;
            while (temp > 0 && pos > 0) {
                pos--;
                buffer[pos] = '0' + (temp % 10);
                temp /= 10;
            }
            tmxc_uart_puts(&buffer[pos]);
            tmxc_uart_puts("     ");
            
            uint8_t state = tmxc_scheduler_processes[i].state;
            if (state == 0) {
                tmxc_uart_puts("READY     ");
            } else if (state == 1) {
                tmxc_uart_puts("RUNNING   ");
            } else if (state == 2) {
                tmxc_uart_puts("BLOCKED   ");
            } else if (state == 3) {
                tmxc_uart_puts("TERMINATED");
            } else if (state == 4) {
                tmxc_uart_puts("ZOMBIE    ");
            } else {
                tmxc_uart_puts("UNKNOWN   ");
            }
            
            pos = 20;
            buffer[pos] = '\0';
            temp = tmxc_scheduler_processes[i].priority;
            while (temp > 0 && pos > 0) {
                pos--;
                buffer[pos] = '0' + (temp % 10);
                temp /= 10;
            }
            tmxc_uart_puts(&buffer[pos]);
            tmxc_uart_puts("\r\n");
        }
    }
}

static void tmxc_shell_cmd_free(void) {
    tmxc_memory_info_t mem_info = tmxc_get_memory_info();
    
    tmxc_uart_puts("\r\nMemory Usage:\r\n");
    tmxc_uart_puts("Total:     ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = mem_info.total / (1024 * 1024);
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" MB\r\n");
    
    tmxc_uart_puts("Available: ");
    pos = 20;
    buffer[pos] = '\0';
    temp = mem_info.available / (1024 * 1024);
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" MB\r\n");
    
    tmxc_uart_puts("Cached:    ");
    pos = 20;
    buffer[pos] = '\0';
    temp = mem_info.cached / (1024 * 1024);
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" MB\r\n");
    
    tmxc_uart_puts("Compressed: ");
    pos = 20;
    buffer[pos] = '\0';
    temp = mem_info.compressed / (1024 * 1024);
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" MB\r\n");
}

static void tmxc_shell_cmd_uptime(void) {
    uint64_t uptime = tmxc_get_uptime();
    
    tmxc_uart_puts("\r\nSystem Uptime:\r\n");
    
    uint64_t seconds = uptime;
    uint64_t minutes = seconds / 60;
    uint64_t hours = minutes / 60;
    uint64_t days = hours / 24;
    
    seconds = seconds % 60;
    minutes = minutes % 60;
    hours = hours % 24;
    
    char buffer[21];
    
    tmxc_uart_puts("Days: ");
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = days;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("Hours: ");
    pos = 20;
    buffer[pos] = '\0';
    temp = hours;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("Minutes: ");
    pos = 20;
    buffer[pos] = '\0';
    temp = minutes;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("Seconds: ");
    pos = 20;
    buffer[pos] = '\0';
    temp = seconds;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

static void tmxc_shell_cmd_kill(const char* args) {
    uint32_t pid = 0;
    uint32_t i = 0;
    
    while (args[i] != '\0' && i < 256) {
        if (args[i] >= '0' && args[i] <= '9') {
            pid = pid * 10 + (args[i] - '0');
        }
        i++;
    }
    
    if (pid == 0) {
        tmxc_uart_puts("\r\nError: Invalid PID\r\n");
        return;
    }
    
    extern void tmxc_process_kill(uint32_t pid);
    tmxc_process_kill(pid);
    
    tmxc_uart_puts("\r\nProcess ");
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
    tmxc_uart_puts(" terminated\r\n");
}

static void tmxc_shell_cmd_battery(void) {
    tmxc_battery_info_t battery = tmxc_get_battery_info();
    
    tmxc_uart_puts("\r\nBattery Information:\r\n");
    tmxc_uart_puts("Voltage:    ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = battery.voltage;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" mV\r\n");
    
    tmxc_uart_puts("Current:    ");
    pos = 20;
    buffer[pos] = '\0';
    temp = battery.current;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" mA\r\n");
    
    tmxc_uart_puts("Capacity:   ");
    pos = 20;
    buffer[pos] = '\0';
    temp = battery.capacity;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" %\r\n");
    
    tmxc_uart_puts("Temperature: ");
    pos = 20;
    buffer[pos] = '\0';
    temp = battery.temperature;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" C\r\n");
    
    tmxc_uart_puts("Charging:   ");
    if (battery.charging) {
        tmxc_uart_puts("Yes\r\n");
    } else {
        tmxc_uart_puts("No\r\n");
    }
    
    tmxc_uart_puts("Health:     ");
    pos = 20;
    buffer[pos] = '\0';
    temp = battery.health;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" %\r\n");
}

static void tmxc_shell_cmd_cpu(void) {
    tmxc_uart_puts("\r\nCPU Information:\r\n");
    
    for (int i = 0; i < 8; i++) {
        tmxc_uart_puts("CPU ");
        char buffer[21];
        int pos = 20;
        buffer[pos] = '\0';
        uint64_t temp = i;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts(":\r\n");
        
        tmxc_uart_puts("  Frequency: ");
        pos = 20;
        buffer[pos] = '\0';
        temp = 2000;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts(" MHz\r\n");
        
        tmxc_uart_puts("  Temperature: ");
        pos = 20;
        buffer[pos] = '\0';
        temp = 45;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts(" C\r\n");
        
        tmxc_uart_puts("  Usage: ");
        pos = 20;
        buffer[pos] = '\0';
        temp = 50;
        while (temp > 0 && pos > 0) {
            pos--;
            buffer[pos] = '0' + (temp % 10);
            temp /= 10;
        }
        tmxc_uart_puts(&buffer[pos]);
        tmxc_uart_puts(" %\r\n");
    }
}

static void tmxc_shell_cmd_memguard(void) {
    extern uint8_t tmxc_memory_guard_is_enabled(void);
    extern uint32_t tmxc_memory_guard_get_corruption_count(void);
    
    tmxc_uart_puts("\r\nMemory Guard Status:\r\n");
    tmxc_uart_puts("Enabled: ");
    if (tmxc_memory_guard_is_enabled()) {
        tmxc_uart_puts("Yes\r\n");
    } else {
        tmxc_uart_puts("No\r\n");
    }
    
    tmxc_uart_puts("Corruptions detected: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = tmxc_memory_guard_get_corruption_count();
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

static void tmxc_shell_cmd_deadlock(void) {
    extern uint8_t tmxc_deadlock_is_enabled(void);
    extern uint32_t tmxc_deadlock_get_lock_count(void);
    
    tmxc_uart_puts("\r\nDeadlock Detection Status:\r\n");
    tmxc_uart_puts("Enabled: ");
    if (tmxc_deadlock_is_enabled()) {
        tmxc_uart_puts("Yes\r\n");
    } else {
        tmxc_uart_puts("No\r\n");
    }
    
    tmxc_uart_puts("Active locks: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = tmxc_deadlock_get_lock_count();
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

static void tmxc_shell_cmd_power(void) {
    extern uint8_t tmxc_power_get_state(void);
    extern uint64_t tmxc_power_get_battery_voltage(void);
    extern uint64_t tmxc_power_get_battery_capacity(void);
    extern uint8_t tmxc_power_is_charging(void);
    
    tmxc_uart_puts("\r\nPower Management Status:\r\n");
    
    uint8_t state = tmxc_power_get_state();
    tmxc_uart_puts("State: ");
    if (state == 0) {
        tmxc_uart_puts("ON\r\n");
    } else if (state == 1) {
        tmxc_uart_puts("SUSPEND\r\n");
    } else if (state == 2) {
        tmxc_uart_puts("HIBERNATE\r\n");
    } else {
        tmxc_uart_puts("OFF\r\n");
    }
    
    tmxc_uart_puts("Battery voltage: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = tmxc_power_get_battery_voltage();
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" mV\r\n");
    
    tmxc_uart_puts("Battery capacity: ");
    pos = 20;
    buffer[pos] = '\0';
    temp = tmxc_power_get_battery_capacity();
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts(" %\r\n");
    
    tmxc_uart_puts("Charging: ");
    if (tmxc_power_is_charging()) {
        tmxc_uart_puts("Yes\r\n");
    } else {
        tmxc_uart_puts("No\r\n");
    }
}

static void tmxc_shell_cmd_clear(void) {
    tmxc_uart_puts("\033[2J\033[H");
}

static void tmxc_shell_cmd_reboot(void) {
    tmxc_uart_puts("\r\nRebooting system...\r\n");
    extern void tmxc_power_reboot(void);
    tmxc_power_reboot();
}

static void tmxc_shell_cmd_shutdown(void) {
    tmxc_uart_puts("\r\nShutting down system...\r\n");
    extern void tmxc_power_shutdown(void);
    tmxc_power_shutdown();
}

void tmxc_shell_init(void) {
    for (int i = 0; i < 64; i++) {
        tmxc_shell.command[i] = 0;
    }
    for (int i = 0; i < 256; i++) {
        tmxc_shell.args[i] = 0;
    }
    tmxc_shell.active = 0;
    
    tmxc_uart_puts("\r\nTMXC OS Shell v1.0\r\n");
    tmxc_uart_puts("Type 'help' for available commands\r\n");
    tmxc_shell_print_prompt();
}

void tmxc_shell_process_input(char c) {
    static char input_buffer[320];
    static uint32_t input_pos = 0;
    
    if (c == '\r' || c == '\n') {
        input_buffer[input_pos] = '\0';
        
        if (input_pos > 0) {
            tmxc_shell_parse_command(input_buffer, tmxc_shell.command, tmxc_shell.args);
            
            if (tmxc_shell.command[0] != '\0') {
                if (tmxc_strcmp(tmxc_shell.command, "help") == 0) {
                    tmxc_shell_cmd_help();
                } else if (tmxc_strcmp(tmxc_shell.command, "ps") == 0) {
                    tmxc_shell_cmd_ps();
                } else if (tmxc_strcmp(tmxc_shell.command, "free") == 0) {
                    tmxc_shell_cmd_free();
                } else if (tmxc_strcmp(tmxc_shell.command, "uptime") == 0) {
                    tmxc_shell_cmd_uptime();
                } else if (tmxc_strcmp(tmxc_shell.command, "kill") == 0) {
                    tmxc_shell_cmd_kill(tmxc_shell.args);
                } else if (tmxc_strcmp(tmxc_shell.command, "battery") == 0) {
                    tmxc_shell_cmd_battery();
                } else if (tmxc_strcmp(tmxc_shell.command, "cpu") == 0) {
                    tmxc_shell_cmd_cpu();
                } else if (tmxc_strcmp(tmxc_shell.command, "memguard") == 0) {
                    tmxc_shell_cmd_memguard();
                } else if (tmxc_strcmp(tmxc_shell.command, "deadlock") == 0) {
                    tmxc_shell_cmd_deadlock();
                } else if (tmxc_strcmp(tmxc_shell.command, "power") == 0) {
                    tmxc_shell_cmd_power();
                } else if (tmxc_strcmp(tmxc_shell.command, "clear") == 0) {
                    tmxc_shell_cmd_clear();
                } else if (tmxc_strcmp(tmxc_shell.command, "reboot") == 0) {
                    tmxc_shell_cmd_reboot();
                } else if (tmxc_strcmp(tmxc_shell.command, "shutdown") == 0) {
                    tmxc_shell_cmd_shutdown();
                } else {
                    tmxc_uart_puts("\r\nUnknown command: ");
                    tmxc_uart_puts(tmxc_shell.command);
                    tmxc_uart_puts("\r\n");
                }
            }
        }
        
        input_pos = 0;
        tmxc_shell_print_prompt();
    } else if (c == '\b' || c == 127) {
        if (input_pos > 0) {
            input_pos--;
            tmxc_uart_puts("\b \b");
        }
    } else if (c >= 32 && c < 127) {
        if (input_pos < 319) {
            input_buffer[input_pos++] = c;
            tmxc_uart_putc(c);
        }
    }
}

static int tmxc_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const uint8_t*)s1 - *(const uint8_t*)s2;
}

void tmxc_process_kill(uint32_t pid) {
    extern tmxc_process_t tmxc_scheduler_processes[];
    
    for (uint32_t i = 0; i < 256; i++) {
        if (tmxc_scheduler_processes[i].pid == pid) {
            tmxc_scheduler_processes[i].state = TMXC_PROCESS_STATE_TERMINATED;
            return;
        }
    }
}

uint8_t tmxc_deadlock_is_enabled(void) {
    extern uint8_t tmxc_deadlock_detection_enabled;
    return tmxc_deadlock_detection_enabled;
}
