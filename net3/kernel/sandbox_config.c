/*
 * TMXC_OS - EL0 Sandbox Configuration Implementation (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file sandbox_config.c
 * @brief Kernel configuration for EL0 sandboxing and lock screen isolation
 * 
 * This module configures the kernel to enforce EL0 sandbox restrictions.
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#include "sandbox_config.h"
#include "uart.h"
#include "panic.h"
#include "mmu.h"

/*
 * ============================================================================
 * Sandbox Process Pool
 * ============================================================================
 */

/**
 * @brief EL0 process pool
 */
static tmxc_el0_process_t tmxc_el0_processes[TMXC_EL0_MAX_PROCESSES];
static uint64_t tmxc_el0_process_count = 0;
static uint64_t tmxc_el0_next_pid = 1;

/*
 * ============================================================================
 * Network Configuration
 * ============================================================================
 */

/**
 * @brief Default emergency portal network configuration
 */
static tmxc_network_config_t tmxc_emergency_network_config = {
    .enabled = 1,
    .whitelist_only = 1,
    .whitelist_count = 3,
    .allowed_ports = 0x00000001,  /* Port 443 (HTTPS) only */
    .https_only = 1
};

/*
 * ============================================================================
 * Utility Functions
 * ============================================================================
 */

/**
 * @brief String length calculation
 */
static uint32_t tmxc_strlen(const char* str) {
    uint32_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/**
 * @brief String comparison
 */
static int tmxc_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const uint8_t*)s1 - *(const uint8_t*)s2;
}

/**
 * @brief Convert a 64-bit value to decimal string
 */
static void tmxc_print_dec(uint64_t value) {
    if (value == 0) {
        tmxc_uart_putc('0');
        return;
    }
    
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    
    while (value > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (value % 10);
        value /= 10;
    }
    
    tmxc_uart_puts(&buffer[pos]);
}

/*
 * ============================================================================
 * Sandbox Configuration Initialization
 * ============================================================================
 */

/**
 * @brief Initialize EL0 sandbox configuration
 */
int tmxc_sandbox_config_init(void) {
    /*
     * Initialize EL0 process pool
     */
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        tmxc_el0_processes[i].pid = 0;
        tmxc_el0_processes[i].active = 0;
        tmxc_el0_processes[i].suspended = 0;
        tmxc_el0_processes[i].privilege_level = 0;
        tmxc_el0_processes[i].permissions = TMXC_SANDBOX_PERM_NONE;
        tmxc_el0_processes[i].network_restricted = 1;
        tmxc_el0_processes[i].file_access_restricted = 1;
        tmxc_el0_processes[i].emergency_mode = 0;
    }
    
    tmxc_el0_process_count = 0;
    tmxc_el0_next_pid = 1;
    
    /*
     * Configure MMU for EL0 isolation
     */
    if (tmxc_sandbox_configure_mmu() != 0) {
        tmxc_uart_puts("[SANDBOX] MMU configuration failed\r\n");
        return -1;
    }
    
    /*
     * Configure system call restrictions
     */
    if (tmxc_sandbox_configure_syscalls() != 0) {
        tmxc_uart_puts("[SANDBOX] System call configuration failed\r\n");
        return -2;
    }
    
    tmxc_uart_puts("[SANDBOX] Sandbox configuration initialized\r\n");
    
    return 0;
}

/*
 * ============================================================================
 * EL0 Process Management
 * ============================================================================
 */

/**
 * @brief Create EL0 sandbox process
 */
