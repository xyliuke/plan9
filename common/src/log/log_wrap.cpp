//
// Created by liuke on 15/12/14.
//

#include "log_wrap.h"
#include <boost/date_time.hpp>
#include <util/time.h>


namespace plan9
{
    std::string log_wrap::path = ".";


    class log_wrap::log_wrap_impl {
    public:

        log_wrap_impl(std::string path, std::string prefix_file) : count(1), day(1), level(INFO) {
            this->path = path;
            this->prefix_file = prefix_file;
            log.reset(new plan9::log);
            log->setFile(get_file(path, prefix_file));
            day = time::day();
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
            if (level == INFO) {
                ss << " [INFO] : ";
            } else if (level == WARN) {
                ss << " [WARN] : ";
            } else if (level == ERROR) {
                ss << " [ERROR] : ";
            }

            ss << msg;
            ss << "\n";

            write(ss.str());
        }

        std::string get_file(std::string path, std::string prefix_file) {
            std::stringstream ss;
            ss << path;
            ss << "/";
            ss << prefix_file;
            ss << "-";
            ss << time::current_data();
            ss << ".log";
            return ss.str();
        }

        void set_log_level(log_level lv) {
            level = lv;
        }

    private:
        std::shared_ptr<plan9::log> log;
        std::string prefix_file;
        std::string path;
        int day;
        unsigned long long count;
        log_level level;
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

    log_wrap log_wrap::other() {
        static log_wrap wrap(log_wrap::path, "other");
        return wrap;
    }

    void log_wrap::set_log_dir(std::string path) {
        log_wrap::path = path;
    }



    log_wrap::log_wrap(std::string path, std::string prefix_file) : impl(new log_wrap_impl(path, prefix_file)) {

    }

    void log_wrap::i(std::string msg) {
        impl->write(INFO, msg);
    }

    void log_wrap::w(std::string msg) {
        impl->write(WARN, msg);
    }

    void log_wrap::e(std::string msg) {
        impl->write(ERROR, msg);
    }

    void log_wrap::set_level(log_level level) {
        impl->set_log_level(level);
    }
}