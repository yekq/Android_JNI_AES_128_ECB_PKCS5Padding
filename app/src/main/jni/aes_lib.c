//
// Created by yekangqi on 2016/11/8.
//

#include <jni.h>
#include "aes_lib.h"
#include "aes.h"


JNIEXPORT jstring JNICALL
Java_com_ykq_demo_jniaes_MainActivity_getAESEncrypt(JNIEnv *env, jobject instance, jstring str_) {
    const char *in=  (*env)->GetStringUTFChars(env, str_, JNI_FALSE);
    char *baseResult= AES_128_ECB_PKCS5Padding_Encrypt(in,  AES_KEY);
    (*env)->ReleaseStringUTFChars(env, str_, in);
    return (*env)->NewStringUTF(env,baseResult);
}

JNIEXPORT jstring JNICALL
Java_com_ykq_demo_jniaes_MainActivity_getAESDecrypt(JNIEnv *env, jobject instance, jstring str_) {
    const char *str = (*env)->GetStringUTFChars(env, str_, JNI_FALSE);
    char * desResult=AES_128_ECB_PKCS5Padding_Decrypt(str,AES_KEY);
    (*env)->ReleaseStringUTFChars(env, str_, str);
//    return (*env)->NewStringUTF(env, desResult);
    //不用系统自带的方法NewStringUTF是因为如果desResult是乱码,会抛出异常
    return charToJstring(env,desResult);
}
jstring charToJstring(JNIEnv* envPtr, char *src) {
    JNIEnv env = *envPtr;

    jsize len = strlen(src);
    jclass clsstring = env->FindClass(envPtr, "java/lang/String");
    jstring strencode = env->NewStringUTF(envPtr, "UTF-8");
    jmethodID mid = env->GetMethodID(envPtr, clsstring, "<init>",
                                     "([BLjava/lang/String;)V");
    jbyteArray barr = env->NewByteArray(envPtr, len);
    env->SetByteArrayRegion(envPtr, barr, 0, len, (jbyte*) src);

    return (jstring) env->NewObject(envPtr, clsstring, mid, barr, strencode);
}