////
//// Created by liuke on 16/1/17.
////
//
//#include <map>
//#include "timer_wrap.h"
//
//namespace plan9
//{
//    class timer_wrap::timer_wrap_impl
//    {
//    public:
//
//        int create_timer(long repeat_millisecond, std::function<void(void)> function) {
//            clear_timer();
//
//            std::shared_ptr<timer> t(new timer);
//            int id = -1;
//            if (repeat_millisecond > 0) {
//                id = t->start(function, delay_millisecond, repeat_millisecond);
//            } else {
//                id = t->start(function, delay_millisecond);
//            }
//            timer_map[id] = t;
//            return id;
//        }
//
//        void clear_timer() {
//            std::map<int, std::shared_ptr<timer>>::iterator it = timer_map.begin();
//            while (it != timer_map.end()) {
//                if (it->second->isOver()) {
//                    timer_map.erase(it ++);
//                } else {
//                    ++ it;
//                }
//            }
//        }
//
//        void cancel_timer(int id) {
//            if (timer_map.find(id) != timer_map.end()) {
//                auto v = timer_map[id];
//                v->cancel();
//                timer_map.erase(id);
//            }
//        }
//
//    private:
//        std::map<int, std::shared_ptr<timer>> timer_map;
//    };
//
//
//    timer_wrap::timer_wrap() : impl(new timer_wrap_impl) {
//
//    }
//
//    timer_wrap timer_wrap::instance() {
//        static timer_wrap timer_;
//        return timer_;
//    }
//
//    int timer_wrap::create_timer(long delay_millisecond, std::function<void(void)> function) {
//        return impl->create_timer(delay_millisecond, 0, function);
//    }
//
//    int timer_wrap::create_timer(long delay_millisecond, long repeat_millisecond, std::function<void(void)> function) {
//        return impl->create_timer(delay_millisecond, repeat_millisecond, function);
//    }
//
//    void timer_wrap::cancel_timer(int id) {
//        impl->cancel_timer(id);
//    }
//}