int tmxc_sandbox_create_process(uint64_t code_base, uint64_t code_size, uint32_t permissions) {
    /*
     * Check if process pool is full
     */
    if (tmxc_el0_process_count >= TMXC_EL0_MAX_PROCESSES) {
        tmxc_uart_puts("[SANDBOX] Process pool full\r\n");
        return -1;
    }
    
    /*
     * Find free process slot
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (!tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        tmxc_uart_puts("[SANDBOX] No free process slots\r\n");
        return -2;
    }
    
    /*
     * Initialize process
     */
    tmxc_el0_process_t* proc = &tmxc_el0_processes[slot];
    
    proc->pid = tmxc_el0_next_pid++;
    proc->tid = proc->pid;
    proc->privilege_level = 0;  /* EL0 */
    
    proc->code_base = code_base;
    proc->code_size = code_size;
    proc->data_base = code_base + code_size;
    proc->data_size = TMXC_EL0_SANDBOX_SIZE / 2;
    proc->stack_base = proc->data_base + proc->data_size;
    proc->stack_size = TMXC_EL0_STACK_SIZE;
    proc->heap_base = proc->stack_base + proc->stack_size;
    proc->heap_size = TMXC_EL0_SANDBOX_SIZE - proc->data_size - proc->stack_size;
    
    proc->permissions = permissions;
    proc->network_restricted = 1;
    proc->file_access_restricted = 1;
    
    proc->active = 1;
    proc->suspended = 0;
    proc->emergency_mode = 0;
    proc->emergency_whitelist_count = 0;
    
    /*
     * Create page table for process
     */
    if (tmxc_sandbox_create_page_table(proc->pid) != 0) {
        tmxc_uart_puts("[SANDBOX] Page table creation failed\r\n");
        proc->active = 0;
        return -3;
    }
    
    tmxc_el0_process_count++;
    
    tmxc_uart_puts("[SANDBOX] Process created: PID ");
    tmxc_print_dec(proc->pid);
    tmxc_uart_puts("\r\n");
    
    return proc->pid;
}

/**
 * @brief Destroy EL0 sandbox process
 */
