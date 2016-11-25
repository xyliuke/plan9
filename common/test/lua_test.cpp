//
// Created by liuke on 15/12/16.
//

#include <test/test_def.h>

#ifdef LUA_TEST

#include <lua/lua_bind.h>
#include "json/JSONObject.h"

TEST(lua_test, call) {
    EXPECT_EQ(plan9::lua_bind::instance().lua_bind_init("./test.lua"), true);
    plan9::JSONObject j;
    j["a"] = "a1";
    plan9::JSONObject b;
    b["b1"] = "b1";
    j["b"] = b;
    plan9::JSONObject c;
    c.append(1);
    c.append(2);
    c.append(5);
    j["c"] = c;

    EXPECT_EQ(plan9::lua_bind::instance().call("a.b.c.d", j), true);
    plan9::lua_bind::instance().call("a.b.c.d", j, [=](plan9::JSONObject result){
        std::cout << "callback: " << result << std::endl;
    });
//    EXPECT_EQ(plan9::lua_bind::instance().call("a.c", j), false);
}

#endif