//
// Created by liuke on 15/12/25.
//

#include "cn_gocoding_common_Bizlayer.h"
#include "util_string.h"
#include <init/common.h>

static jmethodID callback_method = NULL;
static JavaVM* global_vm;
static  jclass bizlayer;

JNIEnv* getEnv() {
    JNIEnv *env;
    int status = global_vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if(status < 0) {
        status = global_vm->AttachCurrentThread(&env, NULL);
        if(status < 0) {
            return nullptr;
        }
    }
    return env;
}

jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    global_vm = vm;
    JNIEnv* env = getEnv();
    jclass tmp = env->FindClass("cn/gocoding/common/Bizlayer");
    bizlayer = (jclass)env->NewGlobalRef(tmp);
    return JNI_VERSION_1_6;
}



void callback(std::string data) {
    JNIEnv* env = getEnv();
    if (env == nullptr) return;

    Java_cn_gocoding_common_Bizlayer_logi(env, bizlayer, util_string::to_jstring(env, "callback to anroid : " + data));

    if (callback_method == NULL) {
        callback_method = env->GetStaticMethodID(bizlayer, "callback", "(Ljava/lang/String;)V");
    }
    if (callback_method != NULL) {
        jstring data_js = util_string::to_jstring(env, data);
        env->CallStaticVoidMethod(bizlayer, callback_method, data_js);
        env->DeleteLocalRef(data_js);
    }
}

void JNICALL Java_cn_gocoding_common_Bizlayer_init(JNIEnv *env, jclass cls, jstring root_path, jstring lua_path)
{
    std::string root_str = util_string::to_string(env, root_path);
    std::string lua_str = util_string::to_string(env, lua_path);
    plan9::common::init(root_str, lua_str);
    plan9::common::set_notify_function([=](std::string msg){
    	callback(msg);
    });
    plan9::common::set_android_platform();
}

void JNICALL Java_cn_gocoding_common_Bizlayer_stop(JNIEnv *, jclass)
{
    plan9::common::stop();
}

void JNICALL Java_cn_gocoding_common_Bizlayer_call(JNIEnv *env, jclass cls, jstring method, jstring param, jboolean isCallback)
{
    std::string method_str = util_string::to_string(env, method);
    if (param != NULL) {
        std::string param_str = util_string::to_string(env, param);
        if ("log" != method_str) {
            std::stringstream ss;
            ss << "call : ";
            ss << method_str;
            ss << " param : ";
            ss << param_str;
            jstring log_msg = util_string::to_jstring(env, ss.str());
            Java_cn_gocoding_common_Bizlayer_logi(env, cls, log_msg);
        }
        if (isCallback) {
            plan9::common::call(method_str, param_str, [=](std::string data){
                callback(data);
            });
        } else {
            plan9::common::call(method_str, param_str, nullptr);
        }
    } else {
        if ("log" != method_str) {
            std::stringstream ss;
            ss << "call : ";
            ss << method_str;
            ss << " param : null";
            jstring log_msg = util_string::to_jstring(env, ss.str());
            Java_cn_gocoding_common_Bizlayer_logi(env, cls, log_msg);
        }
        if (isCallback) {
            plan9::common::call(method_str, "", [=](std::string data){
                callback(data);
            });
        } else {
            plan9::common::call(method_str, "", nullptr);
        }
    }

}
void JNICALL Java_cn_gocoding_common_Bizlayer_logi(JNIEnv *env, jclass cls, jstring msg)
{
    jstring method = env->NewStringUTF("log");
    std::string msg_str = util_string::to_string(env, msg);
    std::stringstream ss;
    ss << "{\"level\":\"info\", \"target\":\"ui\", \"msg\":\"";
    ss << msg_str;
    ss << "\"}";
    jstring msg_param = util_string::to_jstring(env, ss.str());
    Java_cn_gocoding_common_Bizlayer_call(env, cls, method, msg_param, false);
    env->DeleteLocalRef(method);
    env->DeleteLocalRef(msg_param);
}

void JNICALL Java_cn_gocoding_common_Bizlayer_logw(JNIEnv *env, jclass cls, jstring msg)
{
    jstring method = env->NewStringUTF("log");
    std::string msg_str = util_string::to_string(env, msg);
    std::stringstream ss;
    ss << "{\"level\":\"warn\", \"target\":\"ui\", \"msg\":\"";
    ss << msg_str;
    ss << "\"}";
    jstring msg_param = util_string::to_jstring(env, ss.str());
    Java_cn_gocoding_common_Bizlayer_call(env, cls, method, msg_param, false);
    env->DeleteLocalRef(method);
    env->DeleteLocalRef(msg_param);
}

void JNICALL Java_cn_gocoding_common_Bizlayer_loge(JNIEnv *env, jclass cls, jstring msg)
{
    jstring method = env->NewStringUTF("log");
    std::string msg_str = util_string::to_string(env, msg);
    std::stringstream ss;
    ss << "{\"level\":\"error\", \"target\":\"ui\", \"msg\":\"";
    ss << msg_str;
    ss << "\"}";
    jstring msg_param = util_string::to_jstring(env, ss.str());
    Java_cn_gocoding_common_Bizlayer_call(env, cls, method, msg_param, false);
    env->DeleteLocalRef(method);
    env->DeleteLocalRef(msg_param);
}
