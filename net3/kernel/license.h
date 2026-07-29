/*
 * TMXC_OS - Offline Hardware-Bound Licensing System (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file license.h
 * @brief Offline hardware-bound licensing system with RSA signature verification
 * 
 * This module implements a complete offline licensing system that:
 * - Retrieves unique hardware ID from CPU registers
 * - Verifies RSA digital signatures on license files
 * - Refuses to boot if license is invalid or doesn't match hardware
 * - No external dependencies or server calls required
 * 
 * ARMv8-A Hardware ID Sources:
 * - MIDR_EL1: Main ID Register (CPU implementer, variant, part number, revision)
 * - MPIDR_EL1: Multiprocessor Affinity Register (CPU affinity, core ID)
 * - REVIDR_EL1: Revision ID Register
 * - ID_AA64ISAR0_EL1: ISA Feature Register 0
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#ifndef TMXC_LICENSE_H
#define TMXC_LICENSE_H

#include <stdint.h>

/*
 * ============================================================================
 * License Configuration
 * ============================================================================
 */

/**
 * @brief License file magic signature
 */
#define TMXC_LICENSE_MAGIC       0x544D5843  /* "TMXC" in hex */

/**
 * @brief License version
 */
#define TMXC_LICENSE_VERSION     1

/**
 * @brief License file maximum size
 */
#define TMXC_LICENSE_MAX_SIZE    4096

/**
 * @brief RSA key size (2048-bit)
 */
#define TMXC_RSA_KEY_SIZE        256

/**
 * @brief Hardware ID hash size (SHA-256)
 */
#define TMXC_HWID_HASH_SIZE      32

/*
 * ============================================================================
 * Hardware ID Structure
 * ============================================================================
 */

/**
 * @brief Hardware ID structure
 * 
 * Contains unique identifiers from CPU registers.
 */
typedef struct {
    uint64_t midr;          /* Main ID Register */
    uint64_t mpidr;         /* Multiprocessor Affinity Register */
    uint64_t revidr;        /* Revision ID Register */
    uint64_t id_aa64isar0;  /* ISA Feature Register 0 */
    uint64_t id_aa64mmfr0;  /* Memory Model Feature Register 0 */
    uint64_t id_aa64pfr0;   /* Processor Feature Register 0 */
    uint64_t cntfrq;        /* Counter Frequency Register */
    uint8_t  hash[TMXC_HWID_HASH_SIZE];  /* SHA-256 hash of above fields */
} tmxc_hardware_id_t;

/*
 * ============================================================================
 * License File Structure
 * ============================================================================
 */

/**
 * @brief License file header
 */
typedef struct {
    uint32_t magic;         /* Magic signature (TMXC_LICENSE_MAGIC) */
    uint32_t version;       /* License version */
    uint32_t flags;         /* License flags */
    uint32_t reserved;      /* Reserved for future use */
    uint64_t expiry_time;   /* Expiry timestamp (Unix time) */
    uint8_t  hwid_hash[TMXC_HWID_HASH_SIZE];  /* Hardware ID hash */
    uint8_t  signature[TMXC_RSA_KEY_SIZE];     /* RSA signature */
} tmxc_license_t;

/*
 * ============================================================================
 * License Flags
 * ============================================================================
 */

/**
 * @brief License flag bits
 */
#define TMXC_LICENSE_FLAG_PERMANENT   (1 << 0)  /* Permanent license (no expiry) */
#define TMXC_LICENSE_FLAG_DEMO        (1 << 1)  /* Demo license */
#define TMXC_LICENSE_FLAG_ENTERPRISE  (1 << 2)  /* Enterprise license */
#define TMXC_LICENSE_FLAG_DEV         (1 << 3)  /* Developer license */

/*
 * ============================================================================
 * License Function Declarations
 * ============================================================================
 */

/**
 * @brief Retrieve hardware ID
 * 
 * Reads CPU registers to generate a unique hardware ID.
 * Computes SHA-256 hash of the hardware identifiers.
 * 
 * @param hwid Pointer to hardware ID structure to fill
 * @return 0 on success, negative error code on failure
 */
int tmxc_license_get_hardware_id(tmxc_hardware_id_t* hwid);

/**
 * @brief Compute hardware ID hash
 * 
 * Computes SHA-256 hash of the hardware ID fields.
 * 
 * @param hwid Pointer to hardware ID structure
 * @return 0 on success, negative error code on failure
 */
