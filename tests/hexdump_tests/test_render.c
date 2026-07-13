#include "unity.h"
#include "render.h"
#include "reader.h"
#include "hexdump.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char *read_all_from_tmpfile(FILE *f, size_t *out_len)
{
    fflush(f);
    long size = ftell(f);
    rewind(f);

    char *buf = malloc((size_t)size + 1);
    size_t read_bytes = fread(buf, 1, (size_t)size, f);
    buf[read_bytes] = '\0';

    if (out_len)
    {
        *out_len = read_bytes;
    }
    return buf;
}

void test_render_full_line_no_padding(void)
{
    Chunk chunks[8];
    const uint8_t letters[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    for (int i = 0; i < 8; i++)
    {
        chunks[i].bytes[0] = letters[i];
        chunks[i].valid_bytes = 1;
    }
    Line line = { .line_index = 0, .offset = 0, .chunks = chunks, .n_chunks = 8 };

    HexDumpConfig cfg = {
        .file_name = NULL, .dir_name = NULL,
        .offset = 0, .size = -1,
        .chunk_size = 1, .chunk_count = 8,
        .format = NULL
    };

    FILE *tmp = tmpfile();
    TEST_ASSERT_NOT_NULL(tmp);

    int r = render_default_line(tmp, &line, &cfg);
    TEST_ASSERT_EQUAL_INT(0, r);

    char *out = read_all_from_tmpfile(tmp, NULL);

    const char *expected =
        "00000000  " "41 42 43 44 45 46 47 48" " | " "ABCDEFGH" "\n";

    TEST_ASSERT_EQUAL_STRING(expected, out);

    free(out);
    fclose(tmp);
}

void test_render_short_line_padding(void)
{
    Chunk chunks[2];
    chunks[0].bytes[0] = 0x41;
    chunks[0].valid_bytes = 1;
    Line line = { .line_index = 0, .offset = 0, .chunks = chunks, .n_chunks = 1 };

    HexDumpConfig cfg = {
        .file_name = NULL, .dir_name = NULL,
        .offset = 0, .size = -1,
        .chunk_size = 1, .chunk_count = 2,
        .format = NULL
    };

    FILE *tmp = tmpfile();
    TEST_ASSERT_NOT_NULL(tmp);

    int r = render_default_line(tmp, &line, &cfg);
    TEST_ASSERT_EQUAL_INT(0, r);

    char *out = read_all_from_tmpfile(tmp, NULL);

    const char *expected = "00000000  " "41" "   " " | " "A" "\n";

    TEST_ASSERT_EQUAL_STRING(expected, out);

    free(out);
    fclose(tmp);
}

void test_render_multi_byte_chunk_no_char_section(void)
{
    Chunk chunks[2];
    chunks[0].bytes[0] = 0x41; chunks[0].bytes[1] = 0x42; chunks[0].valid_bytes = 2;
    chunks[1].bytes[0] = 0x43; chunks[1].bytes[1] = 0x44; chunks[1].valid_bytes = 2;
    Line line = { .line_index = 0, .offset = 0, .chunks = chunks, .n_chunks = 2 };

    HexDumpConfig cfg = {
        .file_name = NULL, .dir_name = NULL,
        .offset = 0, .size = -1,
        .chunk_size = 2, .chunk_count = 2,
        .format = NULL
    };

    FILE *tmp = tmpfile();
    TEST_ASSERT_NOT_NULL(tmp);

    int r = render_default_line(tmp, &line, &cfg);
    TEST_ASSERT_EQUAL_INT(0, r);

    char *out = read_all_from_tmpfile(tmp, NULL);

    const char *expected = "00000000  " "4241 4443" "\n";

    TEST_ASSERT_EQUAL_STRING(expected, out);

    free(out);
    fclose(tmp);
}

void test_render_spec_example_second_line(void)
{
    Chunk chunks[16];
    chunks[0].bytes[0] = 0x33; chunks[0].valid_bytes = 1;
    chunks[1].bytes[0] = 0x34; chunks[1].valid_bytes = 1;
    Line line = { .line_index = 1, .offset = 16, .chunks = chunks, .n_chunks = 2 };

    HexDumpConfig cfg = {
        .file_name = NULL, .dir_name = NULL,
        .offset = 0, .size = -1,
        .chunk_size = 1, .chunk_count = 16,
        .format = NULL
    };

    FILE *tmp = tmpfile();
    TEST_ASSERT_NOT_NULL(tmp);

    int r = render_default_line(tmp, &line, &cfg);
    TEST_ASSERT_EQUAL_INT(0, r);

    char *out = read_all_from_tmpfile(tmp, NULL);
    char padding[43];
    memset(padding, ' ', 42);
    padding[42] = '\0';

    char expected[128];
    snprintf(expected, sizeof(expected), "00000010  33 34%s | 34\n", padding);

    TEST_ASSERT_EQUAL_STRING(expected, out);

    free(out);
    fclose(tmp);
}
void test_render_custom_plain_text(void)
{
    Chunk chunks[1];
    chunks[0].bytes[0] = 0x11;
    chunks[0].valid_bytes = 1;
    Line line = { .line_index = 0, .offset = 0, .chunks = chunks, .n_chunks = 1 };

    HexDumpConfig cfg = {
        .chunk_size = 1, .chunk_count = 1,
        .format = "Hello World"
    };

    FILE *tmp = tmpfile();
    TEST_ASSERT_NOT_NULL(tmp);

    int r = render_custom_line(tmp, &line, &cfg);
    TEST_ASSERT_EQUAL_INT(0, r);

    char *out = read_all_from_tmpfile(tmp, NULL);
    TEST_ASSERT_EQUAL_STRING("Hello World", out);

    free(out);
    fclose(tmp);
}

void test_render_custom_index_and_offset(void)
{
    Chunk chunks[1];
    chunks[0].bytes[0] = 0xAA;
    chunks[0].valid_bytes = 1;
    Line line = { .line_index = 4, .offset = 64, .chunks = chunks, .n_chunks = 1 };

    HexDumpConfig cfg = {
        .chunk_size = 1, .chunk_count = 1,
        .format = "Строка %i имеет смещение %n"
    };

    FILE *tmp = tmpfile();
    TEST_ASSERT_NOT_NULL(tmp);

    int r = render_custom_line(tmp, &line, &cfg);
    TEST_ASSERT_EQUAL_INT(0, r);

    char *out = read_all_from_tmpfile(tmp, NULL);
    
    TEST_ASSERT_EQUAL_STRING("Строка 4 имеет смещение 00000040", out);

    free(out);
    fclose(tmp);
}

void test_render_custom_chunk_formatting(void)
{
    Chunk chunks[2];
    chunks[0].bytes[0] = 0x41; chunks[0].valid_bytes = 1; // 'A'
    chunks[1].bytes[0] = 0x42; chunks[1].valid_bytes = 1; // 'B'
    Line line = { .line_index = 0, .offset = 0, .chunks = chunks, .n_chunks = 2 };

    HexDumpConfig cfg = {
        .chunk_size = 1, .chunk_count = 2,
        .format = "Chunk0: %0x (%0c), Chunk1: %1x (%1c)"
    };

    FILE *tmp = tmpfile();
    TEST_ASSERT_NOT_NULL(tmp);

    int r = render_custom_line(tmp, &line, &cfg);
    TEST_ASSERT_EQUAL_INT(0, r);

    char *out = read_all_from_tmpfile(tmp, NULL);
    TEST_ASSERT_EQUAL_STRING("Chunk0: 41 (A), Chunk1: 42 (B)", out);

    free(out);
    fclose(tmp);
}
