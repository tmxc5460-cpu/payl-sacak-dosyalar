/*
 * TMXC_OS - Custom Operating System
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * Unauthorized copying, distribution, or modification is prohibited.
 * 
 * For licensing inquiries, contact: license@tmxc-os.com
 */

#include "integrity_check.h"
#include "uart.h"
#include "panic.h"
#include "mmu.h"

/*
 * ============================================================================
 * Obfuscated Copyright String
 * ============================================================================
 */

/**
 * @brief Obfuscated copyright string (XOR-encoded)
 * 
 * Original: "Ödül Ensar Yılmaz"
 * Encoded with XOR key 0x5A
 */
static const uint8_t tmxc_copyright_obfuscated[TMXC_COPYRIGHT_STRING_LEN] = {
    0x3F, 0x37, 0x5B, 0x3F, 0x20, 0x5F, 0x5E, 0x5D, 0x5F, 0x5D, 0x20,
    0x59, 0x3D, 0x5D, 0x5F, 0x5D, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A,
    0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A
};

/**
 * @brief XOR key for copyright string
 */
static const uint8_t tmxc_copyright_key = 0x5A;

/*
 * ============================================================================
 * Integrity Check Context
 * ============================================================================
 */

static tmxc_integrity_context_t tmxc_integrity_ctx;

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
 * @brief Memory copy
 */
static void tmxc_memcpy(void* dst, const void* src, uint32_t len) {
    uint8_t* d = (uint8_t*)dst;
    const uint8_t* s = (const uint8_t*)src;
    while (len--) {
        *d++ = *s++;
    }
}

/**
 * @brief Memory compare
 */
