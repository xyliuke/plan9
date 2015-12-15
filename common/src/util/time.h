//
// Created by liuke on 15/12/15.
//

#ifndef COMMON_TIME_H
#define COMMON_TIME_H


#include <iosfwd>

namespace plan9
{
    /**
     * 用于创建时间相关的函数
     */
    class time {
    public:
        /**
         * 生成当前日期的字符串,格式为2015-12-15
         */
        static std::string current_data();
        /**
         * 生成当前日期和时间的字符串,格式为2015-12-15 11:11:11.123.精确到毫秒
         */
        static std::string current_data_time();

        /**
         * 得到1970至今的微秒数
         */
        static long long int microseconds();
        /**
         * 得到1970至今的毫秒数
         */
        static long milliseconds();
        /**
         * 得到1970至今的秒数
         */
        static long seconds();
        /**
         * 今天是几号,范围（1-31)
         */
        static int day();

    private:
        //1秒中的微秒数
        static int microsecond_in_second();
    };

}


#endif //COMMON_TIME_H
