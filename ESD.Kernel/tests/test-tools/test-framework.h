#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

#define TEST_COLOR_GREEN 0x0A
#define TEST_COLOR_RED 0x0C
#define TEST_COLOR_YELLOW 0x0E
#define TEST_COLOR_WHITE 0x0F

#define TEST_PASS 0
#define TEST_FAIL 1
#define TEST_SKIP 2
#define TEST_CRASH 3 

typedef struct {
    const char* name;
    int result;
    const char* message;
} test_result_t;

extern test_result_t test_results[100];
extern int test_count;
extern int pass_count;
extern int fail_count;

void test_init();
void test_report_start(const char* test_name);
void test_report_result(int result, const char* message);
void test_report_summary();

#define TEST_ASSERT(condition, message) \
    if (condition) { \
        test_report_result(TEST_PASS, "PASS: " message); \
        pass_count++; \
    } else { \
        test_report_result(TEST_FAIL, "FAIL: " message); \
        fail_count++; \
    }

#define TEST_EXPECT_CRASH(code, message) \
    test_report_result(TEST_SKIP, "SKIP: Cannot safely test crash: " message);

#define TEST_CASE(name) \
    test_count++; \
    test_report_start(name);

#define TEST_SUMMARY() \
    test_report_summary();

#endif
