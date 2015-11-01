#include <iostream>
#include <thread>

#include "network/tcp_client.h"
#include "test/test.h"
#include "log/log.h"
#include "thread/thread_dispatch.h"
#include "thread/thread_wrap.h"


using namespace std;

void t_test(){
    log::logI("t_test");
}

int main(int argc, char* argv[]) {
#ifdef GOOGLE_TEST_ENABLE
    testing::InitGoogleTest(&argc, argv);
#endif

    log::init("./");

    plan9::thread_wrap::post_background([](){
        log::logI("hello world11111");
    });

    plan9::thread_wrap::post_background([](){
        log::logI("hello world22222");
    });

    plan9::thread_wrap::post_background([](){
        log::logI("hello world33333");
    });

    plan9::thread_wrap::post_background([](){
        log::logI("hello world44444");
    });

    plan9::thread_wrap::post_network([](){
        log::logI("hello world555555");
    });

    plan9::thread_wrap::post_network([](){
        log::logI("hello world6666666");
    });

//    plan9::thread_dispatch thread_dispatch;
//    plan9::thread_dispatch::create(1);
//    plan9::thread_dispatch::post(1, [](){
//        log::logI("hello world11111");
//    });
//
//    plan9::thread_dispatch::post(1, [](){
//        log::logI("hello world22222");
//    });
//
//    plan9::thread_dispatch::post(1, [](){
//        log::logI("hello world22222");
//    });
//
//    sleep(5);
//
//    plan9::thread_dispatch::post(1, [](){
//        log::logI("hello world22222");
//    });
//
//
//    plan9::thread_dispatch::post(1, [](){
//        log::logI("hello world222200002");
//    });
//
//    plan9::thread_dispatch::create(2);
//    plan9::thread_dispatch::post(2, [](){
//        log::logI("2hello world1111122222222");
//    });

//    plan9::thread_dispatch::post(2, [](){
//        log::logI("hello world22222222222222");
//    });
//    plan9::thread_dispatch::run(1);

//    thread_dispatch.post(1, [](){
//        log::logI("hello world33333");
//    });

//    thread_dispatch.post(1, [](){
//        log::logI("hello world44444");
//    });

//    sleep(4);

    while (true) {
        int out;
        std::cin >> out;
        if (out == 0) {
            break;
        }
    }

//    plan9::thread_dispatch::stop();


//    std::map<int, std::vector<std::function<void(void)>>> map;
//    {
//        std::vector<std::function<void(void)>> v;
//        v.push_back([](){
//            log::logI("hello world11111");
//        });
//
//        v.push_back([](){
//            log::logI("hello world22222");
//        });
//
//        v.push_back([](){
//            log::logI("hello world33333");
//        });
//
//        map.insert(std::pair<int,std::vector<std::function<void(void)>>>(1, v));
//    }
//
//    {
//        std::vector<std::function<void(void)>> v = map[1];
//        v.push_back([](){
//            log::logI("hello world11111");
//        });
//
//        v.push_back([](){
//            log::logI("hello world22222");
//        });
//
//        v.push_back([](){
//            log::logI("hello world33333");
//        });
//
////        map.insert(std::pair<int,std::vector<std::function<void(void)>>>(1, v));
//    }



//    std::vector<int> v;
//    v.push_back(1);
//    v.push_back(2);
//    v.push_back(3);
//    v.erase(v.begin());
//    for (int i = 0; i < 2; i ++) {
//        string aaa = "4444444444444444";
//        string bbb = "5555555555555555";
//        auto ttt = [=]() {
//            while (true) {
//                if (i == 0) {
//                    log::logI("t_test 111" + aaa);
//                } else {
//                    log::logI("t_test 111" + bbb);
//                }
//                sleep(2);
//            }
//        };
//        std::thread t1(ttt);
////        t1.join();
//        t1.detach();
//    }
//    log::logI("hello world11111");
//    sleep(1);
//    log::logI("hello world");
//    log::logW("www");
//    log::logE("errrrrrr");
//    log::logF("fffffff");

//    google::InitGoogleLogging(argv[0]);
//    google::InstallFailureSignalHandler();
//    LOG(INFO) << "info: hello world!";
//    LOG(WARNING) << "warning: hello world!";
//    LOG(ERROR) << "error: hello world!";
//    VLOG(0) << "vlog0: hello world!";
//    VLOG(1) << "vlog1: hello world!";
//    VLOG(2) << "vlog2: hello world!";
//    VLOG(3) << "vlog3: hello world!";
//    DLOG(INFO) << "DLOG: hello world!";

//    tcp_client tcpClient;
//    tcpClient.connect("127.0.0.1", 8888);
//    std::string msg;
//    while (true) {
//        std::cin >> msg;
////        std::cout << msg;
//        if ("quit" == msg)
//            break;
//        tcpClient.write(msg);
//    }
//    std::shared_ptr<std::set<std::string>> ips = tcpClient.getipbyname("www.163.com");
//    set<std::string>::iterator it; //定义前向迭代器
//    中序遍历集合中的所有元素
//    for(it = ips->begin(); it != ips->end(); it++)
//    {
//        cout << *it << " ";
//    }
#ifdef GOOGLE_TEST_ENABLE
    return RUN_ALL_TESTS();
#elif
    return 0;
#endif
}