/*
main.c

Кучуков Ридаль Радикович
МК-101
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "hexdump.h"
#include <bits/getopt_core.h>

static void print_usage(const char *prog_name)
{
    printf("Usage: %s [-i file] [-d directory] [-o offset] [-l size] [-g chunk_size] [-n chunk_count] [-f format]\n", prog_name);
}

int main(int argc, char *argv[])
{
    HexDumpConfig cfg = {
        .file_name = NULL,
        .dir_name = NULL,
        .offset = 0,
        .size = -1,
        .chunk_size = 1,
        .chunk_count = 16,
        .format = NULL
    };

    int opt;
    while ((opt = getopt(argc, argv, "i:o:l:g:n:d:f:")) != -1)
    {
        switch (opt)
        {
            case 'i': cfg.file_name = optarg; break;
            case 'd': cfg.dir_name = optarg; break;
            case 'o': cfg.offset = strtol(optarg, NULL, 0); break;
            case 'l': cfg.size = strtol(optarg, NULL, 0); break;
            case 'g': cfg.chunk_size = atoi(optarg); break;
            case 'n': cfg.chunk_count = atoi(optarg); break;
            case 'f': cfg.format = optarg; break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    if (cfg.dir_name != NULL)
    {
        if (hexdump_process_dir(&cfg, stdout) != 0)
        {
            return 1;
        }
    }
    else if (cfg.file_name != NULL)
    {
        if (hexdump_process(&cfg, stdout) != 0)
        {
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Error: you must specify a file (-i) or directory (-d).\n");
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}