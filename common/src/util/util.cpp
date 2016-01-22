//
// Created by liuke on 15/12/19.
//

#include "util.h"


namespace plan9
{
    class util::util_impl {
    public:
        bool isSuffix(std::string str, char end) {
            int pos = str.rfind(end);
            return str.length() - 1 == pos;
        }

        bool isPrefix(std::string str, char begin) {
            int pos = str.find(begin);
            return 0 == pos;
        }

        std::string &ltrim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
            return s;
        }

        std::string &rtrim(std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
            return s;
        }

        std::string trim(std::string str){
//            return str.erase(str.find_last_not_of(" \n\r\t") + 1);
            return ltrim(rtrim(str));
        }
    };


    util util::instance() {
        static util u;
        return u;
    }

    util::util() : impl(new util_impl) {

    }


    bool util::isSuffix(std::string str, char end) {
        return impl->isSuffix(str, end);
    }

    std::string util::trim(std::string str) {
        return impl->trim(str);
    }
}