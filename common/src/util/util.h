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
        std::string trim2(std::string str);

        /**
         * 将char数据转变成string打印
         */
        std::string char_to_char_string(const char* data, int len);

        std::string char_to_dex_string(const char* data, int len);

        std::string char_to_string(const char* data, int len);
        /**
         * 复制一个文件夹到另一个目录下，包括以下所有子文件夹和文件
         * @param src_dir 源目录
         * @param dest_dir 目标目录
         * @param copy_file 是否复制文件
         * @return
         */
        bool copy_dir(std::string src_dir, std::string dest_dir, bool copy_file = true);

        void rename(std::string src, std::string dest);
        /**
         * 得到一个目录的容量，剩余空间和可用空间。
         * @param path  路径，如果是文件，则使用文件所在路径
         * @param capacity 输出容量
         * @param free 输出剩余空间
         * @param available 输出可用空间
         * @return
         */
        bool get_dir_info(std::string path, long *capacity, long* free, long* available);

    private:
        util();
        class util_impl;
        std::shared_ptr<util_impl> impl;
    };

}


#endif //COMMON_UTIL_H
