//
// Created by liuke on 15/12/12.
//

#ifndef COMMON_UUID_H
#define COMMON_UUID_H


#include <iosfwd>

namespace plan9
{
    class UUID {
    public:
        /**
         * 生成一个标准的UUID,但会比较耗时,运行10000次,共耗时53s250ms（本机测试）
         */
        static std::string uuid();
        /**
         * 生成一个不重复id,以当前时间为基准,性能比较好.运行10000次,共耗时9ms（本机测试）
         */
        static std::string id();
    };

}


#endif //COMMON_UUID_H
