# ESD.OS Keyboard Input Prototype

This document outlines the structure and build process for the `03-keyboard-input` prototype of ESD.OS.

## Prerequisites

Ensure you have the following tools installed:
- NASM (assembler)
- GCC (C compiler with 32-bit support)
- ld (linker)
- grub-mkrescue
- xorriso
- mtools
- QEMU for testing

On Debian-based systems:
```bash
sudo apt-get update
sudo apt-get install nasm gcc-multilib binutils grub-common grub-pc-bin xorriso mtools qemu-system-x86
```

## Directory Structure
```
03-keyboard-input/
├── build/            # Build artifacts
│   └── iso/          # ISO staging area
│       └── boot/
│           ├── kernel.bin
│           └── grub/
│               └── grub.cfg
├── config/
│   ├── grub.cfg      # GRUB configuration
│   └── linker.ld     # Linker script
├── dist/             # Output directory for ISO
├── src/
│   ├── boot.s        # Boot entry point
│   ├── simple_kernel.c # Main kernel code
│   ├── simple_kernel.h # Kernel header
│   ├── idt.c         # Interrupt Descriptor Table implementation
│   ├── idt.h         # IDT header
│   ├── isr.c         # Interrupt Service Routines
│   ├── isr.h         # ISR header
│   ├── isr_asm.s     # Assembly interrupt handlers
│   ├── keyboard.c    # Keyboard handler implementation
│   └── keyboard.h    # Keyboard handler header
├── README.md         # This file
└── Makefile          # Build automation
```

## Features

This prototype implements:
- Basic polling-based PS/2 keyboard driver
- Simple scancode-to-ASCII conversion
- Text input with backspace support
- Display of unknown scancodes for debugging
- Colorful terminal interface

## Building and Running

The Makefile provides several targets:
- `make build`: Compile and create the ISO
- `make run`: Run the built ISO in QEMU
- `make clean`: Remove build artifacts
- `make all`: Clean, build, and run in sequence

Example:
```bash
make all
```

## Notes

This kernel uses a polling-based keyboard driver instead of interrupts for simplicity. The implementation focuses on basic keyboard input without complex key mapping or modifier handling.
