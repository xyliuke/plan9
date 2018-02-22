//
// Created by ke liu on 19/01/2018.
// Copyright (c) 2018 ke liu. All rights reserved.
//

#include "local_proxy.h"


#ifdef _WIN32
//define something for Windows (32-bit and 64-bit, this part is common)
static std::shared_ptr<std::map<std::string, std::string>> get_local_proxy() {
    auto ret = std::make_shared<std::map<std::string, std::string>>();
    return ret;
};
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__
#include <CFNetwork/CFProxySupport.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFNumber.h>
#include <CoreFoundation/CFString.h>
#include <sstream>

static std::string get_string_value(CFDictionaryRef dic, CFStringRef key);

static int get_int_value(CFDictionaryRef dic, CFStringRef key) {
    const void* value = CFDictionaryGetValue(dic, key);
    int ret = 0;
    if (value != NULL) {
        if (CFGetTypeID(value) == CFNumberGetTypeID()) {
            CFNumberGetValue((CFNumberRef)value, kCFNumberSInt32Type, &ret);
        } else if (CFGetTypeID(value) == CFStringGetTypeID()){
            std::string v = get_string_value(dic, key);
            return atoi(v.c_str());
        }
    }
    return ret;
}
static std::string get_string_value(CFDictionaryRef dic, CFStringRef key) {
    const void* value = CFDictionaryGetValue(dic, key);
    if (value != NULL) {
        if (CFGetTypeID(value) == CFStringGetTypeID()) {
            char buf[100];
            CFStringGetCString((CFStringRef)value, buf, 100, kCFStringEncodingUTF8);
            return std::string(buf);
        } else if (CFGetTypeID(value) == CFNumberGetTypeID()){
            int num = get_int_value(dic, key);
            std::stringstream ss;
            ss << num;
            return ss.str();
        }
    }
    return "";
}

static std::shared_ptr<std::map<std::string, std::string>> get_local_proxy() {
    auto ret = std::make_shared<std::map<std::string, std::string>>();
    CFDictionaryRef dic = CFNetworkCopySystemProxySettings();

    int httpEnable = get_int_value(dic, kCFNetworkProxiesHTTPEnable);
    if (httpEnable == 1) {
        (*ret)["HTTPProxy"] = get_string_value(dic, kCFNetworkProxiesHTTPProxy);
        (*ret)["HTTPPort"] = get_string_value(dic, kCFNetworkProxiesHTTPPort);
    }

//    int httpsEnable = get_int_value(dic, kCFNetworkProxiesHTTPSEnable);
    CFStringRef HTTPSEnable = CFStringCreateWithCString(NULL, "HTTPSEnable", kCFStringEncodingUTF8);
    int httpsEnable = get_int_value(dic, HTTPSEnable);
    CFRelease(HTTPSEnable);
    if (httpsEnable == 1) {
        CFStringRef HTTPSProxy = CFStringCreateWithCString(NULL, "HTTPSProxy", kCFStringEncodingUTF8);
        CFStringRef HTTPSPort = CFStringCreateWithCString(NULL, "HTTPSPort", kCFStringEncodingUTF8);
        (*ret)["HTTPSProxy"] = get_string_value(dic, HTTPSProxy);
        (*ret)["HTTPSPort"] = get_string_value(dic, HTTPSPort);
        CFRelease(HTTPSProxy);
        CFRelease(HTTPSPort);
    }

    CFRelease(dic);
    return ret;
};

//#include "TargetConditionals.h"
//#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
//#elif TARGET_OS_IPHONE
// iOS device
//#elif TARGET_OS_MAC
// Other kinds of Mac OS
//#else
//# error "Unknown Apple platform"
//#endif
#elif __ANDROID__
// android
static std::shared_ptr<std::map<std::string, std::string>> get_local_proxy() {
    auto ret = std::make_shared<std::map<std::string, std::string>>();
    return ret;
};
#elif __linux__
    // linux
#elif __unix__ // all unices not caught above
    // Unix
#elif defined(_POSIX_VERSION)
    // POSIX
#else
# error "Unknown compiler"
#endif

namespace plan9
{
    //TODO Android/Windows/Linux平台下获取代理配置功能
    void local_proxy::get_proxy(std::function<void(std::shared_ptr<std::map<std::string, std::string>>)> callback) {
        if (callback) {
            callback(get_local_proxy());
        }
    }
}
