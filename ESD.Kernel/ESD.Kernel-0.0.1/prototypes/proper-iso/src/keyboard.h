#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "isr.h"

void keyboard_install();
void keyboard_handler_main(registers_t *regs);

#endif
