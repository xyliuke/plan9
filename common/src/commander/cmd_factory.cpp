//
// Created by liuke on 15/11/6.
//

#include <commander/cmd_factory.h>
#include <util/UUID.h>

namespace plan9
{
    class cmd_factory::cmd_factory_impl
    {
    public:

        cmd_factory_impl() {

        }

        void register_cmd(std::string cmd, std::function<void(Json::Value)> function) {
            cmd_map_.insert(std::pair<std::string, std::function<void(Json::Value)>>(cmd, function));
        }

        void call(std::string cmd, Json::Value& json, std::function<void(Json::Value)> callback) {
            Json::Value jsonWrap;
            if (!json.isMember("args")) {
                jsonWrap["args"] = json;
            } else {
                jsonWrap = json;
            }
            std::string id = UUID::id();
            if (!jsonWrap["aux"].isMember("id")) {
                jsonWrap["aux"]["id"] = id;
            }
            jsonWrap["aux"]["to"] = cmd;
            if (callback != nullptr) {
                jsonWrap["aux"]["from"].append(id);
                tmp_cmd_map_[id] = callback;
            }
            execute_function(cmd, jsonWrap);
        }

        void callback(Json::Value& json) {
            using namespace std;
            if (json.isMember("aux")) {
                string action = json["aux"]["action"].asString();
                Json::Value& from = json["aux"]["from"];
                if (from.isArray() && from.size() > 0) {
                    Json::Value fv;
                    from.removeIndex(from.size() - 1, &fv);
                    string f = fv.asString();

                    if (tmp_cmd_map_.find(f) != tmp_cmd_map_.end()) {
                        tmp_cmd_map_[f](json);
                    }
                    tmp_cmd_map_.erase(f);
                }
            }
        }

        void execute_function(std::string cmd, Json::Value json) {
            if (cmd_map_.find(cmd) != cmd_map_.end()) {
                auto func = cmd_map_[cmd];
                func(json);
            }
        }

    private:
        std::map<std::string, std::function<void(Json::Value)>> cmd_map_;
        std::map<std::string, std::function<void(Json::Value)>> tmp_cmd_map_;
    };


    cmd_factory::cmd_factory() : impl_(new cmd_factory_impl) {

    }

    cmd_factory cmd_factory::instance() {
        static cmd_factory cmd;
        return cmd;
    }

    void cmd_factory::register_cmd(std::string cmd, std::function<void(Json::Value)> function) {
        impl_->register_cmd(cmd, function);
    }

    void cmd_factory::execute(std::string cmd, Json::Value param, std::function<void(Json::Value)> callback) {
        impl_->call(cmd, param, callback);
    }

    void cmd_factory::callback(Json::Value json) {
        impl_->callback(json);
    }

    void cmd_factory::execute(std::string cmd, Json::Value param) {
        impl_->call(cmd, param, nullptr);
    }
}