#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define KERNEL_CS 0x08

typedef struct {
    uint16_t low_offset;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t high_offset;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

extern idt_entry_t idt_entries[256];
extern idt_ptr_t idt_pointer;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void idt_init();

#endif
