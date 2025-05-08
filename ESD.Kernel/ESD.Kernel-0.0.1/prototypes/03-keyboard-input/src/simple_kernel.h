#ifndef SIMPLE_KERNEL_H
#define SIMPLE_KERNEL_H

#include <stdint.h>

void k_update_cursor(int x, int y);
void k_scroll();
void k_clear_screen();
void k_put_char(char c);
void k_print_string(const char *str);
void k_set_text_attr(unsigned char attr);

static inline void outb(unsigned short port, unsigned char val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define DEFAULT_ATTR 0x0F

#endif
