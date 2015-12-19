//
// Created by liuke on 15/12/13.
//

#include <test/test_def.h>

#ifdef LOG_TEST


#include <log/log.h>
#include <log/log_wrap.h>


using namespace plan9;

TEST(log_test, wrap_io) {

    log_wrap::set_log_dir("./log");
    char* buf = "dsdfsdfsdfsdfsdf";
    log_wrap::io().i("hello", 1, 1.2, "1.3", true, buf);
    log_wrap::io().set_level(log_wrap::WARN);
    log_wrap::io().i("my io log wrietetewdd 1");
    log_wrap::io().w("my io log wrietetewdd 2");
    log_wrap::io().e("my io log wrietetewdd 3");

    log_wrap::net().i("my net log wrietetewdd 1");
    log_wrap::net().w("my net log wrietetewdd 2");
    log_wrap::net().e("my net log wrietetewdd 3");

    log_wrap::ui().set_level(log_wrap::ERROR);
    log_wrap::ui().i("my ui wrietetewdd 1");
    log_wrap::ui().w("my ui wrietetewdd 2");
    log_wrap::ui().e("my ui wrietetewdd 3");

    log_wrap::other().i("my other log wrietetewdd 1");
    log_wrap::other().w("my other log wrietetewdd 2");
    log_wrap::other().e("my other log wrietetewdd 3");

    log_wrap::io().set_duration(7);
}


#endif