//
// Created by ke liu on 23/02/2018.
//

#ifndef COMMON_AHTTP_WRAPPER_H
#define COMMON_AHTTP_WRAPPER_H


#include <memory>
#include <string>
#include <map>
#include <common/common_callback.hpp>

namespace plan9
{
    class ahttp_wrapper {
    public:
        static ahttp_wrapper instance();

        int get(std::string url, int timeout, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(std::shared_ptr<common_callback>, int http_code, std::string data)> callback);
    private:
        ahttp_wrapper();
        class ahttp_wrapper_impl;
        std::shared_ptr<ahttp_wrapper_impl> impl;
    };
}



#endif //COMMON_AHTTP_WRAPPER_H
