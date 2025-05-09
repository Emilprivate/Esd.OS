; filepath: /home/emilog/Clones/Esd.OS/ESD.Kernel/ESD.Kernel-0.0.1/prototypes/03-keyboard-input/src/isr_asm.s
section .text

extern irq_handler_c
extern isr_handler_c

; Exception handler macro
%macro ISR_NO_ERR 1
global isr%1
isr%1:
    pusha                   ; Push all registers
    
    xor eax, eax
    mov ax, ds
    push eax                ; Save data segment
    
    mov ax, 0x10            ; Kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push byte 0             ; Push dummy error code
    push byte %1            ; Push interrupt number
    
    push esp                ; Push pointer to registers struct
    call isr_handler_c      ; Call C handler
    add esp, 4              ; Clean up function argument
    
    add esp, 8              ; Clean up error code and int number
    
    pop eax                 ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa                    ; Restore registers
    iret                    ; Return from interrupt
%endmacro

%macro ISR_ERR 1
global isr%1
isr%1:
    pusha                   ; Push all registers
    
    xor eax, eax
    mov ax, ds
    push eax                ; Save data segment
    
    mov ax, 0x10            ; Kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Error code already pushed by processor
    push byte %1            ; Push interrupt number
    
    push esp                ; Push pointer to registers struct
    call isr_handler_c      ; Call C handler
    add esp, 4              ; Clean up function argument
    
    add esp, 8              ; Clean up error code and int number
    
    pop eax                 ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa                    ; Restore registers
    add esp, 4              ; Skip error code
    iret                    ; Return from interrupt
%endmacro

; CPU exception handlers
ISR_NO_ERR 0
ISR_NO_ERR 1
ISR_NO_ERR 2
ISR_NO_ERR 3
ISR_NO_ERR 4
ISR_NO_ERR 5
ISR_NO_ERR 6
ISR_NO_ERR 7
ISR_ERR    8
ISR_NO_ERR 9
ISR_ERR    10
ISR_ERR    11
ISR_ERR    12
ISR_ERR    13
ISR_ERR    14
ISR_NO_ERR 15
ISR_NO_ERR 16
ISR_NO_ERR 17
ISR_NO_ERR 18
ISR_NO_ERR 19
ISR_NO_ERR 20
ISR_NO_ERR 21
ISR_NO_ERR 22
ISR_NO_ERR 23
ISR_NO_ERR 24
ISR_NO_ERR 25
ISR_NO_ERR 26
ISR_NO_ERR 27
ISR_NO_ERR 28
ISR_NO_ERR 29
ISR_NO_ERR 30
ISR_NO_ERR 31

; IRQ handlers (existing code)
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

IRQ  0, 32
IRQ  1, 33
IRQ  2, 34
IRQ  3, 35
IRQ  4, 36
IRQ  5, 37
IRQ  6, 38
IRQ  7, 39
IRQ  8, 40
IRQ  9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

global idt_load
idt_load:
    mov eax, [esp+4]
    lidt [eax]
    ret