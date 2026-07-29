/*
 * TMXC_OS - EL0 Sandbox Configuration (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file sandbox_config.h
 * @brief Kernel configuration for EL0 sandboxing and lock screen isolation
 * 
 * This module configures the kernel to enforce EL0 sandbox restrictions:
 * - Memory isolation between EL0 and EL1
 * - System call restrictions for EL0
 * - Network access restrictions for emergency portal
 * - Privilege level enforcement
 * 
 * ARMv8-A Architecture Reference Manual:
 * - EL0: Lowest exception level (user mode)
 * - EL1: Kernel mode (controls EL0 access)
 * - MMU enforces memory isolation
 * - System registers control privilege levels
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#ifndef TMXC_SANDBOX_CONFIG_H
#define TMXC_SANDBOX_CONFIG_H

#include <stdint.h>

/*
 * ============================================================================
 * Sandbox Configuration Constants
 * ============================================================================
 */

/**
 * @brief EL0 sandbox memory base address
 */
#define TMXC_EL0_SANDBOX_BASE     0x50000000

/**
 * @brief EL0 sandbox memory size
 */
#define TMXC_EL0_SANDBOX_SIZE     (16 * 1024 * 1024)  /* 16MB */

/**
 * @brief EL0 sandbox stack size
 */
#define TMXC_EL0_STACK_SIZE       (64 * 1024)  /* 64KB */

/**
 * @brief Maximum number of EL0 processes
 */
#define TMXC_EL0_MAX_PROCESSES    8

/*
 * ============================================================================
 * Sandbox Permission Flags
 * ============================================================================
 */

/**
 * @brief Sandbox permission flags
 */
typedef enum {
    TMXC_SANDBOX_PERM_NONE = 0x00000000,
    TMXC_SANDBOX_PERM_READ = 0x00000001,
    TMXC_SANDBOX_PERM_WRITE = 0x00000002,
    TMXC_SANDBOX_PERM_EXECUTE = 0x00000004,
    TMXC_SANDBOX_PERM_NETWORK = 0x00000008,
    TMXC_SANDBOX_PERM_FILE_READ = 0x00000010,
    TMXC_SANDBOX_PERM_FILE_WRITE = 0x00000020,
    TMXC_SANDBOX_PERM_UI = 0x00000040,
    TMXC_SANDBOX_PERM_TIMER = 0x00000080
} tmxc_sandbox_perm_t;

/*
 * ============================================================================
 * Sandbox Process Structure
 * ============================================================================
 */

/**
 * @brief EL0 sandbox process structure
 */
typedef struct {
    uint64_t pid;                     /* Process ID */
    uint64_t tid;                     /* Thread ID */
    uint64_t privilege_level;          /* Privilege level (0 = EL0) */
    
    /* Memory layout */
    uint64_t code_base;               /* Code base address */
    uint64_t code_size;               /* Code size */
    uint64_t data_base;               /* Data base address */
    uint64_t data_size;               /* Data size */
    uint64_t stack_base;              /* Stack base address */
    uint64_t stack_size;              /* Stack size */
    uint64_t heap_base;               /* Heap base address */
    uint64_t heap_size;               /* Heap size */
    
    /* Permissions */
    uint32_t permissions;            /* Sandbox permissions */
    uint8_t network_restricted;       /* Network access restricted flag */
    uint8_t file_access_restricted;   /* File access restricted flag */
    
    /* State */
    uint8_t active;                   /* Process active flag */
    uint8_t suspended;                /* Process suspended flag */
    
    /* Emergency portal specific */
    uint8_t emergency_mode;          /* Emergency mode flag */
    uint64_t emergency_whitelist_count;  /* Number of whitelisted URLs */
} tmxc_el0_process_t;

/*
 * ============================================================================
 * Network Access Configuration
 * ============================================================================
 */

/**
 * @brief Network access restriction structure
 */
typedef struct {
    uint8_t enabled;                  /* Network access enabled */
    uint8_t whitelist_only;           /* Whitelist-only mode */
    uint32_t whitelist_count;        /* Number of whitelisted URLs */
    uint64_t allowed_ports;           /* Allowed ports bitmask */
    uint8_t https_only;               /* HTTPS-only mode */
} tmxc_network_config_t;

/*
 * ============================================================================
 * Sandbox Configuration Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize EL0 sandbox configuration
 * 
 * Initializes the kernel configuration for EL0 sandboxing.
 * Sets up memory isolation, system call restrictions, and network access controls.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_config_init(void);

/**
 * @brief Create EL0 sandbox process
 * 
 * Creates a new EL0 process with sandbox restrictions.
 * 
 * @param code_base Code base address
 * @param code_size Code size
 * @param permissions Sandbox permissions
 * @return int Process ID on success, negative error code on failure
 */
int tmxc_sandbox_create_process(uint64_t code_base, uint64_t code_size, uint32_t permissions);

