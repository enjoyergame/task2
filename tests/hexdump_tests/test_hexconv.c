/*
tests

Кучуков Ридаль Радикович
МК-101
*/

#include "unity.h"
#include "hexconv.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_byte_to_hex_basic(void)
{
    char out[2];

    byte_to_hex(0x00, out);
    TEST_ASSERT_EQUAL_CHAR('0', out[0]);
    TEST_ASSERT_EQUAL_CHAR('0', out[1]);

    byte_to_hex(0xFF, out);
    TEST_ASSERT_EQUAL_CHAR('F', out[0]);
    TEST_ASSERT_EQUAL_CHAR('F', out[1]);

    byte_to_hex(0xAB, out);
    TEST_ASSERT_EQUAL_CHAR('A', out[0]);
    TEST_ASSERT_EQUAL_CHAR('B', out[1]);
}

void test_chunk_to_hex_size1_no_reverse(void)
{
    uint8_t bytes[1] = {0x10};
    char out[3];

    chunk_to_hex(bytes, 1, 1, out);
    TEST_ASSERT_EQUAL_STRING("10", out);
}

void test_chunk_to_hex_size3_reversed(void)
{
    uint8_t bytes[3] = {0x13, 0xFA, 0xFB};
    char out[7];

    chunk_to_hex(bytes, 3, 3, out);
    TEST_ASSERT_EQUAL_STRING("FBFA13", out);
}

void test_chunk_to_hex_size4_reversed(void)
{
    uint8_t bytes[2] = {0x41, 0x42};
    char out[5];

    chunk_to_hex(bytes, 2, 2, out);
    TEST_ASSERT_EQUAL_STRING("4241", out);
}

void test_chunk_to_char_direct_order(void)
{
    uint8_t bytes[2] = {0x41, 0x42};
    char out[3];

    chunk_to_char(bytes, 2, 2, out);
    TEST_ASSERT_EQUAL_STRING("AB", out);
}

void test_chunk_to_char_non_printable(void)
{
    uint8_t bytes[4] = {0x00, 0x20, 0xFA, 0x41};
    char out[5];

    chunk_to_char(bytes, 4, 4, out);
    TEST_ASSERT_EQUAL_STRING(". .A", out);
}

void test_chunk_to_char_partial_valid_bytes(void)
{
    uint8_t bytes[2] = {0x41, 0x00};
    char out[3];

    chunk_to_char(bytes, 2, 1, out);
    TEST_ASSERT_EQUAL_STRING("A.", out);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_byte_to_hex_basic);
    RUN_TEST(test_chunk_to_hex_size1_no_reverse);
    RUN_TEST(test_chunk_to_hex_size3_reversed);
    RUN_TEST(test_chunk_to_hex_size4_reversed);
    RUN_TEST(test_chunk_to_char_direct_order);
    RUN_TEST(test_chunk_to_char_non_printable);
    RUN_TEST(test_chunk_to_char_partial_valid_bytes);

    return UNITY_END();
}