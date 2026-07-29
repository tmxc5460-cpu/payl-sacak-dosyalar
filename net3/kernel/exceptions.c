/*
 * TMXC_OS - Exception Vector Table Implementation (AArch64)
 * Copyright (c) 2024 TMXC_OS Development Team
 * 
 * @file exceptions.c
 * @brief Exception Vector Table and exception handlers implementation
 * 
 * This module implements the exception vector table and handlers for EL1.
 * 
 * Zero-Dependency: This code does not use any standard C library.
 */

#include "exceptions.h"
#include "uart.h"
#include "sensor_hal.h"
#include "mmu.h"

/*
 * ============================================================================
 * Exception Vector Table (Assembly)
 * ============================================================================
 */

/**
 * @brief Exception vector table
 * 
 * The exception vector table must be aligned to 2KB.
 * It contains 16 vectors (4 exception types × 4 exception sources).
 * Each vector is 128 bytes (32 instructions).
 * 
 * The table is defined in assembly to ensure proper alignment and
 * to save/restore processor state correctly.
 */
__attribute__((aligned(2048)))
extern void tmxc_exception_vectors(void);

/*
 * ============================================================================
 * Exception Handler Implementations
 * ============================================================================
 */

/**
 * @brief Synchronous exception handler
 * 
 * Handles synchronous exceptions such as:
 * - Instruction aborts (page faults, permission faults)
 * - Data aborts (page faults, permission faults)
 * - Trapped system instructions (MRS/MSR, system calls)
 * - Illegal execution state
 * 
 * ARMv8-A: Synchronous exceptions occur due to the current instruction.
 * The ESR_EL1 register provides details about the exception.
 * 
 * @param ctx Exception context (saved processor state)
 */
void tmxc_handle_sync(tmxc_exception_context_t* ctx) {
    uint32_t ec = (ctx->esr >> ESR_EC_SHIFT) & 0x3F;
    uint32_t iss = ctx->esr & ESR_ISS_MASK;
    
    tmxc_uart_puts("\r\n[EXCEPTION] Synchronous Exception\r\n");
    tmxc_uart_puts("[EXCEPTION] EC: ");
    tmxc_print_hex(ec);
    tmxc_uart_puts(", ISS: ");
    tmxc_print_hex(iss);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("[EXCEPTION] PC: 0x");
    tmxc_print_hex(ctx->pc);
    tmxc_uart_puts("\r\n");
    
    /*
     * Handle different exception classes
     */
    switch (ec) {
        case 0x00:  /* Unknown reason */
            tmxc_uart_puts("[EXCEPTION] Unknown exception\r\n");
            break;
            
        case 0x20:  /* Instruction abort from lower level */
        case 0x21:  /* Instruction abort from same level */
            tmxc_uart_puts("[EXCEPTION] Instruction abort\r\n");
            tmxc_uart_puts("[EXCEPTION] FAR: 0x");
            tmxc_print_hex(ctx->far);
            tmxc_uart_puts("\r\n");
            break;
            
        case 0x24:  /* Data abort from lower level */
        case 0x25:  /* Data abort from same level */
            tmxc_uart_puts("[EXCEPTION] Data abort\r\n");
            tmxc_uart_puts("[EXCEPTION] FAR: 0x");
            tmxc_print_hex(ctx->far);
            tmxc_uart_puts("\r\n");
            
            /* Check if write or read */
            if (ctx->esr & ESR_WnR) {
                tmxc_uart_puts("[EXCEPTION] Write fault\r\n");
            } else {
                tmxc_uart_puts("[EXCEPTION] Read fault\r\n");
            }
            break;
            
        case 0x15:  /* Trapped system register access */
            tmxc_uart_puts("[EXCEPTION] Trapped system register access\r\n");
            break;
            
        case 0x22:  /* Trapped FP/NEON */
            tmxc_uart_puts("[EXCEPTION] Trapped FP/NEON instruction\r\n");
            break;
            
        case 0x26:  /* Trapped AArch32 execution */
            tmxc_uart_puts("[EXCEPTION] Trapped AArch32 execution\r\n");
            break;
            
        case 0x30:  /* Breakpoint from lower level */
        case 0x31:  /* Breakpoint from same level */
            tmxc_uart_puts("[EXCEPTION] Breakpoint hit\r\n");
            break;
            
        default:
            tmxc_uart_puts("[EXCEPTION] Unhandled exception class\r\n");
            break;
    }
    
    /*
     * For now, halt on synchronous exceptions
     * In a full implementation, this would:
     * - Send signal to user process (if from EL0)
     * - Attempt to recover (e.g., demand paging)
     * - Call kernel panic if unrecoverable
     */
    tmxc_uart_puts("[EXCEPTION] Halting system\r\n");
    while (1) {
        __asm__ volatile("wfi");
    }
}

