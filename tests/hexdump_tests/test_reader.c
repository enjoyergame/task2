/*
тесты для ридера

Кучуков Ридаль Радикович
МК-101
*/
#include "unity.h"
#include "reader.h"
#include "hexdump.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void create_test_file(const char *filename, const uint8_t *content, size_t length)
{
    FILE *f = fopen(filename, "wb");
    TEST_ASSERT_NOT_NULL(f);
    fwrite(content, 1, length, f);
    fclose(f);
}

// файл из примера в задании:
// 0x10 0x11 0x12 0x13 0xFA 0xFB 0xFC 0xFD 0x00 0x20 0x30 0x40 0x41 0x42 0x31 0x32 0x33 0x34
static const uint8_t SAMPLE_FILE[] = {
    0x10, 0x11, 0x12, 0x13, 0xFA, 0xFB, 0xFC, 0xFD,
    0x00, 0x20, 0x30, 0x40, 0x41, 0x42, 0x31, 0x32,
    0x33, 0x34};

void test_reader_basic_full_file(void)
{
    create_test_file("reader_test1.bin", SAMPLE_FILE, sizeof(SAMPLE_FILE));

    FILE *f = fopen("reader_test1.bin", "rb");
    TEST_ASSERT_NOT_NULL(f);

    HexDumpConfig cfg = {
        .file_name = "reader_test1.bin", .dir_name = NULL, .offset = 0, .size = -1, .chunk_size = 1, .chunk_count = 16, .format = NULL};

    ReaderState state;
    TEST_ASSERT_EQUAL_INT(0, reader_init(&state, f, &cfg));

    Chunk chunks[16];
    Line line = {.chunks = chunks};

    int r = reader_next_line(&state, &line);
    TEST_ASSERT_EQUAL_INT(1, r);
    TEST_ASSERT_EQUAL_INT(0, line.line_index);
    TEST_ASSERT_EQUAL(0, line.offset);
    TEST_ASSERT_EQUAL_INT(16, line.n_chunks);
    TEST_ASSERT_EQUAL_HEX8(0x10, line.chunks[0].bytes[0]);
    TEST_ASSERT_EQUAL_HEX8(0x32, line.chunks[15].bytes[0]);

    r = reader_next_line(&state, &line);
    TEST_ASSERT_EQUAL_INT(1, r);
    TEST_ASSERT_EQUAL_INT(1, line.line_index);
    TEST_ASSERT_EQUAL(16, line.offset);
    TEST_ASSERT_EQUAL_INT(2, line.n_chunks);
    TEST_ASSERT_EQUAL_HEX8(0x33, line.chunks[0].bytes[0]);
    TEST_ASSERT_EQUAL_HEX8(0x34, line.chunks[1].bytes[0]);

    r = reader_next_line(&state, &line);
    TEST_ASSERT_EQUAL_INT(0, r);

    fclose(f);
}

void test_reader_offset_and_size_and_group_example(void)
{
    // из задания: ./prog -i file -o 3 -l 8 -g 3
    create_test_file("reader_test2.bin", SAMPLE_FILE, sizeof(SAMPLE_FILE));

    FILE *f = fopen("reader_test2.bin", "rb");
    TEST_ASSERT_NOT_NULL(f);

    HexDumpConfig cfg = {
        .file_name = "reader_test2.bin", .dir_name = NULL, .offset = 3, .size = 8, .chunk_size = 3, .chunk_count = 16, .format = NULL};

    ReaderState state;
    TEST_ASSERT_EQUAL_INT(0, reader_init(&state, f, &cfg));

    Chunk chunks[16];
    Line line = {.chunks = chunks};

    int r = reader_next_line(&state, &line);
    TEST_ASSERT_EQUAL_INT(1, r);
    TEST_ASSERT_EQUAL(3, line.offset);
    TEST_ASSERT_EQUAL_INT(3, line.n_chunks);

    TEST_ASSERT_EQUAL_INT(3, line.chunks[0].valid_bytes);
    TEST_ASSERT_EQUAL_HEX8(0x13, line.chunks[0].bytes[0]);
    TEST_ASSERT_EQUAL_HEX8(0xFA, line.chunks[0].bytes[1]);
    TEST_ASSERT_EQUAL_HEX8(0xFB, line.chunks[0].bytes[2]);

    TEST_ASSERT_EQUAL_INT(3, line.chunks[1].valid_bytes);
    TEST_ASSERT_EQUAL_HEX8(0xFC, line.chunks[1].bytes[0]);
    TEST_ASSERT_EQUAL_HEX8(0xFD, line.chunks[1].bytes[1]);
    TEST_ASSERT_EQUAL_HEX8(0x00, line.chunks[1].bytes[2]);

    TEST_ASSERT_EQUAL_INT(2, line.chunks[2].valid_bytes);
    TEST_ASSERT_EQUAL_HEX8(0x20, line.chunks[2].bytes[0]);
    TEST_ASSERT_EQUAL_HEX8(0x30, line.chunks[2].bytes[1]);
    TEST_ASSERT_EQUAL_HEX8(0x00, line.chunks[2].bytes[2]);

    fclose(f);
}

void test_reader_chunk_count_splits_lines(void)
{
    // ./prog -o 2 -g 4 -n 2 -l 12 -i file
    create_test_file("reader_test3.bin", SAMPLE_FILE, sizeof(SAMPLE_FILE));

    FILE *f = fopen("reader_test3.bin", "rb");
    TEST_ASSERT_NOT_NULL(f);

    HexDumpConfig cfg = {
        .file_name = "reader_test3.bin", .dir_name = NULL, .offset = 2, .size = 12, .chunk_size = 4, .chunk_count = 2, .format = NULL};

    ReaderState state;
    TEST_ASSERT_EQUAL_INT(0, reader_init(&state, f, &cfg));

    Chunk chunks[2];
    Line line = {.chunks = chunks};

    int r = reader_next_line(&state, &line);
    TEST_ASSERT_EQUAL_INT(1, r);
    TEST_ASSERT_EQUAL(2, line.offset);
    TEST_ASSERT_EQUAL_INT(2, line.n_chunks);

    r = reader_next_line(&state, &line);
    TEST_ASSERT_EQUAL_INT(1, r);
    TEST_ASSERT_EQUAL(10, line.offset);
    TEST_ASSERT_EQUAL_INT(1, line.n_chunks);

    r = reader_next_line(&state, &line);
    TEST_ASSERT_EQUAL_INT(0, r);

    fclose(f);
}

void test_reader_invalid_offset(void)
{
    create_test_file("reader_test4.bin", SAMPLE_FILE, sizeof(SAMPLE_FILE));
    FILE *f = fopen("reader_test4.bin", "rb");
    TEST_ASSERT_NOT_NULL(f);

    HexDumpConfig cfg = {
        .file_name = "reader_test4.bin", .dir_name = NULL, .offset = 1000, .size = -1, .chunk_size = 1, .chunk_count = 16, .format = NULL};

    ReaderState state;
    TEST_ASSERT_EQUAL_INT(-1, reader_init(&state, f, &cfg));

    fclose(f);
}