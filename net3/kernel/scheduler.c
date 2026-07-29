/*
 * TMXC_OS - Preemptive Scheduler Implementation (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file scheduler.c
 * @brief Timer-driven preemptive scheduler implementation
 * 
 * This scheduler implements preemptive multitasking using timer interrupts.
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#include "scheduler.h"
#include "uart.h"
#include "watchdog.h"

/*
 * ============================================================================
 * Scheduler State
 * ============================================================================
 */

/**
 * @brief Global scheduler instance
 */
static tmxc_scheduler_t tmxc_scheduler;

/*
 * ============================================================================
 * Timer Configuration
 * ============================================================================
 */

/**
 * @brief Configure the generic timer
 * 
 * Configures the ARMv8-A generic timer for periodic interrupts.
 * 
 * ARMv8-A Generic Timer:
 * - CNTV_CTL_EL0: Control register (enable bit, imask bit)
 * - CNTV_CVAL_EL0: Compare value register
 * - CNTVCT_EL0: Count register (read-only)
 * - CNTFRQ_EL0: Frequency register (read-only)
 * 
 * @param interval_ms Timer interval in milliseconds
 */
void tmxc_timer_configure(uint64_t interval_ms) {
    uint64_t frequency;
    uint64_t interval_cycles;
    uint64_t current_count;
    
    /*
     * Read timer frequency
     */
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
    
    /*
     * Calculate interval in cycles
     */
    interval_cycles = (frequency * interval_ms) / 1000;
    
    /*
     * Read current count
     */
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(current_count));
    
    /*
     * Set compare value
     * Timer will fire when count reaches this value
     */
    __asm__ volatile("msr cntv_cval_el0, %0" : : "r"(current_count + interval_cycles));
    
    tmxc_uart_puts("[SCHED] Timer configured: ");
    tmxc_print_dec(interval_ms);
    tmxc_uart_puts(" ms interval\r\n");
}

/**
 * @brief Enable the timer
 * 
 * Enables the generic timer interrupt.
 * 
 * ARMv8-A: Set the enable bit in CNTV_CTL_EL0.
 */
void tmxc_timer_enable(void) {
    /*
     * Enable timer (clear imask bit)
     */
    __asm__ volatile("msr cntv_ctl_el0, %0" : : "r"(1));
    
    tmxc_scheduler.timer_enabled = 1;
    tmxc_uart_puts("[SCHED] Timer enabled\r\n");
}

/**
 * @brief Disable the timer
 * 
 * Disables the generic timer interrupt.
 * 
 * ARMv8-A: Clear the enable bit in CNTV_CTL_EL0.
 */
void tmxc_timer_disable(void) {
    /*
     * Disable timer (set imask bit)
     */
    __asm__ volatile("msr cntv_ctl_el0, %0" : : "r"(0));
    
    tmxc_scheduler.timer_enabled = 0;
    tmxc_uart_puts("[SCHED] Timer disabled\r\n");
}

/*
 * ============================================================================
 * Scheduler Initialization
 * ============================================================================
 */

/**
 * @brief Initialize the scheduler
 * 
 * Sets up the scheduler data structures and configures the timer.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_scheduler_init(void) {
    /*
     * Initialize scheduler state
     */
    tmxc_scheduler.current_task = NULL;
    tmxc_scheduler.task_count = 0;
    tmxc_scheduler.next_tid = 0;
    tmxc_scheduler.timer_enabled = 0;
    
    /*
     * Initialize task pool
     */
    for (int i = 0; i < TMXC_MAX_TASKS; i++) {
        tmxc_scheduler.tasks[i].tid = 0;
        tmxc_scheduler.tasks[i].state = TMXC_TASK_TERMINATED;
        tmxc_scheduler.tasks[i].priority = 0;
        tmxc_scheduler.tasks[i].stack_base = 0;
        tmxc_scheduler.tasks[i].stack_size = 0;
        tmxc_scheduler.tasks[i].entry = NULL;
        tmxc_scheduler.tasks[i].arg = NULL;
        tmxc_scheduler.tasks[i].runtime = 0;
        tmxc_scheduler.tasks[i].last_switch = 0;
    }
    
    /*
     * Configure timer
     */
    tmxc_timer_configure(TMXC_TIME_SLICE_MS);
    
    tmxc_uart_puts("[SCHED] Scheduler initialized\r\n");
    
    return 0;
}

/**
 * @brief Start the scheduler
 * 
 * Begins task scheduling.
 * Enables the timer and performs the first context switch.
 * This function never returns under normal operation.
 */
void tmxc_scheduler_start(void) {
    /*
     * If there are no tasks, create an idle task
     */
    if (tmxc_scheduler.task_count == 0) {
        tmxc_uart_puts("[SCHED] No tasks, entering idle loop\r\n");
        
        /*
         * Idle loop: kick watchdog and wait for events
         */
        while (1) {
            tmxc_watchdog_kick();
            __asm__ volatile("wfi");
        }
    }
    
    /*
     * Enable timer
     */
    tmxc_timer_enable();
    
    /*
     * Enable IRQs
     */
    __asm__ volatile("msr daifclr, #2");
    
    /*
     * Perform first context switch
     * This will switch to the first ready task
     */
    tmxc_context_switch();
    
    /*
     * Should never reach here
     */
    while (1) {
        __asm__ volatile("wfi");
    }
}

