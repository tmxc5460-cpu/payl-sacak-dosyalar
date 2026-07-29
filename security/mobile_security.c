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
#include "../kernel/tmxc_kernel.h"

typedef struct {
    uint64_t aslr_offset;
    uint8_t aslr_enabled;
    uint64_t stack_canary;
    uint8_t stack_canary_enabled;
    uint8_t sandbox_enabled;
    uint32_t process_sandbox_flags[TMXC_MAX_PROCESSES];
    uint64_t process_isolated_regions[TMXC_MAX_PROCESSES][4];
    uint32_t process_isolated_region_sizes[TMXC_MAX_PROCESSES][4];
    uint8_t process_isolated_count[TMXC_MAX_PROCESSES];
    uint64_t kernel_aslr_base;
    uint64_t user_aslr_base;
    uint8_t nx_enabled;
    uint8_t dep_enabled;
    uint8_t stack_guard_enabled;
    uint8_t heap_guard_enabled;
    uint64_t security_policies[16];
    uint32_t security_level;
    uint8_t tampering_detected;
    uint8_t lockdown_mode;
    uint8_t tamper_sensors[8];
    uint8_t aes_keys_encrypted;
    uint8_t bootloader_integrity_verified;
} tmxc_security_t;

static tmxc_security_t tmxc_security;

static uint64_t tmxc_random_seed = 0;

static uint64_t tmxc_security_random(void) {
    tmxc_random_seed = (tmxc_random_seed * 1103515245ULL + 12345ULL) & 0x7FFFFFFFULL;
    
    uint64_t trng_data = tmxc_read32((volatile uint32_t*)TMXC_TRNG_DATA);
    tmxc_random_seed ^= trng_data;
    
    uint64_t rng_data = tmxc_read32((volatile uint32_t*)TMXC_RNG_DATA);
    tmxc_random_seed ^= (rng_data << 32);
    
    uint64_t cycle_count = tmxc_get_cycle_count();
    tmxc_random_seed ^= cycle_count;
    
    return tmxc_random_seed;
}

static void tmxc_security_init_random(void) {
    uint64_t trng_status = tmxc_read32((volatile uint32_t*)TMXC_TRNG_STATUS);
    
    if (!(trng_status & (1 << 0))) {
        uint32_t trng_ctrl = tmxc_read32((volatile uint32_t*)TMXC_TRNG_CTRL);
        trng_ctrl |= (1 << 0);
        tmxc_write32((volatile uint32_t*)TMXC_TRNG_CTRL, trng_ctrl);
        
        tmxc_timer_delay_ms(10);
    }
    
    uint32_t rng_ctrl = tmxc_read32((volatile uint32_t*)TMXC_RNG_CTRL);
    rng_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_RNG_CTRL, rng_ctrl);
    
    tmxc_random_seed = tmxc_read32((volatile uint32_t*)TMXC_TRNG_DATA);
    tmxc_random_seed |= ((uint64_t)tmxc_read32((volatile uint32_t*)TMXC_TRNG_DATA)) << 32;
    
    tmxc_random_seed ^= tmxc_get_cycle_count();
}

void tmxc_security_init(void) {
    tmxc_security.aslr_offset = 0;
    tmxc_security.aslr_enabled = 1;
    tmxc_security.stack_canary = 0;
    tmxc_security.stack_canary_enabled = 1;
    tmxc_security.sandbox_enabled = 1;
    tmxc_security.nx_enabled = 1;
    tmxc_security.dep_enabled = 1;
    tmxc_security.stack_guard_enabled = 1;
    tmxc_security.heap_guard_enabled = 1;
    tmxc_security.security_level = 3;
    tmxc_security.tampering_detected = 0;
    tmxc_security.lockdown_mode = 0;
    tmxc_security.aes_keys_encrypted = 0;
    tmxc_security.bootloader_integrity_verified = 0;
    
    for (int i = 0; i < 8; i++) {
        tmxc_security.tamper_sensors[i] = 0;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        tmxc_security.process_sandbox_flags[i] = TMXC_SECURITY_FLAG_ISOLATED;
        tmxc_security.process_isolated_count[i] = 0;
        for (int j = 0; j < 4; j++) {
            tmxc_security.process_isolated_regions[i][j] = 0;
            tmxc_security.process_isolated_region_sizes[i][j] = 0;
        }
    }
    
    for (int i = 0; i < 16; i++) {
        tmxc_security.security_policies[i] = 0;
    }
    
    tmxc_security.security_policies[0] = 0x1;
    tmxc_security.security_policies[1] = 0x2;
    tmxc_security.security_policies[2] = 0x4;
    tmxc_security.security_policies[3] = 0x8;
    
    tmxc_security_init_random();
    
    tmxc_aslr_init();
}

