#!/bin/bash

set -e

BASE_DIR=$(dirname "$0")
SRC_DIR="$BASE_DIR/src"
CONFIG_DIR="$BASE_DIR/config"
BUILD_DIR="$BASE_DIR/build"
DIST_DIR="$BASE_DIR/dist"
ISO_DIR="$BUILD_DIR/iso"
ISO_BOOT_DIR="$ISO_DIR/boot"
ISO_GRUB_DIR="$ISO_BOOT_DIR/grub"

echo "Cleaning previous build..."
rm -rf "$BUILD_DIR"/*
rm -rf "$DIST_DIR"/*

mkdir -p "$BUILD_DIR"
mkdir -p "$DIST_DIR"
mkdir -p "$ISO_GRUB_DIR"

echo "Assembling bootloader..."
nasm -f elf32 "$SRC_DIR/boot.s" -o "$BUILD_DIR/boot.o"

echo "Compiling kernel..."
gcc -m32 -c "$SRC_DIR/simple_kernel.c" -o "$BUILD_DIR/simple_kernel.o" -ffreestanding -O2 -Wall -Wextra

echo "Linking kernel..."
ld -m elf_i386 -T "$CONFIG_DIR/linker.ld" -o "$BUILD_DIR/kernel.bin" "$BUILD_DIR/boot.o" "$BUILD_DIR/simple_kernel.o"

echo "Preparing ISO structure..."
cp "$BUILD_DIR/kernel.bin" "$ISO_BOOT_DIR/kernel.bin"
cp "$CONFIG_DIR/grub.cfg" "$ISO_GRUB_DIR/grub.cfg"

echo "Creating ISO image..."
grub-mkrescue -o "$DIST_DIR/esd-os.iso" "$ISO_DIR"

echo "Build complete! ISO image is at $DIST_DIR/esd-os.iso"
echo "To test, run: qemu-system-i386 -cdrom $DIST_DIR/esd-os.iso"
