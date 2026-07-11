#ifndef HEXCONV_H
#define HEXCONV_H

#include <stddef.h>
#include <stdint.h>

void byte_to_hex(uint8_t byte, char out[2]);

void chunk_to_hex(const uint8_t *bytes, int chunk_size, int valid_bytes, char *out);

void chunk_to_char(const uint8_t *bytes, int chunk_size, int valid_bytes, char *out);

#endif