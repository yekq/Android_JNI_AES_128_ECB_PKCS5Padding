#ifndef JNIAESDEMO_BASE64_H
#define JNIAESDEMO_BASE64_H

#include <stddef.h>
#include <stdint.h>

char *base64_encode(const uint8_t *data, size_t data_length);

uint8_t *base64_decode(const char *data, size_t data_length, size_t *output_length);

#endif
