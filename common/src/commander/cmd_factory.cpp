//
// Created by liuke on 15/11/6.
//

#include <commander/cmd_factory.h>
#include <util/UUID.h>
#include <iostream>
//#include <zconf.h>
#include "log/log_wrap.h"

namespace plan9
{
    class cmd_factory::cmd_factory_impl
    {
    public:

        cmd_factory_impl() {

        }

        void register_cmd(std::string cmd, std::function<void(JSONObject)> function) {
            cmd_map_[cmd] = function;
        }

        void call(std::string cmd, JSONObject json, std::function<void(JSONObject)> callback) {
            JSONObject jsonWrap;
            if (!json.has("args") || !json.has("aux")) {
                jsonWrap["args"] = json;
            } else {
                jsonWrap = json;
            }
            std::string id = UUID::id();
            if (!jsonWrap["aux"].has("id")) {
                jsonWrap["aux"]["id"] = id;
            }
            jsonWrap["aux"]["to"] = cmd;
            if (callback != nullptr) {
                jsonWrap["aux"]["action"] = "callback";
                if (jsonWrap["aux"].has("from") && jsonWrap["aux"]["from"].is_array()) {
                    jsonWrap["aux"]["from"].append(id);
                    tmp_cmd_map_[id] = callback;
                } else {
                    jsonWrap["aux"]["from"].append(jsonWrap["aux"]["id"]);
                    tmp_cmd_map_[jsonWrap["aux"]["id"].get_string()] = callback;
                }


                if (cmd_map_.find(cmd) == cmd_map_.end()) {
                    //没有找到注册函数
                    jsonWrap["result.success"] = false;
                    jsonWrap["result.error"] = "CPP_FUNCTION_NOT_EXIST";
                    jsonWrap["result.reason"] = "the function : " + cmd + " is not exist";
                    callback(jsonWrap);
                    return;
                }
            } else {
                jsonWrap["aux"]["action"] = "direct";
            }
            execute_function(cmd, jsonWrap);
        }

        void call(JSONObject param, std::function<void(JSONObject)> callback) {
            if (param.has("aux") && param["aux"].has("to")) {
                std::string method = param["aux"]["to"].get_string();
                call(method, param, callback);
            }
        }

        void callback(JSONObject json) {
            using namespace std;
            if (json.has("aux")) {
                string action = json["aux"]["action"].get_string();
                if ("callback" == action) {
                    JSONObject from = json["aux.from"];
                    if (from.is_array() && from.size() > 0) {
                        JSONObject fv = from.remove_last();
                        string f = fv.get_string();

                        if (tmp_cmd_map_.find(f) != tmp_cmd_map_.end()) {
                            tmp_cmd_map_[f](json);
                        }
                        tmp_cmd_map_.erase(f);
                    }
                }
            }
        }

        void execute_function(std::string cmd, JSONObject json) {
            if (cmd_map_.find(cmd) != cmd_map_.end()) {
                auto func = cmd_map_[cmd];
                func(json);
            } else {
                log_wrap::io().e("the c++ function : ", cmd , " is not exist");
            }
        }

        JSONObject wrap_callback(JSONObject json, bool result, JSONObject data) {
            json["result"]["success"] = result;
            json["result"]["data"] = data;
            return json;
        }

    private:
        std::map<std::string, std::function<void(JSONObject)>> cmd_map_;
        std::map<std::string, std::function<void(JSONObject)>> tmp_cmd_map_;
    };


    cmd_factory::cmd_factory() : impl_(new cmd_factory_impl) {

    }

    cmd_factory cmd_factory::instance() {
        static cmd_factory cmd;
        return cmd;
    }

    void cmd_factory::register_cmd(std::string cmd, std::function<void(Json::Value)> function) {
//        log_wrap::io().i("register function , method : ", cmd);
//        impl_->register_cmd(cmd, function);
    }

    void cmd_factory::register_cmd(std::string cmd, std::function<void(JSONObject)> function) {
        log_wrap::io().i("register function , method : ", cmd);
        impl_->register_cmd(cmd, function);
    }

    void cmd_factory::execute(std::string cmd, Json::Value param, std::function<void(Json::Value)> callback) {
//        impl_->call(cmd, param, callback);
    }

    void cmd_factory::execute(std::string cmd, JSONObject param, std::function<void(JSONObject)> callback) {
        impl_->call(cmd, param, callback);
    }

    void cmd_factory::execute(Json::Value param, std::function<void(Json::Value)> callback) {
//        impl_->call(param, callback);
    }

    void cmd_factory::execute(JSONObject param, std::function<void(JSONObject)> callback) {
        impl_->call(param, callback);
    }

    void cmd_factory::callback(Json::Value json) {
//        impl_->callback(json);
    }

    void cmd_factory::callback(JSONObject json) {
        impl_->callback(json);
    }

    void cmd_factory::callback(Json::Value param, bool result, Json::Value data) {
//        Json::Value ret = impl_->wrap_callback(param, result, data);
//        callback(ret);
    }

    void cmd_factory::callback(JSONObject param, bool result, JSONObject data) {
        JSONObject ret = impl_->wrap_callback(param, result, data);
        callback(ret);
    }

    void cmd_factory::callback(Json::Value param, bool result) {
//        callback(param, result, Json::Value());
    }

    void cmd_factory::callback(JSONObject param, bool result) {
        callback(param, result, JSONObject());
    }

    void cmd_factory::execute(std::string cmd, Json::Value param) {
//        impl_->call(cmd, param, nullptr);
    }

    void cmd_factory::execute(std::string cmd, JSONObject param) {
        impl_->call(cmd, param, nullptr);
    }

    void cmd_factory::execute(Json::Value param) {
//        impl_->call(param, nullptr);
    }

    void cmd_factory::execute(JSONObject param) {
        impl_->call(param, nullptr);
    }

//    Json::Value cmd_factory::wrap_callback_data(Json::Value json, bool result, Json::Value data) {
//        return impl_->wrap_callback(json, result, data);
//    }
}