#include <iostream>
#include <thread>

#include "network/tcp_client.h"
#include "log/log.h"
#include "thread/thread_dispatch.h"
#include "thread/thread_wrap.h"
#include <json/json.h>
#include "commander/cmd_factory.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <gtest/gtest.h>

using namespace std;
boost::asio::ip::tcp::socket* sock;

void write_handler(const boost::system::error_code & ec, size_t size) {

}

void connect_handler(const boost::system::error_code & ec) {
    // 如果ec返回成功我们就可以知道连接成功了
    cout << "连接错误码:" << ec << endl;
//    sock->async_write_some("hello world", write_handler);
}



int main(int argc, char* argv[]) {
//    using namespace plan9;
//    plan9::log::init("./");
    
//    string input;
//    while (true) {
//        cin >> input;
//        if (input == "quit") {
//            break;
//        }
//    }


//    using namespace boost::asio;
//    io_service service;
//    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8888);
//    sock = new ip::tcp::socket(service);
//    sock->async_connect(ep, connect_handler);
//    service.run();

    return 0;
}

int t(int a) {
    return a + 1;
}

TEST(t_test, t) {
    EXPECT_EQ(2, t(1));
}


class SuiteName : public ::testing::Test {
    
};
