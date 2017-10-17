//
//  thread_wrap.cpp
//  libuv_test
//
//  Created by ke liu on 11/10/2017.
//  Copyright © 2017 ke liu. All rights reserved.
//

#include "uv_thread_wrap.hpp"
#include "uv.h"
#include <map>
#include <sstream>
#include <list>

class function_wrap;

static uv_thread_t* thread = nullptr;
static uv_loop_t* loop = nullptr;
static std::map<int, uv_timer_t*> timer_map;
static const int TIMER_EVER_LOOP = -1234;
static std::map<int, function_wrap*> serial_map;
static std::map<int, uv_work_t*> concurrent_map;
static std::list<uv_work_t*> reuse_concurrent_array;
static std::list<uv_idle_t*> reuse_idle_array;
static std::list<uv_timer_t*> reuse_timer_array;

class function_wrap {
public:
    function_wrap(std::function<void(void)> func) : function(func) {
    }
    function_wrap(std::function<void(void)> func, int times) : function(func) {
        if (times > 0 || times == TIMER_EVER_LOOP) {
            max_times = times;
            current_times = times;
        }
    }
    ~function_wrap() {
        function = nullptr;
    }
    void run() {
        if (function != nullptr) {
            function();
        }
    }
    bool minus() {
        if (current_times == 0) {
            return true;
        } else if(current_times == TIMER_EVER_LOOP) {
            return false;
        }
        current_times --;
        return false;
    }
    
    int id;
    bool is_canceled;
private:
    std::function<void(void)> function;
    int max_times;
    int current_times;
};

static uv_mutex_t* get_timer_mutex() {
    static uv_mutex_t* timer_map_mutex = nullptr;
    if (timer_map_mutex == nullptr) {
        timer_map_mutex = new uv_mutex_t;
        uv_mutex_init(timer_map_mutex);
    }
    return timer_map_mutex;
}

static uv_mutex_t* get_serial_mutex() {
    static uv_mutex_t* mutex = nullptr;
    if (mutex == nullptr) {
        mutex = new uv_mutex_t;
        uv_mutex_init(mutex);
    }
    return mutex;
}

static uv_mutex_t* get_concurrent_mutex() {
    static uv_mutex_t* mutex = nullptr;
    if (mutex == nullptr) {
        mutex = new uv_mutex_t;
        uv_mutex_init(mutex);
    }
    return mutex;
}

static void idle_callback(uv_idle_t* handle) {
    if (handle->data != nullptr) {
        auto func = (function_wrap*)(handle->data);
        func->run();
        delete func;
        handle->data = nullptr;
        reuse_idle_array.push_back(handle);
    }
    uv_close((uv_handle_t*)handle, nullptr);
}

static void idle_loop_callback(uv_idle_t* handle) {
}

static void thread_cb(void* arg) {
    loop = new uv_loop_t;
    uv_loop_init(loop);
    if (arg != nullptr) {
        auto func = (function_wrap*)arg;
        func->run();
        delete(func);
    }
    uv_idle_t* idle = new uv_idle_t;
    uv_idle_init(loop, idle);
    uv_idle_start(idle, idle_loop_callback);
    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    delete thread;
    thread = nullptr;
    delete loop;
    loop = nullptr;
}

static void async_callback(uv_async_t* async) {
    if (async->data != nullptr) {
        auto func = (function_wrap*)async->data;
        if (!func->is_canceled) {
            func->run();
        }
        uv_mutex_trylock(get_serial_mutex());
        serial_map.erase(func->id);
        uv_mutex_unlock(get_serial_mutex());
        delete func;
    }
    uv_close((uv_handle_t*)async, nullptr);
}

static void init_loop(std::function<void(void)> callback) {
    if (thread == nullptr && loop == nullptr) {
        thread = new uv_thread_t;
        uv_thread_create(thread, thread_cb, new function_wrap(callback));
    } else {
        if (callback) {
            callback();
        }
    }
}

static void timer_callback(uv_timer_t* handle) {
    if (handle->data != nullptr) {
        auto func = (function_wrap*)handle->data;
        if (func->minus()) {
            uv_timer_stop(handle);
            uv_mutex_trylock(get_timer_mutex());
            timer_map.erase(func->id);
            reuse_timer_array.push_back(handle);
            uv_mutex_unlock(get_timer_mutex());
            delete func;
        } else {
            func->run();
        }
    }
}

