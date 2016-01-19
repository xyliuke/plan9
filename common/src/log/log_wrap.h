//
// Created by liuke on 15/12/14.
//

#ifndef COMMON_LOG_WRAP_H
#define COMMON_LOG_WRAP_H


#include <string>
#include <log/log.h>
#include <sstream>
#include <util/util.h>

namespace plan9
{
    class log_wrap {
    public:
        enum class log_level {
            L_DEBUG,
            L_INFO,
            L_WARN,
            L_ERROR
        };

        static log_wrap io();
        static log_wrap net();
        static log_wrap ui();
        static log_wrap lua();
        static log_wrap other();
        static void set_log_dir(std::string path);

        static void set_all_level(log_level level);

        /**
         * 设置日志输出等级
         */
        void set_level(log_level level);
        /**
         * 设置日志保存时间,大于这个天数的日志将被删除,默认值为10000
         * @param days 天数
         */
        void set_duration(int days);

        /**
         * INFO级别日志
         */
        void d_(std::string msg);

        template <typename head, typename ... rail>
        void d(head h, rail... r) {
            d_(plan9::util::instance().cat(h, r...));
        };

        /**
         * INFO级别日志
         */
        void i_(std::string msg);

        template <typename head, typename ... rail>
        void i(head h, rail... r) {
            i_(plan9::util::instance().cat(h, r...));
        };

        /**
         * WARN级别日志
         */
        void w_(std::string msg);
        template <typename head, typename ... rail>
        void w(head h, rail... r) {
            w_(plan9::util::instance().cat(h, r...));
        };
        /**
         * ERROR级别日志
         */
        void e_(std::string msg);
        template <typename head, typename ... rail>
        void e(head h, rail... r) {
            e_(plan9::util::instance().cat(h, r...));
        };

    private:
        log_wrap(std::string, std::string);
        static std::string path;
        class log_wrap_impl;
        std::shared_ptr<log_wrap_impl> impl;
    };

}



#endif //COMMON_LOG_WRAP_H
