//
// Created by liuke on 15/12/9.
//

#include <init/common.h>
#include <util/util.h>
#include "test/test_def.h"


#ifdef COMMANDER_TEST


#include "commander/cmd_factory.h"
#include "../src/json/json_wrap.h"
#include "util/UUID.h"

TEST(cmd_factory_test, run) {
    using namespace std;
    plan9::cmd_factory::instance().register_cmd("hello", [=](Json::Value result){
        std::cout << "run ok, json:" << plan9::json_wrap::to_string(result) << std::endl;
        result["ret_hello"] = "callback ready";
        plan9::cmd_factory::instance().callback(result);
    });

    plan9::cmd_factory::instance().register_cmd("world", [=](Json::Value result){
        std::cout << "run ok, json:" << plan9::json_wrap::to_string(result) << std::endl;
        result["ret_wrold"] = "callback ready";
        plan9::cmd_factory::instance().callback(result);
    });

    Json::Value value;
    value["args"]["a"] = 1;
    value["aux"]["id"] = plan9::UUID::id();
    plan9::cmd_factory::instance().execute("hello", value, [=](Json::Value result){
        cout << "callback ok from hello, json:" << plan9::json_wrap::to_string(result) << endl;
        plan9::cmd_factory::instance().execute("world", result, [=](Json::Value result1){
            cout << "callback ok from world, json:" << plan9::json_wrap::to_string(result1) << endl;
        });
    });

    plan9::cmd_factory::instance().execute("hello", value);
}



#endif