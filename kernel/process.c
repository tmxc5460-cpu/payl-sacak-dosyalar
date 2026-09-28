/*
 * TMXC OS - Process Manager Implementation
 * Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.
 * 
 * This file is part of TMXC OS and is protected under GPL v3.
 * 
 * Process manager implementation for ARM64 architecture.
 */

#include "process.h"
#include "memory.h"
#include "uart.h"

/*
 * ============================================================================
 * Process Manager State
 * ============================================================================
 */

/**
 * @brief Process table
 */
static tmxc_process_t tmxc_processes[TMXC_MAX_PROCESSES];

/**
 * @brief Current process
 */
static tmxc_process_t* tmxc_current_process;

/**
 * @brief Next PID to allocate
 */
static uint64_t tmxc_next_pid = 1;

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
 * @brief String copy
 */
static void tmxc_strcpy(char* dst, const char* src) {
    while (*src != '\0') {
        *dst++ = *src++;
    }
    *dst = '\0';
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

/*
 * ============================================================================
 * Process Manager Implementation
 * ============================================================================
 */

/**
 * @brief Initialize process manager
 * 
 * Initializes the process manager and scheduler.
 */
int tmxc_process_init(void) {
    /*
     * Step 1: Clear process table
     */
    tmxc_memset(tmxc_processes, 0, sizeof(tmxc_processes));
    
    /*
     * Step 2: Set current process to NULL
     */
    tmxc_current_process = NULL;
    
    /*
     * Step 3: Reset next PID
     */
    tmxc_next_pid = 1;
    
    tmxc_uart_puts("[PROCESS] Process manager initialized\r\n");
    
    return 0;
}

/**
 * @brief Create a new process
 * 
 * Creates a new process with the specified name and entry point.
 */
uint64_t tmxc_process_create(const char* name, uint64_t entry, uint64_t stack) {
    /*
     * Step 1: Find free process slot
     */
    uint64_t slot = 0;
    for (uint64_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (!tmxc_processes[i].valid) {
            slot = i;
            break;
        }
    }
    
    if (slot == TMXC_MAX_PROCESSES) {
        /*
         * No free slots
         */
        return 0;
    }
    
    /*
     * Step 2: Allocate PID
     */
    uint64_t pid = tmxc_next_pid++;
    
    /*
     * Step 3: Initialize process structure
     */
    tmxc_process_t* process = &tmxc_processes[slot];
    tmxc_memset(process, 0, sizeof(tmxc_process_t));
    
    process->pid = pid;
    process->ppid = 0;  /* No parent for now */
    process->state = TMXC_PROCESS_CREATED;
    process->priority = 0;  /* Default priority */
    
    /*
     * Set process name
     */
    if (name != NULL) {
        tmxc_strcpy(process->name, name);
    }
    
    /*
     * Set entry point and stack
     */
    process->pc = entry;
    process->sp = stack;
    process->cpsr = 0x3C5;  /* EL1h, IRQ/FIQ disabled */
    
    /*
     * Allocate memory for process (placeholder)
     */
    process->pgd = 0;  /* TODO: Allocate page table */
    process->code_start = 0;
    process->code_end = 0;
    process->data_start = 0;
    process->data_end = 0;
    process->heap_start = 0;
    process->heap_end = 0;
    process->stack_top = stack;
    
    /*
     * Set scheduling parameters
     */
    process->time_slice = 10;  /* 10ms time slice */
    process->cpu_time = 0;
    
    /*
     * Mark process as valid
     */
    process->valid = 1;
    
    /*
     * Set process state to ready
     */
    process->state = TMXC_PROCESS_READY;
    
    tmxc_uart_puts("[PROCESS] Process created: ");
    tmxc_uart_puts(name ? name : "unknown");
    tmxc_uart_puts(" (PID: ");
    tmxc_print_dec(pid);
    tmxc_uart_puts(")\r\n");
    
    return pid;
}

/**
 * @brief Terminate a process
 * 
 * Terminates the specified process.
 */
int tmxc_process_terminate(uint64_t pid) {
    /*
     * Step 1: Find process
     */
    tmxc_process_t* process = tmxc_process_get_by_pid(pid);
    if (process == NULL) {
        return -1;
    }
    
    /*
     * Step 2: Mark process as terminated
     */
    process->state = TMXC_PROCESS_TERMINATED;
    process->valid = 0;
    
    tmxc_uart_puts("[PROCESS] Process terminated (PID: ");
    tmxc_print_dec(pid);
    tmxc_uart_puts(")\r\n");
    
    return 0;
}

/**
 * @brief Get current process
 * 
 * Returns the current running process.
 */
tmxc_process_t* tmxc_process_get_current(void) {
    return tmxc_current_process;
}

/**
 * @brief Get process by PID
 * 
 * Returns the process with the specified PID.
 */
tmxc_process_t* tmxc_process_get_by_pid(uint64_t pid) {
    for (uint64_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (tmxc_processes[i].valid && tmxc_processes[i].pid == pid) {
            return &tmxc_processes[i];
        }
    }
    return NULL;
}

/**
 * @brief Schedule next process
 * 
 * Schedules the next process to run.
 * This function does not return (context switch).
 */
void tmxc_schedule(void) {
    /*
     * Step 1: Find next ready process
     */
    tmxc_process_t* next = NULL;
    for (uint64_t i = 0; i < TMXC_MAX_PROCESSES; i++) {
        if (tmxc_processes[i].valid && tmxc_processes[i].state == TMXC_PROCESS_READY) {
            next = &tmxc_processes[i];
            break;
        }
    }
    
    if (next == NULL) {
        /*
         * No ready processes, halt
         */
        tmxc_uart_puts("[PROCESS] No ready processes, halting\r\n");
        while (1) {
            __asm__ volatile("wfe");
        }
    }
    
    /*
     * Step 2: Save current process context (if any)
     */
    if (tmxc_current_process != NULL) {
        tmxc_current_process->state = TMXC_PROCESS_READY;
    }
    
    /*
     * Step 3: Set next process as current
     */
    tmxc_current_process = next;
    tmxc_current_process->state = TMXC_PROCESS_RUNNING;
    
    tmxc_uart_puts("[PROCESS] Switching to PID: ");
    tmxc_print_dec(tmxc_current_process->pid);
    tmxc_uart_puts("\r\n");
    
    /*
     * Step 4: Context switch to next process
     * TODO: Implement actual context switch
     * For now, just jump to the entry point
     */
    
    /*
     * Jump to process entry point
     * This is a placeholder - real context switch would restore registers
     */
    __asm__ volatile(
        "mov sp, %0\n"
        "mov x30, %1\n"
        "eret"
        :
        : "r"(tmxc_current_process->sp), "r"(tmxc_current_process->pc)
    );
}

/**
 * @brief Yield CPU
 * 
 * Yields the CPU to the next process.
 */
void tmxc_yield(void) {
    /*
     * Call scheduler
     */
    tmxc_schedule();
}
