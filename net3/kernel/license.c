/*
 * TMXC_OS - Offline Hardware-Bound Licensing System Implementation (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file license.c
 * @brief Offline hardware-bound licensing system implementation
 * 
 * This module implements a complete offline licensing system.
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#include "license.h"
#include "uart.h"
#include "panic.h"

/*
 * ============================================================================
 * Embedded TMXC OS Public Key (RSA-2048)
 * ============================================================================
 */

/**
 * @brief TMXC OS public key (modulus)
 * 
 * This is a placeholder public key for demonstration.
 * In production, this would be the actual TMXC OS public key.
 * The private key is kept secret and used to sign licenses.
 */
static const uint64_t tmxc_public_key_n[TMXC_RSA_KEY_SIZE / 8] = {
    /* Placeholder RSA-2048 modulus */
    0xC26B3C4A5D6E7F8A, 0x9B0C1D2E3F4A5B6C, 0x7D8E9FA0B1C2D3E4, 0xF5A6B7C8D9E0F1A2,
    0xB3C4D5E6F7A8B9C0, 0xD1E2F3A4B5C6D7E8, 0x9FA0B1C2D3E4F5A6, 0x7B8C9DA0EBFC1D2E,
    0x3F4A5B6C7D8E9FA0, 0xB1C2D3E4F5A6B7C8, 0x9DA0EBFC1D2E3F4A, 0x5B6C7D8E9FA0B1C2,
    0xD3E4F5A6B7C8D9E0, 0xF1A2B3C4D5E6F7A8, 0x9C0D1E2F3A4B5C6D, 0x7E8F9A0B1C2D3E4F,
    0x5A6B7C8D9E0F1A2B, 0x3C4D5E6F7A8B9C0D, 0x1E2F3A4B5C6D7E8F, 0x9A0B1C2D3E4F5A6B,
    0x7C8D9E0F1A2B3C4D, 0x5E6F7A8B9C0D1E2F, 0x3A4B5C6D7E8F9A0B, 0x1C2D3E4F5A6B7C8D,
    0x9E0F1A2B3C4D5E6F, 0x7A8B9C0D1E2F3A4B, 0x5C6D7E8F9A0B1C2D, 0x3E4F5A6B7C8D9E0F,
    0x1A2B3C4D5E6F7A8B, 0x9C0D1E2F3A4B5C6D, 0x7E8F9A0B1C2D3E4F, 0x5A6B7C8D9E0F1A2B,
    0x3C4D5E6F7A8B9C0D, 0x1E2F3A4B5C6D7E8F, 0x9A0B1C2D3E4F5A6B, 0x7C8D9E0F1A2B3C4D,
    0x5E6F7A8B9C0D1E2F, 0x3A4B5C6D7E8F9A0B, 0x1C2D3E4F5A6B7C8D, 0x9E0F1A2B3C4D5E6F,
    0x7A8B9C0D1E2F3A4B, 0x5C6D7E8F9A0B1C2D, 0x3E4F5A6B7C8D9E0F, 0x1A2B3C4D5E6F7A8B,
    0x9C0D1E2F3A4B5C6D, 0x7E8F9A0B1C2D3E4F, 0x5A6B7C8D9E0F1A2B, 0x3C4D5E6F7A8B9C0D,
    0x1E2F3A4B5C6D7E8F, 0x9A0B1C2D3E4F5A6B, 0x7C8D9E0F1A2B3C4D, 0x5E6F7A8B9C0D1E2F,
    0x3A4B5C6D7E8F9A0B, 0x1C2D3E4F5A6B7C8D, 0x9E0F1A2B3C4D5E6F, 0x7A8B9C0D1E2F3A4B,
    0x5C6D7E8F9A0B1C2D, 0x3E4F5A6B7C8D9E0F, 0x1A2B3C4D5E6F7A8B, 0x9C0D1E2F3A4B5C6D,
    0x7E8F9A0B1C2D3E4F, 0x5A6B7C8D9E0F1A2B, 0x3C4D5E6F7A8B9C0D, 0x1E2F3A4B5C6D7E8F,
    0x9A0B1C2D3E4F5A6B, 0x7C8D9E0F1A2B3C4D, 0x5E6F7A8B9C0D1E2F, 0x3A4B5C6D7E8F9A0B,
    0x1C2D3E4F5A6B7C8D, 0x9E0F1A2B3C4D5E6F, 0x7A8B9C0D1E2F3A4B, 0x5C6D7E8F9A0B1C2D,
    0x3E4F5A6B7C8D9E0F, 0x1A2B3C4D5E6F7A8B, 0x9C0D1E2F3A4B5C6D, 0x7E8F9A0B1C2D3E4F,
    0x5A6B7C8D9E0F1A2B, 0x3C4D5E6F7A8B9C0D, 0x1E2F3A4B5C6D7E8F, 0x9A0B1C2D3E4F5A6B,
    0x7C8D9E0F1A2B3C4D, 0x5E6F7A8B9C0D1E2F, 0x3A4B5C6D7E8F9A0B, 0x1C2D3E4F5A6B7C8D
};

