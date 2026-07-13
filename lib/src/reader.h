/*
заголовок для reader.c

Кучуков Ридаль Радикович
МК-101
*/
#ifndef READER_H
#define READER_H

#include <stdio.h>
#include "hexdump.h"

#define READER_MAX_CHUNK_SIZE 16 // чтоб все влезло

typedef struct
{
    uint8_t bytes[READER_MAX_CHUNK_SIZE]; // все данные
    int valid_bytes;                      // реальные данные
} Chunk;

typedef struct
{
    long line_index; // номер строки
    long offset;     // смещение
    Chunk *chunks;   // массив кусочков
    int n_chunks; // количество успешно прочитанных кусочков 
} Line;

typedef struct
{
    FILE *in;
    const HexDumpConfig *cfg;
    long bytes_left;     // сколько байт еще осталось прочитать
    long current_offset; // текущая смещение в файле
    long line_idx;       // счетчик строк
} ReaderState;

int reader_init(ReaderState *state, FILE *in, const HexDumpConfig *cfg);

int reader_next_line(ReaderState *state, Line *out_line);

#endif // READER_H