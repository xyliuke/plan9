//
//  thread_wrap.hpp
//  libuv_test
//
//  Created by ke liu on 11/10/2017.
//  Copyright © 2017 ke liu. All rights reserved.
//

#ifndef thread_wrap_hpp
#define thread_wrap_hpp

#include <string>
#include <functional>

class uv_thread_wrap {
public:
    static void init(std::function<void(void)> callback);
    static void set_concurrent_pool_size(int size);
    static int post_serial_queue(std::function<void(void)> callback);
    static void cancel_serial(int id);
    static int post_concurrent_queue(std::function<void(void)> callback);
    static void cancel_concurrent(int id);
    static void post_idle(std::function<void(void)> callback);
    static int post_timer(std::function<void(void)> callback, long time, long repeat);
    static int post_timer(std::function<void(void)> callback, long time, long repeat, int repeat_times);
    static void cancel_timer(int timer_id);
};

#endif /* thread_wrap_hpp */
