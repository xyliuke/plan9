////
//// Created by liuke on 16/1/17.
////
//
//#ifndef COMMON_TIMER_WRAP_H
//#define COMMON_TIMER_WRAP_H
//
//#include <memory>
//#include <thread/timer.h>
//
//namespace plan9
//{
//    class timer_wrap {
//    public:
//        static timer_wrap instance();
//
//        /**
//         * 只执行一次的timer
//         * @param delay_millisecond 延迟时间
//         * @param function 执行回调
//         * @return 返回timer的id
//         */
//        int create_timer(long delay_millisecond, std::function<void(void)> function);
//
//        /**
//         * 重复执行的timer
//         * @param repeat_millisecond   重复执行的时间间隔
//         * @param repeat 是否重复
//         * @param function 执行回调
//         * @return 返回timer的id
//         */
//        int create_timer(long repeat_millisecond, bool repeat, std::function<void(void)> function);
//        /**
//         * 取消timer的执行
//         * @param id timer的id
//         */
//        void cancel_timer(int id);
//
//    private:
//        timer_wrap();
//        class timer_wrap_impl;
//        std::shared_ptr<timer_wrap_impl> impl;
//    };
//
//}
//
//
//
//#endif //COMMON_TIMER_WRAP_H