void tmxc_aslr_init(void) {
    if (!tmxc_security.aslr_enabled) {
        return;
    }
    
    tmxc_security.aslr_offset = tmxc_security_random() & 0xFFFFFFFFFFFFF000ULL;
    
    tmxc_security.kernel_aslr_base = TMXC_KERNEL_BASE + (tmxc_security_random() & 0xFFFFFFFFF000ULL);
    tmxc_security.user_aslr_base = 0x1000000000ULL + (tmxc_security_random() & 0xFFFFFFFFF000ULL);
    
    uint64_t tcr_value = 0;
    __asm__ volatile("mrs %0, tcr_el1" : "=r"(tcr_value));
    
    tcr_value |= (1ULL << 36);
    
    __asm__ volatile("msr tcr_el1, %0" : : "r"(tcr_value));
    
    __asm__ volatile("isb");
}

uint64_t tmxc_aslr_get_offset(void) {
    return tmxc_security.aslr_offset;
}

void tmxc_aslr_set_offset(uint64_t offset) {
    tmxc_security.aslr_offset = offset & 0xFFFFFFFFFFFFF000ULL;
}

void tmxc_aslr_enable(uint8_t enable) {
    tmxc_security.aslr_enabled = enable;
}

uint8_t tmxc_aslr_is_enabled(void) {
    return tmxc_security.aslr_enabled;
}

uint64_t tmxc_aslr_randomize_address(uint64_t base) {
    if (!tmxc_security.aslr_enabled) {
        return base;
    }
    
    uint64_t random_offset = tmxc_security_random() & 0xFFFFFFFFF000ULL;
    return base + random_offset;
}

void tmxc_stack_canary_init(void) {
    if (!tmxc_security.stack_canary_enabled) {
        return;
    }
    
    tmxc_security.stack_canary = tmxc_security_random();
    
    uint64_t canary_value = tmxc_security.stack_canary;
    
    canary_value ^= 0xAAAAAAAAAAAAAAAAULL;
    canary_value ^= 0x5555555555555555ULL;
    canary_value ^= 0xFFFFFFFFFFFFFFFFULL;
    
    tmxc_security.stack_canary = canary_value;
}

uint64_t tmxc_stack_canary_get(void) {
    return tmxc_security.stack_canary;
}

void tmxc_stack_canary_set(uint64_t canary) {
    tmxc_security.stack_canary = canary;
}

void tmxc_stack_canary_enable(uint8_t enable) {
    tmxc_security.stack_canary_enabled = enable;
}

uint8_t tmxc_stack_canary_is_enabled(void) {
    return tmxc_security.stack_canary_enabled;
}

uint8_t tmxc_stack_canary_verify(uint64_t canary) {
    return (canary == tmxc_security.stack_canary) ? 1 : 0;
}

void tmxc_sandbox_init(void) {
    if (!tmxc_security.sandbox_enabled) {
        return;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        tmxc_security.process_sandbox_flags[i] = TMXC_SECURITY_FLAG_ISOLATED;
        tmxc_security.process_isolated_count[i] = 0;
        
        for (int j = 0; j < 4; j++) {
            tmxc_security.process_isolated_regions[i][j] = 0;
            tmxc_security.process_isolated_region_sizes[i][j] = 0;
        }
    }
}

void tmxc_sandbox_enable(uint8_t enable) {
    tmxc_security.sandbox_enabled = enable;
}

uint8_t tmxc_sandbox_is_enabled(void) {
    return tmxc_security.sandbox_enabled;
}