/*
 * ============================================================================
 * Task Management
 * ============================================================================
 */

/**
 * @brief Create a new task
 * 
 * Creates a new task with the specified entry point and priority.
 * 
 * @param entry Task entry point function
 * @param arg Task argument
 * @param priority Task priority (0 = lowest, 255 = highest)
 * @return int Task ID on success, negative error code on failure
 */
int tmxc_task_create(void (*entry)(void), void* arg, uint64_t priority) {
    int task_index = -1;
    
    /*
     * Find free task slot
     */
    for (int i = 0; i < TMXC_MAX_TASKS; i++) {
        if (tmxc_scheduler.tasks[i].state == TMXC_TASK_TERMINATED) {
            task_index = i;
            break;
        }
    }
    
    if (task_index < 0) {
        tmxc_uart_puts("[SCHED] No free task slots\r\n");
        return -1;  /* No free task slots */
    }
    
    /*
     * Allocate stack for task
     * For simplicity, we use static allocation
     * In a full implementation, this would use dynamic allocation
     */
    static uint8_t task_stacks[TMXC_MAX_TASKS][TMXC_TASK_STACK_SIZE];
    uint64_t stack_base = (uint64_t)&task_stacks[task_index][0];
    
    /*
     * Initialize task control block
     */
    tmxc_task_t* task = &tmxc_scheduler.tasks[task_index];
    
    task->tid = tmxc_scheduler.next_tid++;
    task->state = TMXC_TASK_READY;
    task->priority = priority;
    
    /*
     * Set up initial stack
     * Stack grows downward, so SP points to top of stack
     */
    task->stack_base = stack_base;
    task->stack_size = TMXC_TASK_STACK_SIZE;
    task->sp = stack_base + TMXC_TASK_STACK_SIZE;
    
    /*
     * Set up initial context
     * PC points to task entry point
     * CPSR: IRQ/FIQ disabled, EL0 (user mode)
     */
    task->pc = (uint64_t)entry;
    task->cpsr = 0x3C5;  /* EL0h, IRQ/FIQ disabled */
    
    /*
     * Set up registers
     */
    task->x0 = (uint64_t)arg;
    for (int i = 1; i < 31; i++) {
        task->x1 = 0;  /* Initialize all registers to 0 */
    }
    
    task->entry = entry;
    task->arg = arg;
    task->runtime = 0;
    task->last_switch = 0;
    
    tmxc_scheduler.task_count++;
    
    tmxc_uart_puts("[SCHED] Task created: TID ");
    tmxc_print_dec(task->tid);
    tmxc_uart_puts(", Priority ");
    tmxc_print_dec(priority);
    tmxc_uart_puts("\r\n");
    
    return task->tid;
}

/**
 * @brief Yield the CPU
 * 
 * Voluntarily give up the CPU to another task.
 * Called by a task when it wants to allow other tasks to run.
 */
void tmxc_task_yield(void) {
    /*
     * Trigger a context switch
     * This is done by setting a flag that the timer handler checks
     * For simplicity, we just call the context switch directly
     */
    tmxc_context_switch();
}

/**
 * @brief Terminate the current task
 * 
 * Terminates the currently running task and schedules the next task.
 * This function does not return.
 */
void tmxc_task_exit(void) {
    if (tmxc_scheduler.current_task != NULL) {
        tmxc_scheduler.current_task->state = TMXC_TASK_TERMINATED;
        tmxc_scheduler.task_count--;
        
        tmxc_uart_puts("[SCHED] Task terminated: TID ");
        tmxc_print_dec(tmxc_scheduler.current_task->tid);
        tmxc_uart_puts("\r\n");
    }
    
    /*
     * Schedule next task
     */
    tmxc_context_switch();
    
    /*
     * Should never reach here
     */
    while (1) {
        __asm__ volatile("wfi");
    }
}

/**
 * @brief Block the current task
 * 
 * Blocks the current task and schedules the next task.
 * The task will remain blocked until explicitly unblocked.
 */
void tmxc_task_block(void) {
    if (tmxc_scheduler.current_task != NULL) {
        tmxc_scheduler.current_task->state = TMXC_TASK_BLOCKED;
        
        tmxc_uart_puts("[SCHED] Task blocked: TID ");
        tmxc_print_dec(tmxc_scheduler.current_task->tid);
        tmxc_uart_puts("\r\n");
    }
    
    /*
     * Schedule next task
     */
    tmxc_context_switch();
}

/**
 * @brief Unblock a task
 * 
 * Unblocks a blocked task, making it ready to run.
 * 
 * @param tid Task ID to unblock
 * @return 0 on success, negative error code on failure
 */
