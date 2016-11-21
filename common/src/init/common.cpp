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
#include "algorithm/compress.h"
#include "http_init.h"
#include "algo_init.h"

namespace plan9
{
#define LUA_FUNCTION_NOT_EXSIT "LUA_FUNCTION_NOT_EXSIT" //lua函数不存在

    std::string common::path = ".";
    std::string common::lua_path = ".";
    std::function<void(std::string)> common::notify_function = nullptr;
    std::function<void(std::string)> common::read_function = nullptr;
    std::function<void(bool)> common::connect_function = nullptr;
    std::function<void(std::string)> common::write_function = nullptr;

    static const char* password = "soapy88Pict";

    void common::init(std::string path, std::string lua_path) {
        common::path = path;
        common::lua_path = lua_path;
        if (!boost::filesystem::exists(path)) {
            boost::filesystem::create_directories(path);
        }
        init_log();
        init_lua();
        init_function();
//        init_network();
        init_config();



        //加载其他库
        http_init::init();
        algo_init::init();
    }

    void common::set_notify_function(std::function<void(std::string)> notify) {
        common::notify_function = notify;
    }

    void common::call_(std::string method, JSONObject param, std::function<void(JSONObject)> callback) {
        thread_wrap::post_background([=](){
            if ("log" == method) {
                cmd_factory::instance().execute(method, param);
                return;
            }
            if (cmd_factory::instance().is_register(method)) {
                cmd_factory::instance().execute(method, param, callback);
            } else {
                lua_bind::instance().call(method, param, [=](JSONObject result) mutable {
                    bool succ;
                    std::string error = success(result, &succ);
                    if (succ) {
                        if (callback != nullptr) {
                            callback(result);
                        }
                    } else {
                        //调用失败,则查询失败原因
                        if (error == LUA_FUNCTION_NOT_EXSIT) {
                            log_wrap::io().e(method, " is not register");
                        }
                        callback(result);
                    }
                });
            }
        });
    }

    void common::call_(std::string method, std::string param, std::function<void(JSONObject result)> callback) {
        if (param == "") {
            call_(method, callback);
            return;
        }

        JSONObject p = JSONObject(param);
        call_(method, p, callback);
    }

    void common::call(std::string method, std::string param, std::function<void(std::string result)> callback) {
        if (callback != nullptr) {
            if ("" == param) {
                call_(method, [=](JSONObject result){
                    callback(result.to_string());
                });
            } else {
                call_(method, param, [=](JSONObject result){
                    callback(result.to_string());
                });
            }
        } else {
            call(method, param);
        }
    }

    void common::call(std::string method, JSONObject param) {
        call_(method, param, nullptr);
    }

    void common::call(std::string method, std::string param) {
        if (param == "") {
            call(method);
        } else {
            call_(method, param, nullptr);
        }
    }

    void common::call_(std::string method, std::function<void(JSONObject result)> callback) {
        call_(method, JSONObject(), callback);
    }

    void common::call(std::string method) {
        call(method, JSONObject());
    }

