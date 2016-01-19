//
// Created by liuke on 16/1/17.
//

#include <map>
#include "timer_wrap.h"

namespace plan9
{
    class timer_wrap::timer_wrap_impl
    {
    public:

        void create_timer(long delay_millisecond, std::function<void(void)> function) {
            clear_timer();

            std::shared_ptr<timer> t(new timer);
            int id = t->start(function, delay_millisecond);
            timer_map[id] = t;
        }

        void clear_timer() {
            std::map<int, std::shared_ptr<timer>>::iterator it = timer_map.begin();
            while (it != timer_map.end()) {
                if (it->second->isOver()) {
                    timer_map.erase(it ++);
                } else {
                    ++ it;
                }
            }
        }

    private:
        std::map<int, std::shared_ptr<timer>> timer_map;
    };


    timer_wrap::timer_wrap() : impl(new timer_wrap_impl) {

    }

    timer_wrap timer_wrap::instance() {
        static timer_wrap timer_;
        return timer_;
    }

    void timer_wrap::create_timer(long delay_millisecond, std::function<void(void)> function) {
        impl->create_timer(delay_millisecond, function);
    }
}