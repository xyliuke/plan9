//
// Created by liuke on 15/12/25.
//

#include "cn_gocoding_common_Bizlayer.h"
#include "util_string.h"
#include <init/common.h>
#include <json/json_wrap.h>
#include <util/UUID.h>

#include<android/log.h>

static jmethodID callback_method = NULL;
static JavaVM* global_vm;
static jclass bizlayer;

//  #define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "bizlayer", __VA_ARGS__)


static void logi(JNIEnv*, std::string);

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
    callback_method = env->GetStaticMethodID(bizlayer, "callback", "(Ljava/lang/String;)V");
    return JNI_VERSION_1_6;
}



static void callback(std::string data) {
    // JNIEnv* env = getEnv();
    // if (env == nullptr) return;

    // if (callback_method == NULL) {
        // callback_method = env->GetStaticMethodID(bizlayer, "callback", "(Ljava/lang/String;)V");
    // }
    // if (callback_method != NULL) {
        // jstring data_js = util_string::to_jstring(env, data);
        // env->CallStaticVoidMethod(bizlayer, callback_method, data_js);
        // env->DeleteLocalRef(data_js);
    // }
//    global_vm->DetachCurrentThread();
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

jstring JNICALL Java_cn_gocoding_common_Bizlayer_createid(JNIEnv *e, jclass)
{
    return util_string::to_jstring(e, plan9::UUID::id());
}

static void call(JNIEnv* env, std::string method, std::string param, bool isCallback) {
    if ("log" != method) {
        std::stringstream ss;
        ss << "call : ";
        ss << method;
        if (param == "") {
            //没有参数
            ss << " param : null";
        } else {
            ss << " param : ";
            ss << param;
        }

        logi(env, ss.str());
    }


    if (isCallback) {
        plan9::common::call(method, param, [=](std::string data) {
            if (callback_method != NULL) {
                JNIEnv* e = getEnv();
                if (e != nullptr) {
                    logi(e, "callback to jni");
                    // logi(e, data);
                    jstring data_js = util_string::to_jstring(e, data);
                    e->CallStaticVoidMethod(bizlayer, callback_method, data_js);
                    e->DeleteLocalRef(data_js);
                }
            }
        });
    } else {
        plan9::common::call(method, param, nullptr);
    }
}

static void logi(JNIEnv* env, std::string msg) {
    Json::Value tmp;
    tmp["level"] = "info";
    tmp["target"] = "ui";
    tmp["msg"] = msg;
    call(env, "log", plan9::json_wrap::to_string(tmp), false);
}

static void logw(JNIEnv* env, std::string msg) {
    Json::Value tmp;
    tmp["level"] = "info";
    tmp["target"] = "warn";
    tmp["msg"] = msg;
    call(env, "log", plan9::json_wrap::to_string(tmp), false);
}

static void loge(JNIEnv* env, std::string msg) {
    Json::Value tmp;
    tmp["level"] = "info";
    tmp["target"] = "error";
    tmp["msg"] = msg;
    call(env, "log", plan9::json_wrap::to_string(tmp), false);
}

void JNICALL Java_cn_gocoding_common_Bizlayer_call(JNIEnv *env, jclass cls, jstring method, jstring param, jboolean isCallback)
{
    std::string method_str = util_string::to_string(env, method);
    std::string param_str = "";
    if (param != NULL) {
        param_str = util_string::to_string(env, param);
    }
    
    call(env, method_str, param_str, isCallback);
}
void JNICALL Java_cn_gocoding_common_Bizlayer_logi(JNIEnv *env, jclass cls, jstring msg)
{
    std::string msg_str = util_string::to_string(env, msg);
    logi(env, msg_str);
}

void JNICALL Java_cn_gocoding_common_Bizlayer_logw(JNIEnv *env, jclass cls, jstring msg)
{
    std::string msg_str = util_string::to_string(env, msg);
    logw(env, msg_str);
}

void JNICALL Java_cn_gocoding_common_Bizlayer_loge(JNIEnv *env, jclass cls, jstring msg)
{
    std::string msg_str = util_string::to_string(env, msg);
    loge(env, msg_str);
}
