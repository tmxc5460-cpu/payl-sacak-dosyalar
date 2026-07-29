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
#include "tmxc_memory_optimization.h"

static tmxc_fragmentation_manager_t tmxc_fragmentation_manager;
static tmxc_cache_optimizer_t tmxc_cache_optimizer;
static tmxc_isr_optimizer_t tmxc_isr_optimizer;
static tmxc_cpu_power_manager_t tmxc_cpu_power_manager;
static tmxc_dma_manager_t tmxc_dma_manager;

void tmxc_memory_optimization_init(void) {
    tmxc_fragmentation_manager_init();
    tmxc_cache_optimizer.cache_aligned_allocations = 0;
    tmxc_cache_optimizer.cache_misses = 0;
    tmxc_cache_optimizer.cache_hits = 0;
    tmxc_cache_optimizer.prefetch_enabled = 1;
    tmxc_cache_optimizer.prefetch_count = 0;
    
    tmxc_isr_optimizer_init();
    tmxc_cpu_power_manager_init();
    tmxc_dma_manager_init();
}

void* tmxc_cache_aligned_alloc(uint64_t size, uint64_t alignment) {
    if (alignment == 0) {
        alignment = TMXC_CACHE_LINE_SIZE;
    }
    
    uint64_t total_size = size + alignment + sizeof(tmxc_aligned_allocation_t);
    void* original_ptr = tmxc_malloc(total_size);
    
    if (original_ptr == NULL) {
        return NULL;
    }
    
    uint64_t aligned_addr = ((uint64_t)original_ptr + sizeof(tmxc_aligned_allocation_t) + alignment - 1) & ~(alignment - 1);
    
    tmxc_aligned_allocation_t* alloc_info = (tmxc_aligned_allocation_t*)(aligned_addr - sizeof(tmxc_aligned_allocation_t));
    alloc_info->original_ptr = original_ptr;
    alloc_info->aligned_ptr = (void*)aligned_addr;
    alloc_info->size = size;
    alloc_info->alignment = alignment;
    
    tmxc_cache_optimizer.cache_aligned_allocations++;
    
    return (void*)aligned_addr;
}

void tmxc_cache_aligned_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    
    tmxc_aligned_allocation_t* alloc_info = (tmxc_aligned_allocation_t*)((uint64_t)ptr - sizeof(tmxc_aligned_allocation_t));
    tmxc_free(alloc_info->original_ptr);
}

void tmxc_enable_cache_prefetch(uint8_t enable) {
    tmxc_cache_optimizer.prefetch_enabled = enable;
}

void tmxc_prefetch_cache_line(void* address) {
    if (!tmxc_cache_optimizer.prefetch_enabled) {
        return;
    }
    
    uint64_t addr = (uint64_t)address & ~TMXC_CACHE_LINE_MASK;
    __asm__ volatile("prfm pldl1keep, [%0]" : : "r"(addr));
    tmxc_cache_optimizer.prefetch_count++;
}

uint64_t tmxc_get_cache_misses(void) {
    return tmxc_cache_optimizer.cache_misses;
}

uint64_t tmxc_get_cache_hits(void) {
    return tmxc_cache_optimizer.cache_hits;
}

void tmxc_fragmentation_manager_init(void) {
    tmxc_fragmentation_manager.allocated_blocks = 0;
    tmxc_fragmentation_manager.fragmented_blocks = 0;
    tmxc_fragmentation_manager.contiguous_blocks = 0;
    tmxc_fragmentation_manager.defrag_count = 0;
    tmxc_fragmentation_manager.defrag_enabled = 1;
    tmxc_fragmentation_manager.total_defrag_time_ns = 0;
}

