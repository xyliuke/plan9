//
// Created by liuke on 15/12/11.
//

#include <test/test_def.h>
#include <json/JSONObject.h>
#include <util/util.h>
#include <unordered_map>


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

std::string s2 = "{  \n\"code\": 0,  \"message\": \"获取数据成功\",  \"data\": {    \"totalCar\": 700,    \"totalDeal\": 50,    \"rate\": \"31.33%\",    \"noteNum\": 1500,    \"taskStatistic\": {      \"task\": 50,  \"task_see\": 0,       \"task_deal\": 15,      \"see_rate\": \"53.33%\",      \"see_deal_rate\": \"53.33%\",      \"task_deal_rate\": \"53.33%\",      \"overdue\": 34,       \"assist\": 23,      \"assist_success\": 3" \
            "},    \"dataList\": [      {        \"level\": \"A级车源\",        \"store_num\": 15,        \"month_store\": 30,        \"deal_num\": 5,        \"note_num\": 23,        \"rate\": \"33%\"      }," \
            "{        \"level\": \"B级车源\",        \"store_num\": 15,        \"month_store\": 30,        \"deal_num\": 5,        \"note_num\": 23,        \"rate\": \"33%\"      }    ]  }}  \n  ";

TEST(json_object_test, json) {

//    std::string ss = " 1, b : 20{}";
//    auto pos = ss.find("{", 0, 1);

//    std::string n1 = "123123";
//    std::string n2 = "123123.32323";
//    std::string n3 = "-123123";
//    std::string n4 = "-123123.5544";
//    std::string n5 = "-123123E+002";
//    std::string n6 = "-123123E-002";
//    std::string n7 = "-123123E002";
//    std::string n8 = "-12312.3E002";
//    std::string n9 = "-12311d2.3E002";

//    int a1 = std::atoi(n1.c_str());
//    double a2 = std::atof(n2.c_str());
//    int a3 = std::atoi(n3.c_str());
//    double a4 = std::atof(n4.c_str());
//    double a5 = std::atof(n5.c_str());
//    double a6 = std::atof(n6.c_str());
//    double a7 = std::atof(n7.c_str());
//    double a8 = std::atof(n8.c_str());
//    double a9 = std::atof(n9.c_str());

//    bool b1 = std::isdigit('9');
//    bool b2 = std::isdigit('E');
//    bool b3 = std::isdigit('-');
//    bool b4 = std::isdigit('+');
//    bool b5 = std::isdigit('e');

//    plan9::JSONObject json_string("{a:13232, b : 20.3}");
//    std::cout << json_string.to_string() << std::endl;
//    plan9::JSONObject json_string1("{\"a\":null, b : 20}");
//    std::cout << json_string1.to_string() << std::endl;
//    plan9::JSONObject json_string2("{\"a\": true , b : 20}");
//    std::cout << json_string2.to_string() << std::endl;
//    plan9::JSONObject json_string3("{ \"a\" : false , b : 20}");
//    std::cout << json_string3.to_string() << std::endl;
//    plan9::JSONObject json_string4("{ \"a\" : \"fsdf sdfls s sd  sd sd\nd ss s\" , b : 20}");
//    std::cout << json_string4.to_string() << std::endl;


//    std::stringstream ss;
//    ss.setf(std::ios::adjustfield);
//    ss.precision(8);
//    double d = 100001000.323423423423423;
//    d = 100001000.5;

//    std::to_string(d);
//    ss << d;
//    std::cout << ss.str() << std::endl;
//    std::cout<<"指定10位小数且为浮点数表示setiosflags(ios::fixed)："<<std::setiosflags(std::ios::fixed)<<std::setprecision(10)<<d<<std::endl;


//    float f = 3.14159268f;
//    std::cout << f << std::endl;

//    float f = 3.14159265358979323846;
//    printf("%.*f\n", FLT_MANT_DIG, f);


//    object_file.parse_file("./small_one.json");
//    std::cout << object_file.to_string().length() << std::endl;


        std::string s1 = "{  \"code\": 0, \"message\": \"获取成功\", \"data\": {\"count\": 6,\"list\": [{\"question\": \"question1\", \"answer\": \"answer1\" }, {\"question\": \"question2\", \"answer\": \"answer2\" }] } } ";

//        std::string s2 = "{\"code\": 0,  \"message\": \"获取数据成功\",  \"data\": {    \"totalCar\": 700,    \"totalDeal\": 50,    \"rate\": \"31.33%\",    \"noteNum\": 1500,    \"taskStatistic\": {      \"task\": 50,  \"task_see\": 0,       \"task_deal\": 15,      \"see_rate\": \"53.33%\",      \"see_deal_rate\": \"53.33%\",      \"task_deal_rate\": \"53.33%\",      \"overdue\": 34,       \"assist\": 23,      \"assist_success\": 3" \
//            "},    \"dataList\": [      {        \"level\": \"A级车源\",        \"store_num\": 15,        \"month_store\": 30,        \"deal_num\": 5,        \"note_num\": 23,        \"rate\": \"33%\"      }," \
//            "{        \"level\": \"B级车源\",        \"store_num\": 15,        \"month_store\": 30,        \"deal_num\": 5,        \"note_num\": 23,        \"rate\": \"33%\"      }    ]  }}";

//    plan9::JSONObject o1(s1);
//    std::cout << "o1 : " << o1.to_format_string() << std::endl;
//    plan9::JSONObject o2 = o1.copy();
//    std::cout << "o2 : " << o2.to_format_string() << std::endl;

    using namespace plan9;
    JSONObject ooo;
    ooo.set_comment("root 测试");
    ooo.put("121", 4444, "测试comment");
    ooo.put("aaa", "dsfsd", "测试comment");
    ooo.put("aab", true, "测试comment");
    ooo.put("abb", 1.2323, "测试comment");
    JSONObject arr;
    arr.set_comment("arr注释");
    arr.append(true, "fdsfs22");
    arr.append(false, "fdsfs33");
    arr.append(1.23, "fdsfs444");
    arr.append(2399, "fdsfs666");
    arr.append("ddsfsdf", "fdsfs12312");
    ooo.put("arr", arr);
    std::cout << "ooo : " << ooo.to_format_string() << std::endl;



//    std::map<int, std::string> m;
//    for (int i = 0; i < 10000000; ++i) {
//        m[i] = "44";
//        m.find(i);
//    }
//    m.insert(std::pair<std::string, std::string>("4", "44"));
//    m.insert(std::pair<std::string, std::string>("2", "55"));


//    for (int i = 0; i < 100; ++i) {
//        plan9::JSONObject object_file;
//        object_file.parse_file("./canada.json");
//    }
//        s2.substr(20, 100);
//        s2.find("message");
//        plan9::util::instance().trim(s2);
//        plan9::JSONObject jsonObject10(s1);
//    std::cout << jsonObject10.to_format_string() << std::endl;
//        plan9::JSONObject jsonObject11(s2);
//        std::cout << jsonObject11.to_format_string() << std::endl;

//    jsonObject11.enumerate([=](std::string key, plan9::JSONObject value, bool only_value){
//        if (only_value) {
//            std::cout << value.to_string() << std::endl;
//        } else {
//            std::cout << key << " : " << value.to_string() << std::endl;
//        }
//    });

//    plan9::JSONObject jsonObject4("a:12");

//        plan9::JSONObject json_string5(
//                "{ \"a\" : { aa: 1 , \"ab\":\" {}{}{}abababa b \", abc:{abcd:34, abcde:\"fsdfsd\"},} , b : 20,}");
//        std::cout << json_string5.to_string() << std::endl;
//        plan9::JSONObject json_string6(
//                "{ \"a\" : [1,2,3,4,5,\"a\", \"b[]{}\", {abc:3,  adde:\"49340304fff\"} ,] , b : 20}");
//        std::cout << json_string6.to_string() << std::endl;

//    }

//    plan9::JSONObject object("a", "aa");
//    object.put("b", "bb");
//    object["c.c1"] = 343;
//    std::cout << object.to_string() << std::endl;
//    plan9::JSONObject o1;
//    o1["xx"] = true;
//    o1["xxx"] = "json string";
//    std::cout << o1.to_string() << std::endl;
//
//    plan9::JSONObject o2 = object["c"];
//    std::cout << o2.to_string() << std::endl;
//    o2 = object["c"];
//    plan9::JSONObject o3;
//    o3 = object["a"].get_string();
//    std::cout << o2.to_string() << std::endl;
//
//    object.enumerate([=](std::string key, plan9::JSONObject value, bool onlyValue){
//        if (onlyValue) {
//            std::cout << "value : " << value.to_string() << std::endl;
//        } else {
//            std::cout << "key: " << key << "\tvalue : " << value.to_string() << std::endl;
//        }
//    });
//
//
//    plan9::JSONObject str("hell owrldfsdfsd");
//    std::cout << str.to_string() << std::endl;

/*
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

//    char buf[10240];
//    for (int i = 0; i < 100000; ++i) {
//
//        for (int j = 0; j < 10240; ++j) {
//            char c = buf[j];
//        }
//    }

//    char c1 = '[', c2 = ']', c3 = '{', c4 = '}', c5 = '\"';
//    char ca = c1 & c2 & c3 & c4 ;
//
//    for (int i = 0; i < 1000000; ++i) {
//        for (int j = 0; j < s2.length(); ++j) {
//            char c = s2.at(j);
//            if ((c & ca) != 0) {
//                if (c == c1) {
//
//                } else if (c == c2) {
//
//                } else if (c == c3) {
//
//                } else if (c == c4) {
//
//                }
//            } else if (c == c5) {
//
//            }
//        }
//    }

}


TEST(json_object_test, performence) {

//    char c1 = '[', c2 = ']', c3 = '{', c4 = '}', c5 = '\"';
//    char ca = c1 & c2 & c3 & c4 ;

//    for (int i = 0; i < 1000000; ++i) {
//        for (int j = 0; j < s2.length(); ++j) {
//            char c = s2.at(j);
//            if (c == c1) {
//
//            } else if (c == c2) {
//
//            } else if (c == c3) {
//
//            } else if (c == c4) {
//
//            } else if (c == c5) {
//
//            }
//        }
//    }

//    char buf[10240];
//    const char* c = "123.35677";
//    char* c1 = (char*) malloc(5);
//    std::copy(c, c + 5, c1);
//    double d = atof(c);
//    d = std::atof(c1);
//    int b = 100;
//    int* a = &b;
//    char c;
//    for (int i = 0; i < 100000; ++i) {
//
//        for (int j = 0; j < 10240; ++j) {
//            c = buf[j];
//        }
//        int c = *a;
//        int cc = b;

//        plan9::util::instance().trim2(s2);
//        std::cout << s << std::endl;

//        std::string s1(c, 50, 10);
//        std::cout << d << std::endl;
//    }
//    std::string f = "./canada.json";
//    for (int i = 0; i < 100; ++i) {
//        plan9::JSONObject object_file;
//        object_file.parse_file(f);
//    }
//    std::unordered_map<int, std::string> m;
//    for (int i = 0; i < 10000000; ++i) {
//        m.insert(std::pair<int , std::string>(i, "44"));
//        m.insert(std::map<int , std::string>::value_type(i, "44"));
//        m.find(i);
//    }
//    m.insert(std::pair<std::string, std::string>("4", "44"));

}

#endif