//
// Created by liuke on 16/1/2.
//

#include <test/test_def.h>
#include <network/tcp.h>


#ifdef NETWORK_TEST

#include "network/tcp.h"
#include <network/tcp_wrap_default.h>
#include <list>
#include <json/json_wrap.h>

TEST(network_test, tcp) {

    plan9::tcp_wrap_default::instance().set_connect_handler([=](bool connect){
        std::cout << "connect : " << connect << std::endl;
        if (connect) {
//            plan9::tcp_wrap_default::instance().send(plan9::network_server_type::SERVER_DATABASE ,"hello wrold from clion");
            Json::Value msg;
            msg["aux"]["id"] = "12121323213";
            msg["aux"]["to"] = "call_sss";
            plan9::tcp_wrap_default::instance().send(plan9::network_server_type::SERVER_DATABASE, msg, [=](Json::Value result){
                std::string result_str = plan9::json_wrap::toString(result);
                std::cout << result_str << std::endl;
            }, 10000);
        }
    });

    plan9::tcp_wrap_default::instance().set_read_handler([=](std::string msg){
        std::cout << "recv : " << msg << std::endl;
    });
    plan9::tcp_wrap_default::instance().connect("127.0.0.1", 8081);
//    std::cout << "finish";

//    plan9::network_server_type type = plan9::network_server_type::SERVER_DATABASE;
//    int itype = static_cast<int>(type);
//    plan9::network_server_type tt = static_cast<plan9::network_server_type>(0x21);
}

#endif