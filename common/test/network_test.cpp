//
// Created by liuke on 16/1/2.
//

#include <test/test_def.h>
#include <network/tcp.h>


#ifdef NETWORK_TEST

#include "network/tcp.h"
#include <network/tcp_wrap_default.h>

TEST(network_test, tcp) {


    plan9::tcp_wrap_default::instance().set_connect_handler([=](bool connect){
        std::cout << "connect : " << connect << std::endl;
        if (connect) {
            plan9::tcp_wrap_default::instance().send("hello wrold from clion");
        }
    });

    plan9::tcp_wrap_default::instance().set_read_handler([=](std::string msg){
        std::cout << "recv : " << msg << std::endl;
    });
    plan9::tcp_wrap_default::instance().connect("127.0.0.1", 8081);
    std::cout << "finish";


//    std::map<int, int> map;
//    map[1] = 1;
//    map[2] = 2;
//    map[3] = 3;
//    std::cout << map.size() << std::endl;
//
//    std::map<int, int>::iterator it = map.begin();
//    while (it != map.end()) {
//        map.erase(it ++);
//        std::cout << map.size() << std::endl;
//    }
}

#endif