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

extern void isr0();
extern void isr1();

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