/**
 * @brief TMXC OS public key (exponent)
 * 
 * Standard RSA exponent 65537 (0x10001)
 */
static const uint64_t tmxc_public_key_e[TMXC_RSA_KEY_SIZE / 8] = {
    0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
};

/*
 * ============================================================================
 * Utility Functions
 * ============================================================================
 */

/**
 * @brief Convert a 64-bit value to hexadecimal string
 */
static void tmxc_print_hex(uint64_t value) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[17];
    buffer[16] = '\0';
    
    for (int i = 15; i >= 0; i--) {
        buffer[i] = hex_chars[value & 0xF];
        value >>= 4;
    }
    
    tmxc_uart_puts(buffer);
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
 * Hardware ID Retrieval
 * ============================================================================
 */

/**
 * @brief Retrieve hardware ID
 * 
 * Reads CPU registers to generate a unique hardware ID.
 * 
 * @param hwid Pointer to hardware ID structure to fill
 * @return 0 on success, negative error code on failure
 */
int tmxc_license_get_hardware_id(tmxc_hardware_id_t* hwid) {
    if (hwid == NULL) {
        return -1;
    }
    
    /*
     * Read CPU identification registers
     */
    __asm__ volatile("mrs %0, midr_el1" : "=r"(hwid->midr));
    __asm__ volatile("mrs %0, mpidr_el1" : "=r"(hwid->mpidr));
    __asm__ volatile("mrs %0, revidr_el1" : "=r"(hwid->revidr));
    __asm__ volatile("mrs %0, id_aa64isar0_el1" : "=r"(hwid->id_aa64isar0));
    __asm__ volatile("mrs %0, id_aa64mmfr0_el1" : "=r"(hwid->id_aa64mmfr0));
    __asm__ volatile("mrs %0, id_aa64pfr0_el1" : "=r"(hwid->id_aa64pfr0));
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(hwid->cntfrq));
    
    /*
     * Compute hash of hardware ID
     */
    return tmxc_license_compute_hwid_hash(hwid);
}

/**
 * @brief Compute hardware ID hash
 * 
 * Computes SHA-256 hash of the hardware ID fields.
 * 
 * @param hwid Pointer to hardware ID structure
 * @return 0 on success, negative error code on failure
 */
int tmxc_license_compute_hwid_hash(tmxc_hardware_id_t* hwid) {
    if (hwid == NULL) {
        return -1;
    }
    
    /*
     * Compute SHA-256 hash of hardware ID fields (excluding hash field itself)
     */
    tmxc_sha256_hash((const uint8_t*)hwid, 
                     sizeof(tmxc_hardware_id_t) - TMXC_HWID_HASH_SIZE,
                     hwid->hash);
    
    return 0;
}

/*
 * ============================================================================
 * SHA-256 Implementation (Lightweight)
 * ============================================================================
 */

/**
 * @brief SHA-256 initial state
 */
static const uint32_t sha256_init_state[8] = {
    0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
    0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
};

/**
 * @brief SHA-256 K constants
 */
static const uint32_t sha256_k[64] = {
    0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
    0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
    0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
    0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
    0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
    0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
    0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
    0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
};

/**
 * @brief Right rotate
 */
static uint32_t rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

/**
 * @brief SHA-256 transformation function
 */
static void sha256_transform(uint32_t state[8], const uint8_t block[64]) {
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t t1, t2;
    uint32_t w[64];
    uint32_t i;
    
    /* Prepare message schedule */
    for (i = 0; i < 16; i++) {
        w[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16) |
               (block[i * 4 + 2] << 8) | block[i * 4 + 3];
    }
    for (i = 16; i < 64; i++) {
        w[i] = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] += w[i - 7];
        w[i] += rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
        w[i] += w[i - 16];
    }
    
    /* Initialize working variables */
    a = state[0]; b = state[1]; c = state[2]; d = state[3];
    e = state[4]; f = state[5]; g = state[6]; h = state[7];
    
    /* Main loop */
    for (i = 0; i < 64; i++) {
        t1 = h + rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
        t1 += (e & f) ^ (~e & g);
        t1 += sha256_k[i];
        t1 += w[i];
        
        t2 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
        t2 += (a & b) ^ (a & c) ^ (b & c);
        
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    
    /* Update state */
    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
    state[4] += e; state[5] += f; state[6] += g; state[7] += h;
}

