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

        void set_level(log_level level);
        void i(std::string msg);
        void w(std::string msg);
        void e(std::string msg);

    private:
        log_wrap(std::string, std::string);
        static std::string path;
        class log_wrap_impl;
        std::shared_ptr<log_wrap_impl> impl;
    };
}



#endif //COMMON_LOG_WRAP_H
