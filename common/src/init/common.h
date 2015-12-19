//
// Created by liuke on 15/12/18.
//

#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

#include <iosfwd>
#include <string>

namespace plan9
{
    class common {
    public:
        static void init(std::string path, std::string lua_path);

    private:
        static void init_function();
        static void init_log();
        static void init_lua();

    private:
        static std::string path;
        static std::string lua_path;
    };

}



#endif //COMMON_COMMON_H