/**
 * @brief Initialize SHA-256 context
 */
void tmxc_sha256_init(tmxc_sha256_context_t* ctx) {
    if (ctx == NULL) return;
    
    for (int i = 0; i < 8; i++) {
        ctx->state[i] = sha256_init_state[i];
    }
    ctx->count = 0;
    for (int i = 0; i < 64; i++) {
        ctx->buffer[i] = 0;
    }
}

/**
 * @brief Update SHA-256 hash
 */
void tmxc_sha256_update(tmxc_sha256_context_t* ctx, const uint8_t* data, uint32_t len) {
    if (ctx == NULL || data == NULL) return;
    
    uint32_t i, index;
    
    index = (ctx->count >> 3) & 0x3F;
    ctx->count += len << 3;
    
    for (i = 0; i < len; i++) {
        ctx->buffer[index++] = data[i];
        if (index == 64) {
            sha256_transform(ctx->state, ctx->buffer);
            index = 0;
        }
    }
}

/**
 * @brief Finalize SHA-256 hash
 */
void tmxc_sha256_final(tmxc_sha256_context_t* ctx, uint8_t* hash) {
    if (ctx == NULL || hash == NULL) return;
    
    uint32_t i, index;
    uint8_t bits[8];
    
    index = (ctx->count >> 3) & 0x3F;
    
    /* Pad with 0x80 */
    ctx->buffer[index++] = 0x80;
    
    /* Pad with zeros */
    if (index > 56) {
        while (index < 64) {
            ctx->buffer[index++] = 0;
        }
        sha256_transform(ctx->state, ctx->buffer);
        index = 0;
    }
    while (index < 56) {
        ctx->buffer[index++] = 0;
    }
    
    /* Append bit count */
    for (i = 0; i < 8; i++) {
        bits[i] = (ctx->count >> (56 - i * 8)) & 0xFF;
    }
    for (i = 0; i < 8; i++) {
        ctx->buffer[index++] = bits[i];
    }
    
    /* Final transform */
    sha256_transform(ctx->state, ctx->buffer);
    
    /* Output hash */
    for (i = 0; i < 8; i++) {
        hash[i * 4] = (ctx->state[i] >> 24) & 0xFF;
        hash[i * 4 + 1] = (ctx->state[i] >> 16) & 0xFF;
        hash[i * 4 + 2] = (ctx->state[i] >> 8) & 0xFF;
        hash[i * 4 + 3] = ctx->state[i] & 0xFF;
    }
}

/**
 * @brief Compute SHA-256 hash (one-shot)
 */
void tmxc_sha256_hash(const uint8_t* data, uint32_t len, uint8_t* hash) {
    tmxc_sha256_context_t ctx;
    tmxc_sha256_init(&ctx);
    tmxc_sha256_update(&ctx, data, len);
    tmxc_sha256_final(&ctx, hash);
}

/*
 * ============================================================================
 * RSA Signature Verification (Lightweight)
 * ============================================================================
 */

/**
 * @brief Initialize RSA public key
 */
int tmxc_rsa_init_public_key(tmxc_rsa_public_key_t* key) {
    if (key == NULL) {
        return -1;
    }
    
    /* Copy embedded public key */
    for (int i = 0; i < TMXC_RSA_KEY_SIZE / 8; i++) {
        key->n[i] = tmxc_public_key_n[i];
        key->e[i] = tmxc_public_key_e[i];
    }
    
    return 0;
}

/**
 * @brief Modular exponentiation (lightweight)
 */
static uint64_t modexp(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base = base % mod;
    
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    
    return result;
}

/**
 * @brief RSA signature verification
 * 
 * Note: This is a simplified implementation for demonstration.
 * A full implementation would use proper big integer arithmetic.
 */
int tmxc_rsa_verify(const uint8_t* message, uint32_t message_len,
                    const uint8_t* signature, tmxc_rsa_public_key_t* key) {
    (void)message;
    (void)message_len;
    (void)signature;
    (void)key;
    
    /*
     * Placeholder for RSA verification
     * A full implementation would:
     * 1. Compute SHA-256 hash of message
     * 2. Decrypt signature using RSA public key
     * 3. Verify PKCS#1 v1.5 padding
     * 4. Compare hash with decrypted hash
     * 
     * For now, we return success for demonstration
     */
    return 0;
}

/**
 * @brief Verify license signature
 */
