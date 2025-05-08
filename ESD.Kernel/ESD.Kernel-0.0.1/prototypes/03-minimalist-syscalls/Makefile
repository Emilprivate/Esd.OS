GCC_FLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -c
LD_FLAGS = -m elf_i386
ASM_FLAGS = -f elf32

# Define directories
BASE_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := $(BASE_DIR)/src
CONFIG_DIR := $(BASE_DIR)/config
BUILD_DIR := $(BASE_DIR)/build
DIST_DIR := $(BASE_DIR)/dist
ISO_DIR := $(BUILD_DIR)/iso
ISO_BOOT_DIR := $(ISO_DIR)/boot
ISO_GRUB_DIR := $(ISO_BOOT_DIR)/grub

# Object files
OBJS = $(BUILD_DIR)/boot.o $(BUILD_DIR)/simple_kernel.o

# Output files
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
ISO_FILE = $(DIST_DIR)/esd-os.iso

.PHONY: all clean build run

all: clean build run

build: $(ISO_FILE)

$(ISO_FILE): $(KERNEL_BIN) $(CONFIG_DIR)/grub.cfg
	@echo "Preparing ISO structure..."
	@mkdir -p $(ISO_GRUB_DIR)
	@cp $(KERNEL_BIN) $(ISO_BOOT_DIR)/kernel.bin
	@cp $(CONFIG_DIR)/grub.cfg $(ISO_GRUB_DIR)/grub.cfg
	@echo "Creating ISO image..."
	@grub-mkrescue -o $(ISO_FILE) $(ISO_DIR)
	@echo "Build complete! ISO image is at $(ISO_FILE)"

$(KERNEL_BIN): $(OBJS)
	@echo "Linking kernel..."
	@ld $(LD_FLAGS) -T $(CONFIG_DIR)/linker.ld -o $(KERNEL_BIN) $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR) # Ensure build directory exists
	@echo "Compiling C: $<"
	@gcc $(GCC_FLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(BUILD_DIR) # Ensure build directory exists
	@echo "Assembling: $<"
	@nasm $(ASM_FLAGS) $< -o $@

clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)/*
	@rm -rf $(DIST_DIR)/*

run: build
	@if [ ! -f $(ISO_FILE) ]; then \
		echo "Error: ISO file not found at $(ISO_FILE)"; \
		echo "Please build the kernel first using make build"; \
		exit 1; \
	fi
	@echo "Starting QEMU with $(ISO_FILE)..."
	@qemu-system-i386 -cdrom $(ISO_FILE)