void tmxc_defragment_memory(void) {
    if (!tmxc_fragmentation_manager.defrag_enabled) {
        return;
    }
    
    uint64_t start_time = tmxc_get_cycle_count();
    
    tmxc_uart_puts("[DEFRAG] Starting memory defragmentation...\r\n");
    
    uint64_t defragged_blocks = 0;
    
    for (uint64_t i = 0; i < tmxc_pmm.total_pages; i++) {
        if (tmxc_pmm_test_bit(i)) {
            uint64_t consecutive = 0;
            for (uint64_t j = i; j < tmxc_pmm.total_pages && tmxc_pmm_test_bit(j); j++) {
                consecutive++;
            }
            
            if (consecutive > 1) {
                defragged_blocks += consecutive;
                tmxc_fragmentation_manager.contiguous_blocks++;
            } else {
                tmxc_fragmentation_manager.fragmented_blocks++;
            }
            
            i += consecutive;
        }
    }
    
    uint64_t end_time = tmxc_get_cycle_count();
    uint64_t defrag_time_ns = (end_time - start_time) * 1000000000ULL / tmxc_get_frequency();
    tmxc_fragmentation_manager.total_defrag_time_ns += defrag_time_ns;
    tmxc_fragmentation_manager.defrag_count++;
    
    tmxc_uart_puts("[DEFRAG] Defragmentation complete. Defragged blocks: ");
    char buffer[21];
    int pos = 20;
    buffer[pos] = '\0';
    uint64_t temp = defragged_blocks;
    while (temp > 0 && pos > 0) {
        pos--;
        buffer[pos] = '0' + (temp % 10);
        temp /= 10;
    }
    tmxc_uart_puts(&buffer[pos]);
    tmxc_uart_puts("\r\n");
}

uint64_t tmxc_get_fragmentation_ratio(void) {
    if (tmxc_fragmentation_manager.allocated_blocks == 0) {
        return 0;
    }
    
    return (tmxc_fragmentation_manager.fragmented_blocks * 1000) / tmxc_fragmentation_manager.allocated_blocks;
}

void tmxc_enable_defrag(uint8_t enable) {
    tmxc_fragmentation_manager.defrag_enabled = enable;
}

void tmxc_isr_optimizer_init(void) {
    tmxc_isr_optimizer.isr_latency_ns = 0;
    tmxc_isr_optimizer.isr_count = 0;
    tmxc_isr_optimizer.max_isr_latency_ns = 0;
    tmxc_isr_optimizer.zero_latency_enabled = TMXC_ZERO_LATENCY_ISR;
    tmxc_isr_optimizer.nested_isr_count = 0;
}

void tmxc_enter_zero_latency_isr(void) {
    if (!tmxc_isr_optimizer.zero_latency_enabled) {
        return;
    }
    
    uint64_t start_time = tmxc_get_cycle_count();
    
    __asm__ volatile("msr daifclr, #2");
    
    tmxc_isr_optimizer.nested_isr_count++;
}

void tmxc_exit_zero_latency_isr(void) {
    if (!tmxc_isr_optimizer.zero_latency_enabled) {
        return;
    }
    
    __asm__ volatile("msr daifset, #2");
    
    tmxc_isr_optimizer.nested_isr_count--;
    tmxc_isr_optimizer.isr_count++;
}

uint64_t tmxc_get_isr_latency(void) {
    return tmxc_isr_optimizer.isr_latency_ns;
}

uint64_t tmxc_get_max_isr_latency(void) {
    return tmxc_isr_optimizer.max_isr_latency_ns;
}

void tmxc_cpu_power_manager_init(void) {
    for (int i = 0; i < 8; i++) {
        tmxc_cpu_power_manager.cpu_c_states[i] = 0;
    }
    
    for (int i = 0; i < TMXC_MAX_CPUS; i++) {
        tmxc_cpu_power_manager.current_c_state[i] = 0;
        tmxc_cpu_power_manager.c_state_transitions[i] = 0;
        tmxc_cpu_power_manager.idle_time_ns[i] = 0;
    }
    
    tmxc_cpu_power_manager.power_management_enabled = 1;
}

