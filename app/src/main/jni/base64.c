#include "base64.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

static const char BASE64_ALPHABET[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int base64_value(unsigned char character) {
    if (character >= 'A' && character <= 'Z') {
        return character - 'A';
    }
    if (character >= 'a' && character <= 'z') {
        return character - 'a' + 26;
    }
    if (character >= '0' && character <= '9') {
        return character - '0' + 52;
    }
    if (character == '+') {
        return 62;
    }
    if (character == '/') {
        return 63;
    }
    return -1;
}

char *base64_encode(const uint8_t *data, size_t data_length) {
    if (data == NULL && data_length != 0) {
        return NULL;
    }
    if (data_length > (SIZE_MAX - 1) / 4 * 3) {
        return NULL;
    }

    size_t encoded_length = ((data_length + 2) / 3) * 4;
    char *output = malloc(encoded_length + 1);
    if (output == NULL) {
        return NULL;
    }

    size_t input_index = 0;
    size_t output_index = 0;
    while (input_index < data_length) {
        size_t remaining = data_length - input_index;
        uint32_t value = (uint32_t) data[input_index] << 16;
        if (remaining > 1) {
            value |= (uint32_t) data[input_index + 1] << 8;
        }
        if (remaining > 2) {
            value |= data[input_index + 2];
        }

        output[output_index++] = BASE64_ALPHABET[(value >> 18) & 0x3f];
        output[output_index++] = BASE64_ALPHABET[(value >> 12) & 0x3f];
        output[output_index++] = remaining > 1 ? BASE64_ALPHABET[(value >> 6) & 0x3f] : '=';
        output[output_index++] = remaining > 2 ? BASE64_ALPHABET[value & 0x3f] : '=';
        input_index += 3;
    }
    output[encoded_length] = '\0';
    return output;
}

uint8_t *base64_decode(const char *data, size_t data_length, size_t *output_length) {
    if (output_length == NULL) {
        return NULL;
    }
    *output_length = 0;
    if (data == NULL || data_length == 0 || data_length % 4 != 0) {
        return NULL;
    }

    size_t padding = 0;
    if (data[data_length - 1] == '=') {
        padding++;
    }
    if (data[data_length - 2] == '=') {
        padding++;
    }

    size_t decoded_length = data_length / 4 * 3 - padding;
    uint8_t *output = malloc(decoded_length == 0 ? 1 : decoded_length);
    if (output == NULL) {
        return NULL;
    }

    size_t output_index = 0;
    for (size_t input_index = 0; input_index < data_length; input_index += 4) {
        int values[4];
        for (size_t i = 0; i < 4; i++) {
            unsigned char character = (unsigned char) data[input_index + i];
            bool is_padding = character == '=';
            bool padding_allowed = input_index + 4 == data_length && i >= 2;
            if (is_padding) {
                if (!padding_allowed || (i == 2 && data[input_index + 3] != '=')) {
                    free(output);
                    return NULL;
                }
                values[i] = 0;
            } else {
                values[i] = base64_value(character);
                if (values[i] < 0) {
                    free(output);
                    return NULL;
                }
            }
        }
        if ((data[input_index + 2] == '=' && (values[1] & 0x0f) != 0)
            || (data[input_index + 3] == '=' && data[input_index + 2] != '='
                && (values[2] & 0x03) != 0)) {
            free(output);
            return NULL;
        }

        uint32_t value = ((uint32_t) values[0] << 18)
                         | ((uint32_t) values[1] << 12)
                         | ((uint32_t) values[2] << 6)
                         | (uint32_t) values[3];
        if (output_index < decoded_length) {
            output[output_index++] = (uint8_t) (value >> 16);
        }
        if (output_index < decoded_length) {
            output[output_index++] = (uint8_t) (value >> 8);
        }
        if (output_index < decoded_length) {
            output[output_index++] = (uint8_t) value;
        }
    }

    *output_length = decoded_length;
    return output;
}
