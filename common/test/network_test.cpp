//
// Created by liuke on 16/1/2.
//

#include <test/test_def.h>
#include <network/tcp.h>


#ifdef NETWORK_TEST

#include "network/tcp.h"
#include <network/tcp_wrap_default.h>
#include <list>
#include <json/json_wrap.h>
#include <network/protocol.h>
#include <network/easy_http.h>
#include <network/asyn_http.h>

TEST(network_test, tcp) {

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

//    std::tuple<char*, int> ret = plan9::protocol::create_protocol(0x12345678, 2, plan9::protocol::CONNECTION_SERVER_TYPE, plan9::protocol::NORMAL_STRING_DATA_TYPE, str.length(), str.c_str());

//    std::tuple<bool, int, char, char, char, int, char*> item = plan9::protocol::get_protocol(std::get<0>(ret), std::get<1>(ret));
//    std::string ret_str(std::get<6>(item), 198);

//    ret = plan9::protocol::create_ping_protocol(0x78563412, 3);
//    item = plan9::protocol::get_protocol(std::get<0>(ret), std::get<1>(ret));


//    char d[] = {'^', 0, 0, 0, 0, 0, 0, 0, 0x05, 'a', 'b', 'c', 'd', 'e', '^', 1, 1, 1, 1, 1, 1, 1, 0x0};
//    int l = plan9::protocol::remove_first_protocol(d, sizeof(d));

//    char* c = "hello world";
//    std::string string(c, 4);
}


TEST(network_test, http_get) {
//    std::shared_ptr<std::map<std::string, std::string>> header(new std::map<std::string, std::string>);
//    header->insert(std::make_pair<std::string, std::string>("sessionId", "5292444c1ccd52f03cfa9065d61a2592"));
//    header->insert(std::make_pair<std::string, std::string>("token", "6d016f1a3ef21d837e2142909cd66011"));
//    plan9::easy_http::instance().get("https://api.guazi-off.com/clientsale/car/expect/?dpi=326&screenWH=750,1334&osv=iOS9.3&model=x86_64&plateform=1&net=wifi&idfa=1D40CC45-A3AD-4C73-9404-FD134B7C912D&clue_id=2288250&deviceId=2D595ADE-9E20-458B-B6EB-ABB8E1D3BE37&versionId=2.3.0&customerId=780&sign=178e39aee69424509af54ecbace64495",
//                                header.get(),
//                                [=](std::string header, char* data, size_t len){
//        std::string str(data, len);
//        std::cout << "data : " << str << std::endl;
//    });
//    plan9::asyn_http::instance().get("http://www.baidu.com", nullptr);
//    plan9::asyn_http::instance().get("http://www.163.com", nullptr);

    plan9::asyn_http::instance().get("http://www.boost.org", 5, [=](int curl_code, std::string debug_trace, long http_state, char* data, size_t len){
        std::cout << "header : " << debug_trace << std::endl;
        if (data != NULL) {
            std::string str(data, len);
            std::cout << "data : " << str << std::endl;
        }
    });
//    plan9::asyn_http::instance().get("http://www.163.com", [=](int curl_code, std::string debug_trace, long http_state, char* data, size_t len){
//        std::cout << "header : " << debug_trace << std::endl;
//        if (data != NULL) {
//            std::string str(data, len);
//            std::cout << "data : " << str << std::endl;
//        }
//    });
//    plan9::asyn_http::instance().get("http://www.ifeng.com", [=](int curl_code, std::string debug_trace, long http_state, char* data, size_t len){
//        std::cout << "header : " << debug_trace << std::endl;
//        if (data != NULL) {
//            std::string str(data, len);
//            std::cout << "data : " << str << std::endl;
//        }
//    });

//    plan9::asyn_http::instance().download("http://wiki.guazi-corp.com/images/d/d3/CocoaPods做iOS程序依赖管理.pdf", "asyn_http.pdf", [=](int curl_code, std::string debug_trace, long http_state){
//        std::cout << curl_code << debug_trace << http_state << std::endl;
//    }, [=](double time, long download, long total){
//        std::cout << "download progress : time  " << time << "\t" << download << "/" << total << std::endl;
//    });


//    asyn_http2* http2 = new asyn_http2;
//    http2->get("http://www.baidu.com");

//    asyn_http3* http3 = new asyn_http3;
//    http3->get();


//    plan9::easy_http::instance().download("http://wiki.guazi-corp.com/images/d/d3/CocoaPods做iOS程序依赖管理.pdf", "a.pdf", [=](int curl_code, std::string debug_trace, long http_state){
//        std::cout << curl_code << debug_trace << http_state << std::endl;
//    }, [=](double time, long download, long total){
//        std::cout << "download progress : time  " << time << "\t" << download << "/" << total << std::endl;
//    });


//    std::shared_ptr<std::map<std::string, std::string>> param(new std::map<std::string, std::string>);
//    param->insert(std::make_pair<std::string, std::string>("clue_id", "2288250"));
//    param->insert(std::make_pair<std::string, std::string>("expect_address", "八维"));
//    param->insert(std::make_pair<std::string, std::string>("lat", "40.04925149185874"));
//    param->insert(std::make_pair<std::string, std::string>("lng", "116.306396493029"));
//    param->insert(std::make_pair<std::string, std::string>("remark", "123445"));
//    param->insert(std::make_pair<std::string, std::string>("timelist", "[{\"day_type\" : 2, \"time_type\" : 3},{\"day_type\" : 3,\"time_type\" : 1}]"));
//    param->insert(std::make_pair<std::string, std::string>("timelist[][day_type]", "2"));
//    param->insert(std::make_pair<std::string, std::string>("timelist[][time_type]", "3"));
//    param->insert(std::make_pair<std::string, std::string>("timelist[][day_type]", "3"));
//    param->insert(std::make_pair<std::string, std::string>("timelist[][time_type]", "3"));
//    plan9::easy_http::instance().post("https://api.guazi-off.com/clientsale/car/expect/?dpi=326&screenWH=750,1334&osv=iOS9.3&model=x86_64&plateform=1&net=wifi&idfa=531E7C32-985A-49D2-A36D-CDB5463A728D&customerId=780&versionId=2.3.0&deviceId=2D595ADE-9E20-458B-B6EB-ABB8E1D3BE37&sign=9068943b0636dda0a5fdbe1946a9b82d",
//                                 header.get(), param.get(), nullptr);

//    std::string path = "/Volumes/Developer/liuke/20164421211445459480.jpg";
//    std::string upload_url = "http://storage.guazi.com/sign.php?dpi=326&screenWH=750,1334&osv=iOS9.3&model=x86_64&plateform=1&net=wifi&idfa=63D6A411-8D86-408F-807D-CB4BB7A9132C&sign=bfeff6a1835aa1351f8662d2d1f881ff";
//    std::map<std::string, std::string> param;
//    param["file_list[0]"] = path;
//    param["type"] = "img";
//    plan9::easy_http::instance().post(upload_url, nullptr, &param, [=](int curl_code, std::string debug_trace, long http_state, char *data, size_t len){
//        std::cout << "debug : " << debug_trace << std::endl;
//        std::cout << "return : " << std::string(data, len) << std::endl;
//    });

//    plan9::easy_http::instance().upload(upload_url, "/Volumes/Developer/liuke/20164421211445459480.jpg", nullptr, &param, nullptr, nullptr);

}

#endif