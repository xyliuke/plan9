//
// Created by liuke on 15/12/19.
//

#include "util.h"
#include <algorithm>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS


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
        std::string trim2(std::string str){
            std::string::size_type b_count = 0, e_count = 0;
            for (std::string::size_type i = 0; i < str.length(); ++ i) {
                char c = str.at(i);
                if (std::isspace(c)) {
                    b_count ++ ;
                } else {
                    break;
                }
            }

            for (std::string::size_type i = str.length()- 1; i >= 0; -- i) {
                char c = str.at(i);
                if (std::isspace(c)) {
                    e_count ++ ;
                } else {
                    break;
                }
            }
            if (b_count == 0 && e_count == 0) {
                return str;
            }
            return str.substr(b_count, str.length() - b_count - e_count);
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


        bool copy_dir(std::string src_dir, std::string dest_dir, bool copy_file = true) {
            namespace fs = boost::filesystem;
            if (!fs::exists(dest_dir)) {
                fs::create_directories(dest_dir);
            }

            fs::path dest_path(dest_dir);
            fs::directory_iterator iterator = fs::directory_iterator(src_dir);
            fs::directory_iterator end;
            while (iterator != end) {
                fs::path sub_path = iterator->path().filename();
                fs::path path = dest_path / sub_path;
                fs::path p = fs::complete(*iterator).native();

                if (fs::is_directory(p)) {
                    if (!fs::exists(path)) {
                        fs::create_directories(path);
                    }
                    copy_dir(p.string(), path.string(), copy_file);
                } else {
                    if (copy_file) {
                        fs::copy_file(p, path, fs::copy_option::overwrite_if_exists);
                    }
                }

                iterator ++;
            }
            return true;
        }

        void rename(std::string src, std::string dest) {
            boost::system::error_code code;
            boost::filesystem::rename(src, dest, code);
        }


        bool get_dir_info(std::string path, long *capacity, long* free, long* available) {
            if (boost::filesystem::exists(path)) {
                boost::filesystem::path p;
                if (boost::filesystem::is_directory(path)) {
                    p = boost::filesystem::path(path);
                } else {
                    p = boost::filesystem::path(path).parent_path();
                }
                boost::filesystem::space_info space = boost::filesystem::space(p);
                *capacity = space.capacity;
                *free = space.free;
                *available = space.available;
                return true;
            }
            return false;
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
    std::string util::trim2(std::string str) {
        return impl->trim2(str);
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

    bool util::copy_dir(std::string src_dir, std::string dest_dir, bool copy_file) {
        return impl->copy_dir(src_dir, dest_dir, copy_file);
    }

    void util::rename(std::string src, std::string dest) {
        impl->rename(src, dest);
    }

    bool util::get_dir_info(std::string path, long *capacity, long *free, long *available) {
        return impl->get_dir_info(path, capacity, free, available);
    }
}
