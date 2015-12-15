//
// Created by liuke on 15/12/15.
//

#include <string>
#include "time.h"
#include <boost/date_time.hpp>

namespace plan9
{

    std::string time::current_data() {
        auto tt = std::chrono::system_clock::to_time_t
                (std::chrono::system_clock::now());
        struct tm* ptm = localtime(&tt);
        char date[60] = {0};
        sprintf(date, "%d-%02d-%02d",
                ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
        return std::string(date);
    }

    std::string time::current_data_time() {
        auto tt = std::chrono::system_clock::to_time_t
                (std::chrono::system_clock::now());
        struct tm* ptm = localtime(&tt);
        char date[60] = {0};
        sprintf(date, "%d-%02d-%02d %02d:%02d:%02d.%d",
                ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
                ptm->tm_hour, ptm->tm_min, ptm->tm_sec, microsecond_in_second());
        return std::string(date);
    }

    long long int time::microseconds() {
        using namespace std::chrono;
        system_clock::time_point today = system_clock::now();
        std::chrono::microseconds m = today.time_since_epoch();
        long long int count = m.count();
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

    int time::microsecond_in_second() {
        long long int micro = microseconds();
        long long int mili = micro / 1000000 * 1000000;
        return micro - mili;
    }
}