//
// Created by liuke on 15/12/18.
//

#include <test/test_def.h>


#ifdef COMMON_TEST


#include <init/common.h>
#include <json/json.h>
#include <commander/cmd_factory.h>
#include <lua/lua_bind.h>

using namespace plan9;

TEST(common_test, init) {
    using namespace plan9;

//    char n = msg[msg.length() - 1]

//    unsigned char c = 0xa2;
//    char cc = (char)c;
//    int ci = (int)cc;
//    int ci1 = (int)c;

//    unsigned char buf[4] = {0x0, 0x0, 0x0, 0xa2};
//    int a1 = buf[0];
//    int a2 = buf[1];
//    int a3 = buf[2];
//    int a4 = buf[3];
//    int ret = (a1 << 24) + (a2 << 16) + (a3 << 8) + a4;

//    plan9::common::init("./data", "../../lua");
//    Json::Value param;
//    param["level"] = "info";
//    param["msg"] = "dddddddddddddddsssssssssxxxxxxxxxxx";
//    param["target"] = "other";
//
//    plan9::common::call_("native.get_error_code", param, [=](Json::Value result){
//        std::cout << "callback common init 1 :" << result << std::endl;
//    });

//    Json::Value p1;
//    p1["aux"]["id"] = "dddxxx";
//    p1["args"] = param;
//    plan9::common::call_("native.get_error_code", Json::Value(), [=](Json::Value result){
//        std::cout << "callback common init 2 :" << result << std::endl;
//    });

//    plan9::common::call_("server.connect", Json::Value(), [=](Json::Value result){
//        Json::Value p2;
//        p2["a"] = "a";
//        plan9::common::call_("server.send", p2, [=](Json::Value result){
//            std::cout << "callback from send :" << result.toStyledString() << std::endl;
//        });
//    });



//    plan9::cmd_factory::instance().execute("log", param);

//    plan9::common::call("server.connect");

//    plan9::common::set_ios_platform();
//    plan9::common::set_android_platform();
//    plan9::common::set_win_platform();
//    plan9::common::set_macosx_platform();
}


#endif