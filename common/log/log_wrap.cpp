//
// Created by liuke on 15/11/1.
//

#include "log_wrap.h"
#include "log.h"
#include "../thread/thread_wrap.h"


void plan9::log_wrap::logI(std::string format, ...) {
    char str[1000] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(str, format.c_str(), arg_ptr);
    va_end(arg_ptr);
    thread_wrap::post_io([=](){
        log::logI(std::string(str));
    });
}

void plan9::log_wrap::logW(std::string format, ...) {
    char str[1000] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(str, format.c_str(), arg_ptr);
    va_end(arg_ptr);
    thread_wrap::post_io([=]() {
        log::logW(std::string(str));
    });
}

void plan9::log_wrap::logE(std::string format, ...) {
    char str[1000] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(str, format.c_str(), arg_ptr);
    va_end(arg_ptr);
    thread_wrap::post_io([=]() {
        log::logE(std::string(str));
    });
}

void plan9::log_wrap::logF(std::string format, ...) {
    char str[1000] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsprintf(str, format.c_str(), arg_ptr);
    va_end(arg_ptr);
    thread_wrap::post_io([=]() {
        log::logF(std::string(str));
    });
}