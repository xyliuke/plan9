//
// Created by liuke on 16/1/13.
//

#ifndef COMMON_TCP_WRAP_DEFAULT_H
#define COMMON_TCP_WRAP_DEFAULT_H

#include <memory>
#include <functional>
#include <string>
#include <json/json.h>
#include "tcp.h"

namespace plan9 {


    /**
     * 默认的tcp连接封装类,自动向服务器发ping包,断线后会自动重连
     */
    class tcp_wrap_default {
    public:
        static tcp_wrap_default instance();

        void connect(std::string ip, int port);
        void send(network_server_type type, std::string msg);
        void send(std::string msg);
        void send(network_server_type type, Json::Value msg, std::function<void(Json::Value)> callback, int timeout);

        void set_connect_handler(std::function<void(bool)> function);
        void set_read_handler(std::function<void(std::string)> function);


    private:
        tcp_wrap_default();
        class tcp_wrap_default_impl;
        std::shared_ptr <tcp_wrap_default_impl> impl;
    };

}


#endif //COMMON_TCP_WRAP_DEFAULT_H
