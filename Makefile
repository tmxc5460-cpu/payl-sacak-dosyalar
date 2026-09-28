# TMXC OS - Microkernel Build System
# ARM64 (AArch64) Architecture
# Copyright (c) 2026 Ödül Ensar Yılmaz. All rights reserved.

# ============================================
# Configuration
# ============================================

# Architecture
ARCH = ARM64

# Build directory
BUILD_DIR = build

# Toolchain configuration
CROSS_PREFIX = aarch64-none-elf-
ARCH_FLAGS = -march=armv8-a -mtune=cortex-a53
LINKER_SCRIPT = linker.ld

# Toolchain binaries
CC = $(CROSS_PREFIX)gcc
AS = $(CROSS_PREFIX)as
LD = $(CROSS_PREFIX)ld
OBJCOPY = $(CROSS_PREFIX)objcopy
OBJDUMP = $(CROSS_PREFIX)objdump

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -O2 \
         -ffreestanding -nostdlib -nostartfiles \
         $(ARCH_FLAGS) \
         -ffunction-sections -fdata-sections \
         -I./kernel

ASFLAGS = $(ARCH_FLAGS)

LDFLAGS = -nostdlib -nostartfiles \
          -T $(LINKER_SCRIPT) \
          --gc-sections

# ============================================
# Source Files
# ============================================

# Boot assembly
BOOT_SOURCES = kernel/boot.S

# Kernel C sources
KERNEL_SOURCES = kernel/kernel_main.c \
                kernel/uart.c \
                kernel/mmu.c \
                kernel/exceptions.c \
                kernel/timer.c \
                kernel/memory.c \
                kernel/process.c

# Combine all sources
ALL_SOURCES = $(BOOT_SOURCES) $(KERNEL_SOURCES)

# Convert sources to object files
BOOT_OBJECTS = $(patsubst kernel/%.S,$(BUILD_DIR)/kernel/%.o,$(BOOT_SOURCES))
KERNEL_OBJECTS = $(patsubst kernel/%.c,$(BUILD_DIR)/kernel/%.o,$(KERNEL_SOURCES))
ALL_OBJECTS = $(BOOT_OBJECTS) $(KERNEL_OBJECTS)

# Output files
KERNEL_ELF = $(BUILD_DIR)/tmxc_os.elf
KERNEL_BIN = $(BUILD_DIR)/tmxc_os.bin

# ============================================
# Build Targets
# ============================================

.PHONY: all clean run help dirs

# Default target
all: dirs $(KERNEL_BIN)

# Create build directories
dirs:
	@mkdir -p $(BUILD_DIR)/kernel

# Compile boot assembly
$(BUILD_DIR)/kernel/%.o: kernel/%.S dirs
	@echo "AS      $<"
	@$(AS) $(ASFLAGS) $< -o $@

# Compile kernel C sources
$(BUILD_DIR)/kernel/%.o: kernel/%.c dirs
	@echo "CC      $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link kernel
$(KERNEL_ELF): $(ALL_OBJECTS)
	@echo "LD      $@"
	@$(LD) $(LDFLAGS) $^ -o $@

# Create binary
$(KERNEL_BIN): $(KERNEL_ELF)
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O binary $< $@
	@echo "Build complete: $@"

# Run in QEMU
run: $(KERNEL_BIN)
	@echo "Starting QEMU..."
	@qemu-system-aarch64 -M virt -m 512M -kernel $(KERNEL_BIN) -serial stdio

# Run with debug
debug: $(KERNEL_BIN)
	@echo "Starting QEMU with debug..."
	@qemu-system-aarch64 -M virt -m 512M -kernel $(KERNEL_BIN) -serial stdio -s -S

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete"

# Show kernel information
info: $(KERNEL_ELF)
	@echo "Kernel information:"
	@$(OBJDUMP) -h $<
	@echo "Kernel size: $$(stat -f%z $(KERNEL_BIN) 2>/dev/null || stat -c%s $(KERNEL_BIN)) bytes"

# Help target
help:
	@echo "TMXC OS Microkernel Build System"
	@echo "================================"
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build kernel binary (default)"
	@echo "  run       - Build and run in QEMU"
	@echo "  debug     - Build and run in QEMU with debug support"
	@echo "  clean     - Remove build artifacts"
	@echo "  info      - Show kernel information"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  ARCH=$(ARCH)"
	@echo "  Build directory: $(BUILD_DIR)"
	@echo "  Toolchain prefix: $(CROSS_PREFIX)"
	@echo ""
	@echo "Examples:"
	@echo "  make"
	@echo "  make run"
	@echo "  make clean run"
