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
#include "tmxc_realtime_scheduler.h"

static tmxc_rt_scheduler_t tmxc_rt_scheduler;
static tmxc_rt_load_balancer_t tmxc_rt_load_balancer;

void tmxc_rt_scheduler_init(void) {
    for (uint32_t i = 0; i < TMXC_MAX_THREADS; i++) {
        tmxc_rt_scheduler.tasks[i].task_id = 0;
        tmxc_rt_scheduler.tasks[i].priority = TMXC_THREAD_PRIORITY_DEFAULT;
        tmxc_rt_scheduler.tasks[i].deadline_ns = 0;
        tmxc_rt_scheduler.tasks[i].execution_time_ns = 0;
        tmxc_rt_scheduler.tasks[i].remaining_time_ns = 0;
        tmxc_rt_scheduler.tasks[i].period_ns = 0;
        tmxc_rt_scheduler.tasks[i].state = TMXC_RT_TASK_TERMINATED;
        tmxc_rt_scheduler.tasks[i].stack_base = 0;
        tmxc_rt_scheduler.tasks[i].stack_size = 0;
        tmxc_rt_scheduler.tasks[i].is_realtime = 0;
        tmxc_rt_scheduler.tasks[i].preemptible = 1;
        tmxc_rt_scheduler.tasks[i].last_run_time = 0;
        tmxc_rt_scheduler.tasks[i].total_run_time = 0;
        for (int j = 0; j < 32; j++) {
            tmxc_rt_scheduler.tasks[i].context[j] = 0;
        }
    }
    
    tmxc_rt_scheduler.task_count = 0;
    tmxc_rt_scheduler.current_task = 0;
    tmxc_rt_scheduler.scheduler_ticks = 0;
    tmxc_rt_scheduler.context_switches = 0;
    tmxc_rt_scheduler.missed_deadlines = 0;
    tmxc_rt_scheduler.total_preemptions = 0;
    tmxc_rt_scheduler.scheduler_running = 0;
    tmxc_rt_scheduler.preempt_enabled = 1;
    tmxc_rt_scheduler.quantum_ns = TMXC_RT_TIME_SLICE_NS;
    
    tmxc_rt_load_balancer_init();
}

void tmxc_rt_scheduler_start(void) {
    tmxc_rt_scheduler.scheduler_running = 1;
    
    while (tmxc_rt_scheduler.scheduler_running) {
        tmxc_rt_scheduler_tick();
        
        if (tmxc_rt_scheduler.task_count > 0) {
            tmxc_rt_task_yield();
        } else {
            tmxc_wfi();
        }
    }
}

void tmxc_rt_scheduler_tick(void) {
    tmxc_rt_scheduler.scheduler_ticks++;
    
    for (uint32_t i = 0; i < TMXC_MAX_THREADS; i++) {
        if (tmxc_rt_scheduler.tasks[i].state == TMXC_RT_TASK_RUNNING) {
            uint64_t current_time = tmxc_get_cycle_count();
            uint64_t elapsed_ns = (current_time - tmxc_rt_scheduler.tasks[i].last_run_time) * 1000000000ULL / tmxc_get_frequency();
            
            tmxc_rt_scheduler.tasks[i].remaining_time_ns -= elapsed_ns;
            tmxc_rt_scheduler.tasks[i].total_run_time += elapsed_ns;
            tmxc_rt_scheduler.tasks[i].last_run_time = current_time;
            
            if (tmxc_rt_scheduler.tasks[i].is_realtime && tmxc_rt_scheduler.tasks[i].remaining_time_ns <= 0) {
                if (current_time > tmxc_rt_scheduler.tasks[i].deadline_ns + TMXC_RT_DEADLINE_TOLERANCE_NS) {
                    tmxc_rt_scheduler.missed_deadlines++;
                }
            }
            
            if (tmxc_rt_scheduler.tasks[i].remaining_time_ns <= 0) {
                tmxc_rt_scheduler.tasks[i].state = TMXC_RT_TASK_READY;
                if (tmxc_rt_scheduler.tasks[i].period_ns > 0) {
                    tmxc_rt_scheduler.tasks[i].deadline_ns += tmxc_rt_scheduler.tasks[i].period_ns;
                    tmxc_rt_scheduler.tasks[i].remaining_time_ns = tmxc_rt_scheduler.tasks[i].execution_time_ns;
                }
            }
        }
    }
    
    if (tmxc_rt_load_balancer.load_balancing_enabled) {
        tmxc_rt_balance_load();
    }
}

