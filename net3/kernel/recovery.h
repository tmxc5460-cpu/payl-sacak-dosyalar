/*
 * TMXC_OS - Cryptography Recovery Module (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file recovery.h
 * @brief One-Time Recovery Code generation and Auth-Override system
 * 
 * This module implements a cryptographically secure recovery protocol:
 * - Generates One-Time Recovery Codes based on Device ID + Timestamp
 * - Strict one-time-use token invalidation
 * - Kernel-level Auth-Override bypassing lock-screen
 * - Master Key decryption for recovery token authorization
 * 
 * ARMv8-A Architecture Reference Manual:
 * - Uses CPU registers for Device ID generation
 * - Uses system timer for timestamp
 * - Kernel-level security enforcement
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#ifndef TMXC_RECOVERY_H
#define TMXC_RECOVERY_H

#include <stdint.h>

/*
 * ============================================================================
 * Recovery Module Configuration
 * ============================================================================
 */

/**
 * @brief Recovery token size (256-bit)
 */
#define TMXC_RECOVERY_TOKEN_SIZE  32

/**
 * @brief Recovery hash size (SHA-256)
 */
#define TMXC_RECOVERY_HASH_SIZE   32

/**
 * @brief Master key size (256-bit)
 */
#define TMXC_MASTER_KEY_SIZE      32

/**
 * @brief Recovery token validity period (seconds)
 */
#define TMXC_TOKEN_VALIDITY       300  /* 5 minutes */

/**
 * @brief Maximum number of active recovery tokens
 */
#define TMXC_MAX_RECOVERY_TOKENS  1

/*
 * ============================================================================
 * Recovery Token Structure
 * ============================================================================
 */

/**
 * @brief Recovery token structure
 */
typedef struct {
    uint8_t token[TMXC_RECOVERY_TOKEN_SIZE];  /* Recovery token */
    uint8_t hash[TMXC_RECOVERY_HASH_SIZE];   /* Token hash */
    uint64_t timestamp;                      /* Generation timestamp */
    uint64_t device_id;                      /* Device ID */
    uint8_t used;                            /* Token used flag */
    uint8_t valid;                           /* Token valid flag */
} tmxc_recovery_token_t;

/**
 * @brief Recovery context structure
 */
typedef struct {
    tmxc_recovery_token_t tokens[TMXC_MAX_RECOVERY_TOKENS];
    uint32_t token_count;
    uint64_t last_generation_time;
    uint8_t recovery_enabled;               /* Recovery system enabled */
    uint8_t auth_override_active;            /* Auth-Override active flag */
} tmxc_recovery_context_t;

/*
 * ============================================================================
 * Device ID Structure
 * ============================================================================
 */

/**
 * @brief Device ID structure
 */
typedef struct {
    uint64_t midr;          /* Main ID Register */
    uint64_t mpidr;         /* Multiprocessor Affinity Register */
    uint64_t revidr;        /* Revision ID Register */
    uint64_t id_aa64isar0;  /* ISA Feature Register 0 */
    uint64_t id_aa64mmfr0;  /* Memory Model Feature Register 0 */
    uint64_t id_aa64pfr0;   /* Processor Feature Register 0 */
    uint64_t cntfrq;        /* Counter Frequency Register */
} tmxc_device_id_t;

/*
 * ============================================================================
 * Recovery Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize recovery module
 * 
 * Initializes the cryptography recovery module.
 * Sets up the recovery context and master key.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_init(void);

/**
 * @brief Generate One-Time Recovery Code
 * 
 * Generates a cryptographically secure recovery code based on:
 * - Device ID (from CPU registers)
 * - Current timestamp
 * - Hardware-specific seed
 * 
 * @param token Pointer to recovery token structure to fill
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_generate_token(tmxc_recovery_token_t* token);

/**
 * @brief Validate Recovery Token
 * 
 * Validates a recovery token:
 * - Checks token hash
 * - Checks timestamp validity
 * - Checks if token has been used
 * 
 * @param token Pointer to recovery token to validate
 * @return 0 on success (token valid), negative error code on failure
 */
int tmxc_recovery_validate_token(tmxc_recovery_token_t* token);

/**
 * @brief Invalidate Recovery Token
 * 
 * Invalidates a recovery token after use.
 * Marks the token as used and prevents reuse.
 * 
 * @param token Pointer to recovery token to invalidate
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_invalidate_token(tmxc_recovery_token_t* token);

/**
 * @brief Get Recovery Token Hash
 * 
 * Returns the hash of the current recovery token for display on lock screen.
 * 
 * @param hash Pointer to hash buffer (32 bytes)
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_get_token_hash(uint8_t* hash);

/**
 * @brief Authorize Recovery Token
 * 
 * Authorizes a recovery token using the Master Key.
 * Decrypts the token and validates the signature.
 * 
 * @param encrypted_token Encrypted recovery token
 * @param token_len Length of encrypted token
 * @return 0 on success (token authorized), negative error code on failure
 */
