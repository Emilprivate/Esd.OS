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
    idt_set_gate(IRQ1, (uint32_t)irq1, KERNEL_CS, 0x8E);
}
