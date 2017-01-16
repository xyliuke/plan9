//
// Created by liuke on 15/12/23.
//

#ifndef COMMON_SQLITE_WARP_H
#define COMMON_SQLITE_WARP_H

#include <string>
#include <functional>
#include <list>
#include <json/JSONObject.h>

namespace plan9
{
    typedef std::list<JSONObject> result_set;
    class sqlite_warp {
    public:
        static sqlite_warp instance();
        bool open_database(std::string db);
        void close_database();
        bool exec(std::string sql);
        bool exec(std::string sql, std::function<void(std::shared_ptr<result_set>)>);


    private:
        sqlite_warp();
        class sqlite_warp_impl;
        std::shared_ptr<sqlite_warp_impl> impl_;
    };
}




#endif //COMMON_SQLITE_WARP_H
