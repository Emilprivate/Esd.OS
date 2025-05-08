/* linker.ld */
ENTRY(_start) /* Set the entry point of the kernel */

SECTIONS {
    /* Kernel starts at 1MB */
    . = 1M;

    .text : ALIGN(4K) {
        KEEP(*(.multiboot)) /* Ensure the multiboot header is at the beginning */
        *(.text)            /* All code */
    }

    .rodata : ALIGN(4K) {
        *(.rodata)          /* Read-only data */
    }

    .data : ALIGN(4K) {
        *(.data)            /* Initialized data */
    }

    .bss : ALIGN(4K) {
        *(.bss)             /* Uninitialized data */
    }
}
