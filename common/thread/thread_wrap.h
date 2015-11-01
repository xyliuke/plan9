//
// Created by liuke on 15/11/1.
//

#ifndef COMMON_THREAD_WRAP_H
#define COMMON_THREAD_WRAP_H


#include <functional>

namespace plan9 {

    class thread_wrap {
    public:

        /**
         * 向底层线程post一个方法
         */
        static void post_background(std::function<void(void)> func);

        /**
         * 向网络线程post一个方法
         */
        static void post_network(std::function<void(void)> func);

        /**
         * 向IO线程post一个方法
         */
        static void post_io(std::function<void(void)> func);

        /**
         * 停止上面所有线程
         */
        static void stop();
    };
}

#endif //COMMON_THREAD_WRAP_H
