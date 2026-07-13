/*
hexdump.c

Кучуков Ридаль Радикович
МК-101
*/
#include "hexdump.h"
#include "reader.h"
#include "render.h"


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

int hexdump_process_dir(const HexDumpConfig *cfg, FILE *out)
{
    if (cfg == NULL || cfg->dir_name == NULL || out == NULL)
    {
        return -1;
    }

    DIR *dir = opendir(cfg->dir_name);
    if (dir == NULL)
    {
        fprintf(stderr, "Ошибка открытия директории: %s\n", cfg->dir_name);
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", cfg->dir_name, entry->d_name);

        struct stat st;
        if (stat(path, &st) == 0 && S_ISREG(st.st_mode))
        {
            fprintf(out, "\n==> %s <==\n", path);
            
            // Создаем копию конфига, подменяем имя файла и передаем в ядро
            HexDumpConfig file_cfg = *cfg;
            file_cfg.file_name = path;
            hexdump_process(&file_cfg, out);
        }
    }

    closedir(dir);
    return 0;
}