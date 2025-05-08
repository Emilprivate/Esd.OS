# Simple Kernel Build Steps

This document outlines the commands used to build the `esd-os.iso` kernel image for the **02-improved-console** prototype.

## Prerequisites

Ensure you have the following tools installed:
- NASM (assembler)
- GCC (C compiler, specifically the 32-bit toolchain if on a 64-bit system)
- ld (linker)
- `grub-mkrescue` (part of GRUB utilities)
- `xorriso` (required by `grub-mkrescue`)
- `mtools` (required by `grub-mkrescue`)
- QEMU (for testing, e.g., `qemu-system-i386`)

On a Debian-based system (like Ubuntu), you might install some of these with:
```bash
sudo apt-get update
sudo apt-get install nasm gcc-multilib binutils grub-common grub-pc-bin xorriso mtools qemu-system-x86
```

## Directory Structure
```
02-improved-console/
├── build/            # Intermediate build files (e.g., .o, kernel.bin)
│   └── iso/          # Staging area for ISO creation
│       └── boot/
│           ├── kernel.bin
│           └── grub/
│               └── grub.cfg
├── config/
│   ├── grub.cfg      # GRUB configuration
│   └── linker.ld     # Linker script
├── dist/             # Output ISO file (esd-os.iso)
├── src/
│   ├── boot.s        # Assembly bootloader
│   └── simple_kernel.c # C kernel code
├── README.md         # This file
├── build.sh          # Build script
└── run.sh            # Run script (for QEMU)
```

## Build Script

The `build.sh` script automates the process of assembling, compiling, linking, and creating the ISO file.

## Manual Build Commands (for reference)

If you prefer to build manually, follow these steps:
1. Assemble the bootloader: `nasm -f bin src/boot.s -o build/boot.bin`
2. Compile the kernel: `gcc -m32 -ffreestanding -c src/simple_kernel.c -o build/simple_kernel.o`
3. Link the kernel: `ld -m elf_i386 -T config/linker.ld -o build/kernel.bin build/simple_kernel.o`
4. Create the ISO: `grub-mkrescue -o dist/esd-os.iso build/iso`

## Testing

After the ISO (`dist/esd-os.iso`) is created using `./build.sh`, you can test the kernel using QEMU.

You can either run the QEMU command directly:
```bash
qemu-system-i386 -cdrom dist/esd-os.iso
```

Or use the provided `run.sh` script:
1. Make it executable: `chmod +x run.sh`
2. Run the script: `./run.sh`
