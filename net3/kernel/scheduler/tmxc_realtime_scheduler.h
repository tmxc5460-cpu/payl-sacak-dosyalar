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
#ifndef TMXC_REALTIME_SCHEDULER_H
#define TMXC_REALTIME_SCHEDULER_H

#include "../tmxc_kernel.h"

#define TMXC_RT_PRIORITY_LEVELS 32
#define TMXC_RT_TIME_SLICE_NS 1000000
#define TMXC_RT_DEADLINE_TOLERANCE_NS 10000

typedef enum {
    TMXC_RT_TASK_IDLE = 0,
    TMXC_RT_TASK_READY = 1,
    TMXC_RT_TASK_RUNNING = 2,
    TMXC_RT_TASK_BLOCKED = 3,
    TMXC_RT_TASK_TERMINATED = 4
} tmxc_rt_task_state_t;

typedef struct {
    uint32_t task_id;
    uint32_t priority;
    uint64_t deadline_ns;
    uint64_t execution_time_ns;
    uint64_t remaining_time_ns;
    uint64_t period_ns;
    tmxc_rt_task_state_t state;
    uint64_t stack_base;
    uint64_t stack_size;
    uint64_t context[32];
    uint8_t is_realtime;
    uint8_t preemptible;
    uint64_t last_run_time;
    uint64_t total_run_time;
} tmxc_rt_task_t;

typedef struct {
    tmxc_rt_task_t tasks[TMXC_MAX_THREADS];
    uint32_t task_count;
    uint32_t current_task;
    uint64_t scheduler_ticks;
    uint64_t context_switches;
    uint64_t missed_deadlines;
    uint64_t total_preemptions;
    uint8_t scheduler_running;
    uint8_t preempt_enabled;
    uint64_t quantum_ns;
} tmxc_rt_scheduler_t;

typedef struct {
    uint64_t cpu_affinity[TMXC_MAX_CPUS];
    uint64_t load_history[TMXC_MAX_CPUS][100];
    uint32_t load_index[TMXC_MAX_CPUS];
    uint64_t migration_count;
    uint8_t load_balancing_enabled;
} tmxc_rt_load_balancer_t;

void tmxc_rt_scheduler_init(void);
void tmxc_rt_scheduler_start(void);
void tmxc_rt_scheduler_tick(void);
uint32_t tmxc_rt_task_create(void (*entry)(void), uint32_t priority, uint64_t deadline_ns, uint64_t period_ns);
void tmxc_rt_task_yield(void);
void tmxc_rt_task_exit(void);
void tmxc_rt_task_set_priority(uint32_t task_id, uint32_t priority);
uint32_t tmxc_rt_task_get_priority(uint32_t task_id);
void tmxc_rt_task_set_deadline(uint32_t task_id, uint64_t deadline_ns);
uint64_t tmxc_rt_task_get_deadline(uint32_t task_id);
void tmxc_rt_enable_preemption(uint8_t enable);
uint64_t tmxc_rt_get_missed_deadlines(void);
uint64_t tmxc_rt_get_context_switches(void);

void tmxc_rt_load_balancer_init(void);
void tmxc_rt_balance_load(void);
void tmxc_rt_set_cpu_affinity(uint32_t task_id, uint32_t cpu_id);
uint32_t tmxc_rt_get_cpu_affinity(uint32_t task_id);

#endif
