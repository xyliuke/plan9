//
// Created by ke liu on 24/01/2018.
// Copyright (c) 2018 ke liu. All rights reserved.
//

#ifndef ANET_CERT_H
#define ANET_CERT_H

#include <functional>
#include <openssl/x509.h>
namespace plan9
{
    class cert {
    public:
        static void get_ca_cert(std::function<void(std::shared_ptr<std::vector<X509*>>)> callback);
    };
}


#endif //ANET_CERT_H
