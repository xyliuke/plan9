//
// Created by liuke on 16/1/17.
//

#ifndef COMMON_TIMER_WRAP_H
#define COMMON_TIMER_WRAP_H

#include <memory>
#import <thread/timer.h>

namespace plan9
{
    class timer_wrap {
    public:
        static timer_wrap instance();
        void create_timer(long delay_millisecond, std::function<void(void)> function);

    private:
        timer_wrap();
        class timer_wrap_impl;
        std::shared_ptr<timer_wrap_impl> impl;
    };

}



#endif //COMMON_TIMER_WRAP_H
