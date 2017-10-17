//
// Created by liuke on 15/12/18.
//

#include "common.h"
#include <boost/filesystem.hpp>
#include <log/log_wrap.h>
#include <commander/cmd_factory.h>
#include <lua/lua_bind.h>
#include <thread/uv_thread_wrap.hpp>
#include <error/error_num.h>
#include <network/async_http.h>
#include <network/async_test.h>
#include "algorithm/compress.h"
#include "http_init.h"
#include "algo_init.h"
#include "init/database_init.h"
#include "init/file_init.h"
#include "tcp_init.h"


namespace plan9
{
#define LUA_FUNCTION_NOT_EXSIT "LUA_FUNCTION_NOT_EXSIT" //lua函数不存在

    static std::string path_ = ".";
    static std::string lua_path_ = ".";
    static std::string tmp_path_ = "";
    static std::string cache_path_ = "";

    std::function<void(std::string)> common::notify_function = nullptr;
    static const char* password = "soapy88Pict";
    static std::string tcp_notify_function = "";

    void common::init(std::string path, std::string lua_path) {
        init(path, lua_path, "");
    }

    void common::init(std::string path, std::string lua_path, std::string tmp_path) {
        init(path, lua_path, tmp_path, nullptr);
    }

    void common::init(std::string path, std::string lua_path, std::string tmp_path, std::function<void(void)> callback) {
        uv_thread_wrap::init([=](){
            uv_thread_wrap::post_serial_queue([=](){
                tmp_path_ = tmp_path;
                path_ = path;
                cache_path_ = path_ + "/cache";
                lua_path_ = lua_path;
                if (!boost::filesystem::exists(path)) {
                    boost::filesystem::create_directories(path);
                }
                if (!boost::filesystem::exists(cache_path_)) {
                    boost::filesystem::create_directories(cache_path_);
                }
                if (tmp_path_ == "") {
                    tmp_path_ = path_ + "/tmp";
                }
                if (!boost::filesystem::exists(tmp_path_)) {
                    boost::filesystem::create_directories(tmp_path_);
                }
                init_log();
                init_lua();
                init_config();
                init_function();
                if (callback) {
                    callback();
                }
            });
        });
    }

    void common::set_notify_function(std::function<void(std::string)> notify) {
        common::notify_function = notify;
        lua_bind::instance().set_notify_function(notify);
    }

