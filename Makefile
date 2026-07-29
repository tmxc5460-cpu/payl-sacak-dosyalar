# TMXC OS - Professional Build System
# Supports ARM64 and x86_64 architectures
# Author: TMXC OS Development Team

# ============================================
# Configuration
# ============================================

# Architecture selection (ARM64 or x86_64)
ARCH ?= ARM64

# Build directories
BUILD_DIR = build
ISO_DIR = $(BUILD_DIR)/iso

# Toolchain configuration
ifeq ($(ARCH),ARM64)
    CROSS_PREFIX = aarch64-none-elf-
    ARCH_FLAGS = -march=armv8-a -mtune=cortex-a53
    LINKER_SCRIPT = linker_arm64.ld
    BOOTLOADER_SRC = boot/arm64_bootloader.S
else ifeq ($(ARCH),x86_64)
    CROSS_PREFIX = x86_64-elf-
    ARCH_FLAGS = -march=x86-64 -mtune=generic
    LINKER_SCRIPT = linker_x86_64.ld
    BOOTLOADER_SRC = boot/x86_64_bootloader.S
else
    $(error Unsupported architecture: $(ARCH). Use ARM64 or x86_64)
endif

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
         -I./net/kernel \
         -I./security \
         -I./drivers \
         -I./apps \
         -I./ui

ASFLAGS = $(ARCH_FLAGS)

LDFLAGS = -nostdlib -nostartfiles \
          -T $(LINKER_SCRIPT) \
          --gc-sections

# ============================================
# Source Files
# ============================================

# Bootloader
BOOTLOADER_OBJ = $(BUILD_DIR)/boot/bootloader.o

# Kernel sources
KERNEL_SOURCES = $(wildcard net/kernel/*.c) \
                 $(wildcard net/kernel/core/*.c) \
                 $(wildcard net/kernel/*.c)

# Security sources
SECURITY_SOURCES = $(wildcard security/*.c) \
                   $(wildcard security/biometric/*.c) \
                   $(wildcard security/license/*.c)

# Driver sources
DRIVER_SOURCES = $(wildcard drivers/*.c) \
                 $(wildcard drivers/audio/*.c) \
                 $(wildcard drivers/display/*.c) \
                 $(wildcard drivers/sensors/*.c)

# UI sources
UI_SOURCES = $(wildcard ui/*.c) \
             $(wildcard ui/adaptive/*.c) \
             $(wildcard ui/dynamic/*.c)

# App sources
APP_SOURCES = $(wildcard apps/*.c)

# Combine all sources
ALL_SOURCES = $(KERNEL_SOURCES) $(SECURITY_SOURCES) $(DRIVER_SOURCES) $(UI_SOURCES) $(APP_SOURCES)

# Convert sources to object files
ALL_OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(ALL_SOURCES))

# Output files
KERNEL_ELF = $(BUILD_DIR)/tmxc_os.elf
KERNEL_BIN = $(BUILD_DIR)/tmxc_os.bin
ISO_IMAGE = $(BUILD_DIR)/tmxc_os.iso

# ============================================
# Build Targets
# ============================================

.PHONY: all clean iso run help dirs

# Default target
all: dirs $(KERNEL_BIN)

# Create build directories
dirs:
	@mkdir -p $(BUILD_DIR)/boot
	@mkdir -p $(BUILD_DIR)/net/kernel/core
	@mkdir -p $(BUILD_DIR)/security/biometric
	@mkdir -p $(BUILD_DIR)/security/license
	@mkdir -p $(BUILD_DIR)/drivers/audio
	@mkdir -p $(BUILD_DIR)/drivers/display
	@mkdir -p $(BUILD_DIR)/drivers/sensors
	@mkdir -p $(BUILD_DIR)/ui/adaptive
	@mkdir -p $(BUILD_DIR)/ui/dynamic
	@mkdir -p $(BUILD_DIR)/apps

# Compile bootloader
$(BOOTLOADER_OBJ): $(BOOTLOADER_SRC) dirs
	@echo "AS      $<"
	@$(AS) $(ASFLAGS) $< -o $@

# Compile C sources
$(BUILD_DIR)/%.o: %.c dirs
	@echo "CC      $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link kernel
$(KERNEL_ELF): $(BOOTLOADER_OBJ) $(ALL_OBJECTS)
	@echo "LD      $@"
	@$(LD) $(LDFLAGS) $^ -o $@

# Create binary
$(KERNEL_BIN): $(KERNEL_ELF)
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O binary $< $@
	@echo "Build complete: $@"

# Create ISO image
iso: $(KERNEL_BIN)
	@echo "Creating ISO image..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_BIN) $(ISO_DIR)/boot/tmxc_os.bin
	@cp grub.cfg $(ISO_DIR)/boot/grub/
	@grub-mkrescue -o $(ISO_IMAGE) $(ISO_DIR) 2>/dev/null || \
	 xorriso -as mkisofs -r -b boot/grub/i386-pc/eltorito.img \
	 -no-emul-boot -boot-load-size 4 -boot-info-table \
	 -o $(ISO_IMAGE) $(ISO_DIR)
	@echo "ISO created: $(ISO_IMAGE)"

# Run in QEMU
run: iso
	@echo "Starting QEMU..."
	@qemu-system-$(ARCH) -m 512M -cdrom $(ISO_IMAGE) -serial stdio

# Run with debug
debug: iso
	@echo "Starting QEMU with debug..."
	@qemu-system-$(ARCH) -m 512M -cdrom $(ISO_IMAGE) -serial stdio -s -S

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete"

# Clean everything including generated files
distclean: clean
	@echo "Removing generated files..."
	@rm -f tmxc_keys.txt
	@rm -f tmxc_secure_keys.txt
	@rm -rf TMXC_Client_Packages
	@rm -f *.log
	@echo "Distribution clean complete"

# Show kernel information
info: $(KERNEL_ELF)
	@echo "Kernel information:"
	@$(OBJDUMP) -h $<
	@echo "Kernel size: $$(stat -f%z $(KERNEL_BIN) 2>/dev/null || stat -c%s $(KERNEL_BIN)) bytes"

# Help target
help:
	@echo "TMXC OS Build System"
	@echo "===================="
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build kernel binary (default)"
	@echo "  iso       - Create bootable ISO image"
	@echo "  run       - Build and run in QEMU"
	@echo "  debug     - Build and run in QEMU with debug support"
	@echo "  clean     - Remove build artifacts"
	@echo "  distclean - Remove all generated files"
	@echo "  info      - Show kernel information"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  ARCH=$(ARCH)"
	@echo "  Build directory: $(BUILD_DIR)"
	@echo "  Toolchain prefix: $(CROSS_PREFIX)"
	@echo ""
	@echo "Examples:"
	@echo "  make ARCH=ARM64"
	@echo "  make ARCH=x86_64 iso"
	@echo "  make clean run"

# ============================================
# Dependencies
# ============================================

# Include dependency files if they exist
-include $(ALL_OBJECTS:.o=.d)

# Generate dependency files
$(BUILD_DIR)/%.d: %.c dirs
	@$(CC) $(CFLAGS) -MM -MT $(BUILD_DIR)/$*.o $< -o $@
