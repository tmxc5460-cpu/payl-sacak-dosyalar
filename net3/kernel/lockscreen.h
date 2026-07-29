/*
 * TMXC_OS - Emergency Recovery Portal Lock Screen (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file lockscreen.h
 * @brief Isolated lock screen UI module with Emergency Support portal
 * 
 * This module implements a minimal, sandboxed UI for the lock screen that:
 * - Runs at EL0 (lowest privilege level)
 * - Has NO access to user data, system files, or private APIs
 * - Provides an "Emergency Support" button
 * - Launches a restricted webview with URL whitelist
 * - Cannot bypass lock screen security
 * 
 * ARMv8-A Architecture Reference Manual:
 * - EL0: Lowest exception level (user mode)
 * - EL1: Kernel mode (kernel controls EL0 access)
 * - MMU enforces memory isolation between EL0 and EL1
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#ifndef TMXC_LOCKSCREEN_H
#define TMXC_LOCKSCREEN_H

#include <stdint.h>

/*
 * ============================================================================
 * Lock Screen Configuration
 * ============================================================================
 */

/**
 * @brief Lock screen state enumeration
 */
typedef enum {
    TMXC_LOCKSCREEN_LOCKED = 0,      /* Screen locked */
    TMXC_LOCKSCREEN_UNLOCKING,       /* Unlock in progress */
    TMXC_LOCKSCREEN_UNLOCKED,        /* Screen unlocked */
    TMXC_LOCKSCREEN_EMERGENCY        /* Emergency portal active */
} tmxc_lockscreen_state_t;

/**
 * @brief Emergency portal state enumeration
 */
typedef enum {
    TMXC_EMERGENCY_IDLE = 0,         /* Emergency portal inactive */
    TMXC_EMERGENCY_LOADING,          /* Loading emergency page */
    TMXC_EMERGENCY_ACTIVE,           /* Emergency page loaded */
    TMXC_EMERGENCY_ERROR             /* Emergency page failed to load */
} tmxc_emergency_state_t;

/**
 * @brief Emergency portal configuration
 */
#define TMXC_EMERGENCY_URL_MAX_LEN   256
#define TMXC_EMERGENCY_WHITELIST_MAX 10

/**
 * @brief Emergency portal whitelist entry
 */
typedef struct {
    char url[TMXC_EMERGENCY_URL_MAX_LEN];  /* Allowed URL pattern */
    uint8_t enabled;                        /* Entry enabled flag */
} tmxc_emergency_whitelist_entry_t;

/*
 * ============================================================================
 * Lock Screen Structure
 * ============================================================================
 */

/**
 * @brief Lock screen context structure
 * 
 * Contains all state for the lock screen UI module.
 * This structure is isolated and sandboxed at EL0.
 */
typedef struct {
    tmxc_lockscreen_state_t state;          /* Current lock screen state */
    tmxc_emergency_state_t emergency_state; /* Emergency portal state */
    
    /* Emergency portal configuration */
    char emergency_url[TMXC_EMERGENCY_URL_MAX_LEN];  /* Current emergency URL */
    tmxc_emergency_whitelist_entry_t whitelist[TMXC_EMERGENCY_WHITELIST_MAX];
    uint32_t whitelist_count;
    
    /* UI state */
    uint32_t button_pressed;                /* Emergency button pressed flag */
    uint64_t last_activity;                /* Last activity timestamp */
    
    /* Security flags */
    uint8_t sandbox_active;                /* Sandbox active flag */
    uint8_t privilege_level;               /* Current privilege level (0 = EL0) */
} tmxc_lockscreen_context_t;

