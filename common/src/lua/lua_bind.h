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
        /**
         * 初始化lua文件,指定文件路径
         */
        bool lua_bind_init(std::string lua_file);
        /**
         * 调用lua函数,支持调用table中的函数,规则为a.b
         * @param method 函数名,支持a.b调用
         * @param param lua参数的支持,为Json对象
         * @return 如果函数存在,则返回true,否则返回false
         */
        bool call_lua(std::string method, Json::Value param);

        /**
         *
         */
        bool call(std::string method, Json::Value param);
        /**
         * 调用lua函数,支持调用table中的函数,规则为a.b
         * @param method 函数名,支持a.b调用
         * @param param lua参数的支持,为Json对象
         * @param callback 调用Lua后的回调
         * @return 如果函数存在,则返回true,否则返回false
         */
        bool call_lua(std::string method, Json::Value param, std::function<void(Json::Value result)> callback);

        bool call(std::string method, Json::Value param, std::function<void(Json::Value result)> callback);


        /**
         * 这个函数用来处理lua的回调,供在lua中注册的函数调用.
         */
        void lua_callback(lua_State* L);

        /**
         * 这个函数供lua调用,调用C++中函数
         */
        void native_call(lua_State* L);



    private:
        lua_bind();
        class lua_bind_impl;
        std::shared_ptr<lua_bind_impl> impl;
    };
}



#endif //LUA_BIND_LUA_BIND_H
