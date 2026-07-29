/*
 * TMXC_OS - Preemptive Scheduler (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file scheduler.h
 * @brief Timer-driven preemptive scheduler for task switching
 * 
 * This scheduler implements preemptive multitasking using timer interrupts.
 * It eliminates busy-waiting loops by using IRQ-driven context switching.
 * 
 * ARMv8-A Architecture Reference Manual:
 * - Uses the Generic Timer (CNTVCTL_EL0, CNTV_CVAL_EL0)
 * - Timer interrupts are delivered via the GIC
 * - Context switching saves/restores general-purpose registers
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#ifndef TMXC_SCHEDULER_H
#define TMXC_SCHEDULER_H

#include <stdint.h>

/*
 * ============================================================================
 * Scheduler Configuration
 * ============================================================================
 */

/**
 * @brief Maximum number of tasks
 */
#define TMXC_MAX_TASKS          16

/**
 * @brief Task stack size
 * 
 * Each task gets its own stack.
 */
#define TMXC_TASK_STACK_SIZE    (64 * 1024)  /* 64KB */

/**
 * @brief Scheduler time slice
 * 
 * Each task runs for this many milliseconds before being preempted.
 */
#define TMXC_TIME_SLICE_MS      10

/**
 * @brief Timer frequency
 * 
 * System timer frequency in Hz.
 */
#define TMXC_TIMER_FREQUENCY    1000  /* 1kHz for 1ms resolution */

/*
 * ============================================================================
 * Task States
 * ============================================================================
 */

/**
 * @brief Task state enumeration
 */
typedef enum {
    TMXC_TASK_READY = 0,      /* Task is ready to run */
    TMXC_TASK_RUNNING,         /* Task is currently running */
    TMXC_TASK_BLOCKED,         /* Task is blocked (waiting for I/O, etc.) */
    TMXC_TASK_TERMINATED      /* Task has terminated */
} tmxc_task_state_t;

/*
 * ============================================================================
 * Task Control Block (TCB)
 * ============================================================================
 */

/**
 * @brief Task Control Block
 * 
 * Contains all information needed to manage a task.
 */
typedef struct {
    uint64_t tid;              /* Task ID */
    tmxc_task_state_t state;   /* Task state */
    uint64_t priority;        /* Task priority (higher = more important) */
    
    /* CPU context */
    uint64_t sp;              /* Stack pointer */
    uint64_t pc;              /* Program counter */
    uint64_t cpsr;            /* Current program status register */
    
    /* General purpose registers */
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint64_t x11;
    uint64_t x12;
    uint64_t x13;
    uint64_t x14;
    uint64_t x15;
    uint64_t x16;
    uint64_t x17;
    uint64_t x18;
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29;             /* Frame pointer */
    uint64_t x30;             /* Link register */
    
    /* Stack information */
    uint64_t stack_base;      /* Base of task stack */
    uint64_t stack_size;      /* Size of task stack */
    
    /* Task function */
    void (*entry)(void);      /* Task entry point */
    void* arg;                /* Task argument */
    
    /* Time accounting */
    uint64_t runtime;         /* Total runtime in microseconds */
    uint64_t last_switch;     /* Time of last context switch */
} tmxc_task_t;

/*
 * ============================================================================
 * Scheduler State
 * ============================================================================
 */

/**
 * @brief Scheduler state structure
 */
typedef struct {
    tmxc_task_t* current_task; /* Currently running task */
    tmxc_task_t tasks[TMXC_MAX_TASKS];  /* Task pool */
    uint64_t task_count;      /* Number of tasks */
    uint64_t next_tid;        /* Next task ID to assign */
    uint64_t timer_enabled;   /* Timer enabled flag */
} tmxc_scheduler_t;

/*
 * ============================================================================
 * Scheduler Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize the scheduler
 * 
 * Sets up the scheduler data structures and configures the timer.
 * 
 * Steps:
 * 1. Initialize task pool
 * 2. Configure generic timer
 * 3. Enable timer interrupts
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_scheduler_init(void);

/**
 * @brief Start the scheduler
 * 
 * Begins task scheduling.
 * Enables the timer and performs the first context switch.
 * This function never returns under normal operation.
 */
void tmxc_scheduler_start(void) __attribute__((noreturn));

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
int tmxc_task_create(void (*entry)(void), void* arg, uint64_t priority);

/**
 * @brief Yield the CPU
 * 
 * Voluntarily give up the CPU to another task.
 * Called by a task when it wants to allow other tasks to run.
 */
void tmxc_task_yield(void);

/**
 * @brief Terminate the current task
 * 
 * Terminates the currently running task and schedules the next task.
 * This function does not return.
 */
void tmxc_task_exit(void) __attribute__((noreturn));

/**
 * @brief Block the current task
 * 
 * Blocks the current task and schedules the next task.
 * The task will remain blocked until explicitly unblocked.
 */
void tmxc_task_block(void);

/**
 * @brief Unblock a task
 * 
 * Unblocks a blocked task, making it ready to run.
 * 
 * @param tid Task ID to unblock
 * @return 0 on success, negative error code on failure
 */
int tmxc_task_unblock(uint64_t tid);

/**
 * @brief Get the current task ID
 * 
 * @return uint64_t Current task ID
 */
uint64_t tmxc_task_get_current(void);

/**
 * @brief Get task state
 * 
 * @param tid Task ID
 * @return tmxc_task_state_t Task state
 */
tmxc_task_state_t tmxc_task_get_state(uint64_t tid);

/**
 * @brief Set task priority
 * 
 * @param tid Task ID
 * @param priority New priority
 * @return 0 on success, negative error code on failure
 */
int tmxc_task_set_priority(uint64_t tid, uint64_t priority);

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
 * @param interval_ms Timer interval in milliseconds
 */
void tmxc_timer_configure(uint64_t interval_ms);

/**
 * @brief Enable the timer
 * 
 * Enables the generic timer interrupt.
 */
void tmxc_timer_enable(void);

/**
 * @brief Disable the timer
 * 
 * Disables the generic timer interrupt.
 */
void tmxc_timer_disable(void);

/**
 * @brief Timer interrupt handler
 * 
 * Called when the timer interrupt fires.
 * Triggers a context switch to the next task.
 */
void tmxc_timer_handler(void);

/*
 * ============================================================================
 * Context Switching
 * ============================================================================
 */

/**
 * @brief Save current task context
 * 
 * Saves the current CPU context to the current task's TCB.
 * Called by the timer interrupt handler.
 */
void tmxc_context_save(void);

/**
 * @brief Restore next task context
 * 
 * Restores the CPU context from the next task's TCB.
 * Called by the timer interrupt handler.
 */
void tmxc_context_restore(void);

/**
 * @brief Perform context switch
 * 
 * Switches from the current task to the next ready task.
 * Implements round-robin scheduling with priority support.
 */
void tmxc_context_switch(void);

#endif /* TMXC_SCHEDULER_H */