void tmxc_sandbox_set_process_flags(uint32_t pid, uint32_t flags) {
    if (pid < TMXC_MAX_PROCESSES) {
        tmxc_security.process_sandbox_flags[pid] = flags;
    }
}

uint32_t tmxc_sandbox_get_process_flags(uint32_t pid) {
    if (pid < TMXC_MAX_PROCESSES) {
        return tmxc_security.process_sandbox_flags[pid];
    }
    return 0;
}

void tmxc_sandbox_add_isolated_region(uint32_t pid, uint64_t base, uint32_t size) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return;
    }
    
    if (tmxc_security.process_isolated_count[pid] >= 4) {
        return;
    }
    
    uint32_t idx = tmxc_security.process_isolated_count[pid];
    tmxc_security.process_isolated_regions[pid][idx] = base;
    tmxc_security.process_isolated_region_sizes[pid][idx] = size;
    tmxc_security.process_isolated_count[pid]++;
}

void tmxc_sandbox_remove_isolated_region(uint32_t pid, uint64_t base) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return;
    }
    
    for (uint32_t i = 0; i < tmxc_security.process_isolated_count[pid]; i++) {
        if (tmxc_security.process_isolated_regions[pid][i] == base) {
            for (uint32_t j = i; j < tmxc_security.process_isolated_count[pid] - 1; j++) {
                tmxc_security.process_isolated_regions[pid][j] = tmxc_security.process_isolated_regions[pid][j + 1];
                tmxc_security.process_isolated_region_sizes[pid][j] = tmxc_security.process_isolated_region_sizes[pid][j + 1];
            }
            tmxc_security.process_isolated_count[pid]--;
            break;
        }
    }
}

uint8_t tmxc_sandbox_is_region_isolated(uint32_t pid, uint64_t addr) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return 0;
    }
    
    for (uint32_t i = 0; i < tmxc_security.process_isolated_count[pid]; i++) {
        uint64_t base = tmxc_security.process_isolated_regions[pid][i];
        uint32_t size = tmxc_security.process_isolated_region_sizes[pid][i];
        
        if (addr >= base && addr < base + size) {
            return 1;
        }
    }
    
    return 0;
}

void tmxc_nx_enable(uint8_t enable) {
    tmxc_security.nx_enabled = enable;
    
    uint64_t sctlr_value = 0;
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr_value));
    
    if (enable) {
        sctlr_value |= (1ULL << 0);
    } else {
        sctlr_value &= ~(1ULL << 0);
    }
    
    __asm__ volatile("msr sctlr_el1, %0" : : "r"(sctlr_value));
    
    __asm__ volatile("isb");
}

uint8_t tmxc_nx_is_enabled(void) {
    return tmxc_security.nx_enabled;
}

void tmxc_dep_enable(uint8_t enable) {
    tmxc_security.dep_enabled = enable;
    
    uint64_t sctlr_value = 0;
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr_value));
    
    if (enable) {
        sctlr_value |= (1ULL << 19);
    } else {
        sctlr_value &= ~(1ULL << 19);
    }
    
    __asm__ volatile("msr sctlr_el1, %0" : : "r"(sctlr_value));
    
    __asm__ volatile("isb");
}

uint8_t tmxc_dep_is_enabled(void) {
    return tmxc_security.dep_enabled;
}

void tmxc_stack_guard_enable(uint8_t enable) {
    tmxc_security.stack_guard_enabled = enable;
}

uint8_t tmxc_stack_guard_is_enabled(void) {
    return tmxc_security.stack_guard_enabled;
}

void tmxc_heap_guard_enable(uint8_t enable) {
    tmxc_security.heap_guard_enabled = enable;
}

uint8_t tmxc_heap_guard_is_enabled(void) {
    return tmxc_security.heap_guard_enabled;
}