/**
 * @brief IRQ handler
 * 
 * Handles normal interrupt requests.
 * Dispatches to the appropriate device driver based on interrupt ID.
 * 
 * ARMv8-A: IRQs are masked by the I bit in PSTATE.
 * The interrupt controller (GIC) provides the interrupt ID.
 * 
 * @param ctx Exception context (saved processor state)
 */
void tmxc_handle_irq(tmxc_exception_context_t* ctx) {
    (void)ctx;  /* Context not used for simple IRQ handling */
    
    /*
     * Read interrupt ID from interrupt controller
     * For QEMU virt, we use the GIC (Generic Interrupt Controller)
     * 
     * This is a placeholder for the full GIC driver implementation.
     * The full implementation would:
     * 1. Read interrupt acknowledge register (IAR)
     * 2. Dispatch to appropriate handler based on interrupt ID
     * 3. Write end of interrupt register (EOIR)
     */
    
    tmxc_uart_puts("[IRQ] Interrupt received\r\n");
    
    /*
     * For now, just acknowledge and return
     */
}

/**
 * @brief FIQ handler
 * 
 * Handles fast interrupt requests.
 * Used for high-priority interrupts (e.g., watchdog timer).
 * 
 * ARMv8-A: FIQs are masked by the F bit in PSTATE.
 * FIQs have higher priority than IRQs.
 * 
 * @param ctx Exception context (saved processor state)
 */
void tmxc_handle_fiq(tmxc_exception_context_t* ctx) {
    (void)ctx;  /* Context not used for simple FIQ handling */
    
    tmxc_uart_puts("[FIQ] Fast interrupt received\r\n");
    
    /*
     * For now, just acknowledge and return
     * In a full implementation, this would:
     * 1. Check if watchdog triggered
     * 2. Take appropriate action (e.g., reset)
     */
}

/**
 * @brief SError handler
 * 
 * Handles system errors such as:
 * - External aborts (bus errors from peripherals)
 * - Parity errors (ECC errors)
 * - Asynchronous data aborts
 * 
 * ARMv8-A: SErrors are asynchronous exceptions that occur due to
 * external events, not the current instruction.
 * 
 * @param ctx Exception context (saved processor state)
 */
void tmxc_handle_serror(tmxc_exception_context_t* ctx) {
    uint32_t ec = (ctx->esr >> ESR_EC_SHIFT) & 0x3F;
    uint32_t iss = ctx->esr & ESR_ISS_MASK;
    
    tmxc_uart_puts("\r\n[EXCEPTION] System Error (SError)\r\n");
    tmxc_uart_puts("[EXCEPTION] EC: ");
    tmxc_print_hex(ec);
    tmxc_uart_puts(", ISS: ");
    tmxc_print_hex(iss);
    tmxc_uart_puts("\r\n");
    
    /*
     * Check if external abort
     */
    if (ctx->esr & ESR_EA) {
        tmxc_uart_puts("[EXCEPTION] External abort\r\n");
    }
    
    /*
     * SErrors are typically fatal
     * Halt the system
     */
    tmxc_uart_puts("[EXCEPTION] Halting system\r\n");
    while (1) {
        __asm__ volatile("wfi");
    }
}

