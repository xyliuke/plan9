//
// Created by liuke on 15/12/18.
//

#include "common.h"
#include <boost/filesystem.hpp>
#include <log/log_wrap.h>
#include <commander/cmd_factory.h>
#include <lua/lua_bind.h>
#include <json/json_wrap.h>
#include <network/tcp_wrap_default.h>
#include <thread/timer.h>
#include <thread/thread_wrap.h>

namespace plan9
{
#define LUA_FUNCTION_NOT_EXSIT "LUA_FUNCTION_NOT_EXSIT" //lua函数不存在

    std::string common::path = ".";
    std::string common::lua_path = ".";
    std::function<void(std::string)> common::notify_function = nullptr;
    std::function<void(std::string)> common::read_function = nullptr;
    std::function<void(bool)> common::connect_function = nullptr;
    std::function<void(std::string)> common::write_function = nullptr;

    void common::init(std::string path, std::string lua_path) {
        common::path = path;
        common::lua_path = lua_path;
        if (!boost::filesystem::exists(path)) {
            boost::filesystem::create_directories(path);
        }
        init_log();
        init_lua();
        init_function();
        init_network();
        init_config();
    }

    void common::set_notify_function(std::function<void(std::string)> notify) {
        common::notify_function = notify;
    }

    void common::call_(std::string method, Json::Value param, std::function<void(Json::Value result)> callback) {
        thread_wrap::post_background([=](){
            if ("log" == method) {
                cmd_factory::instance().execute(method, param);
                return;
            }
            lua_bind::instance().call(method, param, [=](Json::Value result){
                bool succ;
                std::string error = success(result, &succ);
                if (succ) {
                    if (callback != nullptr) {
                        callback(result);
                    }
                } else {
                    //调用失败,则查询失败原因
                    if (error == LUA_FUNCTION_NOT_EXSIT) {
                        if (callback != nullptr) {
                            log_wrap::io().i("execute function , method : ", method, ", param : ",
                                             json_wrap::to_string(param), ", callback : yes");
                            cmd_factory::instance().execute(method, param, callback);
                        } else {
                            log_wrap::io().i("execute function , method : ", method, ", param : ",
                                             json_wrap::to_string(param), ", callback : no");
                            cmd_factory::instance().execute(method, param);
                        }
                    } else {
                        callback(result);
                    }
                }
            });
        });
    }

    void common::call_(std::string method, std::string param, std::function<void(Json::Value result)> callback) {
        if (param == "") {
            call_(method, callback);
            return;
        }

        bool error = false;
        Json::Value p = json_wrap::parse(param, &error);
        if (error) {
            log_wrap::io().e("the method : ", method, " param : ", param, " is not json format");
        } else {
            call_(method, p, callback);
        }
    }

    void common::call(std::string method, std::string param, std::function<void(std::string result)> callback) {
        if (callback != nullptr) {
            if ("" == param) {
                call_(method, [=](Json::Value result){
                    callback(json_wrap::to_string(result));
                });
            } else {
                call_(method, param, [=](Json::Value result){
                    callback(json_wrap::to_string(result));
                });
            }
        } else {
            call(method, param);
        }
    }

    void common::call(std::string method, Json::Value param) {
        call_(method, param, nullptr);
    }

    void common::call(std::string method, std::string param) {
        if (param == "") {
            call(method);
        } else {
            call_(method, param, nullptr);
        }
    }

    void common::call_(std::string method, std::function<void(Json::Value result)> callback) {
        call_(method, Json::Value(), callback);
    }

    void common::call(std::string method) {
        call(method, Json::Value());
    }

    std::string common::success(Json::Value result, bool* success) {
        if (result.isMember("result") && result["result"].isMember("success") && result["result"]["success"].isBool()) {
            *success = result["result"]["success"].asBool();
            if (*success) {
                return "";
            } else {
                if (result["result"].isMember("error")) {
                    return result["result"]["error"].asString();
                } else {
                    return "";
                }
            }
        }
        *success = false;
        return "";
    }


    void common::stop() {

    }

    void common::set_ios_platform() {
        Json::Value p;
        p["platform"] = "ios";
        call("native.set_platform", p);
    }

    void common::set_android_platform() {
        Json::Value p;
        p["platform"] = "android";
        call("native.set_platform", p);
    }

    void common::set_win_platform() {
        Json::Value p;
        p["platform"] = "win";
        call("native.set_platform", p);
    }

    void common::set_macosx_platform() {
        Json::Value p;
        p["platform"] = "macosx";
        call("native.set_platform", p);
    }

    void common::init_log() {
        boost::filesystem::path p(path);
        p /= "log";
        if (!boost::filesystem::exists(p)) {
            boost::filesystem::create_directories(p);
        }
        log_wrap::set_log_dir(p.string());
        log_wrap::io().i("init log");
        log_wrap::io().set_duration(7);
        log_wrap::ui().set_duration(7);
        log_wrap::net().set_duration(7);
        log_wrap::lua().set_duration(7);
        log_wrap::other().set_duration(7);
    }

    void common::init_network() {
        log_wrap::io().i("init network");
        common::connect_function = [=] (bool connect) {
            Json::Value tmp;
            tmp["aux"]["to"] = "network";
            tmp["result"]["success"] = connect;
            std::string msg = json_wrap::to_string(tmp);
            send_notify_msg(msg);
        };

        common::read_function = [=] (std::string msg) {
            send_notify_msg(msg);
        };

        tcp_wrap_default::instance().set_connect_handler(common::connect_function);
        tcp_wrap_default::instance().set_read_handler(common::read_function);
    }

