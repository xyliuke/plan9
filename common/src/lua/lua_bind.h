//
// Created by liuke on 15/9/21.
//

#ifndef LUA_BIND_LUA_BIND_H
#define LUA_BIND_LUA_BIND_H

#include <iosfwd>
#include <string>
#include <json/json.h>
#include "lua.hpp"

namespace plan9
{
    class lua_bind {
    public:
        static lua_bind instance();
        bool lua_bind_init(std::string lua_file);
        bool call(std::string method, Json::Value param);
        void lua_bind_call(std::string name, std::shared_ptr<Json::Value> param, std::function<void(std::string result)> callback);
        void json2table(std::shared_ptr<Json::Value> doc);

        void lua_callback(lua_State* L);

    private:
        lua_bind();
        class lua_bind_impl;
        std::shared_ptr<lua_bind_impl> impl;
    };
}



#endif //LUA_BIND_LUA_BIND_H
