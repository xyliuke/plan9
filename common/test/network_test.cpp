//
// Created by liuke on 16/1/2.
//

#include <test/test_def.h>


#ifdef NETWORK_TEST

#include "network/tcp.h"
plan9::tcp tcp;

TEST(network_test, tcp) {

    tcp.set_connect_handler([=](bool connect){
        std::cout << "connect : " << connect << std::endl;
        tcp.enable_ping(true);
//        if (!connect) {
//            tcp.reconnect();
//        } else {
//            tcp.write("hello world");
//        }
    });

    tcp.set_write_handler([](std::string data){
        std::cout << "write : " << data << std::endl;
    });

//    int a = 0;
    tcp.set_read_handler([=](std::string data) mutable{
        std::cout << "recv : " << data << std::endl;
//        std::stringstream ss;
//        ss << "data : ";
//        ss << a;
//        a ++;
//        tcp.write(ss.str());
//        if (a > 10) {
//            tcp.close();
//        }
    });

    tcp.connect("127.0.0.1", 8888);
//    plan9::tcp::instance().write("hello world 你好");
}

#endif