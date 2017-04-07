//
// Created by liuke on 16/1/15.
//

#include "timer.h"
#include <thread/thread_wrap.h>

namespace plan9
{
    class timer::timer_impl {

    public:

        timer_impl() : id(-1) {

        }

        int start(std::function<void(void)> function, long delay_milliseconds, bool repeat) {
            if (delay_milliseconds > 0) {
                repeat = false;
                id = thread_wrap::post_background(function , delay_milliseconds, repeat);
            }
            return id;
        }

        void cancel() {
            if (id >= 0) {
                thread_wrap::cancel_background_function(id);
            }
            id = -1;
        }

    private:
        int id;
    };




    timer::timer() : impl(new timer_impl){

    }

    timer::~timer() {

    }

    int timer::start(std::function<void(void)> function, long delay_milliseconds, bool repeat) {
        return impl->start(function, delay_milliseconds, repeat);
    }

    void timer::cancel() {
        impl->cancel();
    }
}