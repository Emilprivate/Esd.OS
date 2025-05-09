#include "idt.h"
#include <stddef.h>

idt_entry_t idt_entries[256];
idt_ptr_t idt_pointer;

extern void idt_load(idt_ptr_t *idt_ptr);

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].low_offset = (base & 0xFFFF);
    idt_entries[num].high_offset = (base >> 16) & 0xFFFF;
    idt_entries[num].sel = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags = flags;
}

void idt_init() {
    idt_pointer.limit = (sizeof(idt_entry_t) * 256) - 1;
    idt_pointer.base = (uint32_t)&idt_entries;

    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    idt_load(&idt_pointer);
}
