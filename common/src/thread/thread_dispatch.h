//
// Created by liuke on 15/10/31.
//

#ifndef COMMON_THREAD_DISPATCH_H
#define COMMON_THREAD_DISPATCH_H


#include <functional>
#include <thread>
#include <map>
#include <vector>

namespace plan9 {

    class thread_dispatch {
    public:

        static bool create(int tid);

        static void post(int tid, std::function<void(void)> func);

        static void run(int tid);

        static void stop();

    private:
        static std::map<int, std::shared_ptr<std::thread>> thread_map;
        static std::map<int, std::shared_ptr<std::vector<std::function<void(void)>>>> thread_queue;
        static std::mutex mutex;
        static bool stop_;
    };
}


#endif //COMMON_THREAD_DISPATCH_H
