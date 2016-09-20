//
// Created by liuke on 15/12/11.
//

#include <test/test_def.h>
#include <json/JSONObject.h>


#ifdef JSON_TEST

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
    std::cout << plan9::json_wrap::to_string(json) << std::endl;
    Json::Value::Members m = json.getMemberNames();
    for (int i = 0; i < m.size(); ++i) {
        std::cout << m[i] << json[m[i]] << std::endl;
    }

    json["aaaa"]["xxxx"] = 1;
    auto it = json.begin();
    while (it != json.end()) {
        std::cout << it.name() << "\t" << *it << std::endl;
        ++ it;
    }

    std::shared_ptr<Json::Value> ptr;
    if (ptr) {
        std::cout << "init" << std::endl;
    } else {
//        std::cout << ptr.get() << std::endl;
    }
}

TEST(json_wrap_test, parse_file) {

}


TEST(json_wrap_test, json_pointer) {
//    std::string string = "{\"a\":1, \"b\":\"b\",\"c\":{\"c1\":1.2},\"d\":[1,2,3]}";
//    bool error;
//    auto json = plan9::json_wrap::parse2(string, &error);
//    std::cout << plan9::json_wrap::to_string(json) << std::endl;
//    plan9::JSONObject other = json;
//    std::cout << plan9::json_wrap::to_string(json) << std::endl;
//    std::cout << plan9::json_wrap::to_string(other) << std::endl;
//
//    Json::Value xx;
//    xx["xx"] = 333;
//    (*json)["xx"] = xx;
//
//    Json::Value axx = (*json)["xx"];
//    xx["xx"] = 444;
//    (*json)["xx"] = 555;
//    axx = 666;
//    std::cout << &axx << std::endl;
//    std::cout << plan9::json_wrap::to_string(xx) << std::endl;
//    std::cout << plan9::json_wrap::to_string((*json)["xx"]) << std::endl;
//    std::cout << plan9::json_wrap::to_string(axx) << std::endl;
//
//    std::cout << plan9::json_wrap::to_string2(xx) << std::endl;
//    xx["bbb"] = 3334;
//    std::cout << plan9::json_wrap::to_string2(xx) << std::endl;
}


