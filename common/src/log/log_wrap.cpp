//
// Created by liuke on 15/12/14.
//

#include "log_wrap.h"
#include <boost/date_time.hpp>

namespace plan9
{
    std::string log_wrap::path = ".";

    log_wrap log_wrap::io() {
        static log_wrap wrap;
        static bool init = false;
        if (!init) {
            wrap.log.reset(new plan9::log);
            wrap.log->setFile(log_wrap::path + "/io.log");
            init = true;
        }
        std::string strTime = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
        // 这时候strTime里存放时间的格式是YYYYMMDDTHHMMSS，日期和时间用大写字母T隔开了
        int pos = strTime.find('T');
        strTime.replace(pos,1,std::string("-"));
        strTime.replace(pos + 3,0,std::string(":"));
        strTime.replace(pos + 6,0,std::string(":"));
        std::cout << strTime.c_str() << std::endl;

        strTime = boost::gregorian::to_iso_string(boost::gregorian::day_clock::local_day());
        std::cout << strTime.c_str() << std::endl;
        return wrap;
    }

    log_wrap log_wrap::net() {
        static log_wrap wrap;
        return wrap;
    }

    log_wrap log_wrap::debug() {
        static log_wrap wrap;
        return wrap;
    }

    void log_wrap::set_log_dir(std::string path) {
        log_wrap::path = path;
    }

    log_wrap log_wrap::other() {
        static log_wrap wrap;
        return wrap;
    }
}