    void common::init_function() {
        log_wrap::io().i("register function");
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
                std::string msg = json_wrap::to_string(param["args"]["msg"]);
                if ("ui" == target) {
                    if (level == "info") {
                        log_wrap::ui().i(msg);
                    } else if (level == "warn") {
                        log_wrap::ui().w(msg);
                    } else if (level == "error") {
                        log_wrap::ui().e(msg);
                    } else {
                        log_wrap::ui().i(msg);
                    }
                } else if ("io" == target) {
                    if (level == "info") {
                        log_wrap::io().i(msg);
                    } else if (level == "warn") {
                        log_wrap::io().w(msg);
                    } else if (level == "error") {
                        log_wrap::io().e(msg);
                    } else {
                        log_wrap::io().i(msg);
                    }
                } else if ("net" == target) {
                    if (level == "info") {
                        log_wrap::net().i(msg);
                    } else if (level == "warn") {
                        log_wrap::net().w(msg);
                    } else if (level == "error") {
                        log_wrap::net().e(msg);
                    } else {
                        log_wrap::net().i(msg);
                    }
                } else if ("lua" == target) {
                    if (level == "info") {
                        log_wrap::lua().i(msg);
                    } else if (level == "warn") {
                        log_wrap::lua().w(msg);
                    } else if (level == "error") {
                        log_wrap::lua().e(msg);
                    } else {
                        log_wrap::lua().i(msg);
                    }
                } else if ("other" == target) {
                    if (level == "info") {
                        log_wrap::other().i(msg);
                    } else if (level == "warn") {
                        log_wrap::other().w(msg);
                    } else if (level == "error") {
                        log_wrap::other().e(msg);
                    } else {
                        log_wrap::other().i(msg);
                    }
                } else {
                    if (level == "info") {
                        log_wrap::other().i(msg);
                    } else if (level == "warn") {
                        log_wrap::other().w(msg);
                    } else if (level == "error") {
                        log_wrap::other().e(msg);
                    } else {
                        log_wrap::other().i(msg);
                    }
                }
            }
            cmd_factory::instance().callback(param, true);
        });


        /**
         * 连接服务器
         * 传入参数为:
         * ip   : 服务器的ip
         * port : 服务器的端口
         *
         * 返回结果为:
         * 成功返回true,失败返回false和原因
         */
        cmd_factory::instance().register_cmd("connect", [=](Json::Value param){
            if (param.isMember("args")) {
                Json::Value args = param["args"];
                if (args.isMember("ip") && args.isMember("port")) {
                    std::string ip = args["ip"].asString();
                    int port = args["port"].asInt();
                    tcp_wrap_default::instance().connect(ip, port);
                }
            }
        });

        /**
         * 向服务器发送数据,用于测试
         * 传入参数:
         * msg  : 发送的字符串
         * server : 发送服务器类型
         * 返回结果为:
         * data {
         *      msg : 返回的字符串
         * }
         */
        cmd_factory::instance().register_cmd("send", [=](Json::Value param){
            log_wrap::net().i("call send : ", json_wrap::to_string(param));
            if (param.isMember("args")) {
                Json::Value args = param["args"];
                char type = 0;
                if (args.isMember("server")) {
                    type = (char)args["server"].asInt();
                }
                int cid = 0;
                if (args.isMember("client_id")) {
                    cid = args["client_id"].asInt();
                }
                int timeout = 0;
                if (args.isMember("timeout")) {
                    timeout = args["timeout"].asInt();
                }
                param["aux"]["to"] = param["args"]["to"];
                param["args"].removeMember("to");

                tcp_wrap_default::instance().send(cid, type, param, [=](Json::Value data){
                    cmd_factory::instance().callback(data);
                }, timeout);
            }
        });
    }

    void common::init_lua() {
        log_wrap::io().i("init lua");
        namespace bfs = boost::filesystem;
        if (!bfs::exists(lua_path)) {
            log_wrap::io().e("init lua error, lua path : ", lua_path, " is not exist");
            return;
        }
        lua_bind::instance().lua_bind_init(lua_path);

        bfs::path p(lua_path);

        bfs::path bridge = p / "bridge.lua";
        bfs::path common = p / "common.lua";
        lua_bind::instance().lua_bind_loadfile(bridge.string());
        lua_bind::instance().lua_bind_loadfile(common.string());
    }

    void common::send_notify_msg(std::string msg) {
        if (common::notify_function != nullptr) {
            log_wrap::io().i("post message : ", msg);
            common::notify_function(msg);
        }
    }


    void common::init_config() {
        log_wrap::io().i("init config");
        call_("config.get_config", Json::Value(), [=](Json::Value result){
            bool succ;
            success(result, &succ);
            if (succ) {
                Json::Value result_json = result["result"];
                if (result_json.isMember("data")) {
                    Json::Value data = result_json["data"];

                    if (data.isMember("log_level")) {
                        std::string lel = data["log_level"].asString();
                        if ("debug" == lel) {
                            log_wrap::set_all_level(log_wrap::log_level::L_DEBUG);
                        } else if ("info" == lel) {
                            log_wrap::set_all_level(log_wrap::log_level::L_INFO);
                        } else if ("warn" == lel) {
                            log_wrap::set_all_level(log_wrap::log_level::L_WARN);
                        } else if ("error" == lel) {
                            log_wrap::set_all_level(log_wrap::log_level::L_ERROR);
                        }
                    }

                    if (data.isMember("compress")) {
                        bool compress = data["compress"].asBool();
                        tcp_wrap_default::instance().set_compress(compress);
                    }

                    if (data.isMember("encrypt")) {
                        bool encrypt = data["encrypt"].asBool();
                        tcp_wrap_default::instance().set_encrypt(encrypt);
                    }

                }
            }
            log_wrap::io().d("read config : ", json_wrap::to_string(result));
        });
    }
}