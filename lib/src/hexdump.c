/*
hexdump.c

Кучуков Ридаль Радикович
МК-101
*/

#include "hexdump.h"
#include "reader.h"
#include "render.h"

// функия проверки тот же файл
static int is_same_file(const char *path1, const char *path2)
{

    if (path1 == NULL || path2 == NULL)
    {
        return 0; // не тру
    }

    if (strcmp(path1, path2) == 0) // строки равны
    {
        return 1; // типа тру
    }

#ifndef _WIN32
    struct stat st1, st2;                                 // stat - метаданные
    if (stat(path1, &st1) == 0 && stat(path2, &st2) == 0) // если все норм и мы получили данные
    {
        return (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino); // 1 если все совпадает
    }
#else
    char full1[MAX_PATH];//max path определен в windows.h
    char full2[MAX_PATH];
    if (_fullpath(full1, path1, MAX_PATH) != NULL &&
        _fullpath(full2, path2, MAX_PATH) != NULL)//_fullpath получает полный путь
    {
        return (_stricmp(full1, full2) == 0);//возвращаем 1 если пути совпадают
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

    ReaderState state; //структурка из reader.h
    if (reader_init(&state, in, cfg) != 0)
    {
        fclose(in);
        return -1;
    }

    Chunk *chunks = (Chunk *)calloc((size_t)cfg->chunk_count, sizeof(Chunk)); //массив кусочков
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
    if (cfg == NULL || cfg->dir_name == NULL || out == NULL) // проверка
    {
        return -1;
    }
    DIR *dir = opendir(cfg->dir_name); // указатель на поток директории как FILE*
    // dir* - структурка из #include <dirent.h> она вродь под линукс но гцц и под винду соберет
    if (dir == NULL)
    {
        fprintf(stderr, "Error opening directory: %s\n", cfg->dir_name);
        return -1;
    }

    int file_printed = 0; // флажок для -i и -d одновременно
    struct dirent *entry; // указатель на структуру с инфой о папке из дирент.h

    while ((entry = readdir(dir)) != NULL) // readdir(dir) - отдает имя файла и перемещается к следующему
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) // пропускаем текущую и родительскую директории
        {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", cfg->dir_name, entry->d_name); // записывает в path путь до файла не больше чем sizeof(path)
        // stat из <sys/stat.h>
        struct stat st;                                  // структурка для метаданных файла
        if (stat(path, &st) == 0 && S_ISREG(st.st_mode)) // stat идет к файлу по path и записывает инфу в st и проверяем что это стандартный файл
        {
            fprintf(out, "\n== %s ==\n", path);                               // разделить
            if (cfg->file_name != NULL && is_same_file(cfg->file_name, path)) // проверка что файл не равен тому что в -i передается
            {
                file_printed = 1; // флажок что файл из -и уже выведен
            }

            HexDumpConfig file_cfg = *cfg;   // копия конфига
            file_cfg.file_name = path;       // во временный конфиг записываем путь
            hexdump_process(&file_cfg, out); // запуск чтения файла
        }
    }

    closedir(dir);

    if (cfg->file_name != NULL && !file_printed) // если файла не было в директории
    {
        fprintf(out, "\n== %s ==\n", cfg->file_name);
        hexdump_process(cfg, out);
    }

    return 0;
}