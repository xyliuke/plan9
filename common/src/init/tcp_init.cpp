//
// Created by liuke on 29/03/2017.
//

#include <commander/cmd_factory.h>
#include <error/error_num.h>
#include <util/util.h>
#include "tcp_init.h"
//#include "network/tcp.h"
#include "common.h"

namespace plan9
{
//    static std::shared_ptr<tcp> tcp_;
    void tcp_init::init() {
        /**
         * 解析url得到ip
         * url url地址
         * port 端口号, 默认为80端口
         */
//        JSONObject resolver_json;
//        resolver_json["url"] = "url, required";
//        resolver_json["port"] = "port optional, default 80";
//        std::string resolver_str = resolver_json.to_string();
//        cmd_factory::instance().register_cmd("resolver_url", [=](JSONObject params){
//            if (params.has("args")) {
//                JSONObject args = params["args"];
//                int port = 80;
//                if (args.has("url")) {
//                    std::string url = args["url"].get_string();
//                    if (args.has("port")) {
//                        port = args["port"].get_int();
//                    }
//                    tcp::resolver(url, port, [=](std::shared_ptr<std::vector<std::tuple<std::string, int >>> ips){
//                        JSONObject ip_json;
//                        for (int i = 0; i < ips->size(); i ++) {
//                            std::tuple<std::string, int> t = ips->at(i);
//                            JSONObject v;
//                            v.put("ip", std::get<0>(t));
//                            v.put("port", std::get<1>(t));
//                            ip_json.append(v);
//                        }
//                        JSONObject ret;
//                        ret["value"] = ip_json;
//                        cmd_factory::instance().callback(params, true, ret);
//                    });
//                } else {
//                    cmd_factory::instance().callback(params, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), " refer to ", resolver_str));
//                }
//            } else {
//                cmd_factory::instance().callback(params, ERROR_PARAMETER, ERROR_STRING(ERROR_PARAMETER));
//            }
//        });
//
//        tcp_.reset(new tcp);
//        tcp_->set_connect_handler([=](bool connected) {
//            JSONObject data;
//            if (connected) {
//                data["to"] = "connected";
//            } else {
//                data["to"] = "disconnected";
//            }
//            common::call(common::get_tcp_notify_function(), data);
//        });
//
//        tcp_->set_read_handler([](char* data, int len){
//            std::string str(data, len);
//            JSONObject json;
//            json["to"] = "read";
//            json["data"] = str;
//            common::call(common::get_tcp_notify_function(), json);
//        });
//
//        tcp_->set_write_handler([](char* data, int len){
//            std::string str(data, len);
//            JSONObject json;
//            json["to"] = "write";
//            json["data"] = str;
//            common::call(common::get_tcp_notify_function(), json);
//        });
//
//
//
//        cmd_factory::instance().register_cmd("tcp_connect", [=](JSONObject param){
//            if (param.has("args")) {
//                JSONObject args = param["args"];
//                if (args.has("ip") && args.has("port")) {
//                    std::string ip = args["ip"].get_string();
//                    int port = args["port"].get_int();
//                    tcp_->connect(ip, port);
//                    cmd_factory::instance().callback(param);
//                }
//            } else {
//                cmd_factory::instance().callback(param, ERROR_PARAMETER, ERROR_STRING(ERROR_PARAMETER));
//            }
//        });
//
//        cmd_factory::instance().register_cmd("tcp_close", [=](JSONObject param){
//            tcp_->close();
//            cmd_factory::instance().callback(param);
//        });
//
//        cmd_factory::instance().register_cmd("tcp_reconnect", [=](JSONObject param){
//            tcp_->reconnect();
//            cmd_factory::instance().callback(param);
//        });
//
//        cmd_factory::instance().register_cmd("tcp_write", [=](JSONObject param){
//            if (param.has("args.data")) {
//                JSONObject args = param["args"];
//                std::string data = args["data"].get_string();
//                tcp_->write(data.c_str(), data.length());
//            } else {
//                cmd_factory::instance().callback(param, ERROR_PARAMETER, ERROR_STRING(ERROR_PARAMETER));
//            }
//        });
    }
}