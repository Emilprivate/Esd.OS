#include "keyboard.h"
#include "simple_kernel.h"
#include "isr.h"

void keyboard_handler_main(registers_t *regs) {
    inb(0x60);
    k_print_string("KEY ");
}

void keyboard_install() {
    k_print_string("Installing keyboard handler...\n");
    isr_install_handler(IRQ1, keyboard_handler_main);
    pic_mask_all();
    k_print_string("Keyboard handler installed.\n");
}