void tmxc_set_c_state(uint32_t cpu_id, uint8_t c_state) {
    if (cpu_id >= TMXC_MAX_CPUS || c_state > 7) {
        return;
    }
    
    if (tmxc_cpu_power_manager.current_c_state[cpu_id] != c_state) {
        tmxc_cpu_power_manager.c_state_transitions[cpu_id]++;
        tmxc_cpu_power_manager.current_c_state[cpu_id] = c_state;
    }
    
    switch (c_state) {
        case 0:
            break;
        case 1:
            __asm__ volatile("wfi");
            break;
        case 2:
            __asm__ volatile("wfe");
            break;
        default:
            __asm__ volatile("wfi");
            break;
    }
}

uint8_t tmxc_get_c_state(uint32_t cpu_id) {
    if (cpu_id >= TMXC_MAX_CPUS) {
        return 0;
    }
    return tmxc_cpu_power_manager.current_c_state[cpu_id];
}

void tmxc_enable_power_management(uint8_t enable) {
    tmxc_cpu_power_manager.power_management_enabled = enable;
}

void tmxc_cpu_idle(uint32_t cpu_id) {
    if (!tmxc_cpu_power_manager.power_management_enabled) {
        return;
    }
    
    if (cpu_id >= TMXC_MAX_CPUS) {
        return;
    }
    
    uint64_t idle_start = tmxc_get_cycle_count();
    tmxc_set_c_state(cpu_id, 1);
    uint64_t idle_end = tmxc_get_cycle_count();
    
    tmxc_cpu_power_manager.idle_time_ns[cpu_id] += (idle_end - idle_start) * 1000000000ULL / tmxc_get_frequency();
}

void tmxc_dma_manager_init(void) {
    for (int i = 0; i < 16; i++) {
        tmxc_dma_manager.dma_channels[i] = 0;
        tmxc_dma_manager.dma_transfers[i] = 0;
        tmxc_dma_manager.dma_bytes_transferred[i] = 0;
        tmxc_dma_manager.dma_enabled[i] = 0;
        tmxc_dma_manager.dma_errors[i] = 0;
    }
    tmxc_dma_manager.dma_coherent = 1;
}

uint32_t tmxc_dma_allocate_channel(void) {
    for (int i = 0; i < 16; i++) {
        if (!tmxc_dma_manager.dma_enabled[i]) {
            tmxc_dma_manager.dma_enabled[i] = 1;
            return i;
        }
    }
    return 0xFFFFFFFF;
}

void tmxc_dma_free_channel(uint32_t channel) {
    if (channel >= 16) {
        return;
    }
    tmxc_dma_manager.dma_enabled[channel] = 0;
}

int tmxc_dma_transfer(uint32_t channel, uint64_t src, uint64_t dst, uint64_t size) {
    if (channel >= 16 || !tmxc_dma_manager.dma_enabled[channel]) {
        return -1;
    }
    
    if (src == 0 || dst == 0 || size == 0) {
        tmxc_dma_manager.dma_errors[channel]++;
        return -2;
    }
    
    uint8_t* src_ptr = (uint8_t*)src;
    uint8_t* dst_ptr = (uint8_t*)dst;
    
    for (uint64_t i = 0; i < size; i++) {
        dst_ptr[i] = src_ptr[i];
    }
    
    tmxc_dma_manager.dma_transfers[channel]++;
    tmxc_dma_manager.dma_bytes_transferred[channel] += size;
    
    if (tmxc_dma_manager.dma_coherent) {
        __asm__ volatile("dsb sy");
    }
    
    return 0;
}

void tmxc_dma_enable_coherent(uint8_t enable) {
    tmxc_dma_manager.dma_coherent = enable;
}

uint64_t tmxc_dma_get_bytes_transferred(uint32_t channel) {
    if (channel >= 16) {
        return 0;
    }
    return tmxc_dma_manager.dma_bytes_transferred[channel];
}