/*
 * ============================================================================
 * Exception Control Functions
 * ============================================================================
 */

/**
 * @brief Enable IRQs
 * 
 * Unmasks IRQ exceptions by clearing the I bit in DAIF.
 */
void tmxc_enable_irqs(void) {
    __asm__ volatile("msr daifclr, #2");
}

/**
 * @brief Disable IRQs
 * 
 * Masks IRQ exceptions by setting the I bit in DAIF.
 */
void tmxc_disable_irqs(void) {
    __asm__ volatile("msr daifset, #2");
}

/**
 * @brief Enable FIQs
 * 
 * Unmasks FIQ exceptions by clearing the F bit in DAIF.
 */
void tmxc_enable_fiqs(void) {
    __asm__ volatile("msr daifclr, #1");
}

/**
 * @brief Disable FIQs
 * 
 * Masks FIQ exceptions by setting the F bit in DAIF.
 */
void tmxc_disable_fiqs(void) {
    __asm__ volatile("msr daifset, #1");
}

/*
 * ============================================================================
 * Exception Printing Utility
 * ============================================================================
 */

/**
 * @brief Print exception information
 * 
 * Outputs exception details to UART for debugging.
 * 
 * @param ctx Exception context (saved processor state)
 * @param type Exception type
 */
void tmxc_print_exception(tmxc_exception_context_t* ctx, tmxc_exception_type_t type) {
    const char* type_str;
    
    switch (type) {
        case TMXC_EXCEPTION_SYNC:
            type_str = "Synchronous";
            break;
        case TMXC_EXCEPTION_IRQ:
            type_str = "IRQ";
            break;
        case TMXC_EXCEPTION_FIQ:
            type_str = "FIQ";
            break;
        case TMXC_EXCEPTION_SERROR:
            type_str = "SError";
            break;
        default:
            type_str = "Unknown";
            break;
    }
    
    tmxc_uart_puts("\r\n[EXCEPTION] Type: ");
    tmxc_uart_puts(type_str);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("[EXCEPTION] PC: 0x");
    tmxc_print_hex(ctx->pc);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("[EXCEPTION] SP: 0x");
    tmxc_print_hex(ctx->sp);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("[EXCEPTION] CPSR: 0x");
    tmxc_print_hex(ctx->cpsr);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("[EXCEPTION] ESR: 0x");
    tmxc_print_hex(ctx->esr);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("[EXCEPTION] FAR: 0x");
    tmxc_print_hex(ctx->far);
    tmxc_uart_puts("\r\n");
}

/*
 * ============================================================================
 * Exception Initialization
 * ============================================================================
 */

/**
 * @brief Initialize exception vector table
 * 
 * Configures the exception vector table for EL1.
 * Sets VBAR_EL1 to point to the exception vector table.
 * 
 * @return 0 on success, negative error code on failure
 */
int tmxc_exceptions_init(void) {
    /*
     * Set VBAR_EL1 to point to the exception vector table
     * The table is defined in boot.S
     */
    __asm__ volatile("msr vbar_el1, %0" : : "r"((uint64_t)&tmxc_exception_vectors));
    
    /*
     * Ensure the change takes effect
     */
    __asm__ volatile("isb");
    
    tmxc_uart_puts("[EXCEPTION] Exception vector table configured\r\n");
    
    return 0;
}

/*
 * ============================================================================
 * System Call Implementation
 * ============================================================================
 */

/**
 * @brief System call handler
 * 
 * Handles system calls from userspace.
 * System calls are triggered via the SVC instruction.
 * The syscall number is in x8, arguments in x0-x5.
 */
