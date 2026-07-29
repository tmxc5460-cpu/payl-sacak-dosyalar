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
#include "tmxc_kernel.h"

extern void tmxc_uart_init(void);
extern void tmxc_uart_putc(char c);
extern void tmxc_uart_puts(const char* str);
extern void tmxc_gic_init(void);
extern void tmxc_gic_enable_irq(uint32_t irq);
extern void tmxc_timer_init(void);
extern void tmxc_timer_delay_ms(uint32_t ms);
extern void tmxc_memory_init(void);
extern void tmxc_mmu_init(void);
extern void tmxc_process_init(void);
extern void tmxc_scheduler_init(void);
extern void tmxc_scheduler_start(void);
extern void tmxc_ipc_init(void);
extern void tmxc_irq_init(void);
extern void tmxc_graphics_init(void);
extern void tmxc_touch_init(void);
extern void tmxc_sensor_init(void);
extern void tmxc_security_init(void);
extern void tmxc_faceid_init(void);
extern void tmxc_hal_config_init(void);
extern void tmxc_stealthview_init(void);
extern void tmxc_lockscreen_init(void);
extern void tmxc_menu_init(void);
extern void tmxc_audio_init(void);
extern void tmxc_camera_init(void);
extern void tmxc_network_init(void);
extern void tmxc_power_init(void);
extern void tmxc_power_monitor_battery(void);
extern void tmxc_deadlock_detector_init(void);
extern void tmxc_shell_init(void);
extern void tmxc_modem_init(void);
extern void tmxc_dialer_init(void);
extern void tmxc_calculator_init(void);
extern void tmxc_compass_init(void);
extern void tmxc_bluetooth_init(void);
extern void tmxc_share_init(void);
extern void tmxc_nfc_wallet_init(void);
extern void tmxc_promesh_init(void);
extern void tmxc_power_pro_init(void);
extern void tmxc_ecosystem_init(void);
extern void tmxc_failsafe_init(void);
extern void tmxc_security_shield_init(void);
extern void tmxc_network_init(void);
extern void tmxc_5g_uplift_init(void);
extern void tmxc_lockscreen_init(void);
extern void tmxc_dvfs_init(void);
extern void tmxc_security_check_tampering(void);
extern void tmxc_game_mode_init(void);
extern void tmxc_thermal_monitor(void);

extern void tmxc_microkernel_init(void);
extern void tmxc_microkernel_boot(void);
extern void tmxc_rt_scheduler_init(void);
extern void tmxc_memory_optimization_init(void);
extern void tmxc_mesh_init(void);
extern void tmxc_quantum_crypto_init(void);
extern void tmxc_6g_satellite_init(void);
extern void tmxc_cyber_shield_init(void);
extern void tmxc_biometric_encryption_init(void);
extern void tmxc_advanced_sensors_init(void);
extern void tmxc_charging_system_init(void);
extern void tmxc_dynamic_island_init(void);
extern void tmxc_ai_assistant_init(void);
extern void tmxc_ecosystem_init(void);

extern void tmxc_neural_recovery_init(void);
extern void tmxc_neural_save_state(void);
extern void tmxc_neural_restore_state(void);
extern void tmxc_context_audio_init(void);
extern void tmxc_bio_feedback_init(void);
extern void tmxc_hardware_prediction_init(void);
extern void tmxc_holographic_display_init(void);

extern void tmxc_sandbox_init(void);
extern void tmxc_proactive_ai_init(void);
extern void tmxc_ram_compression_init(void);
extern void tmxc_panic_button_init(void);
extern void tmxc_thermal_sync_init(void);

extern void tmxc_ghost_mode_init(void);
extern void tmxc_neural_firewall_init(void);
extern void tmxc_hyper_boot_init(void);
extern void tmxc_secure_clipboard_init(void);
extern void tmxc_modular_device_manager_init(void);

extern void tmxc_sonic_protocol_init(void);
extern void tmxc_biometric_signature_init(void);
extern void tmxc_hardware_recovery_init(void);
extern void tmxc_energy_critical_mode_init(void);

