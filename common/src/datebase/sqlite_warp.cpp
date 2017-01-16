//
// Created by liuke on 15/12/23.
//

#include "sqlite_warp.h"
#include <sqlite3.h>
#include <iostream>
#import "log/log_wrap.h"
namespace plan9
{
    static std::map<int, std::shared_ptr<result_set>> tmp_;

    static int exec_callback(void *data, int argc, char **argv, char **azColName) {
        int index = *((int*)data);
        std::shared_ptr<result_set> result = tmp_[index];
        if (result != nullptr) {
            JSONObject json;
            for (int i = 0; i < argc; ++i) {
                std::string k(azColName[i]);
                std::string v(argv[i]);
                json[k] = v;
            }
            result->push_back(json);
        }
        return 0;
    }

    class sqlite_warp::sqlite_warp_impl {

    public:


        bool open_database(std::string file) {
            int rc = sqlite3_open(file.c_str(), &sql);
            if (rc != SQLITE_OK) {
                log_wrap::io().e("open database  ", file, " error, the reason : ", sqlite3_errmsg(sql));
                return false;
            }
            return true;
        }

        bool exec(std::string sql, std::function<void(std::shared_ptr<result_set>)> callback) {
            if (this->sql != nullptr) {
                static int count = 0;

                count ++;
                std::shared_ptr<std::list<JSONObject>> list = nullptr;
                int v = -1;
                if (callback != nullptr) {
                    list.reset(new std::list<JSONObject>);
                    tmp_[count] = list;
                    v = count;
                }
                char *zErrMsg = 0;
                if (sqlite3_exec(this->sql, sql.c_str(), exec_callback, &v, &zErrMsg) != SQLITE_OK) {
                    log_wrap::io().e("exec sql error, the reason : ", zErrMsg);
                    sqlite3_free(zErrMsg);
                    if (list != nullptr) {
                        tmp_.erase(count);
                    }
                    return false;
                }
                if (list != nullptr) {
                    callback(list);
                    tmp_.erase(count);
                }
            } else {
                log_wrap::io().e("exec sql error, the database is not open");
                return false;
            }
            return true;
        }

        void close() {
            if (sql != nullptr) {
                sqlite3_close(sql);
            }
        }

    private:
        sqlite3* sql;

    };


    sqlite_warp sqlite_warp::instance() {
        static sqlite_warp sw;
        return sw;
    }

    sqlite_warp::sqlite_warp() : impl_(new sqlite_warp_impl) {

    }

    bool sqlite_warp::open_database(std::string db) {
        return impl_->open_database(db);
    }

    void sqlite_warp::close_database() {
        impl_->close();
    }

    bool sqlite_warp::exec(std::string sql) {
        return impl_->exec(sql, nullptr);
    }

    bool sqlite_warp::exec(std::string sql, std::function<void(std::shared_ptr<result_set>)> callback) {
        return impl_->exec(sql, callback);
    }
}
