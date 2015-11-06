//
// Created by liuke on 15/11/5.
//

#include <iostream>
#include "json_wrap.h"

using namespace Json;

namespace plan9
{

    Reader json_wrap::reader_;
    FastWriter json_wrap::writer_;

    json_wrap::json_wrap() {

    }

    json_wrap::json_wrap(std::string json_str) {
        reader_.parse(json_str, *this);
    }

    json_wrap::~json_wrap() {

    }

    std::string json_wrap::toString() {
        std::string ret;
        if (this->isObject() || this->isArray()) {
            ret = writer_.write(*this);
            if (ret.substr(ret.length() - 1, 1) == "\n") {
                ret = ret.substr(0, ret.length() - 1);
            }
        } else {
            ret = this->asString();
        }
        return ret;
    }

}
