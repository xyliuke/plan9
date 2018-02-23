//
// Created by liuke on 15/12/14.
//

#include "log_wrap.h"
#include <boost/date_time.hpp>
#include <util/time.h>
#include <boost/filesystem.hpp>
#include <thread>

namespace plan9
{
    std::string log_wrap::path = "./";

    class log_wrap::log_wrap_impl {
    public:

        log_wrap_impl(std::string path, std::string prefix_file) : count(1), day(1), level(log_level::L_DEBUG) {
            this->path = path;
            this->prefix_file = prefix_file;
            log.reset(new plan9::log);
            log->setFile(get_file(path, prefix_file));
            day = time::day();
            set_duration(10000);
        }

        void write(std::string msg) {
            if (count % 10 == 0) {
                int nd = time::day();
                if (day != nd) {
                    day = nd;
                    log.reset(new plan9::log);
                    log->setFile(get_file(path, prefix_file));
                }
            }
            count ++;
            log->write(msg);
        }

        void write(log_level level, std::string msg) {
            if (level < this->level) {
                return;
            }

            std::stringstream ss;

            ss << time::current_data_time();

            ss << " [thread:";
            ss << std::this_thread::get_id();
            ss << "]";

            if (level == log_level::L_INFO) {
                ss << " [INFO] : ";
            } else if (level == log_level::L_WARN) {
                ss << " [WARN] : ";
            } else if (level == log_level::L_ERROR) {
                ss << " [ERROR]: ";
            } else if (level == log_level::L_DEBUG) {
                ss << " [DEBUG]: ";
            }

            ss << msg;
//            if (!util::instance().is_suffix(msg, '\n')) {
                ss << "\n";
//            }

            write(ss.str());
        }

        std::string get_file(std::string path, std::string prefix_file) {
            std::stringstream ss;
            ss << path;
            ss << "/";
            ss << time::current_data();
            ss << "-";
            ss << prefix_file;
            ss << ".log";
            return ss.str();
        }

        void set_log_level(log_level lv) {
            level = lv;
        }

        void set_duration(int days) {
            duration = days;
            delete_log_day_before(days);
        }

        void delete_log_day_before(int days) {
            namespace fs = boost::filesystem;
            boost::filesystem::path p(this->path);
            if (fs::exists(p) && fs::is_directory(p)) {
                fs::directory_iterator begin(p);
                fs::directory_iterator end;
                for ( ; begin != end; begin ++ ) {
                    fs::path item = *begin;
                    if (fs::is_regular_file(item)) {
                        std::string file = item.filename().string();
                        if (file.substr(0, prefix_file.size()) == prefix_file) {
                            std::string file_date = file.substr(prefix_file.size() + 1, file.size() - prefix_file.size() - 5);
                            if(time::duration_days(file_date) >= duration) {
                                //删除
                                fs::remove(item);
                            }
                        }
                    }
                }
            }
        }

    private:
        std::shared_ptr<plan9::log> log;
        std::string prefix_file;
        std::string path;
        int day;
        unsigned long long count;
        log_level level;
        int duration;
    };


    log_wrap log_wrap::io() {
        static log_wrap wrap(log_wrap::path, "io");
        return wrap;
    }

    log_wrap log_wrap::net() {
        static log_wrap wrap(log_wrap::path, "net");
        return wrap;
    }

    log_wrap log_wrap::ui() {
        static log_wrap wrap(log_wrap::path, "ui");
        return wrap;
    }

    log_wrap log_wrap::lua() {
        static log_wrap wrap(log_wrap::path, "lua");
        return wrap;
    }

    log_wrap log_wrap::other() {
        static log_wrap wrap(log_wrap::path, "other");
        return wrap;
    }

    void log_wrap::set_log_dir(std::string path) {
        log_wrap::path = path;
    }



    log_wrap::log_wrap(std::string path, std::string prefix_file) : impl(new log_wrap_impl(path, prefix_file)) {

    }


    void log_wrap::d_(std::string msg) {
//        thread_wrap::post_serial_background([=](){
            impl->write(log_level::L_DEBUG, msg);
//        });
    }

    void log_wrap::i_(std::string msg) {
//        thread_wrap::post_serial_background([=]() {
            impl->write(log_level::L_INFO, msg);
//        });
    }

    void log_wrap::w_(std::string msg) {
//        thread_wrap::post_serial_background([=]() {
            impl->write(log_level::L_WARN, msg);
//        });
    }

    void log_wrap::e_(std::string msg) {
//        thread_wrap::post_serial_background([=]() {
            impl->write(log_level::L_ERROR, msg);
//        });
    }

    void log_wrap::set_level(log_level level) {
        impl->set_log_level(level);
    }

    void log_wrap::set_duration(int days) {
        impl->set_duration(days);
    }


    void log_wrap::set_all_level(log_wrap::log_level level) {
        log_wrap::io().set_level(level);
        log_wrap::net().set_level(level);
        log_wrap::ui().set_level(level);
        log_wrap::other().set_level(level);
        log_wrap::lua().set_level(level);
    }
}