//
// Created by liuke on 15/12/11.
//

#include <test/test_def.h>




#ifdef TEST_ENABLE

#include "../src/json/json_wrap.h"



TEST(json_wrap_test, parse_string) {
    std::string string = "{\"a\":1, \"b\":\"b\",\"c\":{\"c1\":1.2},\"d\":[1,2,3]}";
    bool error;
    Json::Value json = plan9::json_wrap::parse(string, &error);
    EXPECT_EQ(error, false);
    EXPECT_EQ(json["a"].asInt(), 1);
    EXPECT_EQ(json["b"].asString(), "b");
    EXPECT_EQ(json["c"].isObject(), true);
    EXPECT_EQ(json["d"].isArray(), true);
    EXPECT_EQ(json["c"]["c1"].asDouble(), 1.2);
    EXPECT_EQ(json["d"].size(), 3);
    std::cout << plan9::json_wrap::toString(json) << std::endl;
}

TEST(json_wrap_test, parse_file) {

}


#endif