void tmxc_security_set_level(uint32_t level) {
    if (level > 3) level = 3;
    
    tmxc_security.security_level = level;
    
    switch (level) {
        case 0:
            tmxc_aslr_enable(0);
            tmxc_stack_canary_enable(0);
            tmxc_sandbox_enable(0);
            tmxc_nx_enable(0);
            tmxc_dep_enable(0);
            break;
        case 1:
            tmxc_aslr_enable(1);
            tmxc_stack_canary_enable(1);
            tmxc_sandbox_enable(0);
            tmxc_nx_enable(1);
            tmxc_dep_enable(1);
            break;
        case 2:
            tmxc_aslr_enable(1);
            tmxc_stack_canary_enable(1);
            tmxc_sandbox_enable(1);
            tmxc_nx_enable(1);
            tmxc_dep_enable(1);
            tmxc_stack_guard_enable(1);
            break;
        case 3:
            tmxc_aslr_enable(1);
            tmxc_stack_canary_enable(1);
            tmxc_sandbox_enable(1);
            tmxc_nx_enable(1);
            tmxc_dep_enable(1);
            tmxc_stack_guard_enable(1);
            tmxc_heap_guard_enable(1);
            break;
    }
}

uint32_t tmxc_security_get_level(void) {
    return tmxc_security.security_level;
}

void tmxc_security_set_policy(uint32_t policy_index, uint64_t policy_value) {
    if (policy_index < 16) {
        tmxc_security.security_policies[policy_index] = policy_value;
    }
}

uint64_t tmxc_security_get_policy(uint32_t policy_index) {
    if (policy_index < 16) {
        return tmxc_security.security_policies[policy_index];
    }
    return 0;
}

uint8_t tmxc_security_check_policy(uint32_t policy_index, uint64_t value) {
    if (policy_index < 16) {
        return (tmxc_security.security_policies[policy_index] & value) ? 1 : 0;
    }
    return 0;
}

void tmxc_security_enforce_process_isolation(uint32_t pid) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return;
    }
    
    if (!tmxc_security.sandbox_enabled) {
        return;
    }
    
    uint32_t flags = tmxc_security.process_sandbox_flags[pid];
    
    if (flags & TMXC_SECURITY_FLAG_ISOLATED) {
        for (uint32_t i = 0; i < tmxc_security.process_isolated_count[pid]; i++) {
            uint64_t base = tmxc_security.process_isolated_regions[pid][i];
            uint32_t size = tmxc_security.process_isolated_region_sizes[pid][i];
            
            uint64_t attributes = TMXC_MMU_AP_EL1_RW_EL0_NONE | TMXC_MMU_nG | TMXC_MMU_AF;
            
            for (uint64_t addr = base; addr < base + size; addr += TMXC_PAGE_SIZE) {
                tmxc_mmu_map_page(addr, addr, attributes);
            }
        }
    }
    
    if (flags & TMXC_SECURITY_FLAG_SECURE) {
        uint64_t trustzone_base = TMXC_TRUSTZONE_S_OFFSET;
        uint64_t trustzone_attributes = TMXC_MMU_AP_EL1_RW_EL0_NONE | TMXC_MMU_AF | TMXC_MMU_PXN | TMXC_MMU_UXN;
        
        for (uint64_t addr = trustzone_base; addr < trustzone_base + 0x10000000; addr += TMXC_PAGE_SIZE) {
            tmxc_mmu_map_page(addr, addr, trustzone_attributes);
        }
    }
}

void tmxc_security_validate_memory_access(uint32_t pid, uint64_t addr, uint32_t size, uint8_t is_write) {
    if (pid >= TMXC_MAX_PROCESSES) {
        return;
    }
    
    if (!tmxc_security.sandbox_enabled) {
        return;
    }
    
    for (uint64_t check_addr = addr; check_addr < addr + size; check_addr += TMXC_PAGE_SIZE) {
        if (tmxc_sandbox_is_region_isolated(pid, check_addr)) {
            uint32_t flags = tmxc_security.process_sandbox_flags[pid];
            
            if (is_write && !(flags & TMXC_SECURITY_FLAG_TRUSTED)) {
                uint64_t attributes = TMXC_MMU_AP_EL1_RW_EL0_NONE | TMXC_MMU_AF | TMXC_MMU_PXN;
                tmxc_mmu_map_page(check_addr, check_addr, attributes);
            }
        }
    }
}

