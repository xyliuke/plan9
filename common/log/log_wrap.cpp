//
// Created by liuke on 15/11/1.
//

#include "log_wrap.h"
#include "log.h"
#include "../thread/thread_wrap.h"

//#define LOG_THREAD_ENABLE

void plan9::log_wrap::logI(std::string format, ...) {
    char str[1000] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(str, format.c_str(), arg_ptr);
    va_end(arg_ptr);
#ifdef LOG_THREAD_ENABLE
    thread_wrap::post_io([=](){
#endif
        log::logI(std::string(str));
#ifdef LOG_THREAD_ENABLE
    });
#endif
}

void plan9::log_wrap::logW(std::string format, ...) {
    char str[1000] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(str, format.c_str(), arg_ptr);
    va_end(arg_ptr);
#ifdef LOG_THREAD_ENABLE
    thread_wrap::post_io([=]() {
#endif
        log::logW(std::string(str));
#ifdef LOG_THREAD_ENABLE
    });
#endif
}

void plan9::log_wrap::logE(std::string format, ...) {
    char str[1000] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(str, format.c_str(), arg_ptr);
    va_end(arg_ptr);
#ifdef LOG_THREAD_ENABLE
    thread_wrap::post_io([=]() {
#endif
        log::logE(std::string(str));
#ifdef LOG_THREAD_ENABLE
    });
#endif
}

void plan9::log_wrap::logF(std::string format, ...) {
    char str[1000] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(str, format.c_str(), arg_ptr);
    va_end(arg_ptr);
#ifdef LOG_THREAD_ENABLE
    thread_wrap::post_io([=]() {
#endif
        log::logF(std::string(str));
#ifdef LOG_THREAD_ENABLE
    });
#endif
}