//
// Created by liuke on 15/12/23.
//

#include "sqlite_warp.h"
#include <sqlite3.h>
#include <iostream>
#include "log/log_wrap.h"
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


        std::tuple<bool, std::string> open_database(std::string file, int* handle) {
            if (database_file_map.find(file) != database_file_map.end()) {
                *handle = database_file_map[file];
                return std::make_tuple(true, "");
            }
            static int h = 1;
            sqlite3* sql;
            int rc = sqlite3_open(file.c_str(), &sql);
            if (rc != SQLITE_OK) {
                return std::make_tuple(false, sqlite3_errmsg(sql));
            }
            *handle = h;
            database_file_map[file] = *handle;
            database_map[*handle] = sql;
            h ++;
            return std::make_tuple(true, "");
        }

        std::tuple<bool, std::string> exec(int handle, std::string sql, std::function<void(std::shared_ptr<result_set>)> callback) {
            if (database_map.find(handle) == database_map.end()) {
                return std::make_tuple(false, util::instance().cat("can not find handle ", handle));
            }
            sqlite3* sqlite = database_map[handle];
            if (sqlite != nullptr) {
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
                if (sqlite3_exec(sqlite, sql.c_str(), exec_callback, &v, &zErrMsg) != SQLITE_OK) {
                    std::string reason = std::string(zErrMsg);
                    sqlite3_free(zErrMsg);
                    if (list != nullptr) {
                        tmp_.erase(count);
                    }
                    return std::make_tuple(false, reason);
                }
                if (list != nullptr) {
                    callback(list);
                    tmp_.erase(count);
                }
            } else {
                return std::make_tuple(false, "exec sql error, the database is not open");
            }
            return std::make_tuple(true, "");
        }

        void close(int handle) {
            if (database_map.find(handle) != database_map.end()) {
                sqlite3* s = database_map[handle];
                if (s != nullptr) {
                    sqlite3_close(s);
                }
                database_map.erase(handle);
                std::map<std::string, int>::iterator it = database_file_map.begin();
                while (it != database_file_map.end()) {
                    if (it->second == handle) {
                        break;
                    }
                    it ++;
                }
                database_file_map.erase(it);
            }
        }

        std::string version(){
            return std::string(sqlite3_version);
        }

    private:
        std::map<int, sqlite3*> database_map;
        std::map<std::string, int> database_file_map;
    };


    sqlite_warp sqlite_warp::instance() {
        static sqlite_warp sw;
        return sw;
    }

    sqlite_warp::sqlite_warp() : impl_(new sqlite_warp_impl) {

    }

    std::tuple<bool, std::string> sqlite_warp::open_database(std::string db, int *handle) {
        return impl_->open_database(db, handle);
    }

    void sqlite_warp::close_database(int handle) {
        impl_->close(handle);
    }

    std::tuple<bool, std::string> sqlite_warp::exec(int handle, std::string sql) {
        return impl_->exec(handle, sql, nullptr);
    }

    std::tuple<bool, std::string> sqlite_warp::exec(int handle, std::string sql, std::function<void(std::shared_ptr<result_set>)> callback) {
        return impl_->exec(handle, sql, callback);
    }

    std::string sqlite_warp::version() {
        return impl_->version();
    }
}
