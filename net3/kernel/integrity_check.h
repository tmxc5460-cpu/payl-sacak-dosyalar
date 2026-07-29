/*
 * TMXC_OS - Custom Operating System
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * Unauthorized copying, distribution, or modification is prohibited.
 * 
 * For licensing inquiries, contact: license@tmxc-os.com
 */

#ifndef TMXC_INTEGRITY_CHECK_H
#define TMXC_INTEGRITY_CHECK_H

#include <stdint.h>

/*
 * ============================================================================
 * Integrity Check Configuration
 * ============================================================================
 */

/**
 * @brief SHA-256 hash size in bytes
 */
#define TMXC_INTEGRITY_HASH_SIZE 32

/**
 * @brief Copyright string length
 */
#define TMXC_COPYRIGHT_STRING_LEN 32

/**
 * @brief Maximum number of protected regions
 */
#define TMXC_MAX_PROTECTED_REGIONS 8

/*
 * ============================================================================
 * Integrity Check Structures
 * ============================================================================
 */

/**
 * @brief Integrity check result
 */
typedef enum {
    TMXC_INTEGRITY_OK = 0,
    TMXC_INTEGRITY_HASH_MISMATCH = 1,
    TMXC_INTEGRITY_COPYRIGHT_TAMPERED = 2,
    TMXC_INTEGRITY_SIGNATURE_INVALID = 3,
    TMXC_INTEGRITY_NOT_INITIALIZED = 4
} tmxc_integrity_result_t;

/**
 * @brief Protected memory region
 */
typedef struct {
    uint64_t base_address;
    uint64_t size;
    uint8_t expected_hash[TMXC_INTEGRITY_HASH_SIZE];
    uint8_t is_protected;
    char region_name[32];
} tmxc_protected_region_t;

/**
 * @brief Integrity check context
 */
typedef struct {
    uint8_t kernel_hash[TMXC_INTEGRITY_HASH_SIZE];
    uint8_t copyright_hash[TMXC_INTEGRITY_HASH_SIZE];
    uint8_t copyright_verified;
    uint8_t integrity_verified;
    tmxc_protected_region_t protected_regions[TMXC_MAX_PROTECTED_REGIONS];
    uint32_t region_count;
    uint8_t initialized;
} tmxc_integrity_context_t;

/*
 * ============================================================================
 * Integrity Check Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize integrity check system
 * 
 * Initializes the integrity check system and prepares for verification.
 * Must be called before any other integrity check functions.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_integrity_init(void);

/**
 * @brief Set expected kernel hash
 * 
 * Sets the expected SHA-256 hash of the kernel binary.
 * This hash is compared against the computed hash at runtime.
 * 
 * @param hash Pointer to 32-byte hash array
 * @return 0 on success, negative error code on failure
 */
int tmxc_integrity_set_kernel_hash(const uint8_t* hash);

/**
 * @brief Set expected copyright hash
 * 
 * Sets the expected SHA-256 hash of the copyright string.
 * The copyright string "Ödül Ensar Yılmaz" must be verified.
 * 
 * @param hash Pointer to 32-byte hash array
 * @return 0 on success, negative error code on failure
 */
int tmxc_integrity_set_copyright_hash(const uint8_t* hash);

/**
 * @brief Verify kernel integrity
 * 
 * Computes SHA-256 hash of kernel binary and compares with expected hash.
 * Triggers kernel panic if hash mismatch is detected.
 * 
 * @return tmxc_integrity_result_t Verification result
 */
tmxc_integrity_result_t tmxc_integrity_verify_kernel(void);

/**
 * @brief Verify copyright string
 * 
 * Verifies that the copyright string "Ödül Ensar Yılmaz" has not been tampered with.
 * Uses obfuscated string comparison with runtime-generated key.
 * 
 * @return tmxc_integrity_result_t Verification result
 */
tmxc_integrity_result_t tmxc_integrity_verify_copyright(void);

/**
 * @brief Perform full integrity check
 * 
 * Performs complete integrity verification including:
 * - Kernel hash verification
 * - Copyright string verification
 * - Protected region verification
 * 
 * @return tmxc_integrity_result_t Verification result
 */
tmxc_integrity_result_t tmxc_integrity_verify_all(void);

/**
 * @brief Add protected memory region
 * 
 * Adds a memory region to be monitored for integrity violations.
 * 
 * @param base_address Base address of the region
 * @param size Size of the region in bytes
 * @param name Name of the region (for logging)
 * @return 0 on success, negative error code on failure
 */
int tmxc_integrity_add_protected_region(uint64_t base_address, uint64_t size, const char* name);

/**
 * @brief Verify protected region
 * 
 * Verifies integrity of a specific protected memory region.
 * 
 * @param region_index Index of the region to verify
 * @return tmxc_integrity_result_t Verification result
 */
tmxc_integrity_result_t tmxc_integrity_verify_region(uint32_t region_index);

/**
 * @brief Check if integrity is verified
 * 
 * Returns whether the system has passed integrity verification.
 * 
 * @return int 1 if verified, 0 if not verified
 */
int tmxc_integrity_is_verified(void);

/**
 * @brief Get copyright verification status
 * 
 * Returns whether the copyright string has been verified.
 * 
 * @return int 1 if verified, 0 if not verified
 */
int tmxc_integrity_is_copyright_verified(void);

/**
 * @brief Trigger kernel panic on integrity violation
 * 
 * Triggers kernel panic with integrity violation message.
 * Called automatically when integrity check fails.
 */
void tmxc_integrity_panic(void);

/**
 * @brief Get integrity check result string
 * 
 * Returns a human-readable string for an integrity result code.
 * 
 * @param result Integrity result code
 * @return const char* Result string
 */
const char* tmxc_integrity_result_string(tmxc_integrity_result_t result);

#endif /* TMXC_INTEGRITY_CHECK_H */
