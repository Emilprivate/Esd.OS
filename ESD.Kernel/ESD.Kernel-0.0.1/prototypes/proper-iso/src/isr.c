#include "isr.h"
#include "idt.h"
#include "simple_kernel.h"
#include <stddef.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC_EOI      0x20

isr_t interrupt_handlers[256] = {NULL};

void isr_handler_c(registers_t *regs) {
    if (interrupt_handlers[regs->int_no] != NULL) {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    } else {
        k_print_string("Unhandled interrupt: ");
        k_put_char('\n');
    }
}

void irq_handler_c(registers_t *regs) {
    if (regs->int_no < 32 || regs->int_no > 47) {
        k_print_string("Invalid IRQ number: ");
        return;
    }
    
    if (regs->int_no >= 40) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);

    if (interrupt_handlers[regs->int_no] != NULL) {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    }
}

void isr_install_handler(int isr_number, isr_t handler) {
    interrupt_handlers[isr_number] = handler;
}

void isr_uninstall_handler(int isr_number) {
    interrupt_handlers[isr_number] = NULL;
}

void pic_remap(int offset1, int offset2) {
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    outb(PIC1_DATA, offset1);
    outb(PIC2_DATA, offset2);

    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);

    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void pic_mask_all() {
    outb(PIC1_DATA, 0xFD);
    outb(PIC2_DATA, 0xFF);
}

void pic_unmask_irq(unsigned char irq_line) {
    uint16_t port;
    uint8_t value;

    if (irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }
    value = inb(port) & ~(1 << irq_line);
    outb(port, value);
}

void isr_init_gates() {
    // Exception handlers
    idt_set_gate(0, (uint32_t)isr0, KERNEL_CS, 0x8E);
    idt_set_gate(1, (uint32_t)isr1, KERNEL_CS, 0x8E);
    idt_set_gate(2, (uint32_t)isr2, KERNEL_CS, 0x8E);
    idt_set_gate(3, (uint32_t)isr3, KERNEL_CS, 0x8E);
    idt_set_gate(4, (uint32_t)isr4, KERNEL_CS, 0x8E);
    idt_set_gate(5, (uint32_t)isr5, KERNEL_CS, 0x8E);
    idt_set_gate(6, (uint32_t)isr6, KERNEL_CS, 0x8E);
    idt_set_gate(7, (uint32_t)isr7, KERNEL_CS, 0x8E);
    idt_set_gate(8, (uint32_t)isr8, KERNEL_CS, 0x8E);
    idt_set_gate(9, (uint32_t)isr9, KERNEL_CS, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, KERNEL_CS, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, KERNEL_CS, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, KERNEL_CS, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, KERNEL_CS, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, KERNEL_CS, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, KERNEL_CS, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, KERNEL_CS, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, KERNEL_CS, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, KERNEL_CS, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, KERNEL_CS, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, KERNEL_CS, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, KERNEL_CS, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, KERNEL_CS, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, KERNEL_CS, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, KERNEL_CS, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, KERNEL_CS, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, KERNEL_CS, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, KERNEL_CS, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, KERNEL_CS, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, KERNEL_CS, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, KERNEL_CS, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, KERNEL_CS, 0x8E);
    
    // IRQ handlers
    idt_set_gate(IRQ0, (uint32_t)irq0, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ1, (uint32_t)irq1, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ2, (uint32_t)irq2, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ3, (uint32_t)irq3, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ4, (uint32_t)irq4, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ5, (uint32_t)irq5, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ6, (uint32_t)irq6, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ7, (uint32_t)irq7, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ8, (uint32_t)irq8, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ9, (uint32_t)irq9, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ10, (uint32_t)irq10, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ11, (uint32_t)irq11, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ12, (uint32_t)irq12, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ13, (uint32_t)irq13, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ14, (uint32_t)irq14, KERNEL_CS, 0x8E);
    idt_set_gate(IRQ15, (uint32_t)irq15, KERNEL_CS, 0x8E);
}
