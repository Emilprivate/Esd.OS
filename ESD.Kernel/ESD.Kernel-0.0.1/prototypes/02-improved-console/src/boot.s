; boot.s
section .multiboot
align 4
    dd 0x1BADB002             ; magic number for Multiboot
    dd 0x00                   ; flags
    dd - (0x1BADB002 + 0x00)  ; checksum = -(magic + flags)

section .text
global _start
extern k_main                 ; k_main is defined in simple_kernel.c

_start:
    cli                     ; disable interrupts
    call k_main             ; call our C kernel's main function
    hlt                     ; halt the CPU if k_main returns