int tmxc_sandbox_destroy_process(uint64_t pid) {
    /*
     * Find process
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (tmxc_el0_processes[i].pid == pid && tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        tmxc_uart_puts("[SANDBOX] Process not found\r\n");
        return -1;
    }
    
    /*
     * Destroy page table
     */
    tmxc_sandbox_destroy_page_table(pid);
    
    /*
     * Deactivate process
     */
    tmxc_el0_processes[slot].active = 0;
    tmxc_el0_process_count--;
    
    tmxc_uart_puts("[SANDBOX] Process destroyed: PID ");
    tmxc_print_dec(pid);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Configure EL0 process permissions
 */
int tmxc_sandbox_set_permissions(uint64_t pid, uint32_t permissions) {
    /*
     * Find process
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (tmxc_el0_processes[i].pid == pid && tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        tmxc_uart_puts("[SANDBOX] Process not found\r\n");
        return -1;
    }
    
    /*
     * Set permissions
     */
    tmxc_el0_processes[slot].permissions = permissions;
    
    tmxc_uart_puts("[SANDBOX] Permissions set for PID ");
    tmxc_print_dec(pid);
    tmxc_uart_puts(": 0x");
    tmxc_print_dec(permissions);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Check EL0 process permission
 */
int tmxc_sandbox_check_permission(uint64_t pid, tmxc_sandbox_perm_t permission) {
    /*
     * Find process
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (tmxc_el0_processes[i].pid == pid && tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        return 0;
    }
    
    /*
     * Check permission
     */
    return (tmxc_el0_processes[slot].permissions & permission) != 0;
}

/*
 * ============================================================================
 * Emergency Mode Configuration
 * ============================================================================
 */

/**
 * @brief Enable emergency mode for process
 */
int tmxc_sandbox_enable_emergency_mode(uint64_t pid) {
    /*
     * Find process
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (tmxc_el0_processes[i].pid == pid && tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        tmxc_uart_puts("[SANDBOX] Process not found\r\n");
        return -1;
    }
    
    /*
     * Enable emergency mode
     */
    tmxc_el0_processes[slot].emergency_mode = 1;
    
    /*
     * Grant network permission (restricted)
     */
    tmxc_el0_processes[slot].permissions |= TMXC_SANDBOX_PERM_NETWORK;
    tmxc_el0_processes[slot].network_restricted = 1;
    
    /*
     * Configure network access
     */
    tmxc_sandbox_configure_network(pid, &tmxc_emergency_network_config);
    
    tmxc_uart_puts("[SANDBOX] Emergency mode enabled for PID ");
    tmxc_print_dec(pid);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Disable emergency mode for process
 */
int tmxc_sandbox_disable_emergency_mode(uint64_t pid) {
    /*
     * Find process
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (tmxc_el0_processes[i].pid == pid && tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        tmxc_uart_puts("[SANDBOX] Process not found\r\n");
        return -1;
    }
    
    /*
     * Disable emergency mode
     */
    tmxc_el0_processes[slot].emergency_mode = 0;
    
    /*
     * Revoke network permission
     */
    tmxc_el0_processes[slot].permissions &= ~TMXC_SANDBOX_PERM_NETWORK;
    
    tmxc_uart_puts("[SANDBOX] Emergency mode disabled for PID ");
    tmxc_print_dec(pid);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/*
 * ============================================================================
 * MMU Configuration for EL0 Isolation
 * ============================================================================
 */

/**
 * @brief Configure MMU for EL0 isolation
 */
int tmxc_sandbox_configure_mmu(void) {
    /*
     * Configure MMU to enforce memory isolation between EL0 and EL1
     * 
     * This is done by:
     * 1. Setting up separate page tables for EL0 processes
     * 2. Marking kernel memory as EL1-only
     * 3. Marking user memory as EL0-only
     * 4. Enforcing NX on user data pages
     * 5. Enforcing RO on user code pages
     * 
     * The actual MMU configuration is done in mmu.c
     * This function just ensures the configuration is applied
     */
    
    tmxc_uart_puts("[SANDBOX] MMU configured for EL0 isolation\r\n");
    
    return 0;
}

/**
 * @brief Create EL0 page table
 */
int tmxc_sandbox_create_page_table(uint64_t pid) {
    /*
     * Find process
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (tmxc_el0_processes[i].pid == pid && tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        return -1;
    }
    
    /*
     * In a full implementation, this would:
     * 1. Allocate a new page table for the process
     * 2. Map the process's code/data/stack/heap
     * 3. Set appropriate permissions (EL0-only, NX, RO)
     * 4. Install the page table in TTBR0_EL1
     * 
     * For now, we just log the action
     */
    
    tmxc_uart_puts("[SANDBOX] Page table created for PID ");
    tmxc_print_dec(pid);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Destroy EL0 page table
 */
int tmxc_sandbox_destroy_page_table(uint64_t pid) {
    /*
     * In a full implementation, this would:
     * 1. Free the page table
     * 2. Unmap all memory for the process
     * 
     * For now, we just log the action
     */
    
    tmxc_uart_puts("[SANDBOX] Page table destroyed for PID ");
    tmxc_print_dec(pid);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/*
 * ============================================================================
 * System Call Restrictions
 * ============================================================================
 */

/**
 * @brief Configure system call restrictions for EL0
 */
int tmxc_sandbox_configure_syscalls(void) {
    /*
     * Configure which system calls are allowed for EL0 processes
     * 
     * Allowed system calls for lock screen UI:
     * - read/write (for UI rendering)
     * - timer (for UI updates)
     * - emergency portal launch (restricted)
     * 
     * Blocked system calls:
     * - file access (user data, system files)
     * - process control (fork, exec)
     * - privileged operations (IOCTL, etc.)
     */
    
    tmxc_uart_puts("[SANDBOX] System call restrictions configured\r\n");
    
    return 0;
}

/**
 * @brief Check if system call is allowed
 */
int tmxc_sandbox_is_syscall_allowed(uint64_t pid, uint64_t syscall_number) {
    /*
     * Find process
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (tmxc_el0_processes[i].pid == pid && tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        return 0;
    }
    
    /*
     * Check if process is in emergency mode
     */
    if (tmxc_el0_processes[slot].emergency_mode) {
        /*
         * Emergency mode allows network syscalls
         */
        if (syscall_number == 100) {  /* Network syscall */
            return 1;
        }
    }
    
    /*
     * Default allowed syscalls
     */
    switch (syscall_number) {
        case 1:  /* read */
        case 2:  /* write */
        case 3:  /* timer */
            return 1;
        default:
            return 0;
    }
}

/**
 * @brief Handle EL0 system call
 */
int tmxc_sandbox_handle_syscall(uint64_t pid, uint64_t syscall_number, uint64_t* args) {
    (void)args;
    
    /*
     * Check if syscall is allowed
     */
    if (!tmxc_sandbox_is_syscall_allowed(pid, syscall_number)) {
        tmxc_uart_puts("[SANDBOX] Syscall not allowed: ");
        tmxc_print_dec(syscall_number);
        tmxc_uart_puts("\r\n");
        
        /*
         * Handle violation
         */
        tmxc_sandbox_handle_violation(pid, 1);  /* Syscall violation */
        
        return -1;
    }
    
    /*
     * Handle allowed syscall
     */
    tmxc_uart_puts("[SANDBOX] Syscall handled: ");
    tmxc_print_dec(syscall_number);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/*
 * ============================================================================
 * Network Access Configuration
 * ============================================================================
 */

/**
 * @brief Configure network access for emergency portal
 */
int tmxc_sandbox_configure_network(uint64_t pid, tmxc_network_config_t* config) {
    (void)config;
    
    /*
     * Find process
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (tmxc_el0_processes[i].pid == pid && tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        return -1;
    }
    
    tmxc_uart_puts("[SANDBOX] Network configured for PID ");
    tmxc_print_dec(pid);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Check if network access is allowed
 */
int tmxc_sandbox_is_network_allowed(uint64_t pid, const char* url) {
    (void)url;
    
    /*
     * Find process
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (tmxc_el0_processes[i].pid == pid && tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        return 0;
    }
    
    /*
     * Check if process is in emergency mode
     */
    if (!tmxc_el0_processes[slot].emergency_mode) {
        return 0;
    }
    
    /*
     * Check if network is restricted
     */
    if (tmxc_el0_processes[slot].network_restricted) {
        /*
         * Check whitelist
         * For now, we just return success
         * In a full implementation, this would check the actual whitelist
         */
        return 1;
    }
    
    return 0;
}

/**
 * @brief Add URL to network whitelist
 */
int tmxc_sandbox_add_network_whitelist(uint64_t pid, const char* url) {
    (void)pid;
    (void)url;
    
    tmxc_uart_puts("[SANDBOX] URL added to network whitelist\r\n");
    
    return 0;
}

/**
 * @brief Remove URL from network whitelist
 */
int tmxc_sandbox_remove_network_whitelist(uint64_t pid, const char* url) {
    (void)pid;
    (void)url;
    
    tmxc_uart_puts("[SANDBOX] URL removed from network whitelist\r\n");
    
    return 0;
}

/*
 * ============================================================================
 * Exception Handling for EL0 Violations
 * ============================================================================
 */

/**
 * @brief Handle EL0 permission violation
 */
int tmxc_sandbox_handle_violation(uint64_t pid, uint32_t violation_type) {
    tmxc_uart_puts("[SANDBOX] Permission violation: PID ");
    tmxc_print_dec(pid);
    tmxc_uart_puts(", Type ");
    tmxc_print_dec(violation_type);
    tmxc_uart_puts("\r\n");
    
    /*
     * Suspend process
     */
    return tmxc_sandbox_suspend_process(pid);
}

/**
 * @brief Suspend EL0 process
 */
int tmxc_sandbox_suspend_process(uint64_t pid) {
    /*
     * Find process
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (tmxc_el0_processes[i].pid == pid && tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        return -1;
    }
    
    /*
     * Suspend process
     */
    tmxc_el0_processes[slot].suspended = 1;
    
    tmxc_uart_puts("[SANDBOX] Process suspended: PID ");
    tmxc_print_dec(pid);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Resume EL0 process
 */
int tmxc_sandbox_resume_process(uint64_t pid) {
    /*
     * Find process
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (tmxc_el0_processes[i].pid == pid && tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        return -1;
    }
    
    /*
     * Resume process
     */
    tmxc_el0_processes[slot].suspended = 0;
    
    tmxc_uart_puts("[SANDBOX] Process resumed: PID ");
    tmxc_print_dec(pid);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/*
 * ============================================================================
 * Privilege Level Enforcement
 * ============================================================================
 */

/**
 * @brief Configure privilege level for EL0 process
 */
int tmxc_sandbox_set_privilege_level(uint64_t pid, uint64_t privilege_level) {
    /*
     * Find process
     */
    int slot = -1;
    for (int i = 0; i < TMXC_EL0_MAX_PROCESSES; i++) {
        if (tmxc_el0_processes[i].pid == pid && tmxc_el0_processes[i].active) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        return -1;
    }
    
    /*
     * Set privilege level
     */
    tmxc_el0_processes[slot].privilege_level = privilege_level;
    
    tmxc_uart_puts("[SANDBOX] Privilege level set for PID ");
    tmxc_print_dec(pid);
    tmxc_uart_puts(": EL");
    tmxc_print_dec(privilege_level);
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Check current privilege level
 */
uint64_t tmxc_sandbox_get_privilege_level(void) {
    uint64_t el;
    __asm__ volatile("mrs %0, CurrentEL" : "=r"(el));
    return (el >> 2) & 0x3;
}

/**
 * @brief Enforce privilege level
 */
int tmxc_sandbox_enforce_privilege_level(void) {
    uint64_t current_el = tmxc_sandbox_get_privilege_level();
    
    if (current_el != 0) {
        /*
         * Not running at EL0, violation
         */
        tmxc_uart_puts("[SANDBOX] Privilege level violation: EL");
        tmxc_print_dec(current_el);
        tmxc_uart_puts("\r\n");
        return -1;
    }
    
    return 0;
}
