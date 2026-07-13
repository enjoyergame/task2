/*
заголовок для render.c

Кучуков Ридаль Радикович
МК-101
*/
#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hexdump.h"
#include "reader.h"

int render_custom_line(FILE *out, const Line *line, const HexDumpConfig *cfg);
int render_default_line(FILE *out, const Line *line, const HexDumpConfig *cfg);

#endif // RENDER_H