//
// Created by liuke on 16/1/2.
//

#include <test/test_def.h>


#ifdef NETWORK_TEST

#include <network/tcp_wrap.h>

TEST(network_test, tcp) {
    plan9::tcp_wrap::instance().set_connect_handler([](bool connect){
        std::cout << "connect : " << connect << std::endl;
        plan9::tcp_wrap::instance().write("hello world");
    });

    int a = 0;
    plan9::tcp_wrap::instance().set_read_handler([=](std::string data) mutable{
        std::cout << "recv : " << data << std::endl;
        std::stringstream ss;
        ss << "data : ";
        ss << a;
        a ++;
        plan9::tcp_wrap::instance().write(ss.str());
    });

    plan9::tcp_wrap::instance().connect("127.0.0.1", 8888);

//    plan9::tcp_wrap::instance().write("hello world 你好");
}

#endif