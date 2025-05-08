#include <stdio.h>
#include "../../../test-tools/test-framework.h"
#include <stdio.h>  // Add stdio for printf

// Mocked hardware and memory
unsigned char mock_vidmem[80*25*2];
unsigned char mock_ports[256];
unsigned char mock_input_buffer[256]; // For simulating keyboard input
int mock_input_index = 0;

// Mock I/O functions
void outb(unsigned short port, unsigned char val) {
    mock_ports[port] = val;
}

unsigned char inb(unsigned short port) {
    // Special handling for keyboard data port
    if (port == 0x60) {
        return mock_input_buffer[mock_input_index++];
    }
    // Special handling for keyboard status port
    if (port == 0x64) {
        // Return 1 if we have data in the buffer
        return (mock_input_buffer[mock_input_index] != 0) ? 1 : 0;
    }
    return mock_ports[port];
}

// Constants from keyboard driver
#define KEY_BACKSPACE 0x0E
#define KEY_CTRL      0x1D
#define KEY_LEFT_ALT  0x38
#define KEY_ENTER     0x1C
#define KEY_SPACE     0x39

// From simple_kernel.c
char scancode_to_char(unsigned char scancode) {
    if (scancode == KEY_BACKSPACE) return '\b';
    if (scancode == KEY_CTRL) return 0;
    if (scancode == KEY_LEFT_ALT) return 0;
    
    if (scancode == 0x1E) return 'a';
    // ... other key mappings would be here
    if (scancode == KEY_SPACE) return ' ';
    if (scancode == KEY_ENTER) return '\n';
    return 0;
}

// Simplified polling keyboard function for testing
char poll_keyboard_once() {
    if (inb(0x64) & 1) {
        unsigned char scancode = inb(0x60);
        
        if (!(scancode & 0x80)) { // Key press, not release
            return scancode_to_char(scancode);
        }
    }
    return 0;
}

// Test framework setup
test_result_t test_results[100];
int test_count = 0;
int pass_count = 0;
int fail_count = 0;

void test_init() {
    // Clear mocks
    for (int i = 0; i < 80*25*2; i++) mock_vidmem[i] = 0;
    for (int i = 0; i < 256; i++) mock_ports[i] = 0;
    for (int i = 0; i < 256; i++) mock_input_buffer[i] = 0;
    mock_input_index = 0;
}

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

// Tests for scancode mapping
void test_scancode_to_char() {
    TEST_CASE("scancode_to_char maps keyboard scancodes to correct characters");
    
    test_init();
    
    TEST_ASSERT(scancode_to_char(0x1E) == 'a', "Scancode 0x1E should map to 'a'");
    TEST_ASSERT(scancode_to_char(KEY_SPACE) == ' ', "Space key should map to space character");
    TEST_ASSERT(scancode_to_char(KEY_ENTER) == '\n', "Enter key should map to newline");
    TEST_ASSERT(scancode_to_char(KEY_BACKSPACE) == '\b', "Backspace should map to special character");
    TEST_ASSERT(scancode_to_char(KEY_CTRL) == 0, "Ctrl key should not map to a character");
}

// Test keyboard polling
void test_keyboard_polling() {
    TEST_CASE("poll_keyboard_once reads keyboard input correctly");
    
    test_init();
    
    // Simulate 'a' key press in buffer
    mock_input_buffer[0] = 0x1E; // 'a' scancode
    
    char c = poll_keyboard_once();
    
    TEST_ASSERT(c == 'a', "Should read 'a' character from keyboard");
    TEST_ASSERT(mock_input_index == 1, "Buffer index should be advanced");
}

// Test key release filtering
void test_key_release_filtering() {
    TEST_CASE("poll_keyboard_once ignores key releases");
    
    test_init();
    
    // Simulate 'a' key release in buffer (high bit set)
    mock_input_buffer[0] = 0x1E | 0x80;
    
    char c = poll_keyboard_once();
    
    TEST_ASSERT(c == 0, "Should ignore key release (high bit set)");
}

// Test multiple keys
void test_multiple_key_presses() {
    TEST_CASE("poll_keyboard_once handles multiple key presses");
    
    test_init();
    
    // Simulate multiple keys in buffer
    mock_input_buffer[0] = 0x1E; // 'a'
    mock_input_buffer[1] = 0x30; // 'b'
    
    char c1 = poll_keyboard_once();
    char c2 = poll_keyboard_once();
    
    TEST_ASSERT(c1 == 'a', "First keypress should be 'a'");
    TEST_ASSERT(c2 == 'b', "Second keypress should be 'b'");
}

int main() {
    // Run all tests
    test_scancode_to_char();
    test_keyboard_polling();
    test_key_release_filtering();
    test_multiple_key_presses();
    
    // Report results
    TEST_SUMMARY();
    
    return 0;
}
