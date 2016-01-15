//
// Created by liuke on 16/1/13.
//

#include "tcp_wrap_default.h"
#import <network/tcp.h>
#import <thread/thread_wrap.h>

namespace plan9
{
    class tcp_wrap_default::tcp_wrap_default_impl {

    public:
        tcp_wrap_default_impl() : tcp_(new tcp), connected(false) {
            tcp_->set_connect_handler([=](bool connect) {
                if (connect) {
                    tcp_->enable_ping();
                } else {
                    tcp_->reconnect();
                }
                send_connect_handler(connect);
            });

            tcp_->set_read_handler([=](std::string msg) {
                send_read_handler(msg);
            });

            tcp_->set_write_handler([=](std::string msg) {

            });
        }

        void connect(std::string ip, int port) {
//            thread_wrap::post_network([=](){
                tcp_->connect(ip, port);
//            });
        }

        void send(std::string msg) {
//            thread_wrap::post_network([=](){
                tcp_->write(msg);
//            });
        }

        void set_connect_handler(std::function<void(bool)> function) {
            connect_handler = function;
        }


        void set_read_handler(std::function<void(std::string)> function) {
            read_handler = function;
        }

        void send_connect_handler(bool connect) {
//            thread_wrap::post_network([=](){
                if (connect != connected) {
                    connected = connect;
                    if (connect_handler != nullptr) {
                        connect_handler(connect);
                    }
                }
//            });
        }

        void send_read_handler(std::string msg) {
//            thread_wrap::post_network([=](){
                if (read_handler != nullptr) {
                    read_handler(msg);
                }
//            });
        }

    private:
        std::shared_ptr<tcp> tcp_;
        std::function<void(bool)> connect_handler;
        std::function<void(std::string)> read_handler;
        bool connected;
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

    void tcp_wrap_default::send(std::string msg) {
        impl->send(msg);
    }

    void tcp_wrap_default::set_connect_handler(std::function<void(bool)> function) {
        impl->set_connect_handler(function);
    }

    void tcp_wrap_default::set_read_handler(std::function<void(std::string)> function) {
        impl->set_read_handler(function);
    }

}