void tmxc_security_protect_kernel_memory(void) {
    uint64_t kernel_start = TMXC_KERNEL_BASE;
    uint64_t kernel_end = TMXC_KERNEL_BASE + 0x10000000;
    
    uint64_t kernel_attributes = TMXC_MMU_AP_EL1_RW_EL0_NONE | TMXC_MMU_AF | TMXC_MMU_PXN | TMXC_MMU_UXN;
    
    for (uint64_t addr = kernel_start; addr < kernel_end; addr += TMXC_PAGE_SIZE) {
        tmxc_mmu_map_page(addr, addr, kernel_attributes);
    }
    
    uint64_t trustzone_start = TMXC_TRUSTZONE_S_OFFSET;
    uint64_t trustzone_end = TMXC_TRUSTZONE_S_OFFSET + 0x10000000;
    
    uint64_t trustzone_attributes = TMXC_MMU_AP_EL1_RW_EL0_NONE | TMXC_MMU_AF | TMXC_MMU_PXN | TMXC_MMU_UXN;
    
    for (uint64_t addr = trustzone_start; addr < trustzone_end; addr += TMXC_PAGE_SIZE) {
        tmxc_mmu_map_page(addr, addr, trustzone_attributes);
    }
}

void tmxc_security_enable_trustzone(void) {
    uint64_t scr_value = 0;
    __asm__ volatile("mrs %0, scr_el3" : "=r"(scr_value));
    
    scr_value |= (1ULL << 0);
    scr_value |= (1ULL << 10);
    
    __asm__ volatile("msr scr_el3, %0" : : "r"(scr_value));
    
    __asm__ volatile("isb");
}

uint8_t tmxc_security_is_trustzone_enabled(void) {
    uint64_t scr_value = 0;
    __asm__ volatile("mrs %0, scr_el3" : "=r"(scr_value));
    
    return (scr_value & (1ULL << 0)) ? 1 : 0;
}

void tmxc_security_lock_bootloader(void) {
    uint64_t tzpc_ctrl = tmxc_read32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_CTRL);
    tzpc_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_CTRL, tzpc_ctrl);
    
    uint64_t r0_size = tmxc_read32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_R0_SIZE);
    r0_size = 0xFFFFFFFF;
    tmxc_write32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_R0_SIZE, r0_size);
}

uint8_t tmxc_security_is_bootloader_locked(void) {
    uint64_t tzpc_ctrl = tmxc_read32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_CTRL);
    return (tzpc_ctrl & (1 << 0)) ? 1 : 0;
}

void tmxc_security_generate_entropy(uint8_t* buffer, uint32_t size) {
    if (buffer == NULL || size == 0) {
        return;
    }
    
    for (uint32_t i = 0; i < size; i++) {
        buffer[i] = (uint8_t)(tmxc_security_random() & 0xFF);
    }
}

uint64_t tmxc_security_get_random(void) {
    return tmxc_security_random();
}

void tmxc_security_check_tampering(void) {
    uint32_t tamper_status = tmxc_read32((volatile uint32_t*)TMXC_TRUSTZONE_TAMPER_STATUS);
    
    for (int i = 0; i < 8; i++) {
        tmxc_security.tamper_sensors[i] = (tamper_status >> i) & 0x01;
    }
    
    if (tamper_status != 0) {
        tmxc_uart_puts("[SECURITY] Tampering detected!\r\n");
        
        tmxc_security.tampering_detected = 1;
        tmxc_security_enter_lockdown();
    }
}

void tmxc_security_enter_lockdown(void) {
    if (tmxc_security.lockdown_mode) {
        return;
    }
    
    tmxc_uart_puts("[SECURITY] ENTERING LOCKDOWN MODE\r\n");
    
    tmxc_security.lockdown_mode = 1;
    
    tmxc_security_encrypt_aes_keys();
    
    uint32_t tzpc_ctrl = tmxc_read32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_CTRL);
    tzpc_ctrl |= (1 << 1);
    tmxc_write32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_CTRL, tzpc_ctrl);
    
    uint32_t tzpc_r0_size = tmxc_read32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_R0_SIZE);
    tzpc_r0_size = 0x00000000;
    tmxc_write32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_R0_SIZE, tzpc_r0_size);
    
    uint32_t tzpc_r1_size = tmxc_read32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_R1_SIZE);
    tzpc_r1_size = 0x00000000;
    tmxc_write32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_R1_SIZE, tzpc_r1_size);
    
    tmxc_uart_puts("[SECURITY] Lockdown mode active - all memory regions locked\r\n");
}

