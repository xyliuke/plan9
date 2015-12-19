//
// Created by liuke on 15/12/18.
//

#include "common.h"
#include <boost/filesystem.hpp>
#include <log/log_wrap.h>
#include <commander/cmd_factory.h>

namespace plan9
{
    std::string common::path = ".";
    std::string common::lua_path = ".";

    void common::init(std::string path, std::string lua_path) {
        common::path = path;
        common::lua_path = lua_path;
        if (!boost::filesystem::exists(path)) {
            boost::filesystem::create_directories(path);
        }
        init_log();
        init_lua();
        init_function();
    }

    void common::init_log() {
        boost::filesystem::path p(path);
        p /= "log";
        if (!boost::filesystem::exists(p)) {
            boost::filesystem::create_directories(p);
        }
        log_wrap::set_log_dir(p.string());
        log_wrap::io().set_duration(7);
        log_wrap::ui().set_duration(7);
        log_wrap::net().set_duration(7);
        log_wrap::lua().set_duration(7);
        log_wrap::other().set_duration(7);
    }

    void common::init_function() {
        cmd_factory::instance().register_cmd("log", [=](Json::Value param){
            if (param.isMember("args")) {
                std::string level = param["args"]["level"].asString();
                if (level == "info") {
                    log_wrap::ui().i(param["args"]["msg"].asString());
                } else if (level == "warn") {
                    log_wrap::ui().w(param["args"]["msg"].asString());
                } else if (level == "error") {
                    log_wrap::ui().e(param["args"]["msg"].asString());
                } else {
                    log_wrap::ui().i(param["args"]["msg"].asString());
                }
            }
        });
    }

    void common::init_lua() {
        namespace bfs = boost::filesystem;
        if (bfs::exists(lua_path)) {
            log_wrap::io().e("init lua error, lua path : ", lua_path, " is not exist");
        }
    }
}