uint32_t tmxc_rt_task_create(void (*entry)(void), uint32_t priority, uint64_t deadline_ns, uint64_t period_ns) {
    if (tmxc_rt_scheduler.task_count >= TMXC_MAX_THREADS) {
        return 0;
    }
    
    uint32_t task_id = tmxc_rt_scheduler.task_count + 1;
    
    for (uint32_t i = 0; i < TMXC_MAX_THREADS; i++) {
        if (tmxc_rt_scheduler.tasks[i].task_id == 0) {
            tmxc_rt_scheduler.tasks[i].task_id = task_id;
            tmxc_rt_scheduler.tasks[i].priority = priority > TMXC_THREAD_PRIORITY_MAX ? TMXC_THREAD_PRIORITY_MAX : priority;
            tmxc_rt_scheduler.tasks[i].deadline_ns = deadline_ns > 0 ? deadline_ns : tmxc_get_cycle_count() * 1000000000ULL / tmxc_get_frequency() + 1000000000ULL;
            tmxc_rt_scheduler.tasks[i].execution_time_ns = deadline_ns > 0 ? deadline_ns / 2 : 500000000ULL;
            tmxc_rt_scheduler.tasks[i].remaining_time_ns = tmxc_rt_scheduler.tasks[i].execution_time_ns;
            tmxc_rt_scheduler.tasks[i].period_ns = period_ns;
            tmxc_rt_scheduler.tasks[i].state = TMXC_RT_TASK_READY;
            tmxc_rt_scheduler.tasks[i].stack_base = (uint64_t)tmxc_malloc(TMXC_STACK_SIZE);
            tmxc_rt_scheduler.tasks[i].stack_size = TMXC_STACK_SIZE;
            tmxc_rt_scheduler.tasks[i].is_realtime = (deadline_ns > 0) ? 1 : 0;
            tmxc_rt_scheduler.tasks[i].preemptible = 1;
            tmxc_rt_scheduler.tasks[i].last_run_time = tmxc_get_cycle_count();
            tmxc_rt_scheduler.tasks[i].total_run_time = 0;
            
            for (int j = 0; j < 32; j++) {
                tmxc_rt_scheduler.tasks[i].context[j] = 0;
            }
            
            tmxc_rt_scheduler.tasks[i].context[0] = (uint64_t)entry;
            tmxc_rt_scheduler.tasks[i].context[31] = tmxc_rt_scheduler.tasks[i].stack_base + tmxc_rt_scheduler.tasks[i].stack_size;
            
            tmxc_rt_scheduler.task_count++;
            return task_id;
        }
    }
    
    return 0;
}

void tmxc_rt_task_yield(void) {
    if (!tmxc_rt_scheduler.preempt_enabled) {
        return;
    }
    
    uint32_t current_task = tmxc_rt_scheduler.current_task;
    if (current_task >= TMXC_MAX_THREADS) {
        return;
    }
    
    tmxc_rt_scheduler.tasks[current_task].state = TMXC_RT_TASK_READY;
    
    uint32_t highest_priority = 0;
    uint32_t next_task = current_task;
    uint64_t earliest_deadline = ~0ULL;
    
    for (uint32_t i = 0; i < TMXC_MAX_THREADS; i++) {
        if (tmxc_rt_scheduler.tasks[i].task_id != 0 && tmxc_rt_scheduler.tasks[i].state == TMXC_RT_TASK_READY) {
            if (tmxc_rt_scheduler.tasks[i].is_realtime) {
                if (tmxc_rt_scheduler.tasks[i].deadline_ns < earliest_deadline) {
                    earliest_deadline = tmxc_rt_scheduler.tasks[i].deadline_ns;
                    highest_priority = tmxc_rt_scheduler.tasks[i].priority;
                    next_task = i;
                }
            } else {
                if (tmxc_rt_scheduler.tasks[i].priority > highest_priority) {
                    highest_priority = tmxc_rt_scheduler.tasks[i].priority;
                    next_task = i;
                }
            }
        }
    }
    
    if (next_task != current_task) {
        tmxc_rt_scheduler.context_switches++;
        tmxc_rt_scheduler.total_preemptions++;
        tmxc_rt_scheduler.current_task = next_task;
        tmxc_rt_scheduler.tasks[next_task].state = TMXC_RT_TASK_RUNNING;
        tmxc_rt_scheduler.tasks[next_task].last_run_time = tmxc_get_cycle_count();
    }
}

void tmxc_rt_task_exit(void) {
    uint32_t current_task = tmxc_rt_scheduler.current_task;
    if (current_task >= TMXC_MAX_THREADS) {
        return;
    }
    
    tmxc_rt_scheduler.tasks[current_task].state = TMXC_RT_TASK_TERMINATED;
    tmxc_rt_scheduler.task_count--;
    
    if (tmxc_rt_scheduler.tasks[current_task].stack_base != 0) {
        tmxc_free((void*)tmxc_rt_scheduler.tasks[current_task].stack_base);
    }
    
    tmxc_rt_task_yield();
}

void tmxc_rt_task_set_priority(uint32_t task_id, uint32_t priority) {
    for (uint32_t i = 0; i < TMXC_MAX_THREADS; i++) {
        if (tmxc_rt_scheduler.tasks[i].task_id == task_id) {
            tmxc_rt_scheduler.tasks[i].priority = priority > TMXC_THREAD_PRIORITY_MAX ? TMXC_THREAD_PRIORITY_MAX : priority;
            return;
        }
    }
}

