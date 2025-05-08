#include "simple_kernel.h"
#include "idt.h"
#include "isr.h"
#include "keyboard.h"

unsigned char *vidmem = (unsigned char *)0xb8000;
unsigned char current_attr = DEFAULT_ATTR;
int cursor_x = 0;
int cursor_y = 0;

#define KEY_BACKSPACE 0x0E
#define KEY_CTRL      0x1D
#define KEY_LEFT_ALT  0x38
#define KEY_ENTER     0x1C
#define KEY_SPACE     0x39

void k_update_cursor(int x, int y) {
    unsigned short pos = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
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

char scancode_to_char(unsigned char scancode) {
    if (scancode == KEY_BACKSPACE) return '\b';
    if (scancode == KEY_CTRL) return 0;
    if (scancode == KEY_LEFT_ALT) return 0;
    
    if (scancode == 0x1E) return 'a';
    if (scancode == 0x30) return 'b';
    if (scancode == 0x2E) return 'c';
    if (scancode == 0x20) return 'd';
    if (scancode == 0x12) return 'e';
    if (scancode == 0x21) return 'f';
    if (scancode == 0x22) return 'g';
    if (scancode == 0x23) return 'h';
    if (scancode == 0x17) return 'i';
    if (scancode == 0x24) return 'j';
    if (scancode == 0x25) return 'k';
    if (scancode == 0x26) return 'l';
    if (scancode == 0x32) return 'm';
    if (scancode == 0x31) return 'n';
    if (scancode == 0x18) return 'o';
    if (scancode == 0x19) return 'p';
    if (scancode == 0x10) return 'q';
    if (scancode == 0x13) return 'r';
    if (scancode == 0x1F) return 's';
    if (scancode == 0x14) return 't';
    if (scancode == 0x16) return 'u';
    if (scancode == 0x2F) return 'v';
    if (scancode == 0x11) return 'w';
    if (scancode == 0x2D) return 'x';
    if (scancode == 0x15) return 'y';
    if (scancode == 0x2C) return 'z';
    if (scancode == KEY_SPACE) return ' ';
    if (scancode == KEY_ENTER) return '\n';
    return 0;
}

void handle_backspace() {
    if (cursor_x > 2) {
        cursor_x--;
        
        int offset = (cursor_y * VGA_WIDTH + cursor_x) * 2;
        vidmem[offset] = ' ';
        
        k_update_cursor(cursor_x, cursor_y);
    }
}

void poll_keyboard() {
    if (inb(0x64) & 1) {
        unsigned char scancode = inb(0x60);
        
        if (!(scancode & 0x80)) {
            char c = scancode_to_char(scancode);
            
            if (c == '\b') {
                handle_backspace();
            } else if (c) {
                k_put_char(c);
            } else if (scancode != KEY_CTRL && scancode != KEY_LEFT_ALT) {
                k_print_string("0x");
                char hex[3];
                hex[0] = ((scancode >> 4) & 0xF) + (((scancode >> 4) & 0xF) < 10 ? '0' : 'A' - 10);
                hex[1] = (scancode & 0xF) + ((scancode & 0xF) < 10 ? '0' : 'A' - 10);
                hex[2] = '\0';
                k_print_string(hex);
                k_print_string(" ");
            }
        }
    }
}

void display_watermark() {
    unsigned char prev_attr = current_attr;
    
    int orig_x = cursor_x;
    int orig_y = cursor_y;
    
    const char* version = "ESD.OS Kernel v0.0.1 (03-keyboard-input)";
    int version_len = 0;
    while(version[version_len] != '\0') version_len++;
    
    cursor_x = VGA_WIDTH - version_len - 1;
    cursor_y = VGA_HEIGHT - 1;
    
    k_set_text_attr(0x1B);
    
    k_print_string(version);
    
    cursor_x = orig_x;
    cursor_y = orig_y;
    k_set_text_attr(prev_attr);
    
    k_update_cursor(cursor_x, cursor_y);
}

void k_main() {
    k_clear_screen();
    
    k_set_text_attr(0x1E);
    k_print_string("****************************************\n");
    k_print_string("*            ESD.OS Kernel             *\n");
    k_print_string("****************************************\n\n");
    
    k_set_text_attr(0x0A);
    k_print_string("\n===========================\n");
    k_print_string("OOGA BOOGA! TYPE SOMETHING, MONKE!\n");
    k_print_string("===========================\n\n");
    
    k_set_text_attr(0x0D);
    k_print_string("> ");
    
    k_set_text_attr(DEFAULT_ATTR);
    
    display_watermark();
    
    for (;;) {
        poll_keyboard();
    }
}
