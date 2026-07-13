/*
заголовок для hexdump.c

Кучуков Ридаль Радикович
МК-101
*/

#ifndef HEXDUMP_H
#define HEXDUMP_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#endif

typedef struct {
    const char *file_name; //-i путь к файлу
    const char *dir_name; // -d путь к директории
    long        offset; // -o смещение 
    long        size; // -l сколько байт вывести
    int         chunk_size; // -g размер одного кусочка
    int         chunk_count; // -n сколько кусочков выводить в одной строке
    const char *format; // -f пользовательский формат
} HexDumpConfig;

int hexdump_process(const HexDumpConfig *cfg, FILE *out);

int hexdump_process_dir(const HexDumpConfig *cfg, FILE *out);

#endif // HEXDUMP_H