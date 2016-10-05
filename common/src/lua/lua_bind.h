//
// Created by liuke on 15/9/21.
//

#ifndef LUA_BIND_LUA_BIND_H
#define LUA_BIND_LUA_BIND_H

#include <string>
#include <json/json.h>
#include <functional>
#include <memory>
#include <json/JSONObject.h>
#include "lua.hpp"

namespace plan9
{
    class lua_bind {
    public:
        static lua_bind instance();
        /**
         * 初始化lua文件,指定lua文件根路径
         */
        bool lua_bind_init(std::string lua_path);

        bool lua_bind_loadfile(std::string lua_file);
        /**
         * 调用lua函数,支持调用table中的函数,规则为a.b
         * @param method 函数名,支持a.b调用
         * @param param lua参数的支持,为Json对象
         * @return 如果函数存在,则返回true,否则返回false
         */
        bool call_lua(std::string method, Json::Value param);
        bool call_lua(std::string method, JSONObject param);

        /**
         * 调用lua函数,支持调用table中的函数,规则为a.b
         * @param method 函数名,支持a.b调用
         * @param param lua参数的支持,为Json对象
         * @param callback 调用Lua后的回调
         * @return 如果函数存在,则返回true,否则返回false
         */
        bool call_lua(std::string method, Json::Value param, std::function<void(Json::Value result)> callback);
        bool call_lua(std::string method, JSONObject param, std::function<void(JSONObject result)> callback);

        /**
         * 调用lua函数,调用参数格式按照cmd_facotry格式
         * @param method 函数名,支持a.b调用方式
         * @param param  参数 lua参数的支持,为Json对象
         */
        bool call(std::string method, Json::Value param);
        bool call(std::string method, JSONObject param);

        /**
         * 调用lua函数,调用参数格式按照cmd_facotry格式
         * @param method 函数名,支持a.b调用方式
         * @param param  参数 lua参数的支持,为Json对象
         * @param callback 回调函数
         */
        bool call(std::string method, Json::Value param, std::function<void(Json::Value result)> callback);
        bool call(std::string method, JSONObject param, std::function<void(JSONObject result)> callback);

        /**
         * 函数功能同上,只是不需要传递参数
         */
        bool call(std::string method, std::function<void(Json::Value result)> callback);
        bool call(std::string method, std::function<void(JSONObject result)> callback);

        /**
         * 函数同上,只是不需要参数和回调
         */
        bool call(std::string method);

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
        JSONObject wrap(std::string method, JSONObject json);
    };
}



#endif //LUA_BIND_LUA_BIND_H
