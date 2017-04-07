//
// Created by liuke on 15/10/31.
//

#ifndef COMMON_THREAD_DISPATCH_H
#define COMMON_THREAD_DISPATCH_H


#include <functional>
#include <thread>
#include <map>
#include <list>
#include <mutex>

namespace plan9 {

    class thread_dispatch {
    public:

        //通过id创建一个线程
        static bool create(int tid);
        //向相应线程中post一个方法
        static void post(int tid, std::function<void(void)> func);
        //
//        static void run(int tid);
        //停止所有线程
        static void stop();
        /**
         * 向相应线程中post一个方法,second秒后执行.
         * @param tid 线程id
         * @param func 方法体
         * @param millisecond 延迟的毫秒数
         * @return 返回相应任务的id,用于取消
         */
        static int post(int tid, std::function<void(void)>func, long millisecond, bool repeat);
        /**
         * 取消一个timer
         * @param tid 线程id
         * @param timer_id timer的id,由post(int tid, std::function<void(void)>func, int second)返回值获得
         */
        static void cancel(int tid, int timer_id);

        /**
         * 设置timer的精度.为了减少运行的时间,可设置timer的精度,不需要循环检查各个任务的超时时间
         * @param milliseconds 最小检查时间,超过这个时间再去检查,默认值为50ms.建议最大不用超过1s
         */
        static void set_timer_precision(int milliseconds);

    private:
        class thread_timer;
        class thread_mutex_raii;
        static std::map<int, std::shared_ptr<std::thread>> thread_map;
        static std::map<int, std::shared_ptr<std::list<std::function<void(void)>>>> thread_queue;
        static std::map<int, std::shared_ptr<std::map<int, std::shared_ptr<thread_timer>>>> thread_timer_queue;
        static std::recursive_mutex mutex;
        static bool stop_;
        static int timer_precision;
    private:
        void static op_timer(int tid);
    };
}


#endif //COMMON_THREAD_DISPATCH_H