extern void tmxc_sentinel_vault_init(void);
extern void tmxc_zerotrace_ramfs_init(void);
extern void tmxc_beyaz_kus_ai_init(void);

static uint8_t tmxc_kernel_initialized = 0;
static uint64_t tmxc_boot_time = 0;
static uint64_t tmxc_uptime = 0;

static void tmxc_print_hex(uint64_t value) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[17];
    buffer[16] = '\0';
    
    for (int i = 15; i >= 0; i--) {
        buffer[i] = hex_chars[value & 0xF];
        value >>= 4;
    }
    
    tmxc_uart_puts(buffer);
}

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

static void tmxc_kernel_banner(void) {
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("       TMXC OS - Revolutionary Mobile\r\n");
    tmxc_uart_puts("       Microkernel Architecture\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("Version: ");
    tmxc_print_dec(TMXC_VERSION_MAJOR);
    tmxc_uart_putc('.');
    tmxc_print_dec(TMXC_VERSION_MINOR);
    tmxc_uart_putc('.');
    tmxc_print_dec(TMXC_VERSION_PATCH);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("Architecture: ARM64 (AArch64)\r\n");
    tmxc_uart_puts("Platform: Mobile Devices\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("\r\n");
}

static void tmxc_hardware_detect(void) {
    uint64_t midr;
    __asm__ volatile("mrs %0, midr_el1" : "=r"(midr));
    
    uint64_t mpidr;
    __asm__ volatile("mrs %0, mpidr_el1" : "=r"(mpidr));
    
    uint64_t cpu_count = 1;
    uint32_t affinity = (mpidr >> 8) & 0xFF;
    
    tmxc_uart_puts("[HW] CPU Detected: Implementer ");
    tmxc_print_hex((midr >> 24) & 0xFF);
    tmxc_uart_puts(", Variant ");
    tmxc_print_hex((midr >> 20) & 0xF);
    tmxc_uart_puts(", Architecture ");
    tmxc_print_hex((midr >> 16) & 0xF);
    tmxc_uart_puts(", Part ");
    tmxc_print_hex((midr >> 4) & 0xFFF);
    tmxc_uart_puts(", Revision ");
    tmxc_print_hex(midr & 0xF);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("[HW] CPU Affinity: ");
    tmxc_print_dec(affinity);
    tmxc_uart_puts("\r\n");
    
    uint64_t freq = tmxc_get_frequency();
    tmxc_uart_puts("[HW] Timer Frequency: ");
    tmxc_print_dec(freq);
    tmxc_uart_puts(" Hz\r\n");
    
    uint64_t cntfrq;
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq));
    tmxc_uart_puts("[HW] CNTFRQ_EL0: ");
    tmxc_print_dec(cntfrq);
    tmxc_uart_puts(" Hz\r\n");
    
    uint64_t current_el = tmxc_get_current_el();
    tmxc_uart_puts("[HW] Current Exception Level: EL");
    tmxc_print_dec(current_el);
    tmxc_uart_puts("\r\n");
    
    uint64_t id_aa64mmfr0;
    __asm__ volatile("mrs %0, id_aa64mmfr0_el1" : "=r"(id_aa64mmfr0));
    tmxc_uart_puts("[HW] Memory Model: ");
    tmxc_print_hex(id_aa64mmfr0 & 0xF);
    tmxc_uart_puts("\r\n");
    
    uint64_t id_aa64pfr0;
    __asm__ volatile("mrs %0, id_aa64pfr0_el1" : "=r"(id_aa64pfr0));
    tmxc_uart_puts("[HW] EL0 Execution: ");
    tmxc_print_hex((id_aa64pfr0 >> 0) & 0xF);
    tmxc_uart_puts("\r\n");
}

static void tmxc_memory_info(void) {
    tmxc_uart_puts("[MEM] Total Memory: ");
    tmxc_print_dec(TMXC_MEMORY_SIZE / (1024 * 1024));
    tmxc_uart_puts(" MB\r\n");
    
    tmxc_uart_puts("[MEM] Page Size: ");
    tmxc_print_dec(TMXC_PAGE_SIZE);
    tmxc_uart_puts(" bytes\r\n");
    
    tmxc_uart_puts("[MEM] Kernel Base: 0x");
    tmxc_print_hex(TMXC_KERNEL_BASE);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("[MEM] Physical Base: 0x");
    tmxc_print_hex(TMXC_PHYS_MEMORY_BASE);
    tmxc_uart_puts("\r\n");
}

