#include <iostream>
#include <thread>

#include "network/tcp_client.h"
#include "test/test.h"
#include "log/log.h"
#include "thread/thread_dispatch.h"
#include "thread/thread_wrap.h"
#include <json/json.h>
#include <uv.h>
#include "commander/cmd_factory.h"
#include "log/log_wrap.h"
#import "network/uv_wrap.h"

using namespace std;


int main(int argc, char* argv[]) {
#ifdef GOOGLE_TEST_ENABLE
    testing::InitGoogleTest(&argc, argv);
#endif

    using namespace plan9;
    plan9::log::init("./");

    string input;
    while (true) {
        cin >> input;
        if (input == "quit") {
            break;
        }
    }


//#ifdef GOOGLE_TEST_ENABLE
//    return RUN_ALL_TESTS();
//#elif
//    return 0;
//#endif
}