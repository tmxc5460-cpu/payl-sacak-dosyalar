/*
 * TMXC OS - Process Manager Header
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Process manager for ARM64 architecture.
 * Provides process creation, scheduling, and management.
 */

#ifndef TMXC_PROCESS_H
#define TMXC_PROCESS_H

#include <stdint.h>

/*
 * ============================================================================
 * Process Configuration
 * ============================================================================
 */

/**
 * @brief Maximum number of processes
 */
#define TMXC_MAX_PROCESSES 64

/**
 * @brief Process name length
 */
#define TMXC_PROCESS_NAME_LEN 32

/*
 * ============================================================================
 * Process States
 * ============================================================================
 */

typedef enum {
    TMXC_PROCESS_CREATED = 0,
    TMXC_PROCESS_READY = 1,
    TMXC_PROCESS_RUNNING = 2,
    TMXC_PROCESS_BLOCKED = 3,
    TMXC_PROCESS_TERMINATED = 4
} tmxc_process_state_t;

/*
 * ============================================================================
 * Process Structure
 * ============================================================================
 */

typedef struct {
    uint64_t pid;                    /* Process ID */
    uint64_t ppid;                   /* Parent Process ID */
    tmxc_process_state_t state;      /* Process state */
    uint64_t priority;               /* Scheduling priority */
    
    /* Virtual memory */
    uint64_t pgd;                    /* Page Global Directory */
    uint64_t code_start;             /* Code segment start */
    uint64_t code_end;               /* Code segment end */
    uint64_t data_start;             /* Data segment start */
    uint64_t data_end;               /* Data segment end */
    uint64_t heap_start;             /* Heap start */
    uint64_t heap_end;               /* Heap end */
    uint64_t stack_top;              /* Stack top */
    
    /* Registers */
    uint64_t x0_x30[31];             /* General purpose registers */
    uint64_t sp;                     /* Stack pointer */
    uint64_t pc;                     /* Program counter */
    uint64_t cpsr;                   /* Current program status */
    
    /* Scheduling */
    uint64_t time_slice;             /* Time slice */
    uint64_t cpu_time;               /* CPU time used */
    
    /* Process info */
    char name[TMXC_PROCESS_NAME_LEN]; /* Process name */
    uint8_t valid;                   /* Valid flag */
} tmxc_process_t;

/*
 * ============================================================================
 * Process Manager Function Declarations
 * ============================================================================
 */

/**
 * @brief Initialize process manager
 * 
 * Initializes the process manager and scheduler.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_process_init(void);

/**
 * @brief Create a new process
 * 
 * Creates a new process with the specified name and entry point.
 * 
 * @param name Process name
 * @param entry Entry point address
 * @param stack Stack address
 * @return uint64_t Process ID, or 0 on failure
 */
uint64_t tmxc_process_create(const char* name, uint64_t entry, uint64_t stack);

/**
 * @brief Terminate a process
 * 
 * Terminates the specified process.
 * 
 * @param pid Process ID
 * @return 0 on success, negative error code on failure
 */
int tmxc_process_terminate(uint64_t pid);

/**
 * @brief Get current process
 * 
 * Returns the current running process.
 * 
 * @return tmxc_process_t* Pointer to current process, or NULL if none
 */
tmxc_process_t* tmxc_process_get_current(void);

/**
 * @brief Get process by PID
 * 
 * Returns the process with the specified PID.
 * 
 * @param pid Process ID
 * @return tmxc_process_t* Pointer to process, or NULL if not found
 */
tmxc_process_t* tmxc_process_get_by_pid(uint64_t pid);

/**
 * @brief Schedule next process
 * 
 * Schedules the next process to run.
 * This function does not return (context switch).
 */
void tmxc_schedule(void);

/**
 * @brief Yield CPU
 * 
 * Yields the CPU to the next process.
 */
void tmxc_yield(void);

#endif /* TMXC_PROCESS_H */