int tmxc_license_compute_hwid_hash(tmxc_hardware_id_t* hwid);

/**
 * @brief Verify license signature
 * 
 * Verifies the RSA signature of the license file.
 * Uses the embedded public key to verify the signature.
 * 
 * @param license Pointer to license structure
 * @return 0 on success (signature valid), negative error code on failure
 */
int tmxc_license_verify_signature(tmxc_license_t* license);

/**
 * @brief Validate license
 * 
 * Validates the license file:
 * 1. Checks magic signature
 * 2. Verifies RSA signature
 * 3. Checks hardware ID match
 * 4. Checks expiry date
 * 
 * @param license Pointer to license structure
 * @return 0 on success (license valid), negative error code on failure
 */
int tmxc_license_validate(tmxc_license_t* license);

/**
 * @brief Load license from memory
 * 
 * Loads the license file from a memory address.
 * 
 * @param addr Memory address of license file
 * @param license Pointer to license structure to fill
 * @return 0 on success, negative error code on failure
 */
int tmxc_license_load(uint64_t addr, tmxc_license_t* license);

/**
 * @brief Check license at boot
 * 
 * Performs complete license check at boot time.
 * Refuses to boot if license is invalid.
 * 
 * @return 0 on success (license valid), negative error code on failure
 */
int tmxc_license_boot_check(void);

/**
 * @brief Get license status string
 * 
 * Returns a human-readable status string.
 * 
 * @param status License status code
 * @return const char* Status string
 */
const char* tmxc_license_get_status_string(int status);

/*
 * ============================================================================
 * RSA Signature Verification (Lightweight Implementation)
 * ============================================================================
 */

/**
 * @brief RSA public key structure
 */
typedef struct {
    uint64_t n[TMXC_RSA_KEY_SIZE / 8];  /* Modulus */
    uint64_t e[TMXC_RSA_KEY_SIZE / 8];  /* Exponent */
} tmxc_rsa_public_key_t;

/**
 * @brief Initialize RSA public key
 * 
 * Loads the embedded TMXC OS public key.
 * 
 * @param key Pointer to RSA public key structure
 * @return 0 on success, negative error code on failure
 */
int tmxc_rsa_init_public_key(tmxc_rsa_public_key_t* key);

/**
 * @brief RSA signature verification
 * 
 * Verifies an RSA signature using PKCS#1 v1.5 padding.
 * 
 * @param message Message that was signed
 * @param message_len Length of message
 * @param signature Signature to verify
 * @param key RSA public key
 * @return 0 on success (signature valid), negative error code on failure
 */
int tmxc_rsa_verify(const uint8_t* message, uint32_t message_len,
                    const uint8_t* signature, tmxc_rsa_public_key_t* key);

/*
 * ============================================================================
 * SHA-256 Hash Function (Lightweight Implementation)
 * ============================================================================
 */

/**
 * @brief SHA-256 context structure
 */
typedef struct {
    uint32_t state[8];      /* Hash state */
    uint64_t count;         /* Message length in bits */
    uint8_t  buffer[64];    /* Message buffer */
} tmxc_sha256_context_t;

/**
 * @brief Initialize SHA-256 context
 * 
 * @param ctx Pointer to SHA-256 context
 */
void tmxc_sha256_init(tmxc_sha256_context_t* ctx);

/**
 * @brief Update SHA-256 hash
 * 
 * @param ctx Pointer to SHA-256 context
 * @param data Pointer to data to hash
 * @param len Length of data
 */
void tmxc_sha256_update(tmxc_sha256_context_t* ctx, const uint8_t* data, uint32_t len);

/**
 * @brief Finalize SHA-256 hash
 * 
 * @param ctx Pointer to SHA-256 context
 * @param hash Pointer to output hash (32 bytes)
 */
void tmxc_sha256_final(tmxc_sha256_context_t* ctx, uint8_t* hash);

/**
 * @brief Compute SHA-256 hash (one-shot)
 * 
 * @param data Pointer to data to hash
 * @param len Length of data
 * @param hash Pointer to output hash (32 bytes)
 */
void tmxc_sha256_hash(const uint8_t* data, uint32_t len, uint8_t* hash);

#endif /* TMXC_LICENSE_H */