static void queue_callback(uv_work_t* work) {
    if (work->data != nullptr) {
        function_wrap* func = (function_wrap*)(work->data);
        func->run();
        work->data = nullptr;
        uv_mutex_trylock(get_concurrent_mutex());
        concurrent_map.erase(func->id);
        reuse_concurrent_array.push_back(work);
        uv_mutex_unlock(get_concurrent_mutex());
        delete(func);
    }
}

void uv_thread_wrap::init(std::function<void(void)> callback) {
    init_loop(callback);
}

int uv_thread_wrap::post_serial_queue(std::function<void ()> callback) {
    static int count = 0;
    count ++;
    uv_async_t* async = new uv_async_t;
    auto f = new function_wrap(callback);
    f->id = count;
    f->is_canceled = false;
    async->data = (void*)f;
    uv_async_init(loop, async, async_callback);
    uv_mutex_trylock(get_serial_mutex());
    serial_map[count] = f;
    uv_mutex_unlock(get_serial_mutex());
    uv_async_send(async);
    return count;
}

void uv_thread_wrap::cancel_serial(int id) {
    uv_mutex_trylock(get_serial_mutex());
    if (serial_map.find(id) != serial_map.end()) {
        auto f = serial_map[id];
        f->is_canceled = true;
    }
    uv_mutex_unlock(get_serial_mutex());
}

void uv_thread_wrap::set_concurrent_pool_size(int size) {
    std::stringstream ss;
    ss << size;
    std::string s = ss.str();
    uv_os_setenv("UV_THREADPOOL_SIZE", s.c_str());
}

int uv_thread_wrap::post_concurrent_queue(std::function<void ()> callback) {
    static int count = 0;
    count ++;

    uv_mutex_trylock(get_concurrent_mutex());
    
    uv_work_t* work = nullptr;
    if (reuse_concurrent_array.size() > 0) {
        work = *(reuse_concurrent_array.begin());
        reuse_concurrent_array.erase(reuse_concurrent_array.begin());
    } else {
        work = new uv_work_t;
    }
    
    auto f = new function_wrap(callback);
    f->id = count;
    work->data = (void*)(f);
    concurrent_map[count] = work;
    
    uv_mutex_unlock(get_concurrent_mutex());
    uv_queue_work(loop, work, queue_callback, nullptr);

    return count;
}

void uv_thread_wrap::cancel_concurrent(int id) {
    uv_mutex_trylock(get_concurrent_mutex());
    if (concurrent_map.find(id) != concurrent_map.end()) {
        auto f = concurrent_map[id];
        uv_cancel((uv_req_t*)f);
    }
    uv_mutex_unlock(get_concurrent_mutex());
}

void uv_thread_wrap::post_idle(std::function<void ()> callback) {
    uv_idle_t* idle_handle = nullptr;
    if (reuse_idle_array.size() > 0) {
        idle_handle = *(reuse_idle_array.begin());
        reuse_idle_array.erase(reuse_idle_array.begin());
    } else {
        idle_handle = new uv_idle_t;
    }
    idle_handle->data = new function_wrap(callback);
    uv_idle_init(loop, idle_handle);
    uv_idle_start(idle_handle, idle_callback);
}

int uv_thread_wrap::post_timer(std::function<void ()> callback, long time, long repeat) {
    return post_timer(callback, time, repeat, TIMER_EVER_LOOP);
}

int uv_thread_wrap::post_timer(std::function<void ()> callback, long time, long repeat, int times) {
    static int timer_id = 0;
    timer_id ++;

    uv_mutex_trylock(get_timer_mutex());
    uv_timer_t* timer = nullptr;
    if (reuse_timer_array.size() > 0) {
        timer = *(reuse_timer_array.begin());
        reuse_timer_array.erase(reuse_timer_array.begin());
    } else {
        timer = new uv_timer_t;
    }
    timer_map[timer_id] = timer;
    auto func = new function_wrap(callback, times);
    func->id = timer_id;
    timer->data = (void*)(func);
    uv_mutex_unlock(get_timer_mutex());
    uv_timer_init(loop, timer);
    uv_timer_start(timer, timer_callback, time, repeat);
    
    return timer_id;
}

void uv_thread_wrap::cancel_timer(int timer_id) {
    uv_mutex_trylock(get_timer_mutex());
    if (timer_map.find(timer_id) != timer_map.end()) {
        auto t = timer_map[timer_id];
        uv_timer_stop(t);
        timer_map.erase(timer_id);
        reuse_timer_array.push_back(t);
    }
    uv_mutex_unlock(get_timer_mutex());
}








