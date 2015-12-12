//
// Created by liuke on 15/12/10.
//

#ifndef COMMON_JSON_WRAP_H
#define COMMON_JSON_WRAP_H

#include <json/json.h>

namespace plan9 {

    class json_wrap {
    public:
        static Json::Value parse(std::string json_str, bool* error);
        static Json::Value parseFile(std::string filepath, bool* error);
        static std::string toString(Json::Value);
    };

}

#endif //COMMON_JSON_WRAP_H
