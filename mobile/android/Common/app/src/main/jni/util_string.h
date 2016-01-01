//
// Created by liuke on 15/12/29.
//

#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

#include <jni.h>
#include <string>

class util_string {

public:

    static std::string to_string(JNIEnv*, jstring);
    static jstring to_jstring(JNIEnv* env, std::string str);

};


#endif //COMMON_UTIL_H
