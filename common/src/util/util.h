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
        bool is_suffix(std::string str, char end);

        /**
         * 去掉字符串前后的空格,回车,\r字符
         */
        std::string trim(std::string str);

        /**
         * 将char数据转变成string打印
         */
        std::string char_to_char_string(const char* data, int len);

        std::string char_to_dex_string(const char* data, int len);

        std::string char_to_string(const char* data, int len);

    private:
        util();
        class util_impl;
        std::shared_ptr<util_impl> impl;
    };

}


#endif //COMMON_UTIL_H
