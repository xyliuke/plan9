//
// Created by liuke on 15/12/23.
//

#ifndef COMMON_SQLITE_WARP_H
#define COMMON_SQLITE_WARP_H

#include <string>
#include <functional>
#include <list>
#include <json/JSONObject.h>
#include <tuple>

namespace plan9
{
    typedef std::list<JSONObject> result_set;
    class sqlite_warp {
    public:
        static sqlite_warp instance();
        std::tuple<bool, std::string> open_database(std::string db, int* handle);
        void close_database(int handle);
        std::tuple<bool, std::string> exec(int handle, std::string sql);
        std::tuple<bool, std::string> exec(int handle, std::string sql, std::function<void(std::shared_ptr<result_set>)>);
        std::string version();

    private:
        sqlite_warp();
        class sqlite_warp_impl;
        std::shared_ptr<sqlite_warp_impl> impl_;
    };
}




#endif //COMMON_SQLITE_WARP_H
