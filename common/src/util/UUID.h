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

        /**
         * 生成一个0-9999的随机数
         */
        static int random();
        /**
         * 生成一个0到指定max值的随机数
         * @param max 指定随机数的范围
         */
        static int random(int max);
    };

}


#endif //COMMON_UUID_H
