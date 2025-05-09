/* filepath: /home/emilog/Clones/Esd.OS/ESD.Kernel/ESD.Kernel-0.0.1/prototypes/03-proper-iso/config/linker.ld */
OUTPUT_FORMAT("elf32-i386")
ENTRY(_start)

SECTIONS
{
    /* Begin putting sections at 1MB, a conventional place for kernels */
    . = 1M;

    /* First put the multiboot header, as it is required to be put very early */
    /* in the image or the bootloader won't recognize the file format. */
    .text ALIGN(4K) : {
        *(.multiboot)
        *(.text)
    }

    /* Read-only data */
    .rodata ALIGN(4K) : {
        *(.rodata)
    }

    /* Read-write data (initialized) */
    .data ALIGN(4K) : {
        *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    .bss ALIGN(4K) : {
        *(COMMON)
        *(.bss)
    }

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note*)
    }
}