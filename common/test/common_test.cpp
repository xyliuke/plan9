//
// Created by liuke on 15/12/18.
//

#include <test/test_def.h>


#ifdef COMMON_TEST


#include <init/common.h>
#include <json/json.h>
#include <commander/cmd_factory.h>

using namespace plan9;

TEST(common_test, init) {
    using namespace plan9;

    plan9::common::init("./data", "./data");
    Json::Value param;
    param["level"] = "info";
    param["msg"] = "dddddddddddddddsssssssssxxxxxxxxxxx";
    plan9::cmd_factory::instance().execute("log", param);
}
#endif