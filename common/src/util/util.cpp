//
// Created by liuke on 15/12/19.
//

#include "util.h"
#include <algorithm>


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
            return ltrim(rtrim(str));
        }

        std::string char_to_string(const char* data, int len) {
            std::stringstream ss;
            ss << "[";
            for (int i = 0; i < len; ++i) {
                unsigned char c = (unsigned char)data[i];
                if (c > 0x20 && c < 0x7F) {
                    ss << data[i];
                    ss << "(";
                    ss << (int)c;
                    ss << ")";
                } else {
                    ss << (int)c;
                }

                if (i != (len - 1)) {
                    ss << " ";
                }
            }
            ss << "]";
            return ss.str();
        }

        std::string char_to_dex_string(const char* data, int len) {
            std::stringstream ss;
            ss << "[";
            for (int i = 0; i < len; ++i) {
                unsigned char c = (unsigned char)data[i];
                ss << (int)c;
                if (i != (len - 1)) {
                    ss << " ";
                }
            }
            ss << "]";
            return ss.str();
        }

        std::string char_to_char_string(const char* data, int len) {
            std::stringstream ss;
            ss << "[";
            for (int i = 0; i < len; ++i) {
                ss << data[i];
                if (i != (len - 1)) {
                    ss << " ";
                }
            }
            ss << "]";
            return ss.str();
        }
    };


    util util::instance() {
        static util u;
        return u;
    }

    util::util() : impl(new util_impl) {

    }


    bool util::is_suffix(std::string str, char end) {
        return impl->isSuffix(str, end);
    }

    std::string util::trim(std::string str) {
        return impl->trim(str);
    }

    std::string util::char_to_string(const char *data, int len) {
        return impl->char_to_string(data, len);
    }

    std::string util::char_to_dex_string(const char *data, int len) {
        return impl->char_to_dex_string(data, len);
    }

    std::string util::char_to_char_string(const char *data, int len) {
        return impl->char_to_char_string(data, len);
    }
}