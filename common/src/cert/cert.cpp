//
// Created by ke liu on 24/01/2018.
// Copyright (c) 2018 ke liu. All rights reserved.
//

#include "cert.h"
#include <openssl/x509.h>
#include <iostream>
#include <memory>
#include <vector>



#ifdef _WIN32
    //define something for Windows (32-bit and 64-bit, this part is common)
static std::shared_ptr<std::vector<X509*>> get_system_ca_cert() {
    auto list = std::make_shared<std::vector<X509*>>();
    return list;
}
   #ifdef _WIN64
      //define something for Windows (64-bit only)
   #else
      //define something for Windows (32-bit only)
   #endif
#elif __APPLE__

#include <CoreFoundation/CFArray.h>
#include <Security/SecTrust.h>
#include <Security/SecCertificate.h>
#include <MacTypes.h>

static std::shared_ptr<std::vector<X509*>> get_system_ca_cert() {
    auto list = std::make_shared<std::vector<X509*>>();

//    CFArrayRef array = CFArrayCreate(NULL, NULL, 0, NULL);
//    OSStatus ret = SecTrustCopyAnchorCertificates(&array);
//    if (ret == errSecSuccess && CFArrayGetCount(array) > 0) {
//        for (int i = 0; i < CFArrayGetCount(array); ++i) {
//            SecCertificateRef cert = (SecCertificateRef) CFArrayGetValueAtIndex(array, i);
//            if (CFGetTypeID(cert) == SecCertificateGetTypeID()) {
//                CFDataRef data = SecCertificateCopyData(cert);
//                X509 *c = X509_new();
//                const UInt8 *p = CFDataGetBytePtr(data);
//                d2i_X509(&c, &p, CFDataGetLength(data));
//                list->push_back(c);
//            }
//        }
//    }
//    CFRelease(array);

    return list;
}

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
static std::shared_ptr<std::vector<X509*>> get_system_ca_cert() {
    auto list = std::make_shared<std::vector<X509*>>();
    return list;
}
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
    //TODO Android/Windows/Linux平台下获取系统证书
    void cert::get_ca_cert(std::function<void(std::shared_ptr<std::vector<X509*>>)> callback) {
        if (callback) {
            callback(get_system_ca_cert());
        }
    }
}
