//
// Created by liuke on 16/1/15.
//

#include "timer.h"
#include <thread/thread_wrap.h>

namespace plan9
{
    class timer::timer_impl {

    public:

        timer_impl() : id(-1), repeat(false), function(nullptr) {

        }

        int start(std::function<void(void)> function, long delay_milliseconds) {
            if (delay_milliseconds > 0) {
                repeat = false;
                this->function = function;
                id = thread_wrap::post_background([=](void){
                    if (this->function != nullptr) {
                        this->function();
                    }
                    id = -1;
                    this->function = nullptr;
                }, delay_milliseconds);
            }
            return id;
        }

        void start(std::function<void(void)> function, long delay_milliseconds, long repeat_interval) {
            if (delay_milliseconds > 0 && repeat_interval > 0) {
                repeat = true;
                this->function = function;
                this->next_time = repeat_interval;
                id = thread_wrap::post_background(std::bind(&timer_impl::run, this), delay_milliseconds);
            }
        }

        void cancel() {
            if (id >= 0) {
                thread_wrap::cancel_background_function(id);
            }
            id = -1;
        }

        void run() {
            if (this->function != nullptr) {
                this->function();
            }
            if (id >= 0 && this->next_time > 0) {
                id = thread_wrap::post_background(std::bind(&timer_impl::run, this), this->next_time);
            }
        }

        bool isOver() {
            return id < 0;
        }

    private:
        int id;
        bool repeat;
        std::function<void(void)> function;
        std::function<void(void)> function_inner;
        long next_time;
    };




    timer::timer() : impl(new timer_impl){

    }

    timer::~timer() {

    }

    int timer::start(std::function<void(void)> function, long delay_milliseconds) {
        return impl->start(function, delay_milliseconds);
    }

    void timer::start(std::function<void(void)> function, long delay_milliseconds, long repeat_interval) {
        impl->start(function, delay_milliseconds, repeat_interval);
    }

    void timer::cancel() {
        impl->cancel();
    }


    bool timer::isOver() {
        return impl->isOver();
    }
}