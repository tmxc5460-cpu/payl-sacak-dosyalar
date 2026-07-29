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
#ifndef TMXC_MEMORY_OPTIMIZATION_H
#define TMXC_MEMORY_OPTIMIZATION_H

#include "../tmxc_kernel.h"

#define TMXC_CACHE_LINE_SIZE 64
#define TMXC_CACHE_LINE_MASK (TMXC_CACHE_LINE_SIZE - 1)
#define TMXC_DMA_ALIGNMENT 256
#define TMXC_ZERO_LATENCY_ISR 1

typedef struct {
    uint64_t allocated_blocks;
    uint64_t fragmented_blocks;
    uint64_t contiguous_blocks;
    uint64_t defrag_count;
    uint8_t defrag_enabled;
    uint64_t total_defrag_time_ns;
} tmxc_fragmentation_manager_t;

typedef struct {
    uint64_t cache_aligned_allocations;
    uint64_t cache_misses;
    uint64_t cache_hits;
    uint8_t prefetch_enabled;
    uint64_t prefetch_count;
} tmxc_cache_optimizer_t;

typedef struct {
    uint64_t isr_latency_ns;
    uint64_t isr_count;
    uint64_t max_isr_latency_ns;
    uint8_t zero_latency_enabled;
    uint64_t nested_isr_count;
} tmxc_isr_optimizer_t;

typedef struct {
    uint64_t cpu_c_states[8];
    uint8_t current_c_state[TMXC_MAX_CPUS];
    uint64_t c_state_transitions[TMXC_MAX_CPUS];
    uint64_t idle_time_ns[TMXC_MAX_CPUS];
    uint8_t power_management_enabled;
} tmxc_cpu_power_manager_t;

typedef struct {
    uint64_t dma_channels[16];
    uint64_t dma_transfers[16];
    uint64_t dma_bytes_transferred[16];
    uint8_t dma_enabled[16];
    uint64_t dma_errors[16];
    uint8_t dma_coherent;
} tmxc_dma_manager_t;

typedef struct {
    void* aligned_ptr;
    void* original_ptr;
    uint64_t size;
    uint64_t alignment;
} tmxc_aligned_allocation_t;

void tmxc_memory_optimization_init(void);

void* tmxc_cache_aligned_alloc(uint64_t size, uint64_t alignment);
void tmxc_cache_aligned_free(void* ptr);
void tmxc_enable_cache_prefetch(uint8_t enable);
void tmxc_prefetch_cache_line(void* address);
uint64_t tmxc_get_cache_misses(void);
uint64_t tmxc_get_cache_hits(void);

void tmxc_fragmentation_manager_init(void);
void tmxc_defragment_memory(void);
uint64_t tmxc_get_fragmentation_ratio(void);
void tmxc_enable_defrag(uint8_t enable);

void tmxc_isr_optimizer_init(void);
void tmxc_enter_zero_latency_isr(void);
void tmxc_exit_zero_latency_isr(void);
uint64_t tmxc_get_isr_latency(void);
uint64_t tmxc_get_max_isr_latency(void);

void tmxc_cpu_power_manager_init(void);
void tmxc_set_c_state(uint32_t cpu_id, uint8_t c_state);
uint8_t tmxc_get_c_state(uint32_t cpu_id);
void tmxc_enable_power_management(uint8_t enable);
void tmxc_cpu_idle(uint32_t cpu_id);

void tmxc_dma_manager_init(void);
uint32_t tmxc_dma_allocate_channel(void);
void tmxc_dma_free_channel(uint32_t channel);
int tmxc_dma_transfer(uint32_t channel, uint64_t src, uint64_t dst, uint64_t size);
void tmxc_dma_enable_coherent(uint8_t enable);
uint64_t tmxc_dma_get_bytes_transferred(uint32_t channel);

#endif