static void tmxc_irq_handler_wrapper(uint32_t irq, void* data) {
    switch (irq) {
        case 1:
            tmxc_scheduler_tick();
            break;
        case 27:
            tmxc_touch_interaction_start();
            break;
        case 28:
            tmxc_touch_interaction_end();
            break;
        default:
            break;
    }
}

static void tmxc_exception_handler(uint64_t esr, uint64_t elr, uint64_t far) {
    uint32_t ec = (esr >> 26) & 0x3F;
    uint32_t iss = esr & 0x1FFFFFF;
    
    tmxc_uart_puts("\r\n[EXCEPTION] EC: ");
    tmxc_print_dec(ec);
    tmxc_uart_puts(", ISS: ");
    tmxc_print_dec(iss);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("[EXCEPTION] ELR: 0x");
    tmxc_print_hex(elr);
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("[EXCEPTION] FAR: 0x");
    tmxc_print_hex(far);
    tmxc_uart_puts("\r\n");
    
    while (1) {
        tmxc_wfi();
    }
}

static void tmxc_setup_exception_vectors(void) {
    extern void tmxc_exception_vectors(void);
    
    uint64_t vbar = (uint64_t)&tmxc_exception_vectors;
    __asm__ volatile("msr vbar_el1, %0" : : "r"(vbar));
    
    __asm__ volatile("isb");
    
    tmxc_uart_puts("[INIT] Exception vectors configured\r\n");
}

