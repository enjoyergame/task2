/*
hexdump.c

Кучуков Ридаль Радикович
МК-101
*/
#include "hexdump.h"
#include "reader.h"
#include "render.h"
#include <stdio.h>
#include <stdlib.h>

int hexdump_process(const HexDumpConfig *cfg, FILE *out)
{
    if (cfg == NULL || cfg->file_name == NULL || out == NULL)
    {
        return -1;
    }

    FILE *in = fopen(cfg->file_name, "rb");
    if (in == NULL)
    {
        fprintf(stderr, "Ошибка открытия файла: %s\n", cfg->file_name);
        return -1;
    }

    ReaderState state;
    if (reader_init(&state, in, cfg) != 0)
    {
        fclose(in);
        return -1;
    }

    Chunk *chunks = (Chunk *)calloc((size_t)cfg->chunk_count, sizeof(Chunk));
    if (chunks == NULL)
    {
        fclose(in);
        return -1;
    }

    Line line;
    line.chunks = chunks;

    while (reader_next_line(&state, &line) > 0)
    {
        if (cfg->format != NULL)
        {
            render_custom_line(out, &line, cfg);
        }
        else
        {
            render_default_line(out, &line, cfg);
        }
    }

    free(chunks);
    fclose(in);
    return 0;
}