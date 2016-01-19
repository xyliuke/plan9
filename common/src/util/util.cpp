//
// Created by liuke on 15/12/19.
//

#include "util.h"


namespace plan9
{
    class util::util_impl {
    public:
        bool isSuffix(std::string str, char end) {
            int pos = str.rfind(end);
            return str.length() - 1 == pos;
        }
    };


    util util::instance() {
        static util u;
        return u;
    }

    util::util() : impl(new util_impl) {

    }


    bool util::isSuffix(std::string str, char end) {
        return impl->isSuffix(str, end);
    }
}