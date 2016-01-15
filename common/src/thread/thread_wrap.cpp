//
// Created by liuke on 15/11/1.
//

#include "thread_wrap.h"
#include "thread_dispatch.h"

namespace plan9{


    void thread_wrap::post_background(std::function<void(void)> func) {
        static bool created = false;
        if (!created) {
            thread_dispatch::create(1);
        }

        thread_dispatch::post(1, func);
    }

    void thread_wrap::post_network(std::function<void(void)> func) {
        static bool created = false;
        if (!created) {
            thread_dispatch::create(2);
        }

        thread_dispatch::post(2, func);
    }

    void thread_wrap::post_io(std::function<void(void)> func) {
        static bool created = false;
        if (!created) {
            thread_dispatch::create(3);
        }

        thread_dispatch::post(3, func);
    }

    void thread_wrap::stop() {
        thread_dispatch::stop();
    }
}