void tmxc_kernel_main(void) {
    tmxc_boot_time = tmxc_get_cycle_count();
    
    tmxc_uart_init();
    tmxc_kernel_banner();
    
    tmxc_uart_puts("[INIT] TMXC OS Kernel Starting...\r\n");
    
    tmxc_hardware_detect();
    tmxc_memory_info();
    
    tmxc_uart_puts("[INIT] Initializing Memory Manager...\r\n");
    tmxc_memory_init();
    
    tmxc_uart_puts("[INIT] Initializing MMU...\r\n");
    tmxc_mmu_init();
    
    tmxc_uart_puts("[INIT] Setting up Exception Vectors...\r\n");
    tmxc_setup_exception_vectors();
    
    tmxc_uart_puts("[INIT] Initializing GIC (Interrupt Controller)...\r\n");
    tmxc_gic_init();
    
    tmxc_uart_puts("[INIT] Initializing Timer...\r\n");
    tmxc_timer_init();
    
    tmxc_uart_puts("[INIT] Initializing IRQ Subsystem...\r\n");
    tmxc_irq_init();
    tmxc_irq_register(1, tmxc_irq_handler_wrapper, NULL);
    tmxc_gic_enable_irq(1);
    
    tmxc_uart_puts("[INIT] Enabling IRQs...\r\n");
    tmxc_enable_irqs();
    
    tmxc_uart_puts("[INIT] Initializing Process Manager...\r\n");
    tmxc_process_init();
    
    tmxc_uart_puts("[INIT] Initializing Microkernel...\r\n");
    tmxc_microkernel_init();
    
    tmxc_uart_puts("[INIT] Initializing Real-Time Scheduler...\r\n");
    tmxc_rt_scheduler_init();
    
    tmxc_uart_puts("[INIT] Initializing Memory Optimization...\r\n");
    tmxc_memory_optimization_init();
    
    tmxc_uart_puts("[INIT] Initializing Deadlock Detector...\r\n");
    tmxc_deadlock_detector_init();
    
    tmxc_uart_puts("[INIT] Initializing IPC...\r\n");
    tmxc_ipc_init();
    
    tmxc_uart_puts("[INIT] Initializing Security Subsystem...\r\n");
    tmxc_security_init();
    
    tmxc_uart_puts("[INIT] Initializing Cyber Shield...\r\n");
    tmxc_cyber_shield_init();
    
    tmxc_uart_puts("[INIT] Initializing Biometric Encryption...\r\n");
    tmxc_biometric_encryption_init();
    
    tmxc_uart_puts("[INIT] Initializing FaceID...\r\n");
    tmxc_faceid_init();
    
    tmxc_uart_puts("[INIT] Initializing StealthView Privacy Filter...\r\n");
    tmxc_stealthview_init();
    
    tmxc_uart_puts("[INIT] Initializing Graphics Subsystem...\r\n");
    tmxc_graphics_init();
    
    tmxc_uart_puts("[INIT] Initializing Touchscreen Driver...\r\n");
    tmxc_touch_init();
    tmxc_irq_register(27, tmxc_irq_handler_wrapper, NULL);
    tmxc_irq_register(28, tmxc_irq_handler_wrapper, NULL);
    tmxc_gic_enable_irq(27);
    tmxc_gic_enable_irq(28);
    
    tmxc_uart_puts("[INIT] Initializing Sensor Driver...\r\n");
    tmxc_sensor_init();
    
    tmxc_uart_puts("[INIT] Initializing Advanced Sensors...\r\n");
    tmxc_advanced_sensors_init();
    
    tmxc_uart_puts("[INIT] Initializing Fast Charging System...\r\n");
    tmxc_charging_system_init();
    
    tmxc_uart_puts("[INIT] Initializing Audio Subsystem...\r\n");
    tmxc_audio_init();
    
    tmxc_uart_puts("[INIT] Initializing Camera Driver...\r\n");
    tmxc_camera_init();
    
    tmxc_uart_puts("[INIT] Initializing Network Subsystem...\r\n");
    tmxc_network_init();
    
    tmxc_uart_puts("[INIT] Initializing P2P Mesh Network...\r\n");
    tmxc_mesh_init();
    
    tmxc_uart_puts("[INIT] Initializing Quantum-Safe Cryptography...\r\n");
    tmxc_quantum_crypto_init();
    
    tmxc_uart_puts("[INIT] Initializing 6G Satellite Communication...\r\n");
    tmxc_6g_satellite_init();
    
    tmxc_uart_puts("[INIT] Initializing Lock Screen...\r\n");
    tmxc_lockscreen_init();
    
    tmxc_uart_puts("[INIT] Initializing Menu System...\r\n");
    tmxc_menu_init();
    
    tmxc_uart_puts("[INIT] Initializing Dynamic Island...\r\n");
    tmxc_dynamic_island_init();
    
    tmxc_uart_puts("[INIT] Initializing AI Assistant...\r\n");
    tmxc_ai_assistant_init();
    
    tmxc_uart_puts("[INIT] Initializing Neural-State Recovery...\r\n");
    tmxc_neural_recovery_init();
    
    tmxc_uart_puts("[INIT] Initializing Context-Aware Audio...\r\n");
    tmxc_context_audio_init();
    
    tmxc_uart_puts("[INIT] Initializing Bio-Feedback Integration...\r\n");
    tmxc_bio_feedback_init();
    
    tmxc_uart_puts("[INIT] Initializing Hardware Prediction...\r\n");
    tmxc_hardware_prediction_init();
    
    tmxc_uart_puts("[INIT] Initializing Holographic Display...\r\n");
    tmxc_holographic_display_init();
    
    tmxc_uart_puts("[INIT] Initializing Process Isolation (Sandbox)...\r\n");
    tmxc_sandbox_init();
    
    tmxc_uart_puts("[INIT] Initializing Proactive AI Engine...\r\n");
    tmxc_proactive_ai_init();
    
    tmxc_uart_puts("[INIT] Initializing RAM Compression Engine...\r\n");
    tmxc_ram_compression_init();
    
    tmxc_uart_puts("[INIT] Initializing Panic Button (Secret Mode)...\r\n");
    tmxc_panic_button_init();
    
    tmxc_uart_puts("[INIT] Initializing Thermal Sync Driver...\r\n");
    tmxc_thermal_sync_init();
    
    tmxc_uart_puts("[INIT] Initializing Ghost Mode (Kernel Security)...\r\n");
    tmxc_ghost_mode_init();
    
    tmxc_uart_puts("[INIT] Initializing Neural Firewall (Beyaz Kuş)...\r\n");
    tmxc_neural_firewall_init();
    
    tmxc_uart_puts("[INIT] Initializing Hyper-Fast Boot (RAM-Snapshot)...\r\n");
    tmxc_hyper_boot_init();
    
    tmxc_uart_puts("[INIT] Initializing Quantum-Encrypted Clipboard...\r\n");
    tmxc_secure_clipboard_init();
    
    tmxc_uart_puts("[INIT] Initializing Modular Device Manager...\r\n");
    tmxc_modular_device_manager_init();
    
    tmxc_uart_puts("[INIT] Initializing Sonic Data Protocol...\r\n");
    tmxc_sonic_protocol_init();
    
    tmxc_uart_puts("[INIT] Initializing Biometric Signature Engine...\r\n");
    tmxc_biometric_signature_init();
    
    tmxc_uart_puts("[INIT] Initializing Hardware Recovery Trigger...\r\n");
    tmxc_hardware_recovery_init();
    
    tmxc_uart_puts("[INIT] Initializing Energy Critical Mode...\r\n");
    tmxc_energy_critical_mode_init();
    
    tmxc_uart_puts("[INIT] Initializing Sentinel Vault (1000-Layer Encryption)...\r\n");
    tmxc_sentinel_vault_init();
    
    tmxc_uart_puts("[INIT] Initializing Zero-Trace RAM File System...\r\n");
    tmxc_zerotrace_ramfs_init();
    
    tmxc_uart_puts("[INIT] Initializing AI Syscall Anomaly Detector...\r\n");
    tmxc_beyaz_kus_ai_init();
    
    tmxc_uart_puts("[INIT] Initializing Shell...\r\n");
    tmxc_shell_init();
    
    tmxc_uart_puts("[INIT] Initializing Modem...\r\n");
    tmxc_modem_init();
    
    tmxc_uart_puts("[INIT] Initializing Dialer...\r\n");
    tmxc_dialer_init();
    
    tmxc_uart_puts("[INIT] Initializing Calculator...\r\n");
    tmxc_calculator_init();
    
    tmxc_uart_puts("[INIT] Initializing Compass...\r\n");
    tmxc_compass_init();
    
    tmxc_uart_puts("[INIT] Initializing Bluetooth...\r\n");
    tmxc_bluetooth_init();
    
    tmxc_uart_puts("[INIT] Initializing File Share...\r\n");
    tmxc_share_init();
    
    tmxc_uart_puts("[INIT] Initializing NFC Wallet...\r\n");
    tmxc_nfc_wallet_init();
    
    tmxc_uart_puts("[INIT] Initializing ProMesh...\r\n");
    tmxc_promesh_init();
    
    tmxc_uart_puts("[INIT] Initializing Power Pro...\r\n");
    tmxc_power_pro_init();
    
    tmxc_uart_puts("[INIT] Initializing Ecosystem...\r\n");
    tmxc_ecosystem_init();
    
    tmxc_uart_puts("[INIT] Initializing Fail-Safe...\r\n");
    tmxc_failsafe_init();
    
    tmxc_uart_puts("[INIT] Initializing Security Shield...\r\n");
    tmxc_security_shield_init();
    
    tmxc_uart_puts("[INIT] Initializing Virus Scanner...\r\n");
    tmxc_virus_scanner_init();
    
    tmxc_uart_puts("[INIT] Initializing Leak Detector...\r\n");
    tmxc_leak_detector_init();
    
    tmxc_uart_puts("[INIT] Initializing Security Callbacks...\r\n");
    tmxc_security_callbacks_init();
    
    tmxc_uart_puts("[INIT] Initializing Security Panel...\r\n");
    tmxc_security_panel_init();
    
    tmxc_uart_puts("[INIT] Initializing TMXC Init Process...\r\n");
    tmxc_init_process_init();
    tmxc_init_process_start();
    
    tmxc_uart_puts("[INIT] Initializing TMXC Camera...\r\n");
    tmxc_camera_init();
    
    tmxc_uart_puts("[INIT] Initializing TMXC Gallery...\r\n");
    tmxc_gallery_init();
    
    tmxc_uart_puts("[INIT] Initializing TMXC File Manager...\r\n");
    tmxc_filemanager_init();
    
    tmxc_uart_puts("[INIT] Initializing Kernel Watchdog...\r\n");
    tmxc_kernel_watchdog_init();
    tmxc_kernel_watchdog_start();
    
    tmxc_uart_puts("[INIT] Initializing Network...\r\n");
    tmxc_network_init();
    
    tmxc_uart_puts("[INIT] Initializing 5G Uplift...\r\n");
    tmxc_5g_uplift_init();
    
    tmxc_uart_puts("[INIT] Initializing Lock Screen...\r\n");
    tmxc_lockscreen_init();
    
    tmxc_uart_puts("[INIT] Initializing DVFS...\r\n");
    tmxc_dvfs_init();
    
    tmxc_uart_puts("[INIT] Checking for tampering...\r\n");
    tmxc_security_check_tampering();
    
    tmxc_uart_puts("[INIT] Initializing Extreme Game Engine...\r\n");
    tmxc_game_mode_init();
    
    tmxc_uart_puts("[INIT] Initializing Power Management...\r\n");
    tmxc_power_init();
    
    tmxc_uart_puts("[INIT] Kernel Initialization Complete\r\n");
    tmxc_uart_puts("[INIT] Boot Time: ");
    uint64_t boot_cycles = tmxc_get_cycle_count() - tmxc_boot_time;
    uint64_t boot_ms = (boot_cycles * 1000) / tmxc_get_frequency();
    tmxc_print_dec(boot_ms);
    tmxc_uart_puts(" ms\r\n");
    
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("       TMXC OS Ready\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("\r\n");
    
    tmxc_kernel_initialized = 1;
    
    tmxc_uart_puts("[SCHED] Starting Scheduler...\r\n");
    tmxc_scheduler_start();
    
    while (1) {
        tmxc_power_monitor_battery();
        tmxc_thermal_monitor();
        tmxc_wfi();
    }
}

uint64_t tmxc_get_uptime(void) {
    if (!tmxc_kernel_initialized) {
        return 0;
    }
    
    uint64_t current_cycles = tmxc_get_cycle_count();
    tmxc_uptime = (current_cycles - tmxc_boot_time) / tmxc_get_frequency();
    return tmxc_uptime;
}

void tmxc_kernel_panic(const char* message) {
    tmxc_disable_irqs();
    tmxc_disable_fiqs();
    
    tmxc_uart_puts("\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("           KERNEL PANIC\r\n");
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("PANIC: ");
    tmxc_uart_puts(message);
    tmxc_uart_puts("\r\n");
    
    uint64_t elr;
    __asm__ volatile("mrs %0, elr_el1" : "=r"(elr));
    tmxc_uart_puts("ELR: 0x");
    tmxc_print_hex(elr);
    tmxc_uart_puts("\r\n");
    
    uint64_t esr;
    __asm__ volatile("mrs %0, esr_el1" : "=r"(esr));
    tmxc_uart_puts("ESR: 0x");
    tmxc_print_hex(esr);
    tmxc_uart_puts("\r\n");
    
    uint64_t far;
    __asm__ volatile("mrs %0, far_el1" : "=r"(far));
    tmxc_uart_puts("FAR: 0x");
    tmxc_print_hex(far);
    tmxc_uart_puts("\r\n");
    
    tmxc_uart_puts("========================================\r\n");
    tmxc_uart_puts("System Halted\r\n");
    tmxc_uart_puts("========================================\r\n");
    
    while (1) {
        tmxc_wfi();
    }
}

void tmxc_assert(const char* expr, const char* file, uint32_t line) {
    tmxc_uart_puts("\r\n[ASSERTION FAILED] ");
    tmxc_uart_puts(expr);
    tmxc_uart_puts(" at ");
    tmxc_uart_puts(file);
    tmxc_uart_puts(":");
    tmxc_print_dec(line);
    tmxc_uart_puts("\r\n");
    
    tmxc_kernel_panic("Assertion failed");
}