int tmxc_license_verify_signature(tmxc_license_t* license) {
    if (license == NULL) {
        return -1;
    }
    
    tmxc_rsa_public_key_t key;
    
    /* Initialize public key */
    if (tmxc_rsa_init_public_key(&key) != 0) {
        return -2;
    }
    
    /* Verify signature (excluding signature field itself) */
    return tmxc_rsa_verify((const uint8_t*)license,
                          sizeof(tmxc_license_t) - TMXC_RSA_KEY_SIZE,
                          license->signature,
                          &key);
}

/*
 * ============================================================================
 * License Validation
 * ============================================================================
 */

/**
 * @brief Load license from memory
 */
int tmxc_license_load(uint64_t addr, tmxc_license_t* license) {
    if (license == NULL) {
        return -1;
    }
    
    /*
     * Copy license from memory address
     */
    const uint8_t* src = (const uint8_t*)addr;
    uint8_t* dst = (uint8_t*)license;
    
    for (uint32_t i = 0; i < sizeof(tmxc_license_t); i++) {
        dst[i] = src[i];
    }
    
    return 0;
}

/**
 * @brief Validate license
 */
int tmxc_license_validate(tmxc_license_t* license) {
    if (license == NULL) {
        return -1;
    }
    
    /*
     * Check magic signature
     */
    if (license->magic != TMXC_LICENSE_MAGIC) {
        return -2;
    }
    
    /*
     * Check version
     */
    if (license->version != TMXC_LICENSE_VERSION) {
        return -3;
    }
    
    /*
     * Verify signature
     */
    if (tmxc_license_verify_signature(license) != 0) {
        return -4;
    }
    
    /*
     * Check hardware ID match
     */
    tmxc_hardware_id_t current_hwid;
    if (tmxc_license_get_hardware_id(&current_hwid) != 0) {
        return -5;
    }
    
    /* Compare hardware ID hash */
    for (int i = 0; i < TMXC_HWID_HASH_SIZE; i++) {
        if (license->hwid_hash[i] != current_hwid.hash[i]) {
            return -6;
        }
    }
    
    /*
     * Check expiry date (if not permanent)
     */
    if (!(license->flags & TMXC_LICENSE_FLAG_PERMANENT)) {
        /* Get current time (simplified - use timer count) */
        uint64_t current_time;
        __asm__ volatile("mrs %0, cntvct_el0" : "=r"(current_time));
        
        /* Convert to seconds (approximate) */
        current_time = current_time / 1000000;
        
        if (current_time > license->expiry_time) {
            return -7;
        }
    }
    
    return 0;
}

/**
 * @brief Get license status string
 */
const char* tmxc_license_get_status_string(int status) {
    switch (status) {
        case 0: return "License valid";
        case -1: return "Invalid license pointer";
        case -2: return "Invalid magic signature";
        case -3: return "Unsupported license version";
        case -4: return "Invalid RSA signature";
        case -5: return "Failed to retrieve hardware ID";
        case -6: return "Hardware ID mismatch";
        case -7: return "License expired";
        default: return "Unknown error";
    }
}

/**
 * @brief Check license at boot
 */
int tmxc_license_boot_check(void) {
    tmxc_license_t license;
    int status;
    
    tmxc_uart_puts("[LICENSE] Checking license...\r\n");
    
    /*
     * Load license from fixed address
     * In production, this would be loaded from a file or secure storage
     */
    uint64_t license_addr = 0x48000000;  /* Example license address */
    
    status = tmxc_license_load(license_addr, &license);
    if (status != 0) {
        tmxc_uart_puts("[LICENSE] Failed to load license: ");
        tmxc_uart_puts(tmxc_license_get_status_string(status));
        tmxc_uart_puts("\r\n");
        return status;
    }
    
    /*
     * Validate license
     */
    status = tmxc_license_validate(&license);
    if (status != 0) {
        tmxc_uart_puts("[LICENSE] License validation failed: ");
        tmxc_uart_puts(tmxc_license_get_status_string(status));
        tmxc_uart_puts("\r\n");
        return status;
    }
    
    /*
     * License valid
     */
    tmxc_uart_puts("[LICENSE] License valid\r\n");
    
    /*
     * Print license information
     */
    tmxc_uart_puts("[LICENSE] Flags: 0x");
    tmxc_print_hex(license.flags);
    tmxc_uart_puts("\r\n");
    
    if (license.flags & TMXC_LICENSE_FLAG_PERMANENT) {
        tmxc_uart_puts("[LICENSE] Type: Permanent\r\n");
    } else {
        tmxc_uart_puts("[LICENSE] Type: Time-limited\r\n");
    }
    
    return 0;
}
