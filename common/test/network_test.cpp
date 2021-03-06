//
// Created by liuke on 16/1/2.
//

#include <test/test_def.h>
//#include <network/tcp.h>
//#include <network/http.h>

#ifdef NETWORK_TEST

//#include "network/tcp.h"
//#include <network/tcp_wrap_default.h>
//#include <list>
//#include <json/json_wrap.h>
//#include <network/protocol.h>
//#include <fstream>
#include <network/async_http.h>
#include <network/easy_http.h>
#include <json/JSONObject.h>
#include <network/async_uv_http.h>

TEST(network_test, tcp) {
    using namespace plan9;
//    tcp t;
//    t.connect_url("www.baidu.com", 443);

//    plan9::tcp_wrap_default::instance().set_connect_handler([=](bool connect){
//        std::cout << "connect : " << connect << std::endl;
//        if (connect) {
////            plan9::tcp_wrap_default::instance().send(plan9::network_server_type::SERVER_DATABASE ,"hello wrold from clion");
//            Json::Value msg;
//            msg["aux"]["id"] = "12121323213";
//            msg["aux"]["to"] = "call_sss";
//            plan9::tcp_wrap_default::instance().send(plan9::network_server_type::SERVER_DATABASE, msg, [=](Json::Value result){
//                std::string result_str = plan9::json_wrap::to_string(result);
//                std::cout << result_str << std::endl;
//            }, 10000);
//        }
//    });
//
//    plan9::tcp_wrap_default::instance().set_read_handler([=](std::string msg){
//        std::cout << "recv : " << msg << std::endl;
//    });
//    plan9::tcp_wrap_default::instance().connect("127.0.0.1", 8081);
//    std::cout << "finish";

//    plan9::network_server_type type = plan9::network_server_type::SERVER_DATABASE;
//    int itype = static_cast<int>(type);
//    plan9::network_server_type tt = static_cast<plan9::network_server_type>(0x21);
}

TEST(network_test, protocol) {
//    std::cout << "test protocol" << std::endl;
//    std::string str = "{\"args\":{\"server\":0,\"test_data\":\"hello world from ios\",\"timeout\":50000},\"aux\":{\"action\":\"callback\",\"from\":[\"ID-IOS-1458880327450927-5906-BF\"],\"id\":\"ID-IOS-1458880327450927-5906-BF\",\"to\":\"function\"}}";
//
//    std::tuple<char*, int> ret = plan9::protocol::create_protocol(0x12345678, 2, plan9::protocol::CONNECTION_SERVER_TYPE, plan9::protocol::NORMAL_STRING_DATA_TYPE, str.length(), str.c_str());
//
//    std::tuple<bool, int, char, char, char, int, char*> item = plan9::protocol::get_protocol(std::get<0>(ret), std::get<1>(ret));
//    std::string ret_str(std::get<6>(item), 198);

//    ret = plan9::protocol::create_ping_protocol(0x78563412, 3);
//    item = plan9::protocol::get_protocol(std::get<0>(ret), std::get<1>(ret));


//    char d[] = {'^', 0, 0, 0, 0, 0, 0, 0, 0x05, 'a', 'b', 'c', 'd', 'e', '^', 1, 1, 1, 1, 1, 1, 1, 0x0};
//    int l = plan9::protocol::remove_first_protocol(d, sizeof(d));

//    char* c = "hello world";
//    std::string string(c, 4);
}

