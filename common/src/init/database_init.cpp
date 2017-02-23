//
// Created by liuke on 16/01/2017.
//

//#include <error/error_num.h>
#include "database_init.h"
#include "log/log_wrap.h"
#include "database/sqlite_warp.h"
#include "commander/cmd_factory.h"
#include <tuple>
#include <error/error_num.h>


namespace plan9
{
    void database_init::init() {
        log_wrap::io().i("register database plugin, sqlite version : ", sqlite_warp::instance().version());

        /**
         *  打开数据库文件，如果没有该文件，则创建一个
         *  file    文件路径
         */
        cmd_factory::instance().register_cmd("open_database", [=](JSONObject param){
            if (param.has("args") && param.has("args.file")) {
                std::string file = param["args.file"].get_string();
                int handle;
                std::tuple<bool, std::string> ret = sqlite_warp::instance().open_database(file, &handle);
                if (std::get<0>(ret)) {
                    JSONObject result;
                    result["handle"] = handle;
                    cmd_factory::instance().callback(param, true, result);
                } else {
                    cmd_factory::instance().callback(param, ERROR_OPERATION, std::get<1>(ret));
                }
            } else {
                cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), "refer to {file : \"database file\"}"));
            }
        });

        /**
         *  执行sql语句
         *  handle  数据库的句柄，通过open_database获取
         *  sql    执行的语句
         */
        cmd_factory::instance().register_cmd("sql_exec", [=](JSONObject param){
            if (param.has("args.handle")) {
                JSONObject args = param["args"];
                int handle = args["handle"].get_int();
                std::string sql = args["sql"].get_string();
                std::shared_ptr<std::list<JSONObject>> data;
                auto ret = sqlite_warp::instance().exec(handle, sql, [data](std::shared_ptr<std::list<JSONObject>> result) mutable {
                    data = result;
                });
                if (std::get<0>(ret)) {
                    JSONObject cb;
                    JSONObject result;
                    std::list<JSONObject>::iterator it = data->begin();
                    while (it != data->end()) {
                        result.append(*it);
                    }
                    cb["sql_data"] = result;
                    cmd_factory::instance().callback(param, true, cb);
                } else {
                    cmd_factory::instance().callback(param, ERROR_OPERATION, std::get<1>(ret));
                }
            } else {
                cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), "refer to {handle : \"database handle\", sql : \"sql string\"}"));
            }
        });

        cmd_factory::instance().register_cmd("close_database", [=](JSONObject param) {
            if (param.has("args.handle")) {
                int handle = param["args.handle"].get_int();
                sqlite_warp::instance().close_database(handle);
                cmd_factory::instance().callback(param, true, JSONObject());
            } else {
                cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), "refer to {handle : \"database handle\"}"));
            }
        });
    }
}
