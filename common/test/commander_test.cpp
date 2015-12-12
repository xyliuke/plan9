//
// Created by liuke on 15/12/9.
//

#include "commander/cmd_factory.h"
#include "test/test_def.h"
#include "../src/json/json_wrap.h"

#ifdef TEST_ENABLE

TEST(cmd_factory_test, run) {
    using namespace std;
    plan9::cmd_factory::instance().register_cmd("hello", [=](Json::Value result){
        std::cout << "run ok, json:" << plan9::json_wrap::toString(result) << std::endl;
        result["ret"] = "callback ready";
        plan9::cmd_factory::instance().callback(result);
    });

    Json::Value value;
    value["a"] = 1;
    cout << plan9::json_wrap::toString(value) << endl;
    plan9::cmd_factory::instance().execute("hello", value, [=](Json::Value result){
        cout << "callback ok, json:" << plan9::json_wrap::toString(result) << endl;
    });

    plan9::cmd_factory::instance().execute("hello", value);
}


#endif