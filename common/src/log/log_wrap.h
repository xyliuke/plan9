//
// Created by liuke on 15/12/14.
//

#ifndef COMMON_LOG_WRAP_H
#define COMMON_LOG_WRAP_H


#include <iosfwd>
#include <string>
#include <log/log.h>

namespace plan9
{
    class log_wrap {
    public:
        enum log_level {
            INFO,
            WARN,
            ERROR
        };

        static log_wrap io();
        static log_wrap net();
        static log_wrap ui();
        static log_wrap other();
        static void set_log_dir(std::string path);

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
        void i(std::string msg);
        /**
         * WARN级别日志
         */
        void w(std::string msg);
        /**
         * ERROR级别日志
         */
        void e(std::string msg);

    private:
        log_wrap(std::string, std::string);
        static std::string path;
        class log_wrap_impl;
        std::shared_ptr<log_wrap_impl> impl;
    };
}



#endif //COMMON_LOG_WRAP_H
