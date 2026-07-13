/*
render.c

Кучуков Ридаль Радикович
МК-101
*/
#include "render.h"
#include "hexconv.h"

int render_default_line(FILE *out, const Line *line, const HexDumpConfig *cfg)
{
    if (out == NULL || line == NULL || cfg == NULL)
    {
        return -1;
    }

    fprintf(out, "%08lX  ", (unsigned long)line->offset);

    for (int i = 0; i < cfg->chunk_count; i++)
    {
        if (i > 0)
        {
            fputc(' ', out);
        }

        if (i < line->n_chunks)
        {
            char hex[READER_MAX_CHUNK_SIZE * 2 + 1];
            chunk_to_hex(line->chunks[i].bytes, cfg->chunk_size,
                         line->chunks[i].valid_bytes, hex);
            fputs(hex, out);
        }
        else
        {
            for (int j = 0; j < cfg->chunk_size * 2; j++)
            {
                fputc(' ', out);
            }
        }
    }

    if (cfg->chunk_size == 1)
    {
        fputs(" | ", out);
        for (int i = 0; i < line->n_chunks; i++)
        {
            char ch[2];
            chunk_to_char(line->chunks[i].bytes, 1,
                          line->chunks[i].valid_bytes, ch);
            fputc(ch[0], out);
        }
    }

    fputc('\n', out);

    return 0;
}