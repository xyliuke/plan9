//
// Created by liuke on 15/12/15.
//

#include <string>
#include "time.h"
#include <boost/date_time.hpp>
#include <chrono>

namespace plan9
{
    static int day_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    std::string time::current_data() {
        auto tt = std::chrono::system_clock::to_time_t
                (std::chrono::system_clock::now());
        struct tm* ptm = std::localtime(&tt);
        char date[60] = {0};
        sprintf(date, "%d-%02d-%02d",
                ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
        return std::string(date);
    }

    void time::current_date(int *year, int *month, int *day) {
        auto tt = std::chrono::system_clock::to_time_t
                (std::chrono::system_clock::now());
        struct tm* ptm = std::localtime(&tt);
        *year = ptm->tm_year + 1900;
        *month = ptm->tm_mon + 1;
        *day = ptm->tm_mday;
    }

    std::string time::current_data_time() {
        auto tt = std::chrono::system_clock::to_time_t
                (std::chrono::system_clock::now());
        struct tm* ptm = std::localtime(&tt);
        char date[60] = {0};
        sprintf(date, "%d-%02d-%02d %02d:%02d:%02d.%06d",
                ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
                ptm->tm_hour, ptm->tm_min, ptm->tm_sec, microsecond_in_second());
        return std::string(date);
    }

    long long int time::microseconds() {
        using namespace std::chrono;
        system_clock::time_point today = system_clock::now();
        long long int count = today.time_since_epoch().count();
        return count;
    }

    long time::milliseconds() {
        long long int mic = microseconds();
        long ret = mic / 1000;
        return ret;
    }

    long time::seconds() {
        long long int mic = microseconds();
        long ret = mic / 1000000;
        return ret;
    }

    int time::day() {
        auto tt = std::chrono::system_clock::to_time_t
                (std::chrono::system_clock::now());
        struct tm* ptm = localtime(&tt);
        return ptm->tm_mday;
    }

    int time::duration_days(int year, int month, int day) {
        int cyear, cmonth, cday;
        current_date(&cyear, &cmonth, &cday);
        return days(cyear, cmonth, cday, year, month, day);
    }

    int time::duration_days(std::string date) {
        if (date.size() != 10) {
            return -1;
        }

        int year, month, day;
        std::stringstream ss_y(date.substr(0, 4));
        ss_y >> year;

        std::stringstream ss_m(date.substr(5, 2));
        ss_m >> month;

        std::stringstream ss_d(date.substr(8, 2));
        ss_d >> day;

        return duration_days(year, month, day);
    }

    bool time::is_leap_year(int year) {
        return (((year % 4) == 0) && (year % 100) != 0) || (year % 400 == 0);
    }

    int time::microsecond_in_second() {
        long long int micro = microseconds();
        long long int mili = micro / 1000000 * 1000000;
        return (int)(micro - mili);
    }

    int time::days(int year, int month, int day, int year_old, int month_old, int day_old) {
        if (year == year_old) {
            int old = days(year_old, month_old, day_old);
            int newer = days(year, month, day);
            return newer - old;
        } else {
            int old = days(year_old, month_old, day_old);
            int newer = days(year, month, day);
            int ret = abs(newer - old);
            for (int i = year_old; i < year; i ++) {
                ret += days(i);
            }
            if (year > year_old) {
                return ret;
            } else {
                return -ret;
            }
        }
    }

    int time::days(int year) {
        if (is_leap_year(year)) {
            return 366;
        }
        return 365;
    }

    int time::days(int year, int month, int day) {
        int ret = 0;
        for (int i = 1; i < month; i ++) {
            if (i == 2 && is_leap_year(year)) {
                ret += 1;
            }
            ret += day_month[i - 1];
        }
        ret += day;
        return ret;
    }
}