int tmxc_recovery_authorize_token(const uint8_t* encrypted_token, uint32_t token_len);

/*
 * ============================================================================
 * Auth-Override Function Declarations
 * ============================================================================
 */

/**
 * @brief Trigger Auth-Override
 * 
 * Triggers the Auth-Override at kernel level.
 * Bypasses the lock-screen service without compromising data partition integrity.
 * 
 * @param token Recovery token to authorize the override
 * @return 0 on success (override authorized), negative error code on failure
 */
int tmxc_recovery_trigger_auth_override(tmxc_recovery_token_t* token);

/**
 * @brief Check if Auth-Override is active
 * 
 * Checks if the Auth-Override is currently active.
 * 
 * @return int 1 if active, 0 if not active
 */
int tmxc_recovery_is_auth_override_active(void);

/**
 * @brief Deactivate Auth-Override
 * 
 * Deactivates the Auth-Override and restores normal lock-screen operation.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_deactivate_auth_override(void);

/*
 * ============================================================================
 * Device ID Function Declarations
 * ============================================================================
 */

/**
 * @brief Get Device ID
 * 
 * Retrieves the unique Device ID from CPU registers.
 * 
 * @param device_id Pointer to device ID structure to fill
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_get_device_id(tmxc_device_id_t* device_id);

/**
 * @brief Compute Device ID Hash
 * 
 * Computes SHA-256 hash of the Device ID.
 * 
 * @param device_id Pointer to device ID structure
 * @param hash Pointer to hash buffer (32 bytes)
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_compute_device_id_hash(tmxc_device_id_t* device_id, uint8_t* hash);

/*
 * ============================================================================
 * Master Key Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize Master Key
 * 
 * Initializes the TMXC OS Master Key for token decryption.
 * The Master Key is held by the admin and used to authorize recovery tokens.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_init_master_key(void);

/**
 * @brief Decrypt Recovery Token with Master Key
 * 
 * Decrypts a recovery token using the Master Key.
 * 
 * @param encrypted_token Encrypted recovery token
 * @param token_len Length of encrypted token
 * @param decrypted_token Pointer to decrypted token buffer
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_decrypt_master_key(const uint8_t* encrypted_token, uint32_t token_len,
                                      uint8_t* decrypted_token);

/**
 * @brief Encrypt Recovery Token with Master Key
 * 
 * Encrypts a recovery token using the Master Key.
 * 
 * @param token Recovery token to encrypt
 * @param encrypted_token Pointer to encrypted token buffer
 * @param token_len Pointer to encrypted token length
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_encrypt_master_key(const uint8_t* token, uint32_t token_len,
                                      uint8_t* encrypted_token, uint32_t* encrypted_len);

/*
 * ============================================================================
 * Security Function Declarations
 * ============================================================================
 */

/**
 * @brief Check Recovery Module Security
 * 
 * Performs security checks on the recovery module:
 * - Validates master key integrity
 * - Checks for token replay attacks
 * - Validates timestamp freshness
 * 
 * @return 0 on success (security OK), negative error code on failure
 */
int tmxc_recovery_check_security(void);

/**
 * @brief Lock Recovery Module
 * 
 * Locks the recovery module to prevent further token generation.
 * Used after successful recovery to prevent abuse.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_lock_module(void);

/**
 * @brief Unlock Recovery Module
 * 
 * Unlocks the recovery module to allow token generation.
 * Requires admin authorization.
 * 
 * @param auth_code Admin authorization code
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_unlock_module(uint64_t auth_code);

/*
 * ============================================================================
 * Error Logging Function Declarations
 * ============================================================================
 */

/**
 * @brief Log recovery error
 * 
 * Logs a recovery error to guncelleme_gunlugu.txt in Turkish.
 * 
 * @param error_code Error code
 * @param error_message Error message (in Turkish)
 * @return 0 on success, negative error code on failure
 */
int tmxc_recovery_log_error(int error_code, const char* error_message);

/**
 * @brief Get recovery error message (Turkish)
 * 
 * Returns a Turkish error message for an error code.
 * 
 * @param error_code Error code
 * @return const char* Error message in Turkish
 */
const char* tmxc_recovery_get_error_message(int error_code);

#endif /* TMXC_RECOVERY_H */
