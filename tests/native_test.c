#include "aes.h"
#include "base64.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t TEST_KEY[] = "1234567890abcdef";

static void assert_round_trip(const uint8_t *input, size_t input_length) {
    char *encrypted = NULL;
    assert(AES_128_ECB_PKCS5Padding_Encrypt(
            input, input_length, TEST_KEY, &encrypted) == AES_RESULT_OK);
    assert(encrypted != NULL);

    uint8_t *decrypted = NULL;
    size_t decrypted_length = 0;
    assert(AES_128_ECB_PKCS5Padding_Decrypt(
            encrypted,
            strlen(encrypted),
            TEST_KEY,
            &decrypted,
            &decrypted_length) == AES_RESULT_OK);
    assert(decrypted_length == input_length);
    assert(input_length == 0 || memcmp(decrypted, input, input_length) == 0);

    free(encrypted);
    free(decrypted);
}

int main(void) {
    const uint8_t one[] = "1";
    char *encrypted = NULL;
    assert(AES_128_ECB_PKCS5Padding_Encrypt(
            one, sizeof(one) - 1, TEST_KEY, &encrypted) == AES_RESULT_OK);
    assert(strcmp(encrypted, "qkrxxA9fIF636aITDRJhcg==") == 0);
    free(encrypted);

    const uint8_t utf8_text[] = "中文和 emoji: \xF0\x9F\x98\x80";
    const uint8_t binary_text[] = {'a', '\0', 'b', 0xff};
    assert_round_trip(NULL, 0);
    assert_round_trip(utf8_text, sizeof(utf8_text) - 1);
    assert_round_trip(binary_text, sizeof(binary_text));

    uint8_t *decrypted = NULL;
    size_t decrypted_length = 0;
    assert(AES_128_ECB_PKCS5Padding_Decrypt(
            "not-base64", 10, TEST_KEY, &decrypted, &decrypted_length)
           == AES_RESULT_INVALID_BASE64);
    assert(AES_128_ECB_PKCS5Padding_Decrypt(
            "AAAA", 4, TEST_KEY, &decrypted, &decrypted_length)
           == AES_RESULT_INVALID_CIPHERTEXT_LENGTH);
    assert(AES_128_ECB_PKCS5Padding_Decrypt(
            "AAAAAAAAAAAAAAAAAAAAAA==", 24, TEST_KEY, &decrypted, &decrypted_length)
           == AES_RESULT_INVALID_PADDING);

    size_t decoded_length = 0;
    assert(base64_decode("AA=A", 4, &decoded_length) == NULL);
    assert(base64_decode("A===", 4, &decoded_length) == NULL);
    assert(base64_decode("AB==", 4, &decoded_length) == NULL);
    assert(base64_decode("AAB=", 4, &decoded_length) == NULL);

    puts("原生 AES/Base64 测试全部通过");
    return 0;
}
