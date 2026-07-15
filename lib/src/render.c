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

int render_custom_line(FILE *out, const Line *line, const HexDumpConfig *cfg)
{
    if (out == NULL || line == NULL || cfg == NULL || cfg->format == NULL)
    {
        return -1;
    }

    const char *p = cfg->format;
    while (*p != '\0')
    {
        if (*p == '\\')
        {
            p++;
            if (*p == 'n')      { fputc('\n', out); p++; }
            else if (*p == 'r') { fputc('\r', out); p++; }
            else if (*p == 't') { fputc('\t', out); p++; }
            else if (*p == '\\'){ fputc('\\', out); p++; }
            else if (*p != '\0'){ p++; }
        }
        else if (*p == '%')
        {
            p++;
            if (*p == 'i')
            {
                fprintf(out, "%ld", line->line_index);
                p++;
            }
            else if (*p == 'n')
            {
                fprintf(out, "%08lX", (unsigned long)line->offset);
                p++;
            }
            else
            {
                char *endptr;
                long chunk_idx = strtol(p, &endptr, 10);
                
                // Проверяем, что после числа идет 'x' или 'c'
                if (endptr != p && (*endptr == 'x' || *endptr == 'c'))
                {
                    char type = *endptr;
                    p = endptr + 1; // сдвигаемся за спецификатор

                    // Подготавливаем пустой чанк на случай, если индекс выходит за пределы
                    Chunk dummy;
                    memset(&dummy, 0, sizeof(dummy));
                    dummy.valid_bytes = 0;
                    const Chunk *target_chunk = &dummy;

                    if (chunk_idx >= 0 && chunk_idx < line->n_chunks)
                    {
                        target_chunk = &line->chunks[chunk_idx];
                    }

                    if (type == 'x')
                    {
                        char hex[READER_MAX_CHUNK_SIZE * 2 + 1];
                        chunk_to_hex(target_chunk->bytes, cfg->chunk_size, target_chunk->valid_bytes, hex);//hexconv.c
                        fputs(hex, out);
                    }
                    else if (type == 'c')
                    {
                        char ch[READER_MAX_CHUNK_SIZE + 1];
                        chunk_to_char(target_chunk->bytes, cfg->chunk_size, target_chunk->valid_bytes, ch);
                        fputs(ch, out);
                    }
                }
                else
                {
                    // Если после % не валидный спецификатор, выводим % как есть и идем дальше
                    fputc('%', out);
                }
            }
        }
        else
        {
            fputc(*p, out);
            p++;
        }
    }
    return 0;
}