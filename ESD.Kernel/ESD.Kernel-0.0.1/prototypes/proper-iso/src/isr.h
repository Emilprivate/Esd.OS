#ifndef ISR_H
#define ISR_H

#include <stdint.h>
#include "idt.h"

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
} __attribute__((packed)) registers_t;

#define IRQ0  32
#define IRQ1  33
#define IRQ2  34
#define IRQ3  35
#define IRQ4  36
#define IRQ5  37
#define IRQ6  38
#define IRQ7  39
#define IRQ8  40
#define IRQ9  41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef void (*isr_t)(registers_t*);

void isr_handler_c(registers_t *regs);
void irq_handler_c(registers_t *regs);
void isr_install_handler(int isr_number, isr_t handler);
void isr_uninstall_handler(int isr_number);
void pic_remap(int offset1, int offset2);
void pic_unmask_irq(unsigned char irq_line);
void pic_mask_all();
void isr_init_gates();

// CPU Exception handlers
extern void isr0();  // Division By Zero
extern void isr1();  // Debug
extern void isr2();  // Non Maskable Interrupt
extern void isr3();  // Breakpoint
extern void isr4();  // Into Detected Overflow
extern void isr5();  // Out of Bounds
extern void isr6();  // Invalid Opcode
extern void isr7();  // No Coprocessor
extern void isr8();  // Double Fault
extern void isr9();  // Coprocessor Segment Overrun
extern void isr10(); // Bad TSS
extern void isr11(); // Segment Not Present
extern void isr12(); // Stack Fault
extern void isr13(); // General Protection Fault
extern void isr14(); // Page Fault
extern void isr15(); // Unknown Interrupt
extern void isr16(); // Coprocessor Fault
extern void isr17(); // Alignment Check
extern void isr18(); // Machine Check
extern void isr19(); // Reserved
extern void isr20(); // Reserved
extern void isr21(); // Reserved
extern void isr22(); // Reserved
extern void isr23(); // Reserved
extern void isr24(); // Reserved
extern void isr25(); // Reserved
extern void isr26(); // Reserved
extern void isr27(); // Reserved
extern void isr28(); // Reserved
extern void isr29(); // Reserved
extern void isr30(); // Reserved
extern void isr31(); // Reserved

// IRQ handlers
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

#endif
