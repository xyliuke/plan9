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

    plan9::common::init("./data", "../../lua");
    Json::Value param;
    param["level"] = "info";
    param["msg"] = "dddddddddddddddsssssssssxxxxxxxxxxx";
    param["target"] = "other";

    plan9::common::call_("native.get_error_code", param, [=](Json::Value result){
        std::cout << "callback common init 1 :" << result << std::endl;
    });

    Json::Value p1;
    p1["aux"]["id"] = "dddxxx";
    p1["args"] = param;
    plan9::common::call_("native.get_error_code", p1, [=](Json::Value result){
        std::cout << "callback common init 2 :" << result << std::endl;
    });

    plan9::cmd_factory::instance().execute("log", param);

    plan9::common::set_ios_platform();
    plan9::common::set_android_platform();
    plan9::common::set_win_platform();
    plan9::common::set_macosx_platform();
}


#endif