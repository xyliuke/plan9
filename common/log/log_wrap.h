//
// Created by liuke on 15/11/1.
//

#ifndef COMMON_LOG_WRAP_H
#define COMMON_LOG_WRAP_H

#include <iosfwd>
#include <string>

namespace plan9 {


    class log_wrap {

    public:
        static void logI(std::string format, ...);

        static void logW(std::string format, ...);

        static void logE(std::string format, ...);

        static void logF(std::string format, ...);

    };

}

#endif //COMMON_LOG_WRAP_H
