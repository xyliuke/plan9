//
// Created by ke liu on 13/11/2017.
// Copyright (c) 2017 ke liu. All rights reserved.
//

#ifndef ANET_CASE_INSENSITIVE_MAP_H
#define ANET_CASE_INSENSITIVE_MAP_H

#include <map>
#include <string>
#include "string_parser.hpp"

namespace plan9 {

    class case_insensitive_map {
    public:
        case_insensitive_map();
        void add(std::string key, std::string value);
        void remove(std::string key);
        std::string get(std::string key, bool* find);
        //返回true时继续迭代，false时不再迭代
        void const_iteration(std::function<bool(std::string, std::string)> callback);
        std::shared_ptr<std::map<std::string, std::string>> get();
//        std::string operator[](std::string key);
    private:
        class case_insensitive_map_impl;
        std::shared_ptr<case_insensitive_map_impl> impl_;
    };
}

#endif //ANET_CASE_INSENSITIVE_MAP_H
