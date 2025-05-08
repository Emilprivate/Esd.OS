#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define DEFAULT_ATTR 0x0F

unsigned char *vidmem = (unsigned char *)0xb8000;
unsigned char current_attr = DEFAULT_ATTR;
int cursor_x = 0;
int cursor_y = 0;

static inline void outb(unsigned short port, unsigned char val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

void k_update_cursor(int x, int y) {
    unsigned short pos = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0F); // Select cursor location low register
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E); // Select cursor location high register
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void k_scroll() {
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            int offset_src = (y * VGA_WIDTH + x) * 2;
            int offset_dst = ((y - 1) * VGA_WIDTH + x) * 2;
            vidmem[offset_dst] = vidmem[offset_src];
            vidmem[offset_dst + 1] = vidmem[offset_src + 1];
        }
    }

    int offset_last_line = ((VGA_HEIGHT - 1) * VGA_WIDTH) * 2;
    for (int x = 0; x < VGA_WIDTH; x++) {
        vidmem[offset_last_line + x * 2] = ' ';
        vidmem[offset_last_line + x * 2 + 1] = current_attr;
    }
    cursor_y = VGA_HEIGHT - 1; 
    k_update_cursor(cursor_x, cursor_y);
}

void k_clear_screen() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            int offset = (y * VGA_WIDTH + x) * 2;
            vidmem[offset] = ' ';
            vidmem[offset + 1] = DEFAULT_ATTR;
        }
    }
    cursor_x = 0;
    cursor_y = 0;
    k_update_cursor(cursor_x, cursor_y);
}

void k_put_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c >= ' ') {
        int offset = (cursor_y * VGA_WIDTH + cursor_x) * 2;
        vidmem[offset] = c;
        vidmem[offset + 1] = current_attr;
        cursor_x++;
    }

    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= VGA_HEIGHT) {
        k_scroll();
    }
    k_update_cursor(cursor_x, cursor_y);
}

void k_print_string(const char *str) {
    int i = 0;
    while (str[i] != '\0') {
        k_put_char(str[i]);
        i++;
    }
}

void k_set_text_attr(unsigned char attr) {
    current_attr = attr;
}

void k_main() {
    k_clear_screen(); 

    k_print_string("ESD.OS Kernel - Console v2.0 (Hardware Cursor & Colors)!\n");
    k_print_string("Default color (White on Black)\n");

    k_set_text_attr(0x0A); // Light Green text on Black
    k_print_string("This is Light Green text!\n");

    k_set_text_attr(0x0C); // Light Red text on Black
    k_print_string("This is Light Red text!\n");

    k_set_text_attr(0x1E); // Yellow text on Blue
    k_print_string("Yellow on Blue! Wow!\n");
    
    k_set_text_attr(DEFAULT_ATTR); // Reset to default
    k_print_string("\nScrolling test with new features:\n");

    for (int i = 0; i < 20; i++) {
        k_print_string("Line ");

        char num_str[3];
        if (i + 1 < 10) {
            num_str[0] = (i + 1) + '0';
            num_str[1] = '\0';
        } else {
            num_str[0] = ((i + 1) / 10) + '0';
            num_str[1] = ((i + 1) % 10) + '0';
            num_str[2] = '\0';
        }
        k_print_string(num_str);
        k_print_string("\n");
    }
    k_print_string("This line should cause a scroll!\n");
    k_print_string("And this appears after the scroll.\n");

    k_set_text_attr(0x0B); // Light Cyan
    k_print_string("Testing cursor position after many lines...\n");
    k_print_string("Cursor should be here ->");

    for (;;) {
        asm volatile ("hlt");
    }
}