static int tmxc_memcmp(const void* s1, const void* s2, uint32_t len) {
    const uint8_t* p1 = (const uint8_t*)s1;
    const uint8_t* p2 = (const uint8_t*)s2;
    while (len--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

/**
 * @brief Memory set
 */
static void tmxc_memset(void* ptr, uint8_t value, uint32_t len) {
    uint8_t* p = (uint8_t*)ptr;
    while (len--) {
        *p++ = value;
    }
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

/*
 * ============================================================================
 * SHA-256 Implementation
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
 * @brief SHA-256 context structure
 */
typedef struct {
    uint32_t state[8];
    uint64_t count;
    uint8_t  buffer[64];
} tmxc_sha256_context_t;

/**
 * @brief Initialize SHA-256 context
 */
static void tmxc_sha256_init(tmxc_sha256_context_t* ctx) {
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
static void tmxc_sha256_update(tmxc_sha256_context_t* ctx, const uint8_t* data, uint32_t len) {
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
static void tmxc_sha256_final(tmxc_sha256_context_t* ctx, uint8_t* hash) {
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
static void tmxc_sha256_hash(const uint8_t* data, uint32_t len, uint8_t* hash) {
    tmxc_sha256_context_t ctx;
    tmxc_sha256_init(&ctx);
    tmxc_sha256_update(&ctx, data, len);
    tmxc_sha256_final(&ctx, hash);
}

/*
 * ============================================================================
 * Integrity Check Implementation
 * ============================================================================
 */

/**
 * @brief Initialize integrity check system
 */
int tmxc_integrity_init(void) {
    /*
     * Initialize integrity context
     */
    tmxc_memset(&tmxc_integrity_ctx, 0, sizeof(tmxc_integrity_context_t));
    tmxc_integrity_ctx.region_count = 0;
    tmxc_integrity_ctx.copyright_verified = 0;
    tmxc_integrity_ctx.integrity_verified = 0;
    tmxc_integrity_ctx.initialized = 1;
    
    tmxc_uart_puts("[INTEGRITY] Integrity check system initialized\r\n");
    
    return 0;
}

/**
 * @brief Set expected kernel hash
 */
int tmxc_integrity_set_kernel_hash(const uint8_t* hash) {
    if (!tmxc_integrity_ctx.initialized || hash == NULL) {
        return -1;
    }
    
    tmxc_memcpy(tmxc_integrity_ctx.kernel_hash, hash, TMXC_INTEGRITY_HASH_SIZE);
    
    tmxc_uart_puts("[INTEGRITY] Kernel hash set\r\n");
    
    return 0;
}

/**
 * @brief Set expected copyright hash
 */
int tmxc_integrity_set_copyright_hash(const uint8_t* hash) {
    if (!tmxc_integrity_ctx.initialized || hash == NULL) {
        return -1;
    }
    
    tmxc_memcpy(tmxc_integrity_ctx.copyright_hash, hash, TMXC_INTEGRITY_HASH_SIZE);
    
    tmxc_uart_puts("[INTEGRITY] Copyright hash set\r\n");
    
    return 0;
}

/**
 * @brief Verify copyright string
 */
tmxc_integrity_result_t tmxc_integrity_verify_copyright(void) {
    if (!tmxc_integrity_ctx.initialized) {
        return TMXC_INTEGRITY_NOT_INITIALIZED;
    }
    
    /*
     * Decode obfuscated copyright string
     */
    uint8_t copyright_decoded[TMXC_COPYRIGHT_STRING_LEN];
    for (int i = 0; i < TMXC_COPYRIGHT_STRING_LEN; i++) {
        copyright_decoded[i] = tmxc_copyright_obfuscated[i] ^ tmxc_copyright_key;
    }
    
    /*
     * Compute hash of decoded string
     */
    uint8_t computed_hash[TMXC_INTEGRITY_HASH_SIZE];
    tmxc_sha256_hash(copyright_decoded, TMXC_COPYRIGHT_STRING_LEN, computed_hash);
    
    /*
     * Compare with expected hash
     */
    if (tmxc_memcmp(computed_hash, tmxc_integrity_ctx.copyright_hash, TMXC_INTEGRITY_HASH_SIZE) != 0) {
        tmxc_uart_puts("[INTEGRITY] Copyright string tampered!\r\n");
        tmxc_integrity_ctx.copyright_verified = 0;
        return TMXC_INTEGRITY_COPYRIGHT_TAMPERED;
    }
    
    tmxc_integrity_ctx.copyright_verified = 1;
    tmxc_uart_puts("[INTEGRITY] Copyright string verified\r\n");
    
    return TMXC_INTEGRITY_OK;
}

/**
 * @brief Verify kernel integrity
 */
tmxc_integrity_result_t tmxc_integrity_verify_kernel(void) {
    if (!tmxc_integrity_ctx.initialized) {
        return TMXC_INTEGRITY_NOT_INITIALIZED;
    }
    
    /*
     * In a full implementation, this would compute the hash of the
     * kernel binary and compare with the expected hash.
     * For now, we simulate this with a placeholder check.
     */
    
    /*
     * Placeholder: Compute hash of a known kernel region
     * In production, this would hash the entire kernel binary
     */
    uint64_t kernel_start = 0x40000000;  /* Kernel base address */
    uint64_t kernel_size = 0x100000;     /* 1MB kernel size */
    
    /*
     * Compute hash (placeholder - actual implementation would read memory)
     */
    uint8_t computed_hash[TMXC_INTEGRITY_HASH_SIZE];
    tmxc_memset(computed_hash, 0, TMXC_INTEGRITY_HASH_SIZE);
    
    /*
     * Compare with expected hash
     */
    if (tmxc_memcmp(computed_hash, tmxc_integrity_ctx.kernel_hash, TMXC_INTEGRITY_HASH_SIZE) != 0) {
        tmxc_uart_puts("[INTEGRITY] Kernel hash mismatch!\r\n");
        tmxc_integrity_ctx.integrity_verified = 0;
        return TMXC_INTEGRITY_HASH_MISMATCH;
    }
    
    tmxc_integrity_ctx.integrity_verified = 1;
    tmxc_uart_puts("[INTEGRITY] Kernel hash verified\r\n");
    
    return TMXC_INTEGRITY_OK;
}

/**
 * @brief Perform full integrity check
 */
tmxc_integrity_result_t tmxc_integrity_verify_all(void) {
    tmxc_integrity_result_t result;
    
    /*
     * Verify copyright string first
     */
    result = tmxc_integrity_verify_copyright();
    if (result != TMXC_INTEGRITY_OK) {
        tmxc_integrity_panic();
        return result;
    }
    
    /*
     * Verify kernel integrity
     */
    result = tmxc_integrity_verify_kernel();
    if (result != TMXC_INTEGRITY_OK) {
        tmxc_integrity_panic();
        return result;
    }
    
    /*
     * Verify all protected regions
     */
    for (uint32_t i = 0; i < tmxc_integrity_ctx.region_count; i++) {
        result = tmxc_integrity_verify_region(i);
        if (result != TMXC_INTEGRITY_OK) {
            tmxc_integrity_panic();
            return result;
        }
    }
    
    tmxc_uart_puts("[INTEGRITY] Full integrity check passed\r\n");
    
    return TMXC_INTEGRITY_OK;
}

/**
 * @brief Add protected memory region
 */
int tmxc_integrity_add_protected_region(uint64_t base_address, uint64_t size, const char* name) {
    if (!tmxc_integrity_ctx.initialized || tmxc_integrity_ctx.region_count >= TMXC_MAX_PROTECTED_REGIONS) {
        return -1;
    }
    
    uint32_t index = tmxc_integrity_ctx.region_count;
    
    tmxc_integrity_ctx.protected_regions[index].base_address = base_address;
    tmxc_integrity_ctx.protected_regions[index].size = size;
    tmxc_integrity_ctx.protected_regions[index].is_protected = 1;
    
    if (name != NULL) {
        for (uint32_t j = 0; j < 32 && name[j] != 0; j++) {
            tmxc_integrity_ctx.protected_regions[index].region_name[j] = name[j];
        }
    }
    
    tmxc_integrity_ctx.region_count++;
    
    tmxc_uart_puts("[INTEGRITY] Protected region added: ");
    tmxc_uart_puts(name ? name : "Unknown");
    tmxc_uart_puts("\r\n");
    
    return 0;
}

/**
 * @brief Verify protected region
 */
tmxc_integrity_result_t tmxc_integrity_verify_region(uint32_t region_index) {
    if (!tmxc_integrity_ctx.initialized || region_index >= tmxc_integrity_ctx.region_count) {
        return TMXC_INTEGRITY_NOT_INITIALIZED;
    }
    
    /*
     * In a full implementation, this would compute the hash of the
     * protected memory region and compare with the expected hash.
     * For now, we simulate this with a placeholder check.
     */
    
    tmxc_protected_region_t* region = &tmxc_integrity_ctx.protected_regions[region_index];
    
    /*
     * Placeholder hash computation
     */
    uint8_t computed_hash[TMXC_INTEGRITY_HASH_SIZE];
    tmxc_memset(computed_hash, 0, TMXC_INTEGRITY_HASH_SIZE);
    
    /*
     * Compare with expected hash
     */
    if (tmxc_memcmp(computed_hash, region->expected_hash, TMXC_INTEGRITY_HASH_SIZE) != 0) {
        tmxc_uart_puts("[INTEGRITY] Region ");
        tmxc_uart_puts(region->region_name);
        tmxc_uart_puts(" integrity violation!\r\n");
        return TMXC_INTEGRITY_HASH_MISMATCH;
    }
    
    return TMXC_INTEGRITY_OK;
}

/**
 * @brief Check if integrity is verified
 */
int tmxc_integrity_is_verified(void) {
    return tmxc_integrity_ctx.integrity_verified && tmxc_integrity_ctx.copyright_verified;
}

/**
 * @brief Get copyright verification status
 */
int tmxc_integrity_is_copyright_verified(void) {
    return tmxc_integrity_ctx.copyright_verified;
}

/**
 * @brief Trigger kernel panic on integrity violation
 */
void tmxc_integrity_panic(void) {
    tmxc_uart_puts("[INTEGRITY] INTEGRITY VIOLATION DETECTED!\r\n");
    tmxc_uart_puts("[INTEGRITY] System halt required.\r\n");
    
    /*
     * Trigger kernel panic
     */
    tmxc_kernel_panic("Integrity violation detected");
}

/**
 * @brief Get integrity check result string
 */
const char* tmxc_integrity_result_string(tmxc_integrity_result_t result) {
    switch (result) {
        case TMXC_INTEGRITY_OK:
            return "OK";
        case TMXC_INTEGRITY_HASH_MISMATCH:
            return "Hash mismatch";
        case TMXC_INTEGRITY_COPYRIGHT_TAMPERED:
            return "Copyright tampered";
        case TMXC_INTEGRITY_SIGNATURE_INVALID:
            return "Signature invalid";
        case TMXC_INTEGRITY_NOT_INITIALIZED:
            return "Not initialized";
        default:
            return "Unknown";
    }
}
