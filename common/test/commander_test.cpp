//
// Created by liuke on 15/12/9.
//

#include "test/test_def.h"


#ifdef TEST_ENABLE


#include "commander/cmd_factory.h"
#include "../src/json/json_wrap.h"


TEST(cmd_factory_test, run) {
    using namespace std;
    plan9::cmd_factory::instance().register_cmd("hello", [=](Json::Value result){
        std::cout << "run ok, json:" << plan9::json_wrap::toString(result) << std::endl;
        result["ret_hello"] = "callback ready";
        plan9::cmd_factory::instance().callback(result);
    });

    plan9::cmd_factory::instance().register_cmd("world", [=](Json::Value result){
        std::cout << "run ok, json:" << plan9::json_wrap::toString(result) << std::endl;
        result["ret_wrold"] = "callback ready";
        plan9::cmd_factory::instance().callback(result);
    });

    Json::Value value;
    value["a"] = 1;
    plan9::cmd_factory::instance().execute("hello", value, [=](Json::Value result){
        cout << "callback ok from hello, json:" << plan9::json_wrap::toString(result) << endl;
        plan9::cmd_factory::instance().execute("world", result, [=](Json::Value result1){
            cout << "callback ok from world, json:" << plan9::json_wrap::toString(result1) << endl;
        });
    });

    plan9::cmd_factory::instance().execute("hello", value);
}


#endif