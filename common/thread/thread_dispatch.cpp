//
// Created by liuke on 15/10/31.
//

#include "thread_dispatch.h"
#include "../log/log.h"

namespace plan9 {

    std::map<int, std::shared_ptr<std::thread>> thread_dispatch::thread_map;
    std::map<int, std::shared_ptr<std::vector<std::function<void(void)>>>> thread_dispatch::thread_queue;
    std::mutex thread_dispatch::mutex;
    bool thread_dispatch::stop_ = false;

    bool thread_dispatch::create(int tid) {
        if (thread_map.find(tid) != thread_map.end()) {
            return true;
        }

        std::shared_ptr<std::thread> thread(new std::thread([=]() {
            while (true) {
                if (stop_) {
                    break;
                } else {
                    mutex.lock();
                    if (thread_queue.find(tid) != thread_queue.end()) {
                        auto queue = thread_queue[tid];
                        if (queue->empty()) {
                            mutex.unlock();
                            std::this_thread::yield();
                        } else {
                            std::function<void(void)> func = queue->at(0);
                            queue->erase(queue->begin());

                            mutex.unlock();

                            func();
                        }
                    } else {
                        mutex.unlock();
                    }
                }
            }
        }));
        thread->detach();
        thread_map.insert(std::pair<int, std::shared_ptr<std::thread>>(tid, thread));
        return true;
    }

    void thread_dispatch::post(int tid, std::function<void(void)> func) {
        mutex.lock();
        if (thread_map.find(tid) != thread_map.end()) {
            if (thread_queue.find(tid) != thread_queue.end()) {
                auto queue = thread_queue[tid];
                queue->push_back(func);
                int i = queue->size();
                i ++;
            } else {
                std::shared_ptr<std::vector<std::function<void(void)>>> queue(new std::vector<std::function<void(void)>>);
                queue->push_back(func);
                thread_queue.insert(std::pair<int, std::shared_ptr<std::vector<std::function<void(void)>>>>(tid, queue));
            }
        } else {
            log::logE("没有创建相应线程");
        }
        mutex.unlock();
    }

    void thread_dispatch::run(int tid) {
        auto queue = thread_queue[tid];
        for (std::vector<std::function<void(void)>>::iterator i = queue->begin(); i != queue->end(); ++i) {
            (*i)();
        }
    }

    void thread_dispatch::stop() {
        mutex.lock();
        stop_ = true;
        for (auto it = thread_map.begin(); it != thread_map.end() ; ++ it) {
            if (it->second->joinable()) {
                it->second->join();
            }
        }
        thread_map.clear();
        mutex.unlock();
    }

}