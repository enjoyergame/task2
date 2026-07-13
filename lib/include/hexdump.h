/*
заголовок для hexdump.c

Кучуков Ридаль Радикович
МК-101
*/

#ifndef HEXDUMP_H
#define HEXDUMP_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
    const char *file_name;
    const char *dir_name;
    long        offset;
    long        size;
    int         chunk_size;
    int         chunk_count;
    const char *format;
} HexDumpConfig;

int hexdump_process(const HexDumpConfig *cfg, FILE *out);

#endif // HEXDUMP_H