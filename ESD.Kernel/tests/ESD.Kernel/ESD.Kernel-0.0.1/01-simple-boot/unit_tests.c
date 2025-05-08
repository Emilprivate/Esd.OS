#include <stdio.h>
#include <stdio.h>
#include "../../../test-tools/test-framework.h"

// Mock the VGA memory buffer for testing
unsigned char mock_vidmem[80*25*2];

// Functions to test from 01-simple-boot
void k_put_char(char c, int col, int row, unsigned char attr) {
    unsigned char *vidmem = mock_vidmem;
    int offset;

    if (col >= 80 || row >= 25) {
        return; 
    }

    offset = (row * 80 + col) * 2;
    vidmem[offset] = c;
    vidmem[offset + 1] = attr;
}

// Initialize the test framework
test_result_t test_results[100];
int test_count = 0;
int pass_count = 0;
int fail_count = 0;

void test_init() {
    // Clear mock video memory
    for (int i = 0; i < 80*25*2; i++) {
        mock_vidmem[i] = 0;
    }
    
    // Always print something visible when test init is called
    printf("Initializing test environment...\n");
}

void test_report_start(const char* test_name) {
    // Store test name
    test_results[test_count-1].name = test_name;
    
    // Print to stdout with visible decoration
    printf("\n==== TEST CASE: %s ====\n", test_name);
}

void test_report_result(int result, const char* message) {
    // Store the result
    test_results[test_count-1].result = result;
    test_results[test_count-1].message = message;
    
    // Print to stdout with clear indication of result
    if (result == TEST_PASS) {
        printf("  ✓ PASS: %s\n", message);
    } else if (result == TEST_FAIL) {
        printf("  ✗ FAIL: %s\n", message);
    } else {
        printf("  ? INFO: %s\n", message);
    }
    
    // Flush stdout to ensure output is visible
    fflush(stdout);
}

void test_report_summary() {
    // Print summary information
    printf("\n====== TEST SUMMARY ======\n");
    printf("Total tests: %d\n", test_count);
    printf("Passed: %d\n", pass_count);
    printf("Failed: %d\n", fail_count);
    
    if (fail_count == 0) {
        printf("\n✓✓✓ ALL TESTS PASSED! ✓✓✓\n");
    } else {
        printf("\n✗✗✗ %d of %d TESTS FAILED! ✗✗✗\n", fail_count, test_count);
    }
    
    // Make sure the summary is visible
    fflush(stdout);
}

// Test k_put_char function
void test_k_put_char() {
    TEST_CASE("k_put_char places character correctly in video memory");
    
    // Reset mock memory
    test_init();
    
    // Test placing a character
    char test_char = 'X';
    int test_col = 10;
    int test_row = 5;
    unsigned char test_attr = 0x0F;
    
    k_put_char(test_char, test_col, test_row, test_attr);
    
    // Calculate the expected offset in video memory
    int offset = (test_row * 80 + test_col) * 2;
    
    // Check character was placed correctly
    TEST_ASSERT(mock_vidmem[offset] == test_char, 
                "Character should be placed at the correct offset");
    
    // Check attribute was set correctly
    TEST_ASSERT(mock_vidmem[offset + 1] == test_attr,
                "Attribute should be set at the correct offset");
}

// Test k_put_char bounds checking
void test_k_put_char_bounds() {
    TEST_CASE("k_put_char bounds checking");
    
    // Reset mock memory
    test_init();
    
    // Test out-of-bounds column
    k_put_char('X', 80, 0, 0x0F);  // column 80 is out of bounds (0-79 is valid)
    
    // Test out-of-bounds row
    k_put_char('X', 0, 25, 0x0F);  // row 25 is out of bounds (0-24 is valid)
    
    // Nothing should have changed in video memory
    TEST_ASSERT(mock_vidmem[0] == 0, 
                "Nothing should be written when column is out of bounds");
    TEST_ASSERT(mock_vidmem[25 * 80 * 2] == 0, 
                "Nothing should be written when row is out of bounds");
}

int main() {
    // Run all tests
    test_k_put_char();
    test_k_put_char_bounds();
    
    // Report results
    TEST_SUMMARY();
    
    return 0;
}
