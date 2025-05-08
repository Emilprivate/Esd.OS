void k_put_char(char c, int col, int row, unsigned char attr) {
    unsigned char *vidmem = (unsigned char *)0xb8000;
    int offset;

    if (col >= 80 || row >= 25) {
        return; 
    }

    offset = (row * 80 + col) * 2;
    vidmem[offset] = c;
    vidmem[offset + 1] = attr;
}

void k_main() {
    const char *str = "ESD.OS Kernel Booted!";
    int i = 0;
    int col = 0;
    int row = 12;

    while (str[i] != '\0') {
        k_put_char(str[i], col, row, 0x0F); 
        col++;
        i++;
    }

    for (;;) {
        asm volatile ("hlt");
    }
}
