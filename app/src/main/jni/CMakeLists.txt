cmake_minimum_required(VERSION 3.22.1)
project(JNIAESDemo LANGUAGES C)

add_library(
        JNIAESDemo
        SHARED
        aes.c
        aes_lib.c
        base64.c)

target_compile_features(JNIAESDemo PRIVATE c_std_11)
target_compile_options(JNIAESDemo PRIVATE -Wall -Wextra -Werror)
