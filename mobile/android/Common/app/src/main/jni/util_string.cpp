//
// Created by liuke on 15/12/29.
//

#include "util_string.h"

std::string util_string::to_string(JNIEnv *env, jstring jstr) {
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("UTF-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if(alen > 0) {
        rtn = (char*) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    env->DeleteLocalRef(barr);
    env->DeleteLocalRef(strencode);
    env->DeleteLocalRef(clsstring);
    std::string stemp(rtn);
    free(rtn);
    return stemp;
}

jstring util_string::to_jstring(JNIEnv *env, std::string str) {
    jbyteArray bytes = (env)->NewByteArray(str.length());
    env->SetByteArrayRegion(bytes, 0, str.length(), (jbyte*)str.c_str());
    jstring encoding = (env)->NewStringUTF("UTF-8");
    jclass cls = env->FindClass("java/lang/String");
    jmethodID mid = env->GetMethodID(cls, "<init>", "([BLjava/lang/String;)V");
    jstring ret = (jstring)env->NewObject(cls, mid, bytes, encoding);
    env->DeleteLocalRef(encoding);
    env->DeleteLocalRef(cls);
    env->DeleteLocalRef(bytes);
    return ret;
}