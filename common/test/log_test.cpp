//
// Created by liuke on 15/12/13.
//

#include <log/log.h>
#include <test/test_def.h>
#include <log/log_wrap.h>


using namespace plan9;

TEST(log_test, set_file) {
    log log1;
//    log1.setFile("./log/app/log.txt");
//    log1.write("fsfasfd\n");
//    log1.write("heeeeeeeeeeeeeeeee");
//    log1.close();
}


TEST(log_test, wrap_io) {
    log_wrap::io();
}
