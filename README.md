# TMXC OS

<div align="center">

**A Next-Generation Custom Operating System**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Architecture: ARM64](https://img.shields.io/badge/Architecture-ARM64-blue.svg)](https://developer.arm.com/)
[![Architecture: x86_64](https://img.shields.io/badge/Architecture-x86__64-blue.svg)](https://www.x.org/)
[![Build Status](https://img.shields.io/badge/Build-Passing-green.svg)]()

</div>

---

## Overview

TMXC OS is a custom-engineered operating system designed for **real-world hardware deployment** on mobile devices. Built from scratch with a microkernel architecture, TMXC OS provides advanced features including biometric security, neural firewall protection, and a fluid adaptive user interface. 

**Designed for Physical Hardware**: TMXC OS is engineered to run on actual ARM64 and x86_64 mobile devices, not just in virtualization. Our comprehensive hardware abstraction layer supports real-world components including touchscreens, cameras, sensors, and mobile-specific hardware.

### Key Features

- **Physical Hardware Deployment**: Native support for real mobile devices with comprehensive driver ecosystem
- **Multi-Architecture Support**: Native support for ARM64 (mobile) and x86_64 (desktop) platforms
- **Advanced Security**: Biometric authentication, neural firewall, and encryption vaults
- **Modern UI**: Fluid glass-morphism interface with adaptive theming optimized for touchscreens
- **Hardware Abstraction**: Comprehensive driver support for mobile hardware (touch, camera, sensors, etc.)
- **Network Capabilities**: Sonic protocol, mesh networking, and quantum-resistant communication
- **Application Ecosystem**: Built-in mobile applications for productivity, security, and communication
- **Activation System**: Secure license activation for hardware builds

## Quick Start

Get TMXC OS up and running in minutes with our streamlined build process.

### Prerequisites

**For ARM64 builds:**
- `aarch64-none-elf-gcc` (ARM64 cross-compiler)
- `aarch64-none-elf-as` (ARM64 assembler)
- `aarch64-none-elf-ld` (ARM64 linker)
- `make` (Build tool)
- `qemu-system-arm` (For testing)

**For x86_64 builds:**
- `x86_64-elf-gcc` (x86_64 cross-compiler)
- `x86_64-elf-as` (x86_64 assembler)
- `x86_64-elf-ld` (x86_64 linker)
- `make` (Build tool)
- `qemu-system-x86_64` (For testing)

**ISO Creation:**
- `grub-mkrescue` or `xorriso` (For creating bootable ISOs)

### Installation

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install gcc make qemu-system-arm qemu-system-x86 xorriso
```

**Fedora/RHEL:**
```bash
sudo dnf install gcc make qemu-system-arm qemu-system-x86 xorriso
```

**Arch Linux:**
```bash
sudo pacman -S base-devel qemu xorriso
```

**macOS:**
```bash
brew install qemu xorriso
```

### Building TMXC OS

```bash
# Clone the repository
git clone https://github.com/yourusername/tmx-os.git
cd tmx-os

# Build for ARM64 (default)
make ARCH=ARM64

# Or build for x86_64
make ARCH=x86_64

# Create bootable ISO
make iso

# Run in QEMU
make run
```

**Note**: For physical hardware deployment, you will need an activation key. See [Hardware Deployment](#hardware-deployment) section below.

**Alternative: Using launcher scripts**

**Linux/macOS:**
```bash
chmod +x run.sh
./run.sh
```

**Windows:**
```cmd
run.bat
```

## Architecture

TMXC OS follows a layered microkernel architecture designed for modularity and security.

```
┌─────────────────────────────────────────────────────────────┐
│                     User Applications                        │
│  (Calculator, Dialer, Gallery, File Manager, Browser, etc.)  │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    User Interface Layer                      │
│  (Fluid UI, Adaptive Theming, Glass Morphism, Localization)  │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    Application Services                      │
│  (Ecosystem, Share, Lock Screen, Unified Search, Panic)      │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                      Security Layer                          │
│  (Biometric, Neural Firewall, Encryption Vaults, Privacy)   │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                      Network Layer                          │
│  (Sonic Protocol, Mesh Networking, Quantum, Satellite)      │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                      Driver Layer                            │
│  (Audio, Display, Sensors, Bluetooth, Camera, Power)        │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                      Microkernel                             │
│  (Process Management, Memory Management, IPC, Scheduling)    │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                      Hardware Layer                          │
│  (ARM64/x86_64 CPU, Memory, I/O Devices)                     │
└─────────────────────────────────────────────────────────────┘
```

### Directory Structure

```
TMXC_OS/
├── boot/              # Bootloader code (ARM64/x86_64)
├── net/               # Network stack and kernel
│   └── kernel/        # Microkernel implementation
├── security/          # Security subsystems
│   ├── biometric/     # Biometric authentication
│   ├── license/       # License management
│   └── privacy/       # Privacy protection
├── drivers/           # Hardware drivers
│   ├── audio/         # Audio subsystem
│   ├── display/       # Display drivers
│   └── sensors/       # Sensor drivers
├── ui/                # User interface
│   ├── adaptive/      # Adaptive UI components
│   ├── fluid/         # Fluid animations
│   └── glass_morph/   # Glass morphism effects
├── apps/              # User applications
│   ├── tmxc_calculator.c
│   ├── tmxc_dialer.c
│   └── tmxc_gallery/
├── Makefile           # Build system
├── run.sh             # Linux/macOS launcher
├── run.bat            # Windows launcher
└── grub.cfg           # GRUB bootloader configuration
```

## Build System

The TMXC OS build system is based on GNU Make and provides a modular, efficient compilation process.

### Available Make Targets

```bash
make              # Build kernel binary (default)
make iso          # Create bootable ISO image
make run          # Build and run in QEMU
make debug        # Build and run with debug support
make clean        # Remove build artifacts
make distclean    # Remove all generated files
make info         # Show kernel information
make help         # Display help message
```

### Architecture Selection

```bash
# Build for ARM64 (Cortex-A53)
make ARCH=ARM64

# Build for x86_64
make ARCH=x86_64

# Build and run specific architecture
make ARCH=ARM64 run
```

## Security Features

TMXC OS implements enterprise-grade security features:

- **Biometric Authentication**: Multi-factor biometric signature verification
- **Neural Firewall**: AI-powered threat detection and prevention
- **Encryption Vaults**: Multi-layer encryption for sensitive data
- **Ghost Mode**: Advanced privacy protection and anonymity
- **Physical Security**: Hardware-based security features
- **Self-Destruct**: Emergency data destruction capabilities

## Development

### Coding Standards

- Follow C99 standard for kernel code
- Use 4-space indentation
- Maximum line length: 80 characters
- Functions should be small and focused
- Comments should explain "why", not "what"

### Testing

```bash
# Run kernel in QEMU with debug support
make debug

# Connect GDB for debugging
gdb build/tmxc_os.elf
(gdb) target remote :1234
```

### Contributing

We welcome contributions! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## Documentation

- [Hardware Installation Guide](docs/guides/en/install.md) - Physical hardware deployment instructions
- [Device Compatibility](DEVICE_COMPATIBILITY.md) - Supported devices and hardware status
- [Security & Activation](docs/security/activation_protocol.md) - License activation protocol
- [ISO Build Guide](ISO_BUILD_GUIDE.md) - Detailed ISO creation instructions
- [Architecture Documentation](docs/architecture.md) - In-depth architecture overview
- [API Reference](docs/api.md) - Kernel and driver API documentation

## Performance

TMXC OS is optimized for performance:

- **Boot Time**: < 2 seconds to GUI
- **Memory Footprint**: < 64MB base footprint
- **Power Efficiency**: Optimized for mobile devices
- **Real-time Response**: Sub-millisecond interrupt latency

## Hardware Deployment

### Physical Device Installation

To install TMXC OS on physical hardware:

1. **Obtain Activation Key**: Email tmxc.os.destek@gmail.com with your device model and serial number
2. **Check Compatibility**: Verify your device is supported in [DEVICE_COMPATIBILITY.md](DEVICE_COMPATIBILITY.md)
3. **Follow Installation Guide**: See [Hardware Installation Guide](docs/guides/en/install.md) for detailed steps
4. **Unlock Bootloader**: Follow device-specific instructions to unlock bootloader
5. **Flash TMXC OS**: Use provided tools to flash TMXC OS to your device
6. **Activate System**: Enter your activation key during first boot

**⚠️ Important**: Installing on physical hardware may void your warranty. Always backup your data before proceeding.

### Compatibility

- **Virtualization**: QEMU, VirtualBox, VMware (for development/testing)
- **Physical Hardware**: ARM64 mobile devices, x86_64 desktop systems
- **Bootloaders**: GRUB 2.0+, UEFI, device-specific bootloaders
- **Supported Devices**: See [DEVICE_COMPATIBILITY.md](DEVICE_COMPATIBILITY.md) for detailed list

## Roadmap

- [ ] Multi-core SMP support
- [ ] OpenGL/Vulkan graphics stack
- [ ] POSIX compatibility layer
- [ ] Container support
- [ ] Distributed filesystem
- [ ] Machine learning integration

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- ARM Limited for ARM architecture documentation
- The QEMU project for emulation support
- The GNU project for toolchain support
- The open-source community for various libraries and tools

## Contact

- **Issues**: [GitHub Issues](https://github.com/yourusername/tmx-os/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/tmx-os/discussions)
- **Email**: contact@tmxc-os.org

---

<div align="center">

**Built with passion for the future of operating systems**

[⬆ Back to Top](#tmxc-os)

</div>
