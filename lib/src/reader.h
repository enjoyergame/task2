/*
заголовок для reader.c

Кучуков Ридаль Радикович
МК-101
*/
#ifndef READER_H
#define READER_H

#include <stdio.h>
#include "hexdump.h"

#define READER_MAX_CHUNK_SIZE 16

typedef struct {
    uint8_t bytes[READER_MAX_CHUNK_SIZE];
    int     valid_bytes;
} Chunk;

typedef struct {
    long   line_index;
    long   offset;
    Chunk *chunks;
    int    n_chunks;
} Line;

typedef struct {
    FILE *in;
    const HexDumpConfig *cfg;
    long  bytes_left;
    long  current_offset;
    long  line_idx;
} ReaderState;

int reader_init(ReaderState *state, FILE *in, const HexDumpConfig *cfg);

int reader_next_line(ReaderState *state, Line *out_line);

#endif // READER_H