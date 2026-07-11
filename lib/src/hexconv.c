#include "hexconv.h"
#include <ctype.h>

static const char HEX_DIGITS[] = "0123456789ABCDEF";

void byte_to_hex(uint8_t byte, char out[2])
{
    out[0] = HEX_DIGITS[(byte >> 4) & 0x0F];
    out[1] = HEX_DIGITS[byte & 0x0F];
}

void chunk_to_hex(const uint8_t *bytes, int chunk_size, int valid_bytes, char *out)
{
    int pos = 0;

    for (int i = chunk_size - 1; i >= 0; i--)
    {
        uint8_t b = (i < valid_bytes || valid_bytes < 0) ? bytes[i] : 0x00;

        char hex[2];
        byte_to_hex(b, hex);
        out[pos++] = hex[0];
        out[pos++] = hex[1];
    }

    out[pos] = '\0';
}

void chunk_to_char(const uint8_t *bytes, int chunk_size, int valid_bytes, char *out)
{
    int pos = 0;

    for (int i = 0; i < chunk_size; i++)
    {
        uint8_t b = (i < valid_bytes || valid_bytes < 0) ? bytes[i] : 0x00;

        if (isprint((unsigned char)b))
        {
            out[pos++] = (char)b;
        }
        else
        {
            out[pos++] = '.';
        }
    }

    out[pos] = '\0';
}
