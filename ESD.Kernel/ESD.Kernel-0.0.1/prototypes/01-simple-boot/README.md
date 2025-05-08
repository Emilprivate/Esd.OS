# Simple Kernel Build Steps

This document outlines the commands used to build the `esd-os.iso` kernel image.

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
01-simple-boot/
├── build/            # Intermediate build files
├── config/
│   ├── grub.cfg      # GRUB configuration
│   └── linker.ld     # Linker script
├── dist/             # Output ISO file (esd-os.iso)
├── src/
│   ├── boot.s        # Assembly bootloader
│   └── simple_kernel.c # C kernel code
└── README.md         # This file
└── build.sh          # Build script
```

## Build Script

A `build.sh` script is provided to automate the build process. To use it:
1. Make it executable: `chmod +x build.sh`
2. Run the script: `./build.sh`

The script will compile the kernel, create the `esd-os.iso` image, and place it in the `dist/` directory.

## Manual Build Commands (for reference)

The `build.sh` script automates these steps. They are listed here for understanding the process. All paths are relative to the `01-simple-boot` directory.

1.  **Assemble `src/boot.s`:**
    ```bash
    nasm -f elf32 src/boot.s -o build/boot.o
    ```

2.  **Compile `src/simple_kernel.c`:**
    ```bash
    gcc -m32 -c src/simple_kernel.c -o build/simple_kernel.o -ffreestanding -O2 -Wall -Wextra
    ```

3.  **Link object files into `build/kernel.bin`:**
    ```bash
    ld -m elf_i386 -T config/linker.ld -o build/kernel.bin build/boot.o build/simple_kernel.o
    ```

4.  **Create ISO directory structure:**
    ```bash
    mkdir -p build/iso/boot/grub
    ```

5.  **Copy kernel binary to ISO structure:**
    ```bash
    cp build/kernel.bin build/iso/boot/kernel.bin
    ```

6.  **Copy GRUB configuration to ISO structure:**
    ```bash
    cp config/grub.cfg build/iso/boot/grub/grub.cfg
    ```

7.  **Create the bootable ISO image in `dist/`:**
    ```bash
    grub-mkrescue -o dist/esd-os.iso build/iso
    ```

## Testing

After the ISO (`dist/esd-os.iso`) is created, you can test the kernel using QEMU:

```bash
qemu-system-i386 -cdrom dist/esd-os.iso
```
