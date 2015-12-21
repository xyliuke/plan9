//
// Created by liuke on 15/12/18.
//

#include "common.h"
#include <boost/filesystem.hpp>
#include <log/log_wrap.h>
#include <commander/cmd_factory.h>
#include <lua/lua_bind.h>

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
        /*
         * 注册日志函数
         * 参数有三个,分别为
         * level : info/warn/error  日志级别,使用三个值其一
         * target : ui/net/io/lua/other 日志分类,使用其一
         * msg : 日志内容
         **/
        cmd_factory::instance().register_cmd("log", [=](Json::Value param){
            if (param.isMember("args")) {
                std::string level = param["args"]["level"].asString();
                std::string target = param["args"]["target"].asString();
                if ("ui" == target) {
                    if (level == "info") {
                        log_wrap::ui().i(param["args"]["msg"].asString());
                    } else if (level == "warn") {
                        log_wrap::ui().w(param["args"]["msg"].asString());
                    } else if (level == "error") {
                        log_wrap::ui().e(param["args"]["msg"].asString());
                    } else {
                        log_wrap::ui().i(param["args"]["msg"].asString());
                    }
                } else if ("io" == target) {
                    if (level == "info") {
                        log_wrap::io().i(param["args"]["msg"].asString());
                    } else if (level == "warn") {
                        log_wrap::io().w(param["args"]["msg"].asString());
                    } else if (level == "error") {
                        log_wrap::io().e(param["args"]["msg"].asString());
                    } else {
                        log_wrap::io().i(param["args"]["msg"].asString());
                    }
                } else if ("net" == target) {
                    if (level == "info") {
                        log_wrap::net().i(param["args"]["msg"].asString());
                    } else if (level == "warn") {
                        log_wrap::net().w(param["args"]["msg"].asString());
                    } else if (level == "error") {
                        log_wrap::net().e(param["args"]["msg"].asString());
                    } else {
                        log_wrap::net().i(param["args"]["msg"].asString());
                    }
                } else if ("lua" == target) {
                    if (level == "info") {
                        log_wrap::lua().i(param["args"]["msg"].asString());
                    } else if (level == "warn") {
                        log_wrap::lua().w(param["args"]["msg"].asString());
                    } else if (level == "error") {
                        log_wrap::lua().e(param["args"]["msg"].asString());
                    } else {
                        log_wrap::lua().i(param["args"]["msg"].asString());
                    }
                } else if ("other" == target) {
                    if (level == "info") {
                        log_wrap::other().i(param["args"]["msg"].asString());
                    } else if (level == "warn") {
                        log_wrap::other().w(param["args"]["msg"].asString());
                    } else if (level == "error") {
                        log_wrap::other().e(param["args"]["msg"].asString());
                    } else {
                        log_wrap::other().i(param["args"]["msg"].asString());
                    }
                } else {
                    if (level == "info") {
                        log_wrap::other().i(param["args"]["msg"].asString());
                    } else if (level == "warn") {
                        log_wrap::other().w(param["args"]["msg"].asString());
                    } else if (level == "error") {
                        log_wrap::other().e(param["args"]["msg"].asString());
                    } else {
                        log_wrap::other().i(param["args"]["msg"].asString());
                    }
                }

            }
            cmd_factory::instance().callback(param);
        });
    }

    void common::init_lua() {
        namespace bfs = boost::filesystem;
        if (!bfs::exists(lua_path)) {
            log_wrap::io().e("init lua error, lua path : ", lua_path, " is not exist");
            return;
        }
        bfs::path p(lua_path);
        p /= "bridge.lua";
        lua_bind::instance().lua_bind_init(p.string());
    }
}