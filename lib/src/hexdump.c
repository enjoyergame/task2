/*
hexdump.c

Кучуков Ридаль Радикович
МК-101
*/

#include "hexdump.h"
#include "reader.h"
#include "render.h"

static int is_same_file(const char *path1, const char *path2)
{
    if (path1 == NULL || path2 == NULL)
    {
        return 0;
    }

    if (strcmp(path1, path2) == 0)
    {
        return 1;
    }

#ifndef _WIN32
    struct stat st1, st2;
    if (stat(path1, &st1) == 0 && stat(path2, &st2) == 0)
    {
        return (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino);
    }
#else
    char full1[MAX_PATH];
    char full2[MAX_PATH];
    if (_fullpath(full1, path1, MAX_PATH) != NULL &&
        _fullpath(full2, path2, MAX_PATH) != NULL)
    {
        return (_stricmp(full1, full2) == 0);
    }
#endif

    return 0;
}

int hexdump_process(const HexDumpConfig *cfg, FILE *out)
{
    if (cfg == NULL || cfg->file_name == NULL || out == NULL)
    {
        return -1;
    }

    FILE *in = fopen(cfg->file_name, "rb");
    if (in == NULL)
    {
        fprintf(stderr, "Error opening file: %s\n", cfg->file_name);
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
        fprintf(stderr, "Error opening directory: %s\n", cfg->dir_name);
        return -1;
    }

    int file_printed = 0;
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
            if (cfg->file_name != NULL && is_same_file(cfg->file_name, path))
            {
                file_printed = 1;
            }

            HexDumpConfig file_cfg = *cfg;
            file_cfg.file_name = path;
            hexdump_process(&file_cfg, out);
        }
    }

    closedir(dir);

    if (cfg->file_name != NULL && !file_printed)
    {
        fprintf(out, "\n==> %s <==\n", cfg->file_name);
        hexdump_process(cfg, out);
    }

    return 0;
}