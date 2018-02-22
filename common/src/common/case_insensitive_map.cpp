//
// Created by ke liu on 13/11/2017.
// Copyright (c) 2017 ke liu. All rights reserved.
//

#include <map>
#include <string>
#include "case_insensitive_map.h"
#include "string_parser.hpp"

namespace plan9 {

    struct __lesscasecmp {
        bool operator() (const std::string& a, const std::string& b) const {
            return (strcasecmp (a.c_str ( ), b.c_str ( )) < 0);
        }
    };

    class case_insensitive_map::case_insensitive_map_impl {
    public:
        case_insensitive_map_impl() : data(new std::map<std::string, std::string, __lesscasecmp>) {

        }
        void add(std::string key, std::string value) {
            (*data)[key] = value;
        }
        void remove(std::string key) {
            data->erase(key);
        }
        std::string get(std::string key, bool* find) {
            if (data->find(key) != data->end()) {
                *find = true;
                return (*data)[key];
            }
            *find = false;
            return "";
        }
        void const_iteration(std::function<bool(std::string, std::string)> callback) {
            if (callback) {
                std::map<std::string, std::string, __lesscasecmp>::const_iterator it = data->begin();
                while (it != data->end()) {
                    if (!callback(it->first, it->second)) {
                        break;
                    }
                    it ++;
                }
            }
        }

        std::shared_ptr<std::map<std::string, std::string>> get() {
            std::shared_ptr<std::map<std::string, std::string>> ret(new std::map<std::string, std::string>);
            const_iteration([=](std::string key, std::string value) -> bool {
                (*ret)[key] = value;
                return true;
            });
            return ret;
        }


    private:
        std::shared_ptr<std::map<std::string, std::string, __lesscasecmp>> data;
    };


    case_insensitive_map::case_insensitive_map() : impl_(new case_insensitive_map_impl) {
    }

    void case_insensitive_map::add(std::string key, std::string value) {
        impl_->add(key, value);
    }

    void case_insensitive_map::remove(std::string key) {
        impl_->remove(key);
    }

    std::string case_insensitive_map::get(std::string key, bool *find) {
        return impl_->get(key, find);
    }

    void case_insensitive_map::const_iteration(std::function<bool(std::string, std::string)> callback) {
        impl_->const_iteration(callback);
    }

    std::shared_ptr<std::map<std::string, std::string>> case_insensitive_map::get() {
        return impl_->get();
    }

//    std::string case_insensitive_map::operator[](std::string key) {
//        bool find;
//        std::string ret = get(key, &find);
//        if (find) {
//            return ret;
//        } else {
//            return "";
//        }
//    }
}