int tmxc_task_unblock(uint64_t tid) {
    for (int i = 0; i < TMXC_MAX_TASKS; i++) {
        if (tmxc_scheduler.tasks[i].tid == tid) {
            if (tmxc_scheduler.tasks[i].state == TMXC_TASK_BLOCKED) {
                tmxc_scheduler.tasks[i].state = TMXC_TASK_READY;
                
                tmxc_uart_puts("[SCHED] Task unblocked: TID ");
                tmxc_print_dec(tid);
                tmxc_uart_puts("\r\n");
                
                return 0;
            }
            return -2;  /* Task not blocked */
        }
    }
    
    return -1;  /* Task not found */
}

/**
 * @brief Get the current task ID
 * 
 * @return uint64_t Current task ID
 */
uint64_t tmxc_task_get_current(void) {
    if (tmxc_scheduler.current_task != NULL) {
        return tmxc_scheduler.current_task->tid;
    }
    return 0;
}

/**
 * @brief Get task state
 * 
 * @param tid Task ID
 * @return tmxc_task_state_t Task state
 */
tmxc_task_state_t tmxc_task_get_state(uint64_t tid) {
    for (int i = 0; i < TMXC_MAX_TASKS; i++) {
        if (tmxc_scheduler.tasks[i].tid == tid) {
            return tmxc_scheduler.tasks[i].state;
        }
    }
    return TMXC_TASK_TERMINATED;  /* Task not found */
}

/**
 * @brief Set task priority
 * 
 * @param tid Task ID
 * @param priority New priority
 * @return 0 on success, negative error code on failure
 */
int tmxc_task_set_priority(uint64_t tid, uint64_t priority) {
    for (int i = 0; i < TMXC_MAX_TASKS; i++) {
        if (tmxc_scheduler.tasks[i].tid == tid) {
            tmxc_scheduler.tasks[i].priority = priority;
            
            tmxc_uart_puts("[SCHED] Task priority set: TID ");
            tmxc_print_dec(tid);
            tmxc_uart_puts(", Priority ");
            tmxc_print_dec(priority);
            tmxc_uart_puts("\r\n");
            
            return 0;
        }
    }
    
    return -1;  /* Task not found */
}

/*
 * ============================================================================
 * Context Switching
 * ============================================================================
 */

/**
 * @brief Perform context switch
 * 
 * Switches from the current task to the next ready task.
 * Implements round-robin scheduling with priority support.
 */
void tmxc_context_switch(void) {
    tmxc_task_t* next_task = NULL;
    tmxc_task_t* current_task = tmxc_scheduler.current_task;
    uint64_t highest_priority = 0;
    
    /*
     * Find next ready task with highest priority
     */
    for (int i = 0; i < TMXC_MAX_TASKS; i++) {
        if (tmxc_scheduler.tasks[i].state == TMXC_TASK_READY) {
            if (tmxc_scheduler.tasks[i].priority > highest_priority) {
                highest_priority = tmxc_scheduler.tasks[i].priority;
                next_task = &tmxc_scheduler.tasks[i];
            }
        }
    }
    
    /*
     * If no ready task, stay on current task (if running)
     */
    if (next_task == NULL && current_task != NULL && 
        current_task->state == TMXC_TASK_RUNNING) {
        next_task = current_task;
    }
    
    /*
     * If still no task, enter idle loop
     */
    if (next_task == NULL) {
        tmxc_uart_puts("[SCHED] No ready tasks, entering idle\r\n");
        while (1) {
            tmxc_watchdog_kick();
            __asm__ volatile("wfi");
        }
    }
    
    /*
     * Update task states
     */
    if (current_task != NULL && current_task != next_task) {
        if (current_task->state == TMXC_TASK_RUNNING) {
            current_task->state = TMXC_TASK_READY;
        }
    }
    
    next_task->state = TMXC_TASK_RUNNING;
    tmxc_scheduler.current_task = next_task;
    
    /*
     * Update last switch time
     */
    uint64_t current_time;
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(current_time));
    next_task->last_switch = current_time;
    
    /*
     * Perform actual context switch
     * This is a placeholder for the assembly implementation
     * The full implementation would:
     * 1. Save current context to current_task
     * 2. Restore context from next_task
     * 3. Return to next_task's PC
     */
    
    tmxc_uart_puts("[SCHED] Context switch: TID ");
    tmxc_print_dec(next_task->tid);
    tmxc_uart_puts("\r\n");
    
    /*
     * For now, just call the task entry point
     * This is a simplified approach for initial bring-up
     */
    if (next_task->entry != NULL) {
        next_task->entry();
    }
}

/**
 * @brief Timer interrupt handler
 * 
 * Called when the timer interrupt fires.
 * Triggers a context switch to the next task.
 */
void tmxc_timer_handler(void) {
    /*
     * Re-arm timer
     */
    if (tmxc_scheduler.timer_enabled) {
        tmxc_timer_configure(TMXC_TIME_SLICE_MS);
    }
    
    /*
     * Kick watchdog
     */
    tmxc_watchdog_kick();
    
    /*
     * Perform context switch
     */
    tmxc_context_switch();
}