    void common::call_(std::string method, JSONObject param, std::function<void(JSONObject)> callback) {
        uv_thread_wrap::post_serial_queue([=]() {
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
                    if (!succ) {
                        //调用失败,则查询失败原因
                        if (error == LUA_FUNCTION_NOT_EXSIT) {
                            log_wrap::io().e(method, " is not register");
                        }
                    }
                    if (callback != nullptr) {
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

    std::string common::get_data_path() {
        return path_;
    }

    std::string common::get_cache_path() {
        return cache_path_;
    }

    std::string common::get_tmp_path() {
        return tmp_path_;
    }

    std::string common::get_tcp_notify_function() {
        return tcp_notify_function;
    }

    void common::init_log() {
        boost::filesystem::path p(path_);
        p /= "log";
        if (!boost::filesystem::exists(p)) {
            boost::filesystem::create_directories(p);
        }
        log_wrap::set_log_dir(p.string());
        log_wrap::io().i("init log");
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


        cmd_factory::instance().register_cmd("common", [=](JSONObject param){
            JSONObject ret;

            JSONObject dir;
            dir["path"] = path_;
            dir["cache"] = cache_path_;
            dir["tmp"] = tmp_path_;
            ret["dir"] = dir;

            cmd_factory::instance().callback(param, true, ret);
        });

    }

//    void common::init_network() {
//        log_wrap::io().i("init network");
//        common::connect_function = [=] (bool connect) {
//            Json::Value tmp;
//            tmp["aux"]["to"] = "network";
//            tmp["result"]["success"] = connect;
//            std::string msg = json_wrap::to_string(tmp);
//            send_notify_msg(msg);
//        };
//
//        common::read_function = [=] (std::string msg) {
//            send_notify_msg(msg);
//        };
//
//        tcp_wrap_default::instance().set_connect_handler(common::connect_function);
//        tcp_wrap_default::instance().set_read_handler(common::read_function);
//    }

    void common::init_function() {
        log_wrap::io().i("register function");

        JSONObject timer_json;
        timer_json.put("interval", "timer interval， unit is millisecond, required");
        timer_json.put("repeat_mode", "true or false, optional, default is false");
        std::string timer_str = timer_json.get_string();
        cmd_factory::instance().register_cmd("create_timer", [timer_str](JSONObject param){
//            if (param.has("args")) {
//                JSONObject args = param["args"];
//                if (args.has("interval")) {
//                    long interval = args["interval"].get_long();
//                    bool repeat = false;
//                    if (args.has("repeat_mode")) {
//                        repeat = args["repeat_mode"].get_bool();
//                    }
//                    std::shared_ptr<int> id(new int);
//                    auto callback = [=]()mutable{
//                        JSONObject ret;
//                        ret.put("timer_id", *id);
//                        ret.put("repeat", repeat);
//                        param["aux.once"] = (!repeat);
//                        cmd_factory::instance().callback(param, true, ret);
//                    };
//                    *id = thread_wrap::post_background(callback, interval, repeat);
//                } else {
//                    cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), " refer to ", timer_str));
//                }
//            } else {
//                cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), " refer to ", timer_str));
//            }
        });

        cmd_factory::instance().register_cmd("cancel_timer", [](JSONObject param){
//            if (param.has("args.timer_id")) {
//                thread_wrap::cancel_background_function(param["args.timer_id"].get_int());
//            } else {
//                cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), " refer to timer_id required"));
//            }
        });

        cmd_factory::instance().register_cmd("test_func", [](JSONObject param){
//            async_uv_http::instance().get(param["args.url"].get_string(), 0, nullptr, [=](int curl_code, std::string debug_trace, long http_state, char* data, size_t len){
//                std::string str(data, len);
//                log_wrap::net().e("test_func output ", curl_code, "\n", debug_trace, "\n", http_state, "\n", str);
//            });
            std::string path = common::get_cache_path() + "/test.html";
            async_test::get(param["args.url"].get_string(), path);
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
        if (!bfs::exists(lua_path_)) {
            log_wrap::io().e("init lua error, lua path : ", lua_path_, " is not exist");
            return;
        }

        bool is_zip = false;

        if (bfs::is_regular_file(lua_path_)) {
            //是压缩包,则需要解压
            bfs::path p = bfs::path(lua_path_).parent_path() / "lua_tmp";
            compress_wrap::decompress_zip(lua_path_, password, p.string());
            lua_path_ = p.string();
            is_zip = true;
        }

        lua_bind::instance().lua_bind_init(lua_path_);
        //为了获取版本，需要在init之后调用
        log_wrap::io().i("init lua, lua version ", lua_bind::instance().version());

        bfs::path p(lua_path_);

        bfs::path bridge = p / "bridge.lua";
        bfs::path common = p / "common.lua";
        bool ret = lua_bind::instance().lua_bind_loadfile(bridge.string());
        log_wrap::lua().d("load lua file ", bridge.string(), " success ? ", ret);
        ret = lua_bind::instance().lua_bind_loadfile(common.string());
        log_wrap::lua().d("load lua file ", common.string(), " success ? ", ret);

        if (is_zip) {
            bfs::remove_all(lua_path_);
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
            log_wrap::io().d("read config : ", result.to_string());
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

                    if (data.has("lib") && data["lib"].is_array()) {
                        JSONObject lib = data["lib"];
                        lib.enumerate([=](std::string key, JSONObject value, bool onlyValue){
                            std::string v = value.get_string();
                            if ("http" == v) {
                                http_init::init();
                            } else if ("algo" == v) {
                                algo_init::init();
                            } else if ("database" == v) {
                                database_init::init();
                            } else if ("file" == v) {
                                file_init::init();
                            } else if ("tcp" == v) {
                                tcp_init::init();
                            }
                        });
                    }

                    int days = 7;
                    if (data.has("log_days")) {
                        days = data["log_days"].get_int();
                    }
                    log_wrap::io().set_duration(days);
                    log_wrap::ui().set_duration(days);
                    log_wrap::net().set_duration(days);
                    log_wrap::lua().set_duration(days);
                    log_wrap::other().set_duration(days);

                    if (data.has("tcp_notify_function")) {
                        tcp_notify_function = data["tcp_notify_function"].get_string();
                    }

                    if (data.has("ssl_verify_peer")) {
                        async_http::set_ssl_verifypeer(data["ssl_verify_peer"].get_bool());
                    }

                    if (data.has("ssl_verify_host")) {
                        async_http::set_ssl_verifyhost(data["ssl_verify_host"].get_bool());
                    }

//                    if (data.has("compress")) {
//                        bool compress = data["compress"].get_bool();
//                        tcp_wrap_default::instance().set_compress(compress);
//                    }
//
//                    if (data.has("encrypt")) {
//                        bool encrypt = data["encrypt"].get_bool();
//                        tcp_wrap_default::instance().set_encrypt(encrypt);
//                    }

                }
            }
        });
    }
}