void tmxc_handle_syscall(tmxc_exception_context_t* ctx) {
    uint64_t syscall_number = ctx->x8;
    int64_t result = -1;
    
    /*
     * Dispatch to appropriate system call
     */
    switch (syscall_number) {
        case TMXC_SYSCALL_READ_SENSOR:
            result = tmxc_syscall_read_sensor(ctx->x0, ctx->x1, ctx->x2);
            break;
            
        case TMXC_SYSCALL_QUERY_STATUS:
            result = tmxc_syscall_query_status(ctx->x0, ctx->x1, ctx->x2);
            break;
            
        case TMXC_SYSCALL_EXEC_APP:
            result = tmxc_syscall_exec_app(ctx->x0, ctx->x1, ctx->x2);
            break;
            
        case TMXC_SYSCALL_EXIT:
            /*
             * Process exit - placeholder
             */
            result = 0;
            break;
            
        case TMXC_SYSCALL_YIELD:
            /*
             * Process yield - placeholder
             */
            result = 0;
            break;
            
        default:
            tmxc_uart_puts("[SYSCALL] Unknown syscall: ");
            tmxc_print_hex(syscall_number);
            tmxc_uart_puts("\r\n");
            result = -1;
            break;
    }
    
    /*
     * Return result in x0
     */
    ctx->x0 = result;
}

/**
 * @brief Read sensor system call
 */
int tmxc_syscall_read_sensor(uint64_t sensor_type, uint64_t data_buffer, uint64_t buffer_size) {
    /*
     * Validate parameters
     */
    if (buffer_size < sizeof(tmxc_sensor_data_t)) {
        return -1;
    }
    
    /*
     * Read sensor data
     */
    tmxc_sensor_data_t data;
    int result = tmxc_sensor_hal_read((tmxc_sensor_type_t)sensor_type, &data);
    if (result != 0) {
        return result;
    }
    
    /*
     * Copy data to userspace buffer
     * In a full implementation, this would validate the userspace pointer
     * and use copy_to_userspace
     */
    tmxc_sensor_data_t* user_buffer = (tmxc_sensor_data_t*)data_buffer;
    *user_buffer = data;
    
    return sizeof(tmxc_sensor_data_t);
}

/**
 * @brief Query status system call
 */
int tmxc_syscall_query_status(uint64_t query_type, uint64_t result_buffer, uint64_t buffer_size) {
    /*
     * Validate parameters
     */
    if (buffer_size < sizeof(uint32_t)) {
        return -1;
    }
    
    uint32_t status = 0;
    
    /*
     * Handle different query types
     */
    switch (query_type) {
        case 0:  /* Sensor status */
            {
                uint8_t sensor_status;
                if (tmxc_sensor_hal_get_status((tmxc_sensor_type_t)(query_type >> 8), &sensor_status) == 0) {
                    status = sensor_status;
                }
            }
            break;
            
        case 1:  /* Kernel status */
            status = 0x01;  /* Kernel running */
            break;
            
        case 2:  /* Memory status */
            status = 0x01;  /* Memory OK */
            break;
            
        default:
            return -1;
    }
    
    /*
     * Copy status to userspace buffer
     */
    uint32_t* user_buffer = (uint32_t*)result_buffer;
    *user_buffer = status;
    
    return 0;
}

/**
 * @brief Execute application system call
 */
int tmxc_syscall_exec_app(uint64_t app_path, uint64_t argv, uint64_t envp) {
    /*
     * Validate parameters
     */
    if (app_path == 0) {
        return -1;
    }
    
    /*
     * Placeholder: In a full implementation, this would:
     * 1. Validate the application path
     * 2. Load the application binary
     * 3. Create a new process with isolated memory space
     * 4. Set up the process context
     * 5. Start the process
     * 
     * For now, we return a placeholder PID
     */
    
    tmxc_uart_puts("[SYSCALL] exec_app: ");
    tmxc_uart_puts((const char*)app_path);
    tmxc_uart_puts("\r\n");
    
    /*
     * Return placeholder PID
     */
    return 1;  /* PID 1 */
}