uint32_t tmxc_rt_task_get_priority(uint32_t task_id) {
    for (uint32_t i = 0; i < TMXC_MAX_THREADS; i++) {
        if (tmxc_rt_scheduler.tasks[i].task_id == task_id) {
            return tmxc_rt_scheduler.tasks[i].priority;
        }
    }
    return 0;
}

void tmxc_rt_task_set_deadline(uint32_t task_id, uint64_t deadline_ns) {
    for (uint32_t i = 0; i < TMXC_MAX_THREADS; i++) {
        if (tmxc_rt_scheduler.tasks[i].task_id == task_id) {
            tmxc_rt_scheduler.tasks[i].deadline_ns = deadline_ns;
            tmxc_rt_scheduler.tasks[i].is_realtime = 1;
            return;
        }
    }
}

uint64_t tmxc_rt_task_get_deadline(uint32_t task_id) {
    for (uint32_t i = 0; i < TMXC_MAX_THREADS; i++) {
        if (tmxc_rt_scheduler.tasks[i].task_id == task_id) {
            return tmxc_rt_scheduler.tasks[i].deadline_ns;
        }
    }
    return 0;
}

void tmxc_rt_enable_preemption(uint8_t enable) {
    tmxc_rt_scheduler.preempt_enabled = enable;
}

uint64_t tmxc_rt_get_missed_deadlines(void) {
    return tmxc_rt_scheduler.missed_deadlines;
}

uint64_t tmxc_rt_get_context_switches(void) {
    return tmxc_rt_scheduler.context_switches;
}

void tmxc_rt_load_balancer_init(void) {
    for (int i = 0; i < TMXC_MAX_CPUS; i++) {
        tmxc_rt_load_balancer.cpu_affinity[i] = 0;
        for (int j = 0; j < 100; j++) {
            tmxc_rt_load_balancer.load_history[i][j] = 0;
        }
        tmxc_rt_load_balancer.load_index[i] = 0;
    }
    tmxc_rt_load_balancer.migration_count = 0;
    tmxc_rt_load_balancer.load_balancing_enabled = 1;
}

void tmxc_rt_balance_load(void) {
    uint64_t avg_load = 0;
    
    for (int i = 0; i < TMXC_MAX_CPUS; i++) {
        uint64_t cpu_load = 0;
        for (uint32_t j = 0; j < TMXC_MAX_THREADS; j++) {
            if (tmxc_rt_scheduler.tasks[j].task_id != 0 && tmxc_rt_load_balancer.cpu_affinity[j] == i) {
                cpu_load += tmxc_rt_scheduler.tasks[j].total_run_time;
            }
        }
        tmxc_rt_load_balancer.load_history[i][tmxc_rt_load_balancer.load_index[i]] = cpu_load;
        tmxc_rt_load_balancer.load_index[i] = (tmxc_rt_load_balancer.load_index[i] + 1) % 100;
        avg_load += cpu_load;
    }
    avg_load /= TMXC_MAX_CPUS;
    
    for (int i = 0; i < TMXC_MAX_CPUS; i++) {
        uint64_t current_load = 0;
        for (int j = 0; j < 100; j++) {
            current_load += tmxc_rt_load_balancer.load_history[i][j];
        }
        current_load /= 100;
        
        if (current_load > avg_load * 1.5) {
            for (uint32_t j = 0; j < TMXC_MAX_THREADS; j++) {
                if (tmxc_rt_scheduler.tasks[j].task_id != 0 && tmxc_rt_load_balancer.cpu_affinity[j] == i) {
                    for (int k = 0; k < TMXC_MAX_CPUS; k++) {
                        if (k != i) {
                            uint64_t target_load = 0;
                            for (int l = 0; l < 100; l++) {
                                target_load += tmxc_rt_load_balancer.load_history[k][l];
                            }
                            target_load /= 100;
                            
                            if (target_load < avg_load * 0.8) {
                                tmxc_rt_load_balancer.cpu_affinity[j] = k;
                                tmxc_rt_load_balancer.migration_count++;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void tmxc_rt_set_cpu_affinity(uint32_t task_id, uint32_t cpu_id) {
    if (cpu_id >= TMXC_MAX_CPUS) {
        return;
    }
    
    for (uint32_t i = 0; i < TMXC_MAX_THREADS; i++) {
        if (tmxc_rt_scheduler.tasks[i].task_id == task_id) {
            tmxc_rt_load_balancer.cpu_affinity[i] = cpu_id;
            return;
        }
    }
}

uint32_t tmxc_rt_get_cpu_affinity(uint32_t task_id) {
    for (uint32_t i = 0; i < TMXC_MAX_THREADS; i++) {
        if (tmxc_rt_scheduler.tasks[i].task_id == task_id) {
            return tmxc_rt_load_balancer.cpu_affinity[i];
        }
    }
    return 0;
}
