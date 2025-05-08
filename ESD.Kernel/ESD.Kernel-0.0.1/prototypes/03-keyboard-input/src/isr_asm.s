; filepath: /home/emilog/Clones/Esd.OS/ESD.Kernel/ESD.Kernel-0.0.1/prototypes/03-keyboard-input/src/isr_asm.s
section .text

extern irq_handler_c

%macro IRQ 2
global irq%1
irq%1:
    pusha
    
    xor eax, eax
    mov ax, ds
    push eax
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push byte %2
    push byte 0
    
    push esp
    call irq_handler_c
    add esp, 4
    
    add esp, 8
    
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa
    iret

%endmacro

IRQ  1, 33

global idt_load
idt_load:
    mov eax, [esp+4]
    lidt [eax]
    ret