//
// Created by liuke on 15/12/23.
//

#include <test/test_def.h>
#include <database/sqlite_warp.h>

#ifdef DATABASE_TEST

TEST(database_test, create) {
    using namespace plan9;
    sqlite_warp::instance().open_database("./test.db");
    sqlite_warp::instance().exec("create table if not exists person (id integer primary key not null, name varchar(50));");
//    sqlite_warp::instance().exec("insert into person values(1, 'xxxdddd')");
//    sqlite_warp::instance().exec("insert into person(name) values('ddddxxxdddd')");
//    sqlite_warp::instance().exec("insert into person(name) values('hello world')");
//    sqlite_warp::instance().exec("insert into person(name) values('hello world xxxx ddddd000')", [=](std::shared_ptr<std::list<JSONObject>> data){
//        if (data) {
//            for (auto i = data->begin(); i != data->end(); i ++) {
//                JSONObject d = *i;
//                std::cout << d.to_string() << std::endl;
//            }
//        }
//    });
    sqlite_warp::instance().close_database();
}

#endif
