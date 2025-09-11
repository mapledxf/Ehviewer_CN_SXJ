//
// Created by Xuefeng Ding on 2024/4/30.
//

#include <sstream>
#include <jni.h>
#include <android/log.h>
#include <codecvt> // For std::wstring_convert
#include <locale> // For std::locale

#include "Encryption.h"

extern "C" JNIEXPORT jstring
Java_com_vwm_encryption_Encryption_Encrypt(JNIEnv *env, jclass /* this */, jstring jInput) {
    const char *input = env->GetStringUTFChars(jInput, nullptr);
    if (input == nullptr || *input == '\0') {
        env->ReleaseStringUTFChars(jInput, input);
        return nullptr;
    }
    std::istringstream iss(input);
    std::ostringstream oss;
    vwm::Encryption::encrypt(iss, &oss);
    std::string encrypted = oss.str();
    std::string encoded = vwm::Encryption::base64_encode(reinterpret_cast<const unsigned char*>(encrypted.c_str()), encrypted.length());

    env->ReleaseStringUTFChars(jInput, input);
    return env->NewStringUTF(encoded.c_str());
}

extern "C" JNIEXPORT jstring
Java_com_vwm_encryption_Encryption_Decrypt(JNIEnv *env, jclass /* this */, jstring jInput) {
    const char *input = env->GetStringUTFChars(jInput, nullptr);
    if (input == nullptr || *input == '\0') {
        env->ReleaseStringUTFChars(jInput, input);
        return nullptr;
    }
//    __android_log_print(ANDROID_LOG_INFO, "testme", "%s", input);

    std::string decoded = vwm::Encryption::base64_decode(input);

    std::istringstream iss(decoded);
    std::ostringstream oss;
    long size;
    int version;
    vwm::Encryption::decrypt(iss, &oss, size, version);
    std::string result = oss.str();
//    __android_log_print(ANDROID_LOG_INFO, "testme", "%s", result.c_str());

    env->ReleaseStringUTFChars(jInput, input);
    return env->NewStringUTF(result.c_str());
}