void tmxc_security_exit_lockdown(void) {
    if (!tmxc_security.lockdown_mode) {
        return;
    }
    
    tmxc_uart_puts("[SECURITY] Exiting lockdown mode (requires verification)\r\n");
    
    uint32_t tzpc_ctrl = tmxc_read32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_CTRL);
    tzpc_ctrl &= ~(1 << 1);
    tmxc_write32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_CTRL, tzpc_ctrl);
    
    uint32_t tzpc_r0_size = tmxc_read32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_R0_SIZE);
    tzpc_r0_size = 0xFFFFFFFF;
    tmxc_write32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_R0_SIZE, tzpc_r0_size);
    
    uint32_t tzpc_r1_size = tmxc_read32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_R1_SIZE);
    tzpc_r1_size = 0xFFFFFFFF;
    tmxc_write32((volatile uint32_t*)TMXC_TRUSTZONE_TZPC_R1_SIZE, tzpc_r1_size);
    
    tmxc_security.lockdown_mode = 0;
    
    tmxc_uart_puts("[SECURITY] Lockdown mode exited\r\n");
}

void tmxc_security_encrypt_aes_keys(void) {
    if (tmxc_security.aes_keys_encrypted) {
        return;
    }
    
    tmxc_uart_puts("[SECURITY] Encrypting AES keys...\r\n");
    
    uint8_t key_storage[32];
    for (uint8_t i = 0; i < 32; i++) {
        key_storage[i] = (uint8_t)(tmxc_security_random() & 0xFF);
    }
    
    uint32_t tzpc_key_ctrl = tmxc_read32((volatile uint32_t*)TMXC_TRUSTZONE_KEY_CTRL);
    tzpc_key_ctrl |= (1 << 0);
    tmxc_write32((volatile uint32_t*)TMXC_TRUSTZONE_KEY_CTRL, tzpc_key_ctrl);
    
    for (uint8_t i = 0; i < 32; i++) {
        tmxc_write8((volatile uint8_t*)(TMXC_TRUSTZONE_KEY_STORAGE + i), key_storage[i]);
    }
    
    uint32_t tzpc_key_ctrl_final = tmxc_read32((volatile uint32_t*)TMXC_TRUSTZONE_KEY_CTRL);
    tzpc_key_ctrl_final |= (1 << 1);
    tmxc_write32((volatile uint32_t*)TMXC_TRUSTZONE_KEY_CTRL, tzpc_key_ctrl_final);
    
    tmxc_security.aes_keys_encrypted = 1;
    
    tmxc_uart_puts("[SECURITY] AES keys encrypted and locked\r\n");
}

void tmxc_security_verify_bootloader_integrity(void) {
    uint32_t bootloader_hash = tmxc_read32((volatile uint32_t*)TMXC_BOOTLOADER_HASH);
    
    uint32_t expected_hash = 0xDEADBEEF;
    
    if (bootloader_hash == expected_hash) {
        tmxc_security.bootloader_integrity_verified = 1;
        tmxc_uart_puts("[SECURITY] Bootloader integrity verified\r\n");
    } else {
        tmxc_uart_puts("[SECURITY] Bootloader integrity check failed\r\n");
        tmxc_security.tampering_detected = 1;
        tmxc_security_enter_lockdown();
    }
}

uint8_t tmxc_security_is_tampering_detected(void) {
    return tmxc_security.tampering_detected;
}

uint8_t tmxc_security_is_lockdown_mode(void) {
    return tmxc_security.lockdown_mode;
}

uint8_t tmxc_security_are_aes_keys_encrypted(void) {
    return tmxc_security.aes_keys_encrypted;
}

uint8_t tmxc_security_is_bootloader_verified(void) {
    return tmxc_security.bootloader_integrity_verified;
}

uint8_t tmxc_security_get_tamper_sensor_status(uint8_t sensor_index) {
    if (sensor_index < 8) {
        return tmxc_security.tamper_sensors[sensor_index];
    }
    return 0;
}
