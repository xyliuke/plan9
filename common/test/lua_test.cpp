//
// Created by liuke on 15/12/16.
//

#include <test/test_def.h>

#ifdef TEST_ENABLE

#include <lua/lua_bind.h>

TEST(lua_test, call) {
    EXPECT_EQ(plan9::lua_bind::instance().lua_bind_init("./test.lua"), true);
    Json::Value j;
    j["a"] = "a1";
    Json::Value b;
    b["b1"] = "b1";
    j["b"] = b;
    Json::Value c;
    c.append(1);
    c.append(2);
    c.append(5);
    j["c"] = c;
    plan9::lua_bind::instance().call("app", j);
    plan9::lua_bind::instance().call("app", j);
}

#endif