#include "aes_lib.h"

#include "aes.h"

#include <stdint.h>
#include <stdlib.h>

static const uint8_t AES_KEY[] = "1234567890abcdef";

static void throw_exception(JNIEnv *env, const char *class_name, const char *message) {
    jclass exception_class = (*env)->FindClass(env, class_name);
    if (exception_class != NULL) {
        (*env)->ThrowNew(env, exception_class, message);
        (*env)->DeleteLocalRef(env, exception_class);
    }
}

static void throw_aes_error(JNIEnv *env, AesResult result) {
    switch (result) {
        case AES_RESULT_INVALID_ARGUMENT:
            throw_exception(env, "java/lang/IllegalArgumentException", "输入参数不能为空");
            break;
        case AES_RESULT_INVALID_BASE64:
            throw_exception(env, "java/lang/IllegalArgumentException", "密文不是有效的 Base64 数据");
            break;
        case AES_RESULT_INVALID_CIPHERTEXT_LENGTH:
            throw_exception(env, "java/lang/IllegalArgumentException", "密文长度必须是 16 字节的整数倍");
            break;
        case AES_RESULT_INVALID_PADDING:
            throw_exception(env, "java/lang/IllegalArgumentException", "密文或密钥无效，PKCS#7 填充校验失败");
            break;
        case AES_RESULT_OUT_OF_MEMORY:
            throw_exception(env, "java/lang/OutOfMemoryError", "原生内存分配失败");
            break;
        case AES_RESULT_OK:
            break;
    }
}

static jbyteArray string_to_utf8(JNIEnv *env, jstring input) {
    jclass string_class = (*env)->GetObjectClass(env, input);
    if (string_class == NULL) {
        return NULL;
    }
    jmethodID get_bytes = (*env)->GetMethodID(
            env, string_class, "getBytes", "(Ljava/lang/String;)[B");
    jstring charset_name = (*env)->NewStringUTF(env, "UTF-8");
    if (get_bytes == NULL || charset_name == NULL) {
        (*env)->DeleteLocalRef(env, string_class);
        return NULL;
    }

    jbyteArray result = (jbyteArray) (*env)->CallObjectMethod(env, input, get_bytes, charset_name);
    (*env)->DeleteLocalRef(env, charset_name);
    (*env)->DeleteLocalRef(env, string_class);
    return result;
}

static jstring utf8_to_string(JNIEnv *env, const uint8_t *data, size_t length) {
    if (length > INT32_MAX) {
        throw_exception(env, "java/lang/IllegalArgumentException", "解密结果过长");
        return NULL;
    }

    jbyteArray bytes = (*env)->NewByteArray(env, (jsize) length);
    if (bytes == NULL) {
        return NULL;
    }
    if (length != 0) {
        (*env)->SetByteArrayRegion(env, bytes, 0, (jsize) length, (const jbyte *) data);
    }

    jclass string_class = (*env)->FindClass(env, "java/lang/String");
    jstring charset_name = (*env)->NewStringUTF(env, "UTF-8");
    if (string_class == NULL || charset_name == NULL) {
        (*env)->DeleteLocalRef(env, bytes);
        return NULL;
    }
    jmethodID constructor = (*env)->GetMethodID(
            env, string_class, "<init>", "([BLjava/lang/String;)V");
    if (constructor == NULL) {
        (*env)->DeleteLocalRef(env, charset_name);
        (*env)->DeleteLocalRef(env, string_class);
        (*env)->DeleteLocalRef(env, bytes);
        return NULL;
    }

    jstring result = (jstring) (*env)->NewObject(
            env, string_class, constructor, bytes, charset_name);
    (*env)->DeleteLocalRef(env, charset_name);
    (*env)->DeleteLocalRef(env, string_class);
    (*env)->DeleteLocalRef(env, bytes);
    return result;
}

JNIEXPORT jstring JNICALL
Java_com_ykq_demo_jniaes_MainActivity_getAESEncrypt(
        JNIEnv *env, jobject instance, jstring input) {
    (void) instance;
    if (input == NULL) {
        throw_aes_error(env, AES_RESULT_INVALID_ARGUMENT);
        return NULL;
    }

    jbyteArray input_bytes = string_to_utf8(env, input);
    if (input_bytes == NULL) {
        return NULL;
    }
    jsize input_length = (*env)->GetArrayLength(env, input_bytes);
    jbyte *input_data = (*env)->GetByteArrayElements(env, input_bytes, NULL);
    if (input_data == NULL) {
        (*env)->DeleteLocalRef(env, input_bytes);
        return NULL;
    }

    char *encrypted = NULL;
    AesResult result = AES_128_ECB_PKCS5Padding_Encrypt(
            (const uint8_t *) input_data, (size_t) input_length, AES_KEY, &encrypted);
    (*env)->ReleaseByteArrayElements(env, input_bytes, input_data, JNI_ABORT);
    (*env)->DeleteLocalRef(env, input_bytes);
    if (result != AES_RESULT_OK) {
        throw_aes_error(env, result);
        return NULL;
    }

    jstring encrypted_string = (*env)->NewStringUTF(env, encrypted);
    free(encrypted);
    return encrypted_string;
}

JNIEXPORT jstring JNICALL
Java_com_ykq_demo_jniaes_MainActivity_getAESDecrypt(
        JNIEnv *env, jobject instance, jstring input) {
    (void) instance;
    if (input == NULL) {
        throw_aes_error(env, AES_RESULT_INVALID_ARGUMENT);
        return NULL;
    }

    jbyteArray input_bytes = string_to_utf8(env, input);
    if (input_bytes == NULL) {
        return NULL;
    }
    jsize input_length = (*env)->GetArrayLength(env, input_bytes);
    jbyte *input_data = (*env)->GetByteArrayElements(env, input_bytes, NULL);
    if (input_data == NULL) {
        (*env)->DeleteLocalRef(env, input_bytes);
        return NULL;
    }

    uint8_t *decrypted = NULL;
    size_t decrypted_length = 0;
    AesResult result = AES_128_ECB_PKCS5Padding_Decrypt(
            (const char *) input_data,
            (size_t) input_length,
            AES_KEY,
            &decrypted,
            &decrypted_length);
    (*env)->ReleaseByteArrayElements(env, input_bytes, input_data, JNI_ABORT);
    (*env)->DeleteLocalRef(env, input_bytes);
    if (result != AES_RESULT_OK) {
        throw_aes_error(env, result);
        return NULL;
    }

    jstring decrypted_string = utf8_to_string(env, decrypted, decrypted_length);
    free(decrypted);
    return decrypted_string;
}
