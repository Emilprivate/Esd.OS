; Even simpler multiboot header for VirtualBox compatibility
; Addresses the page fault issue

MBOOT_HEADER_MAGIC  equ 0x1BADB002
MBOOT_PAGE_ALIGN    equ 1<<0
MBOOT_MEM_INFO      equ 1<<1
MBOOT_VIDEO_MODE    equ 1<<2
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

section .multiboot
align 4
    dd MBOOT_HEADER_MAGIC
    dd MBOOT_HEADER_FLAGS
    dd MBOOT_CHECKSUM
    
    ; These fields must be 0 for VirtualBox compatibility (simplified header)
    dd 0        ; header_addr
    dd 0        ; load_addr
    dd 0        ; load_end_addr
    dd 0        ; bss_end_addr
    dd 0        ; entry_addr

; Reserve a large aligned stack to prevent page faults
section .bss
align 16
global stack_bottom
stack_bottom:
    resb 32768   ; 32KB stack to be safe
global stack_top
stack_top:

section .text
global _start
extern k_main

_start:
    ; Critical: zero out registers to avoid accessing invalid memory
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    
    ; Disable interrupts
    cli
    
    ; Set up stack - but make sure ESP is properly aligned
    mov esp, stack_top
    and esp, 0xFFFFFFF0  ; Align stack to 16 bytes
    
    ; Save multiboot info pointer from EBX if available
    push ebx
    
    ; Call kernel main function
    call k_main
    
    ; If we ever return, hang the CPU
.hang:
    hlt
    jmp .hang