/**
 * @brief Destroy EL0 sandbox process
 * 
 * Destroys an EL0 process and releases its resources.
 * 
 * @param pid Process ID
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_destroy_process(uint64_t pid);

/**
 * @brief Configure EL0 process permissions
 * 
 * Configures the permissions for an EL0 process.
 * 
 * @param pid Process ID
 * @param permissions Sandbox permissions
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_set_permissions(uint64_t pid, uint32_t permissions);

/**
 * @brief Check EL0 process permission
 * 
 * Checks if an EL0 process has a specific permission.
 * 
 * @param pid Process ID
 * @param permission Permission to check
 * @return int 1 if permitted, 0 if not permitted
 */
int tmxc_sandbox_check_permission(uint64_t pid, tmxc_sandbox_perm_t permission);

/**
 * @brief Enable emergency mode for process
 * 
 * Enables emergency mode for a process with restricted network access.
 * 
 * @param pid Process ID
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_enable_emergency_mode(uint64_t pid);

/**
 * @brief Disable emergency mode for process
 * 
 * Disables emergency mode for a process.
 * 
 * @param pid Process ID
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_disable_emergency_mode(uint64_t pid);

/*
 * ============================================================================
 * MMU Configuration for EL0 Isolation
 * ============================================================================
 */

/**
 * @brief Configure MMU for EL0 isolation
 * 
 * Configures the MMU to enforce memory isolation between EL0 and EL1.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_configure_mmu(void);

/**
 * @brief Create EL0 page table
 * 
 * Creates a page table for an EL0 process with restricted access.
 * 
 * @param pid Process ID
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_create_page_table(uint64_t pid);

/**
 * @brief Destroy EL0 page table
 * 
 * Destroys the page table for an EL0 process.
 * 
 * @param pid Process ID
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_destroy_page_table(uint64_t pid);

/*
 * ============================================================================
 * System Call Restrictions
 * ============================================================================
 */

/**
 * @brief Configure system call restrictions for EL0
 * 
 * Configures which system calls are allowed for EL0 processes.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_configure_syscalls(void);

/**
 * @brief Check if system call is allowed
 * 
 * Checks if a system call is allowed for an EL0 process.
 * 
 * @param pid Process ID
 * @param syscall_number System call number
 * @return int 1 if allowed, 0 if not allowed
 */
int tmxc_sandbox_is_syscall_allowed(uint64_t pid, uint64_t syscall_number);

/**
 * @brief Handle EL0 system call
 * 
 * Handles a system call from an EL0 process with permission checks.
 * 
 * @param pid Process ID
 * @param syscall_number System call number
 * @param args System call arguments
 * @return int System call return value
 */
int tmxc_sandbox_handle_syscall(uint64_t pid, uint64_t syscall_number, uint64_t* args);

/*
 * ============================================================================
 * Network Access Configuration
 * ============================================================================
 */

/**
 * @brief Configure network access for emergency portal
 * 
 * Configures network access restrictions for the emergency portal.
 * 
 * @param pid Process ID
 * @param config Network configuration
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_configure_network(uint64_t pid, tmxc_network_config_t* config);

/**
 * @brief Check if network access is allowed
 * 
 * Checks if network access is allowed for a specific URL.
 * 
 * @param pid Process ID
 * @param url URL to check
 * @return int 1 if allowed, 0 if not allowed
 */
int tmxc_sandbox_is_network_allowed(uint64_t pid, const char* url);

/**
 * @brief Add URL to network whitelist
 * 
 * Adds a URL to the network whitelist for emergency portal.
 * 
 * @param pid Process ID
 * @param url URL to add
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_add_network_whitelist(uint64_t pid, const char* url);

/**
 * @brief Remove URL from network whitelist
 * 
 * Removes a URL from the network whitelist.
 * 
 * @param pid Process ID
 * @param url URL to remove
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_remove_network_whitelist(uint64_t pid, const char* url);

/*
 * ============================================================================
 * Exception Handling for EL0 Violations
 * ============================================================================
 */

/**
 * @brief Handle EL0 permission violation
 * 
 * Handles a permission violation from an EL0 process.
 * 
 * @param pid Process ID
 * @param violation_type Type of violation
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_handle_violation(uint64_t pid, uint32_t violation_type);

/**
 * @brief Suspend EL0 process
 * 
 * Suspends an EL0 process due to security violation.
 * 
 * @param pid Process ID
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_suspend_process(uint64_t pid);

/**
 * @brief Resume EL0 process
 * 
 * Resumes a suspended EL0 process.
 * 
 * @param pid Process ID
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_resume_process(uint64_t pid);

/*
 * ============================================================================
 * Privilege Level Enforcement
 * ============================================================================
 */

/**
 * @brief Configure privilege level for EL0 process
 * 
 * Configures the privilege level for an EL0 process.
 * 
 * @param pid Process ID
 * @param privilege_level Privilege level (0 = EL0)
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_set_privilege_level(uint64_t pid, uint64_t privilege_level);

/**
 * @brief Check current privilege level
 * 
 * Checks the current privilege level of the running process.
 * 
 * @return uint64_t Current privilege level (0 = EL0, 1 = EL1)
 */
uint64_t tmxc_sandbox_get_privilege_level(void);

/**
 * @brief Enforce privilege level
 * 
 * Enforces the privilege level for the current process.
 * 
 * @return 0 on success, negative error code on violation
 */
int tmxc_sandbox_enforce_privilege_level(void);

#endif /* TMXC_SANDBOX_CONFIG_H */
