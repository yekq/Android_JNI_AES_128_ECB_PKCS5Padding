#ifndef JNIAESDEMO_AES_H
#define JNIAESDEMO_AES_H

#include <stddef.h>
#include <stdint.h>

#ifndef CBC
#define CBC 0
#endif

#ifndef ECB
#define ECB 1
#endif

#define AES_BLOCK_LENGTH 16

typedef enum {
    AES_RESULT_OK = 0,
    AES_RESULT_INVALID_ARGUMENT,
    AES_RESULT_INVALID_BASE64,
    AES_RESULT_INVALID_CIPHERTEXT_LENGTH,
    AES_RESULT_INVALID_PADDING,
    AES_RESULT_OUT_OF_MEMORY
} AesResult;

#if defined(ECB) && ECB

void AES128_ECB_encrypt(const uint8_t *input, const uint8_t *key, uint8_t *output);

void AES128_ECB_decrypt(const uint8_t *input, const uint8_t *key, uint8_t *output);

AesResult AES_128_ECB_PKCS5Padding_Encrypt(
        const uint8_t *input,
        size_t input_length,
        const uint8_t *key,
        char **output);

AesResult AES_128_ECB_PKCS5Padding_Decrypt(
        const char *input,
        size_t input_length,
        const uint8_t *key,
        uint8_t **output,
        size_t *output_length);

#endif

#endif
