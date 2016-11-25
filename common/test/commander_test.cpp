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
#include "json/JSONObject.h"

void exec(std::string method, plan9::JSONObject p) {
    using namespace std;
    plan9::cmd_factory::instance().execute(method, p, [=](plan9::JSONObject result){
        cout << "callback ok from hello, json:" << result.to_string() << endl;
        plan9::cmd_factory::instance().execute("world", result, [=](plan9::JSONObject result1){
            cout << "callback ok from world, json:" << result1.to_string() << endl;
        });
    });
}
void call() {
    plan9::JSONObject value;
    exec("hello", value);
}



TEST(cmd_factory_test, run) {
    using namespace std;
    plan9::cmd_factory::instance().register_cmd("hello", [=](plan9::JSONObject result){
        std::cout << "run ok, json:" << result.to_string() << std::endl;
        result["ret_hello"] = "callback ready";
        plan9::cmd_factory::instance().execute("world", result, [=](plan9::JSONObject data){
            plan9::cmd_factory::instance().callback_multi(result, true, 0, "", plan9::JSONObject());
            plan9::cmd_factory::instance().callback(result, true, plan9::JSONObject());
        });
    });

    plan9::cmd_factory::instance().register_cmd("world", [=](plan9::JSONObject result){
        std::cout << "run ok, json:" << result.to_string() << std::endl;
        result["ret_wrold"] = "callback ready";
        plan9::cmd_factory::instance().callback(result);
    });

    plan9::JSONObject value;
//    value["args"]["a"] = 1;
//    value["aux"]["id"] = plan9::UUID::id();
//    plan9::cmd_factory::instance().execute("hello", value, [=](plan9::JSONObject result){
//        cout << "callback ok from hello, json:" << result.to_string() << endl;
//        plan9::cmd_factory::instance().execute("world", result, [=](plan9::JSONObject result1){
//            cout << "callback ok from world, json:" << result1.to_string() << endl;
//        });
//    });

//    call();
    plan9::cmd_factory::instance().execute("hello", value, [=](plan9::JSONObject data){
        cout << "execute result" << data.to_string() << endl;
    });
}




#endif