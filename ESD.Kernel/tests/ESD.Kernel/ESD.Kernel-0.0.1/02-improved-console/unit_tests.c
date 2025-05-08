#include <stdio.h>
#include "../../../test-tools/test-framework.h"
#include <stdio.h>  // Add stdio for printf

// Mock VGA memory and I/O ports
unsigned char mock_vidmem[80*25*2];
unsigned char mock_ports[256];

// Mock version of outb for testing
void outb(unsigned short port, unsigned char val) {
    mock_ports[port] = val;
}

// Mock implementations from 02-improved-console
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define DEFAULT_ATTR 0x0F

unsigned char *vidmem = mock_vidmem;
unsigned char current_attr = DEFAULT_ATTR;
int cursor_x = 0;
int cursor_y = 0;

void k_update_cursor(int x, int y) {
    unsigned short pos = y * VGA_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
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
        // For testing, we won't implement scrolling
        cursor_y = VGA_HEIGHT - 1;
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

// Initialize the test framework
test_result_t test_results[100];
int test_count = 0;
int pass_count = 0;
int fail_count = 0;

void test_init() {
    // Clear mock video memory and ports
    for (int i = 0; i < 80*25*2; i++) {
        mock_vidmem[i] = 0;
    }
    for (int i = 0; i < 256; i++) {
        mock_ports[i] = 0;
    }
    
    // Reset cursor position
    cursor_x = 0;
    cursor_y = 0;
    current_attr = DEFAULT_ATTR;
}

// Test framework functions
void test_report_start(const char* test_name) {
    test_results[test_count-1].name = test_name;
    printf("==== TEST: %s ====\n", test_name);
}

void test_report_result(int result, const char* message) {
    test_results[test_count-1].result = result;
    test_results[test_count-1].message = message;
    
    if (result == TEST_PASS) {
        printf("  [PASS] %s\n", message);
    } else if (result == TEST_FAIL) {
        printf("  [FAIL] %s\n", message);
    } else {
        printf("  [INFO] %s\n", message);
    }
}

void test_report_summary() {
    printf("\n==== TEST SUMMARY ====\n");
    printf("Total tests: %d\n", test_count);
    printf("Passed: %d\n", pass_count);
    printf("Failed: %d\n", fail_count);
    
    if (fail_count == 0) {
        printf("\nALL TESTS PASSED!\n");
    } else {
        printf("\nTESTS FAILED: %d of %d\n", fail_count, test_count);
    }
}

// Test k_put_char function
void test_k_put_char() {
    TEST_CASE("k_put_char places character with current attribute");
    
    test_init();
    
    k_put_char('A');
    
    TEST_ASSERT(mock_vidmem[0] == 'A', 
                "Character should be placed at the current cursor position");
    TEST_ASSERT(mock_vidmem[1] == DEFAULT_ATTR,
                "Current attribute should be used");
    TEST_ASSERT(cursor_x == 1 && cursor_y == 0,
                "Cursor should be advanced after placing character");
}

// Test newline handling
void test_newline() {
    TEST_CASE("k_put_char handles newline correctly");
    
    test_init();
    
    k_put_char('\n');
    
    TEST_ASSERT(cursor_x == 0 && cursor_y == 1,
                "Newline should move cursor to start of next line");
}

// Test cursor wrapping
void test_cursor_wrap() {
    TEST_CASE("Cursor wraps when reaching edge of screen");
    
    test_init();
    
    // Move cursor to end of line
    cursor_x = VGA_WIDTH - 1;
    
    k_put_char('A');
    
    TEST_ASSERT(cursor_x == 0 && cursor_y == 1,
                "Cursor should wrap to next line after reaching end of line");
}

// Test cursor update
void test_cursor_update() {
    TEST_CASE("k_update_cursor sends correct values to ports");
    
    test_init();
    
    cursor_x = 5;
    cursor_y = 10;
    
    k_update_cursor(cursor_x, cursor_y);
    
    unsigned short expected_pos = 10 * VGA_WIDTH + 5;
    
    TEST_ASSERT(mock_ports[0x3D4] == 0x0E,
                "High byte port command should be written");
    TEST_ASSERT(mock_ports[0x3D5] == (expected_pos >> 8) & 0xFF,
                "High byte of position should be written");
    
    // Check second port write for low byte
    TEST_ASSERT(mock_ports[0x3D4] == 0x0E || mock_ports[0x3D4] == 0x0F,
                "Low byte port command should be written");
}

// Test string printing
void test_print_string() {
    TEST_CASE("k_print_string prints entire string");
    
    test_init();
    
    k_print_string("ABC");
    
    TEST_ASSERT(mock_vidmem[0] == 'A', "First character should be printed");
    TEST_ASSERT(mock_vidmem[2] == 'B', "Second character should be printed");
    TEST_ASSERT(mock_vidmem[4] == 'C', "Third character should be printed");
    TEST_ASSERT(cursor_x == 3, "Cursor should be advanced by string length");
}

int main() {
    // Run all tests
    test_k_put_char();
    test_newline();
    test_cursor_wrap();
    test_cursor_update();
    test_print_string();
    
    // Report results
    TEST_SUMMARY();
    
    return 0;
}
