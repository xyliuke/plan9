//
// Created by liuke on 15/10/27.
//

#ifndef COMMON_LOG_H
#define COMMON_LOG_H


#include <iosfwd>
#include <string>

class log {
public:
    static void init(std::string log_dir);

    static void logI(std::string msg);
    static void logW(std::string msg);
    static void logE(std::string msg);
    static void logF(std::string msg);


private:
//    class log_impl_;
//    static bool is_init;
//    static std::shared_ptr<log_impl_> impl_;
};


#endif //COMMON_LOG_H