/*
 * ============================================================================
 * Lock Screen Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize lock screen UI module
 * 
 * Initializes the isolated lock screen UI module.
 * Sets up the sandbox and emergency portal whitelist.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_lockscreen_init(void);

/**
 * @brief Render lock screen UI
 * 
 * Renders the lock screen interface including:
 * - Time display
 * - Unlock prompt
 * - Emergency Support button
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_lockscreen_render(void);

/**
 * @brief Handle lock screen input
 * 
 * Handles user input on the lock screen.
 * Processes unlock attempts and emergency button presses.
 * 
 * @param input_type Input type (touch, keyboard, etc.)
 * @param input_data Input data
 * @return 0 on success, negative error code on failure
 */
int tmxc_lockscreen_handle_input(uint32_t input_type, uint64_t input_data);

/**
 * @brief Check if screen is locked
 * 
 * @return int 1 if locked, 0 if unlocked
 */
int tmxc_lockscreen_is_locked(void);

/**
 * @brief Attempt to unlock screen
 * 
 * Attempts to unlock the screen with provided credentials.
 * 
 * @param credentials User credentials (PIN, password, etc.)
 * @return 0 on success (unlocked), negative error code on failure
 */
int tmxc_lockscreen_attempt_unlock(const char* credentials);

/**
 * @brief Lock the screen
 * 
 * Locks the screen and activates the lock screen UI.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_lockscreen_lock(void);

/*
 * ============================================================================
 * Emergency Portal Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize emergency portal
 * 
 * Initializes the emergency portal with URL whitelist.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_emergency_portal_init(void);

/**
 * @brief Launch emergency portal
 * 
 * Launches the restricted webview for emergency support.
 * Only whitelisted URLs are allowed.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_emergency_portal_launch(void);

/**
 * @brief Close emergency portal
 * 
 * Closes the emergency portal and returns to lock screen.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_emergency_portal_close(void);

/**
 * @brief Check if URL is whitelisted
 * 
 * Checks if a URL is in the emergency portal whitelist.
 * 
 * @param url URL to check
 * @return int 1 if whitelisted, 0 if not whitelisted
 */
int tmxc_emergency_is_url_whitelisted(const char* url);

/**
 * @brief Add URL to whitelist
 * 
 * Adds a URL pattern to the emergency portal whitelist.
 * 
 * @param url URL pattern to add
 * @return 0 on success, negative error code on failure
 */
int tmxc_emergency_add_whitelist(const char* url);

/**
 * @brief Remove URL from whitelist
 * 
 * Removes a URL pattern from the emergency portal whitelist.
 * 
 * @param url URL pattern to remove
 * @return 0 on success, negative error code on failure
 */
int tmxc_emergency_remove_whitelist(const char* url);

/*
 * ============================================================================
 * Sandbox Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize EL0 sandbox
 * 
 * Initializes the sandbox for the lock screen UI module.
 * Restricts access to user data, system files, and private APIs.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_sandbox_init(void);

/**
 * @brief Check sandbox status
 * 
 * Checks if the sandbox is active and enforcing restrictions.
 * 
 * @return int 1 if sandbox active, 0 if not active
 */
int tmxc_sandbox_is_active(void);

/**
 * @brief Enforce sandbox restrictions
 * 
 * Enforces sandbox restrictions on memory access and system calls.
 * 
 * @return 0 on success, negative error code on violation
 */
int tmxc_sandbox_enforce(void);

/*
 * ============================================================================
 * Error Logging Function Declarations
 * ============================================================================
 */

/**
 * @brief Log emergency portal error
 * 
 * Logs an error from the emergency portal to guncelleme_gunlugu.txt.
 * 
 * @param error_code Error code
 * @param error_message Error message (in Turkish)
 * @return 0 on success, negative error code on failure
 */
int tmxc_emergency_log_error(int error_code, const char* error_message);

/**
 * @brief Get error message string (Turkish)
 * 
 * Returns a Turkish error message for an error code.
 * 
 * @param error_code Error code
 * @return const char* Error message in Turkish
 */
const char* tmxc_emergency_get_error_message(int error_code);

#endif /* TMXC_LOCKSCREEN_H */
