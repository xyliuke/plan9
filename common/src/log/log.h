//
// Created by liuke on 15/12/13.
//

#ifndef COMMON_LOG_H
#define COMMON_LOG_H


#include <iosfwd>
#include <string>
#include <fstream>

namespace plan9
{
    class log {
    public:
        void setFile(std::string filepath);
        void write(std::string msg);
        void close();

    private:
        std::ofstream ofs;
    };

}



#endif //COMMON_LOG_H
