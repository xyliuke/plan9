//
// Created by liuke on 15/11/1.
//

#include "thread_wrap.h"
#include "thread_dispatch.h"
#include <thread/thread_define.h>
namespace plan9{


    static bool background = false;

    void thread_wrap::post_background(std::function<void(void)> func) {
#ifdef THREAD_ENABLE
        if (!background) {
            thread_dispatch::create(1);
        }

        thread_dispatch::post(1, func);
#else
        func();
#endif
    }

    int thread_wrap::post_background(std::function<void(void)> function, long milliseconds) {
        if (!background) {
            thread_dispatch::create(1);
        }
        return thread_dispatch::post(1, function, milliseconds);
    }

    void thread_wrap::cancel_background_function(int id) {
        thread_dispatch::cancel(1, id);
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