TEST(json_object_test, json) {

//    std::string ss = " 1, b : 20{}";
//    auto pos = ss.find("{", 0, 1);

    std::string n1 = "123123";
    std::string n2 = "123123.32323";
    std::string n3 = "-123123";
    std::string n4 = "-123123.5544";
    std::string n5 = "-123123E+002";
    std::string n6 = "-123123E-002";
    std::string n7 = "-123123E002";
    std::string n8 = "-12312.3E002";
    std::string n9 = "-12311d2.3E002";

    int a1 = std::atoi(n1.c_str());
    double a2 = std::atof(n2.c_str());
    int a3 = std::atoi(n3.c_str());
    double a4 = std::atof(n4.c_str());
    double a5 = std::atof(n5.c_str());
    double a6 = std::atof(n6.c_str());
    double a7 = std::atof(n7.c_str());
    double a8 = std::atof(n8.c_str());
    double a9 = std::atof(n9.c_str());

    bool b1 = std::isdigit('9');
    bool b2 = std::isdigit('E');
    bool b3 = std::isdigit('-');
    bool b4 = std::isdigit('+');
    bool b5 = std::isdigit('e');

//    plan9::JSONObject json_string("{a:13232, b : 20}");
//    std::cout << json_string.to_string() << std::endl;
//    plan9::JSONObject json_string1("{\"a\":null, b : 20}");
//    std::cout << json_string1.to_string() << std::endl;
//    plan9::JSONObject json_string2("{\"a\": true , b : 20}");
//    std::cout << json_string2.to_string() << std::endl;
//    plan9::JSONObject json_string3("{ \"a\" : false , b : 20}");
//    std::cout << json_string3.to_string() << std::endl;
//    plan9::JSONObject json_string4("{ \"a\" : \"fsdf sdfls s sd  sd sd\nd ss s\" , b : 20}");
//    std::cout << json_string4.to_string() << std::endl;
    std::string s1 = "{  \"code\": 0, \"message\": \"获取成功\", \"data\": {\"count\": 6,\"list\": [{\"question\": \"question1\", \"answer\": \"answer1\" }, {\"question\": \"question2\", \"answer\": \"answer2\" }] } } ";

    std::string s2 = "{\"code\": 0,  \"message\": \"获取数据成功\",  \"data\": {    \"totalCar\": 700,    \"totalDeal\": 50,    \"rate\": \"31.33%\",    \"noteNum\": 1500,    \"taskStatistic\": {      \"task\": 50,  \"task_see\": 0,       \"task_deal\": 15,      \"see_rate\": \"53.33%\",      \"see_deal_rate\": \"53.33%\",      \"task_deal_rate\": \"53.33%\",      \"overdue\": 34,       \"assist\": 23,      \"assist_success\": 3" \
            "},    \"dataList\": [      {        \"level\": \"A级车源\",        \"store_num\": 15,        \"month_store\": 30,        \"deal_num\": 5,        \"note_num\": 23,        \"rate\": \"33%\"      }," \
            "{        \"level\": \"B级车源\",        \"store_num\": 15,        \"month_store\": 30,        \"deal_num\": 5,        \"note_num\": 23,        \"rate\": \"33%\"      }    ]  }}";
    plan9::JSONObject jsonObject10(s1);
    std::cout << jsonObject10.to_format_string() << std::endl;
    plan9::JSONObject jsonObject11(s2);
    std::cout << jsonObject11.to_format_string() << std::endl;

    jsonObject11.enumerate([=](std::string key, plan9::JSONObject value, bool only_value){
        if (only_value) {
            std::cout << value.to_string() << std::endl;
        } else {
            std::cout << key << " : " << value.to_string() << std::endl;
        }
    });


    plan9::JSONObject json_string5("{ \"a\" : { aa: 1 , \"ab\":\" {}{}{}abababa b \", abc:{abcd:34, abcde:\"fsdfsd\"},} , b : 20,}");
    std::cout << json_string5.to_string() << std::endl;
    plan9::JSONObject json_string6("{ \"a\" : [1,2,3,4,5,\"a\", \"b[]{}\", {abc:3,  adde:\"49340304fff\"} ,] , b : 20}");
    std::cout << json_string6.to_string() << std::endl;



    /*
    plan9::JSONObject object("a", "aa");
    object.put("b", "bb");
    object["c.c1"] = 343;
    std::cout << object.to_string() << std::endl;

    plan9::JSONObject x_obj("xx", "aaxx");
    object.put("x", x_obj);
    std::cout << object.to_string() << std::endl;
    std::cout << object.get("x.xx").to_string() << std::endl;
    std::cout << object.get("xxx.xx").to_string() << std::endl;
    std::cout << object.get("x.xx1").to_string() << std::endl;
    std::cout << object["x.xx1"].to_string() << std::endl;
    std::cout << object["xxx.xx"].to_string() << std::endl;
    std::cout << object["x.xx1"].to_string() << std::endl;


    auto keys = object.all_keys();
    auto it = keys->begin();
    while (it != keys->end()) {
        std::cout << *it << "\t";
        ++ it;
    }
    std::cout << "\n";

    auto values = object.all_values();
    auto it_v = values->begin();
    while (it_v != values->end()) {
        std::cout << (*it_v).to_string() << "\t";
        ++ it_v;
    }
    std::cout << "\n";

    plan9::JSONObject array;
    array.append(false);
    array.append(12);
    array.append(34.0f);
    array.append("56");
    std::string s78("78");
    array.append(s78);
    std::cout << array.to_string() << std::endl;

    object.put("arr", array);
    std::cout << object.to_string() << std::endl;

    array.remove(2);
    std::cout << array.to_string() << std::endl;
    array.remove_first();
    std::cout << array.to_string() << std::endl;
    array.remove_last();
    std::cout << array.to_string() << std::endl;

    std::cout << object.to_string() << std::endl;

    plan9::JSONObject oo;
    oo["i"] = 2;
    oo["l"] = 4L;
    oo["f"] = 3.f;
    oo["d"] = 2.3;
    oo["b"] = true;
    oo["o"] = object;
    oo["a"] = array;
    std::cout << oo.to_string() << std::endl;
    std::cout << oo.to_format_string() << std::endl;

    bool und = !(oo.get("fsdfsd"));
    if (oo.get("xxcxcxcx")) {
        std::cout << "exist";
    } else {
        std::cout << "not exist";
    }
    std::cout << "define " << und << std::endl;
*/

}


#endif