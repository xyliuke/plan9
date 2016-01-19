//
// Created by liuke on 15/12/19.
//

#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H


#include <memory>
#include <sstream>

namespace plan9 {

    class util {
    public:
        static util instance();

        template <typename T>
        std::string cat(T v) {
            std::stringstream ss;
            ss << v;
            return ss.str();
        }

        template <typename head, typename ... rail>
        std::string cat(head h, rail... r) {
            std::stringstream ss;
            ss << h;
            ss << cat(r...);
            return ss.str();
        };


        /**
         * 判断字符串是否以指定字符结尾
         * @param str 字符串
         * @param end 结尾字符
         * @return 如果str是以end结尾,返回true;否则返回false
         */
        bool isSuffix(std::string str, char end);

    private:
        util();
        class util_impl;
        std::shared_ptr<util_impl> impl;
    };

}


#endif //COMMON_UTIL_H
