//
// Created by liuke on 15/10/31.
//

#include "thread_dispatch.h"
#include <log/log_wrap.h>
#include <util/time.h>


namespace plan9 {

    class thread_dispatch::thread_timer {
    public:
        int id;
        long millisecond;//执行时的时间点,为1970到今的毫秒数
        long interval;
        std::function<void(void)> function;
        bool repeat;
    };

    static std::map<int, std::string> tid_thread_id;

    std::map<int, std::shared_ptr<std::thread>> thread_dispatch::thread_map;
    std::map<int, std::shared_ptr<std::list<std::function<void(void)>>>> thread_dispatch::thread_queue;
    std::map<int, std::shared_ptr<std::map<int, std::shared_ptr<thread_dispatch::thread_timer>>>> thread_dispatch::thread_timer_queue;
    std::recursive_mutex thread_dispatch::mutex;
    bool thread_dispatch::stop_ = false;
    int thread_dispatch::timer_precision = 50;//默认timer精度
    static const uint16_t timer_loop_count = 5000;
    class thread_dispatch::thread_mutex_raii {
    public:
        thread_mutex_raii() {
            thread_dispatch::mutex.lock();
        }
        ~thread_mutex_raii() {
            thread_dispatch::mutex.unlock();
        }

    };


    bool thread_dispatch::create(int tid) {
        if (thread_map.find(tid) != thread_map.end()) {
            return true;
        }
        uint16_t count = 0;
        std::shared_ptr<std::thread> thread(new std::thread([=]() mutable {
            std::stringstream ss;
            ss << std::this_thread::get_id();
            tid_thread_id[tid] = ss.str();
            while (true) {
                if (stop_) {
                    break;
                }
                
                std::function<void(void)> func = nullptr;
                
                {
                    thread_mutex_raii();
                    
                    if (timer_loop_count == count) {
                        op_timer(tid);
                        count = 0;
                    }
                    ++ count;

                    
                    if (thread_queue.find(tid) != thread_queue.end()) {
                        auto queue = thread_queue[tid];
                        if (queue->empty()) {
                            std::this_thread::yield();
                        } else {
                            func = queue->front();
                            queue->pop_front();
                        }
                    }
    
                }
                
                if (func != nullptr) {
                    func();
                }
            }
        }));
        thread->detach();
        thread_map.insert(std::pair<int, std::shared_ptr<std::thread>>(tid, thread));
        return true;
    }



    void thread_dispatch::post(int tid, std::function<void(void)> func) {
        thread_mutex_raii();
        if (thread_map.find(tid) != thread_map.end()) {
            auto t = thread_map[tid];
            std::stringstream ss;
            ss << std::this_thread::get_id();
            if (tid_thread_id[tid] == ss.str()) {
                log_wrap::io().d("current thread is background thread");
                func();
                return;
            }
            if (thread_queue.find(tid) != thread_queue.end()) {
                auto queue = thread_queue[tid];
                queue->push_back(func);
            } else {
                std::shared_ptr<std::list<std::function<void(void)>>> queue(new std::list<std::function<void(void)>>);
                queue->push_back(func);
                thread_queue.insert(std::pair<int, std::shared_ptr<std::list<std::function<void(void)>>>>(tid, queue));
            }
        } else {
            log_wrap::io().e("没有创建相应线程");
        }
    }

    int thread_dispatch::post(int tid, std::function<void(void)> func, long millisecond, bool repeat) {
        static int id = 0;
        thread_mutex_raii();

        ++ id;

        std::shared_ptr<thread_dispatch::thread_timer> timer(new thread_timer);
        timer->id = id;
        timer->function = func;
        timer->millisecond = (plan9::time::milliseconds() + millisecond);
        timer->repeat = repeat;
        timer->interval = millisecond;

        if (thread_map.find(tid) != thread_map.end()) {
            if (thread_timer_queue.find(tid) != thread_timer_queue.end()) {
                auto queue = thread_timer_queue[tid];
                queue->insert(std::pair<int, std::shared_ptr<thread_dispatch::thread_timer>>(id, timer));
            } else {
                std::shared_ptr<std::map<int, std::shared_ptr<thread_dispatch::thread_timer>>> queue(new std::map<int, std::shared_ptr<thread_dispatch::thread_timer>>);
                queue->insert(std::pair<int, std::shared_ptr<thread_dispatch::thread_timer>>(id, timer));
                thread_timer_queue.insert(std::pair<int, std::shared_ptr<std::map<int, std::shared_ptr<thread_dispatch::thread_timer>>>>(tid, queue));
            }
        } else {
            log_wrap::io().e("没有创建相应线程");
        }

        return id;
    }


    void thread_dispatch::cancel(int tid, int timer_id) {
        thread_mutex_raii();

        if (thread_map.find(tid) != thread_map.end()) {
            if (thread_timer_queue.find(tid) != thread_timer_queue.end()) {
                auto queue = thread_timer_queue[tid];
                if (queue->find(timer_id) != queue->end()) {
                    auto t = (*queue)[timer_id];
                    t->repeat = false;
                    t->millisecond = 0;
                    queue->erase(timer_id);
                }
            }
        } else {
            log_wrap::io().e("没有创建相应线程");
        }
    }

    void thread_dispatch::set_timer_precision(int milliseconds) {
        if (milliseconds > 0 && milliseconds < 1000) {
            timer_precision = milliseconds;
        }
    }

    void thread_dispatch::op_timer(int tid) {
        static long last_op_time = 0;

        long current_time = plan9::time::milliseconds();
        if (current_time - last_op_time < timer_precision) {
            //少于最小精度值不执行
            return;
        }
        last_op_time = current_time;
        if(thread_timer_queue.find(tid) != thread_timer_queue.end()) {
            auto queue = thread_timer_queue[tid];
            std::map<int, std::shared_ptr<thread_dispatch::thread_timer>>::iterator it = queue->begin();
            while (queue->size() > 0 && it != queue->end()) {
                auto func = it->second;
                if (!func) {
                    queue->erase(it);
                    continue;
                }
                if (current_time > func->millisecond) {
                    if (func && func->function != nullptr) {
                        func->function();
                    }
                    if (func && func->repeat) {
                        func->millisecond += func->interval;
                    }
                }
                if (queue->size() > 0 && it->second && it != queue->end()) {
                    if (func && func->repeat) {
                        ++ it;
                    } else {
                        it = queue->erase(it);
                    }
                }
            }
        }
    }

    void thread_dispatch::stop() {
        thread_mutex_raii();
        stop_ = true;
        for (auto it = thread_map.begin(); it != thread_map.end() ; ++ it) {
            if (it->second->joinable()) {
                it->second->join();
            }
        }
        thread_map.clear();
    }
}
