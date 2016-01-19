//
// Created by liuke on 16/1/13.
//

#include "tcp_wrap_default.h"
#import <network/tcp.h>
#include <thread/timer.h>
#include <log/log_wrap.h>
#include <json/json_wrap.h>
#include <thread/timer_wrap.h>
#include <thread/thread_wrap.h>
#include <thread/thread_define.h>

namespace plan9
{
    class tcp_wrap_default::tcp_wrap_default_impl {

    public:
        tcp_wrap_default_impl() : tcp_(new tcp), connected(false), timer(new plan9::timer), next_connect_time(1000)  {
            tcp_->set_connect_handler([=](bool connect) {
                thread_wrap::post_background([=](){
                    if (connect) {
                        tcp_->enable_ping();
                        next_connect_time = 1000;
                        timer->cancel();
                    } else {
                        timer->start([=]() {
                            tcp_->reconnect();
                        }, next_connect_time);

                        if (next_connect_time > 60000) {
                            next_connect_time = 1000;
                        } else {
                            next_connect_time *= 2;
                        }
                    }
                    send_connect_handler(connect);
                });
            });

            tcp_->set_read_handler([=](std::string msg) {
                thread_wrap::post_background([=]() {
                    log_wrap::net().d("recv data from tcp_wrap_default : ", msg);
                    bool error;
                    Json::Value data = json_wrap::parse(msg, &error);
                    if (!error) {
                        if (data.isMember("aux") && data["aux"].isMember("id")) {
                            std::string id = data["aux"]["id"].asString();
                            if (send_map.find(id) != send_map.end()) {
                                auto callback = send_map[id];
                                callback(data);
                                send_map.erase(id);
                            } else {
                                send_read_handler(msg);
                            }
                        } else {
                            log_wrap::net().e("recv data is illegal json format : ", msg);
                        }
                    } else {
                        log_wrap::net().e("recv data is illegal json format : ", msg);
                        send_read_handler(msg);
                    }
                });
            });

            tcp_->set_write_handler([=](std::string msg) {
                thread_wrap::post_background([=]() {
                    log_wrap::net().i("send data : ", msg);
                });
            });
        }

        void connect(std::string ip, int port) {
            tcp_->connect(ip, port);
        }

        void send(network_server_type type, std::string msg) {
            tcp_->write(type, msg);
        }

        void send(network_server_type type, Json::Value msg, std::function<void(Json::Value)> callback, int timeout) {
            if (msg.isMember("aux") && msg["aux"].isMember("id")) {
                std::string id = msg["aux"]["id"].asString();
                send_map[id] = callback;
                log_wrap::net().d("send data from tcp_wrap_default : ", json_wrap::toString(msg));
                tcp_->write(type, json_wrap::toString(msg));
#ifdef THREAD_ENABLE
                if (timeout > 0) {
//                    timer_wrap::instance().create_timer(timeout, [=]() mutable {
//                        send_map.erase(id);
//                        msg["result"]["success"] = false;
//                        msg["result"]["reason"] = "timeout";
//                        callback(msg);
//                    });
                }
#endif
            } else {
                log_wrap::net().e("send data is illegal json format : ", json_wrap::toString(msg));
            }
        }

        void set_connect_handler(std::function<void(bool)> function) {
            connect_handler = function;
        }


        void set_read_handler(std::function<void(std::string)> function) {
            read_handler = function;
        }

        void send_connect_handler(bool connect) {
            if (connected == connect) return;
            connected = connect;
            if (connect_handler != nullptr) {
                connect_handler(connect);
            }
        }

        void send_read_handler(std::string msg) {
            if (read_handler != nullptr) {
                read_handler(msg);
            }
        }

    private:
        std::shared_ptr<tcp> tcp_;
        std::function<void(bool)> connect_handler;
        std::function<void(std::string)> read_handler;
        bool connected;
        std::shared_ptr<timer> timer;
        long next_connect_time;
        std::map<std::string, std::function<void(Json::Value)>> send_map;
    };



    tcp_wrap_default::tcp_wrap_default() : impl(new tcp_wrap_default_impl) {
    }

    tcp_wrap_default tcp_wrap_default::instance() {
        static tcp_wrap_default tcp;
        return tcp;
    }

    void tcp_wrap_default::connect(std::string ip, int port) {
        impl->connect(ip, port);
    }

    void tcp_wrap_default::send(network_server_type type, std::string msg) {
        impl->send(type, msg);
    }

    void tcp_wrap_default::send(std::string msg) {
        impl->send(network_server_type::SERVER_CONNECT, msg);
    }

    void tcp_wrap_default::set_connect_handler(std::function<void(bool)> function) {
        impl->set_connect_handler(function);
    }

    void tcp_wrap_default::set_read_handler(std::function<void(std::string)> function) {
        impl->set_read_handler(function);
    }

    void tcp_wrap_default::send(network_server_type type, Json::Value msg, std::function<void(Json::Value)> callback,
                                int timeout) {
        impl->send(type, msg, callback, timeout);
    }
}