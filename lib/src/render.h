/*
заголовок для render.c

Кучуков Ридаль Радикович
МК-101
*/
#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
#include "hexdump.h"
#include "reader.h"

int render_default_line(FILE *out, const Line *line, const HexDumpConfig *cfg);

#endif // RENDER_H