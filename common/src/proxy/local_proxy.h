//
// Created by ke liu on 19/01/2018.
// Copyright (c) 2018 ke liu. All rights reserved.
//

#ifndef ANET_LOCAL_PROXY_H
#define ANET_LOCAL_PROXY_H

#include <string>
#include <map>

namespace plan9 {
    class local_proxy {
    public:
        static void get_proxy(std::function<void(std::shared_ptr<std::map<std::string, std::string>>)> callback);
    };
}


#endif //ANET_LOCAL_PROXY_H
