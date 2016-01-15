//
// Created by liuke on 16/1/15.
//

#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H


#include <memory>
#include <functional>

namespace plan9 {

    class timer {

    public:

        timer();

        ~timer();

        /**
         * 启动一个timer,延迟delay_milliseconds毫秒后执行,只执行一次
         * @param function 执行函数体
         * @param delay_milliseconds 延迟时间,单位为毫秒
         */
        void start(std::function<void(void)> function, long delay_milliseconds);

        /**
         * 启动一个timer,延迟delay_milliseconds毫秒后执行,循环执行
         * @param function 执行函数体
         * @param delay_milliseconds 延迟时间,单位为毫秒
         * @param repeat_interval 循环间隔时间,单位为毫秒
         */
        void start(std::function<void(void)> function, long delay_milliseconds, long repeat_interval);

        void cancel();

    private:
        class timer_impl;
        std::shared_ptr<timer_impl> impl;
    };

}
#endif //COMMON_TIMER_H
