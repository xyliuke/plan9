//
// Created by liuke on 15/12/18.
//

#include <test/test_def.h>


#ifdef COMMON_TEST


#include <init/common.h>
#include <json/json.h>
#include <commander/cmd_factory.h>
#include <lua/lua_bind.h>
#include <json/json_wrap.h>
#include <json/JSONObject.h>
#include <wchar.h>
#include <iconv.h>

using namespace plan9;


TEST(common_test, init) {

//    std::string utf = "\\u5f90\\u5dde";
//    for (int i = 0; i < utf.length(); ++i) {
//        std::cout << i << ":" << utf.at(i) << std::endl;
//    }
//    std::cout << utf << std::endl;
//    std::string utff = replace_all(utf, "\\", "");
//    std::cout << utff << std::endl;

//    std::wstring ws = s2ws(utf);
//    std::string utf1 = "\u5f90\u5dde\u0041";
//    std::cout << utf1 << "\n";
//    std::string utf2 = "徐州A";
//    std::cout << utf2 << "\n";

//    for (int i = 0; i < utf1.length(); ++i) {
//        std::cout << i << ":" << (int)utf1.at(i) << std::endl;
//    }
//    for (int i = 0; i < utf2.length(); ++i) {
//        std::cout << i << ":" << (int)utf1.at(i) << std::endl;
//    }
//    std::cout << utf << ", " << utf1 << ", ";
//    std::wcout << ws;
//    plan9::common::init("./data", "./abc.zip");

//    plan9::common::init("./data", "../lua");
//    plan9::common::set_notify_function([=](std::string data){
//        std::cout << "notify data " << data << std::endl;
//    });
//    plan9::common::set_macosx_platform();

    std::map<int, int> m;
    m[1] = 10;
    m[2] = 20;
    m[3] = 30;
    auto it = m.begin();
    while (it != m.end()) {
        it = m.erase(it);
//        it ++;
    }

//    JSONObject p;
//    p["file"] = "./1.jpg";
//    p["text"] = "abc12312312312312323123232312312312321312312312312312312312312312312";
//    plan9::common::call_("base64", p, [=](JSONObject result){
//        std::cout << "http result " << result.to_string();
//    });

//    plan9::common::call_("http.test_get", JSONObject(), [=](JSONObject data){
//        std::cout << "http result " << data.to_string();
//    });
//    plan9::common::call_("http.test_download", JSONObject(), [=](JSONObject data){
//        std::cout << "http result " << data.to_string();
//    });
//    plan9::common::call_("http.test_upload", p, [=](JSONObject data){
//        std::cout << "http result " << data.to_string();
//    });

//    JSONObject p;
//    p["url"] = "http://api.guazipai.com/customer/geo/allcity/";
//    p["model"] = "sync";
//    p["test"] = "\\u5f90\\u5dde";
//    plan9::common::call_("http.get", p, [=](JSONObject data){
//        std::cout << "http result " << data.to_string();
//    });

//    JSONObject p1;
//    p1["url"] = "https://qingg.im/download/Qingg-2.3.8.dmg";
//    p1["url"] = "http://download-installer.cdn.mozilla.net/pub/firefox/releases/50.0/mac/en-US/Firefox%2050.0.dmg";
//    p1["url"] = "http://cdn.qq.ime.sogou.com/96d354a9b3028caf9d205de6f7221861/58306f46/QQPinyin_Setup_5.4.3311.400.exe";
//    p1["path"] = "./ff.dmg";
//    plan9::common::call_("http.download", p1, [=](JSONObject data){
//        int a = 0;
//        a ++;
//        std::cout << "callback ok" << std::endl;
//        std::cout << "http download result " << data.to_string();
//    });
//    JSONObject p;
//    p["type"] = "get";
//    p["model"] = "sync";
//    p["url"] = "http://www.baidu.com";
//    plan9::common::call_("http", p, [=](JSONObject data){
//        std::cout << "http result " << data.to_string();
//    });
    /*
    JSONObject p1;
    p1["model"] = "sync";
    p1["url"] = "https://1gaanhct1gczdrctofa4dnmmrgyzgramjct4zya5uf3ts65e.ourdvsss.com/d0.baidupcs.com/file/13804637d71ce50e2da4971095c57df5?bkt=p-c937d18681587c7f958833c541413e28&xcode=30b23d85dc90800fd35e357f7abfe24d290323f44aaa75be0b2977702d3e6764&fid=1828745741-250528-2519836085&time=1479379715&sign=FDTAXGERLBH-DCb740ccc5511e5e8fedcff06b081203-U0TUqsZuRZjbUPMMdEoiPG4m1Hc%3D&to=d7&fm=Nan,B,U,mn&sta_dx=11255825&sta_cs=6686&sta_ft=rar&sta_ct=7&sta_mt=7&fm2=Nanjing,B,U,mn&newver=1&newfm=1&secfm=1&flow_ver=3&pkey=140013804637d71ce50e2da4971095c57df57fe0df4c000000abc011&sl=76480590&expires=8h&rt=sh&r=104272080&mlogid=7463073508543308707&vuk=3810993788&vbdid=918612304&fin=1%281%29.rar&slt=pm&uta=0&rtype=1&iv=0&isw=0&dp-logid=7463073508543308707&dp-callid=0.1.1&hps=1&csl=80&csign=XZEA1C89KqMdZPj88dJ3Wz1qqT8%3D&wshc_tag=0&wsts_tag=582d8b04&wsid_tag=7a0b2553&wsiphost=ipdbm";
    p1["path"] = "./http.jpg";
    plan9::common::call_("download", p1, [=](JSONObject data){
        std::cout << "http result " << data.to_string() << std::endl;
    });
     */

//    JSONObject p2;
//    p2["action"] = "compress";
//    p2["src_path"] = "./test";
//    p2["dest_path"] = "./test.zip";
//    p2["code"] = "1234564";
//    p2["action"] = "uncompress";
//    p2["dest_path"] = "./test233";
//    p2["src_path"] = "./test.zip";
//    plan9::common::call_("zip", p2, [=](JSONObject data){
//        std::cout << "zip result " << data.to_string() << std::endl;
//    });
//    Json::Value param;
//    param["level"] = "info";
//    Json::Value msg;
//    msg["aaa"] = 5;
//    param["msg"] = "xxx : {\"a\":1}";
//    param["target"] = "other";

//    plan9::common::call("log", json_wrap::to_string(param), [=](std::string result){
//        std::cout << "callback common init 1 :" << result << std::endl;
//    });

//    Json::Value p1;
//    p1["aux"]["id"] = "dddxxx";
//    p1["args"] = param;
//    plan9::common::call_("native.get_error_code", Json::Value(), [=](Json::Value result){
//        std::cout << "callback common init 2 :" << result << std::endl;
//    });

//    Json::Value p;
//    p["test"] = "hello world from ios";
//    plan9::common::call_("server.send", p, [=](Json::Value result){
//        Json::Value p2;
//        p2["a"] = "a";
//        plan9::common::call_("server.send", p2, [=](Json::Value result){
//            std::cout << "callback from send :" << result.toStyledString() << std::endl;
//        });
//    });


//    plan9::common::call("server.connect");

//    plan9::common::set_ios_platform();
//    plan9::common::set_android_platform();
//    plan9::common::set_win_platform();
//    plan9::common::set_macosx_platform();
}


#endif