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
         * 得到当前年月日
         */
        static void current_date(int* yead, int* month, int* day);

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

        /**
         * 计算指定年月日和当前时间的天数差
         */
        static int duration_days(int year, int month, int day);

        /**
         * 计算指定时间格式字符串和当前时间的天数差
         * @param date 时间格式为:2015-12-13,否则返回-1
         */
        static int duration_days(std::string date);

        /**
         * 是否闰年
         */
        static bool is_leap_year(int yead);

        /*
         * 计算前面三个参数距后面三个参数的天数
         */
        static int days(int year, int month, int day, int year_old, int month_old, int day_old);
        /**
         * 计算指定年份有多少天,闰年366年,其他365天
         */
        static int days(int year);
        /**
         * 计算指定参数给定的时间这一年,一共过了多少天
         */
        static int days(int year, int month, int day);

    private:
        //1秒中的微秒数
        static int microsecond_in_second();

    };

}


#endif //COMMON_TIME_H
