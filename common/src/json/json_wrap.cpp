//
// Created by liuke on 15/12/10.
//

#include "json_wrap.h"
#include <fstream>
#include <util/util.h>

namespace plan9
{
    Json::Value json_wrap::parse(std::string json_str, bool* error) {
        Json::Reader reader;
        Json::Value ret;
        *error = !reader.parse(json_str, ret);
        return ret;
    }

    Json::Value json_wrap::parseFile(std::string filepath, bool* error) {
        static Json::Reader reader;
        Json::Value root;

        std::ifstream is;
        is.open (filepath, std::ios::binary );
        *error = !reader.parse(is, root);
        is.close();
        return root;
    }

    std::string json_wrap::to_string(Json::Value value) {
        Json::FastWriter writer;
        std::string ret = writer.write(value);
        if (util::instance().is_suffix(ret, '\n')) {
            ret = ret.erase(ret.length() - 1, 1);
        }
        return ret;
    }

}