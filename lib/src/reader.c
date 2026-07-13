/*
reader.c

Кучуков Ридаль Радикович
МК-101
*/
#include "reader.h"
#include <string.h>

int reader_init(ReaderState *state, FILE *in, const HexDumpConfig *cfg)
{
    if (state == NULL || in == NULL || cfg == NULL)
    {
        return -1;
    }

    if (cfg->chunk_size <= 0 || cfg->chunk_size > READER_MAX_CHUNK_SIZE || cfg->chunk_count <= 0)
    {
        return -1;
    }

    if (fseek(in, 0, SEEK_END) != 0) // прыгаем в конец
    {
        return -1;
    }
    long file_size = ftell(in); // получаем размер файла фтел ретернит текущая позиция курсора в байтах от начала файла
    if (file_size < 0)
    {
        return -1;
    }

    if (cfg->offset < 0 || cfg->offset > file_size)
    {
        return -1;
    }

    if (fseek(in, cfg->offset, SEEK_SET) != 0) // сдвигаемся на фосет
    {
        return -1;
    }

    long remaining_in_file = file_size - cfg->offset;                                                    // остаток в файле
    long bytes_left = (cfg->size < 0) ? remaining_in_file                                                // если не задан то все оставшее выводить
                                      : (cfg->size < remaining_in_file ? cfg->size : remaining_in_file); // либо все оставшееся либо что ввел пользователь
    // инициализация структурки
    state->in = in;
    state->cfg = cfg;
    state->bytes_left = bytes_left;
    state->current_offset = cfg->offset;
    state->line_idx = 0;

    return 0;
}

int reader_next_line(ReaderState *state, Line *out_line)
{
    if (state == NULL || out_line == NULL || out_line->chunks == NULL)
    {
        return -1;
    }

    if (state->bytes_left <= 0)
    {
        return 0;
    }

    const HexDumpConfig *cfg = state->cfg;
    int n_chunks = 0;

    out_line->line_index = state->line_idx;
    out_line->offset = state->current_offset;

    for (int i = 0; i < cfg->chunk_count; i++)
    {
        if (state->bytes_left <= 0)
        {
            break;
        }

        Chunk *chunk = &out_line->chunks[i];
        memset(chunk->bytes, 0, sizeof(chunk->bytes));

        long to_read = cfg->chunk_size;
        if (to_read > state->bytes_left)
        {
            to_read = state->bytes_left;
        }

        size_t actually_read = fread(chunk->bytes, 1, (size_t)to_read, state->in);
        chunk->valid_bytes = (int)actually_read;

        state->bytes_left -= (long)actually_read;
        state->current_offset += (long)actually_read;

        n_chunks++;

        if ((long)actually_read < to_read)
        {
            break;
        }
    }

    out_line->n_chunks = n_chunks;
    state->line_idx++;

    return n_chunks > 0 ? 1 : 0;
}
