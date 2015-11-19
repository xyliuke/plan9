//
// Created by liuke on 15/11/8.
//

#include "tcp_wrap.h"
#include "tcp_client.h"
#include "../thread/thread_wrap.h"

namespace plan9
{
    tcp_wrap::tcp_wrap() : tcp_(new tcp_client) {

    }

    void tcp_wrap::connect(std::string ip, int port) {
        tcp_->connect(ip, port);
    }

    void tcp_wrap::send(std::string msg) {
        thread_wrap::post_network([=](){
            tcp_->write(msg);
        });
    }


}