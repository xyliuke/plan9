//
// Created by liuke on 15/12/18.
//

#include <test/test_def.h>


#ifdef COMMON_TEST


#include <init/common.h>
#include <json/json.h>
#include <commander/cmd_factory.h>
#include <lua/lua_bind.h>
#include <json/json_wrap.h>

using namespace plan9;

TEST(common_test, init) {
    using namespace plan9;

    plan9::common::init("./data", "../../lua");
    Json::Value param;
    param["level"] = "info";
    Json::Value msg;
    msg["aaa"] = 5;
    param["msg"] = "xxx : {\"a\":1}";
    param["target"] = "other";

//    plan9::common::call("log", json_wrap::to_string(param), [=](std::string result){
//        std::cout << "callback common init 1 :" << result << std::endl;
//    });

//    Json::Value p1;
//    p1["aux"]["id"] = "dddxxx";
//    p1["args"] = param;
//    plan9::common::call_("native.get_error_code", Json::Value(), [=](Json::Value result){
//        std::cout << "callback common init 2 :" << result << std::endl;
//    });

    Json::Value p;
    p["test"] = "hello world from ios";
    plan9::common::call_("server.send", p, [=](Json::Value result){
//        Json::Value p2;
//        p2["a"] = "a";
//        plan9::common::call_("server.send", p2, [=](Json::Value result){
            std::cout << "callback from send :" << result.toStyledString() << std::endl;
//        });
    });


//    plan9::common::call("server.connect");

//    plan9::common::set_ios_platform();
//    plan9::common::set_android_platform();
//    plan9::common::set_win_platform();
//    plan9::common::set_macosx_platform();
}


#endif