    std::string common::success(JSONObject result, bool* success) {
        if (result.has("result") && result["result"].has("success") && result["result"]["success"].is_bool()) {
            *success = result["result"]["success"].get_bool();
            if (*success) {
                return "";
            } else {
                if (result["result"].has("error")) {
                    return result["result"]["error"].get_string();
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
        JSONObject p;
        p["platform"] = "ios";
        call("native.set_platform", p);
    }

    void common::set_android_platform() {
        JSONObject p;
        p["platform"] = "android";
        call("native.set_platform", p);
    }

    void common::set_win_platform() {
        JSONObject p;
        p["platform"] = "win";
        call("native.set_platform", p);
    }

    void common::set_macosx_platform() {
        JSONObject p;
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
         * level : debug/info/warn/error  日志级别,使用四个值其一，默认为debug
         * target : ui/net/io/lua/other 日志分类,使用其一
         * msg : 日志内容
         **/
        cmd_factory::instance().register_cmd("log", [=](JSONObject param){
            if (param.has("args")) {
                std::string level = param["args"]["level"].get_string();
                std::string target = param["args"]["target"].get_string();
                std::string msg = param["args"]["msg"].to_string();
                if ("ui" == target) {
                    if (level == "info") {
                        log_wrap::ui().i(msg);
                    } else if (level == "warn") {
                        log_wrap::ui().w(msg);
                    } else if (level == "error") {
                        log_wrap::ui().e(msg);
                    } else {
                        log_wrap::ui().d(msg);
                    }
                } else if ("io" == target) {
                    if (level == "info") {
                        log_wrap::io().i(msg);
                    } else if (level == "warn") {
                        log_wrap::io().w(msg);
                    } else if (level == "error") {
                        log_wrap::io().e(msg);
                    } else {
                        log_wrap::io().d(msg);
                    }
                } else if ("net" == target) {
                    if (level == "info") {
                        log_wrap::net().i(msg);
                    } else if (level == "warn") {
                        log_wrap::net().w(msg);
                    } else if (level == "error") {
                        log_wrap::net().e(msg);
                    } else {
                        log_wrap::net().d(msg);
                    }
                } else if ("lua" == target) {
                    if (level == "info") {
                        log_wrap::lua().i(msg);
                    } else if (level == "warn") {
                        log_wrap::lua().w(msg);
                    } else if (level == "error") {
                        log_wrap::lua().e(msg);
                    } else {
                        log_wrap::lua().d(msg);
                    }
                } else if ("other" == target) {
                    if (level == "info") {
                        log_wrap::other().i(msg);
                    } else if (level == "warn") {
                        log_wrap::other().w(msg);
                    } else if (level == "error") {
                        log_wrap::other().e(msg);
                    } else {
                        log_wrap::other().d(msg);
                    }
                } else {
                    if (level == "info") {
                        log_wrap::other().i(msg);
                    } else if (level == "warn") {
                        log_wrap::other().w(msg);
                    } else if (level == "error") {
                        log_wrap::other().e(msg);
                    } else {
                        log_wrap::other().d(msg);
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
//        cmd_factory::instance().register_cmd("connect", [=](Json::Value param){
//            if (param.isMember("args")) {
//                Json::Value args = param["args"];
//                if (args.isMember("ip") && args.isMember("port")) {
//                    std::string ip = args["ip"].asString();
//                    int port = args["port"].asInt();
//                    tcp_wrap_default::instance().connect(ip, port);
//                }
//            }
//        });

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
//        cmd_factory::instance().register_cmd("send", [=](Json::Value param){
//            log_wrap::net().i("call send : ", json_wrap::to_string(param));
//            if (param.isMember("args")) {
//                Json::Value args = param["args"];
//                char type = 0;
//                if (args.isMember("server")) {
//                    type = (char)args["server"].asInt();
//                }
//                int cid = 0;
//                if (args.isMember("client_id")) {
//                    cid = args["client_id"].asInt();
//                }
//                int timeout = 0;
//                if (args.isMember("timeout")) {
//                    timeout = args["timeout"].asInt();
//                }
//                param["aux"]["to"] = param["args"]["to"];
//                param["args"].removeMember("to");
//
//                tcp_wrap_default::instance().send(cid, type, param, [=](Json::Value data){
//                    cmd_factory::instance().callback(data);
//                }, timeout);
//            }
//        });
    }

    void common::init_lua() {
        namespace bfs = boost::filesystem;
        if (!bfs::exists(lua_path)) {
            log_wrap::io().e("init lua error, lua path : ", lua_path, " is not exist");
            return;
        }

        bool is_zip = false;

        if (bfs::is_regular_file(lua_path)) {
            //是压缩包,则需要解压
            bfs::path p = bfs::path(lua_path).parent_path() / "lua_tmp";
            compress_wrap::decompress_zip(lua_path, password, p.string());
            lua_path = p.string();
            is_zip = true;
        }

        lua_bind::instance().lua_bind_init(lua_path);
        //为了获取版本，需要在init之后调用
        log_wrap::io().i("init lua, lua version ", lua_bind::instance().version());

        bfs::path p(lua_path);

        bfs::path bridge = p / "bridge.lua";
        bfs::path common = p / "common.lua";
        lua_bind::instance().lua_bind_loadfile(bridge.string());
        lua_bind::instance().lua_bind_loadfile(common.string());

        if (is_zip) {
            bfs::remove_all(lua_path);
        }
    }


    void common::send_notify_msg(std::string msg) {
        if (common::notify_function != nullptr) {
            log_wrap::io().i("post message : ", msg);
            common::notify_function(msg);
        }
    }


    void common::init_config() {
        log_wrap::io().i("init config");
        call_("config.get_config", JSONObject(), [=](JSONObject result){
            bool succ;
            success(result, &succ);
            if (succ) {
                JSONObject result_json = result["result"];
                if (result_json.has("data")) {
                    JSONObject data = result_json["data"];

                    if (data.has("log_level")) {
                        std::string lel = data["log_level"].get_string();
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

                    if (data.has("compress")) {
                        bool compress = data["compress"].get_bool();
                        tcp_wrap_default::instance().set_compress(compress);
                    }

                    if (data.has("encrypt")) {
                        bool encrypt = data["encrypt"].get_bool();
                        tcp_wrap_default::instance().set_encrypt(encrypt);
                    }

                }
            }
            log_wrap::io().d("read config : ", result.to_string());
        });
    }
}