TEST(network_test, http) {
//    plan9::http http;
//    std::string url = "http://image.baidu.com/search/down?tn=download&word=download&ie=utf8&fr=detail&url=http%3A%2F%2Fb.hiphotos.baidu.com%2Fzhidao%2Fwh%253D600%252C800%2Fsign%3Dabb1ea66bb12c8fcb4a6fecbcc33be7d%2F4ec2d5628535e5dd3e2b3d9974c6a7efce1b6275.jpg&thumburl=http%3A%2F%2Fimg3.imgtn.bdimg.com%2Fit%2Fu%3D2438896525%2C1071232700%26fm%3D21%26gp%3D0.jpg";
//    http.get(url, [=](char* data, size_t len){
//        std::ofstream ostream;
//        ostream.open("./a.jpg", std::ios::trunc);
//        ostream.write(data, len);
//        ostream.close();
//        std::string ret(data, len);
//        std::cout << "download begin" << std::endl;
//        std::cout << ret << std::endl;
//        std::cout << "download end" << std::endl;
//    });

//    http.get(url, "./a/b.jpg", [=](long now, long total){
//        std::cout << "now download " << now << " , total " << total << std::endl;
//    });
//    http.get_string("https://www.microsoft.com/zh-cn", [=](std::string ret){
//        std::cout << ret << std::endl;
//    });

//    std::string data = "<dsdfslfsl\"";
//    std::string str = "<dsdfslfsl\"fdsfsf<html></html>sdf\"";
//    plan9::JSONObject json;
//    json["data"] = str;
//    std::string j_str = json.to_string();
//    std::string str2 = json["data"].get_string();
//
//    plan9::JSONObject j2(j_str);
//    std::string d = j2["data"].get_string();
//    std::cout << j2.to_format_string() << std::endl;
//    std::cout << d << std::endl;

//    plan9::easy_http::instance().get("http://www.baidu.com", [=](int curl_code, std::string debug_trace, long http_state, char* data, size_t len){
//        std::string str(data, len);
//        plan9::JSONObject json;
//        json["data"] = str;
//        std::string j_str = json.to_string();
////
//        plan9::JSONObject j2(j_str);
//        std::string d = j2["data"].get_string();
//        std::cout << j2.to_format_string() << std::endl;
//        std::cout << d << std::endl;
////
//    });


//    plan9::easy_http::instance().download("http://dlsw.baidu.com/sw-search-sp/soft/4c/27608/wallpaper_2.0.2.35_setup.1446003787.exe"
//    ,"./test.exe", nullptr, [=](int curl_code, std::string debug_trace, long http_state){
//                std::cout << "result ok" << std::endl;
//            }, [=](double time, long downloaded, long total){
//                std::cout << "process " << time << "," << downloaded <<"," << total << std::endl;
//    });

//    std::shared_ptr<std::map<std::string, std::string>> form(new std::map<std::string, std::string>);
//    (*form)["token"] = "VykDspRznX2ik0NPRA__6GUWfktpGsIinDOoGHaZ:K9LBPvGXtnyKYq69mhTEkcNJDMU=:eyJzY29wZSI6Imd1YXppMDAxOnFuMTYxMTI5MTg1MTExNDFkNDljMjhmMzVhMjc1OWMzYzA2NjZmZGM5YjcwYjYuanBnIiwiZGVhZGxpbmUiOjE0ODA0MjAyNzF9";
//    (*form)["key"] = "qn16112918511141d49c28f35a2759c3c0666fdc9b70b6.jpg";
//    plan9::easy_http::instance().upload("http://up.qiniu.com", "./1.jpg", "file", nullptr, form, [=](int curl_code, std::string debug_trace, long http_state, char* data, size_t data_len){
//        std::cout << "upload result " << std::string(data, data_len) << std::endl;
//    }, [=](double time, long uploaded, long total){
//        std::cout << "upload process " << time << " \t " << uploaded << "/" << total << std::endl;
//    });
}


TEST(network_test, uv_http) {
//    uv_default_loop();
//    plan9::async_uv_http::instance().get("https://api.guazipai.com", 0, nullptr);
//    plan9::async_uv_http::instance().get("http://www.baidu.com", 0, nullptr);
//    plan9::async_uv_http::instance().get("https://www.baidu.com", 0, nullptr, [=](int curl_code, std::string debug_trace, long http_state, char* data, size_t len){
//        std::string str(data, len);
//        std::cout << str;
//    });
//    plan9::async_uv_http::instance().get("http://api.guazipai.com", 0, nullptr);
}

#endif