//
// Created by liuke on 15/12/19.
//

#include "util.h"


namespace plan9
{
    class util::util_impl {

    };


    util util::instance() {
        static util u;
        return u;
    }

    util::util() : impl(new util_impl) {

    }
}