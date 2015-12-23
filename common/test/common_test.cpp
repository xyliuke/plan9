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

    plan9::common::call("native.get_error_code", param, [=](Json::Value result){
        std::cout << "callback common init :" << result << std::endl;
    });

//    plan9::cmd_factory::instance().execute("log", param);



//    lua_bind::instance().call("native.get_error_code", [=](Json::Value result){
//        std::cout << "callback common init :" << result << std::endl;
//    });
//
//    lua_bind::instance().call("biz.test", param, [=](Json::Value result){
//        std::cout << "callback common init :" << result << std::endl;
//    });
}
#endif