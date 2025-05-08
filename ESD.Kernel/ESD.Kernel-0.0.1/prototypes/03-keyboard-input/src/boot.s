; filepath: /home/emilog/Clones/Esd.OS/ESD.Kernel/ESD.Kernel-0.0.1/prototypes/03-keyboard-input/src/boot.s
section .multiboot
align 4
    dd 0x1BADB002
    dd 0x00
    dd - (0x1BADB002 + 0x00)

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .text
global _start
extern k_main
extern idt_load

_start:
    cli
    mov esp, stack_top
    
    call k_main
    
    cli
    hlt
    jmp $