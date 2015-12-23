//
// Created by liuke on 15/9/21.
//

#include "lua_bind.h"

#include <json/json_wrap.h>

#include <iostream>
#include <util/UUID.h>
#include <commander/cmd_factory.h>
#include <util/UUID.h>
#include <log/log_wrap.h>



namespace plan9 {

    static lua_State *L;

    /**
     * 由lua来调用的callback
     */
    static int callback(lua_State *L) {
        lua_bind::instance().lua_callback(L);
        return 0;
    }

    /**
     * lua调用C++的函数
     */
    static int nativecall(lua_State *L) {
        lua_bind::instance().native_call(L);
        return 0;
    }

    static int getid(lua_State* L) {
        lua_pushstring(L, UUID::id().c_str());
        return 1;
    }

    class lua_bind::lua_bind_impl {
    public:
        lua_bind_impl() : is_load_(false) {

        }

        bool lua_init(std::string lua_path) {
            if (is_load_)
                return true;
            L = luaL_newstate();
            luaL_openlibs(L);

            lua_getglobal(L, "package");
            lua_getfield(L, -1, "path"); // get field "path" from table at top of stack (-1)
            std::string cur_path = lua_tostring( L, -1 ); // grab path string from top of stack
            cur_path.append( ";" ); // do your path magic here
            cur_path.append(lua_path);
            cur_path.append("/?.lua");
            lua_pop( L, 1 ); // get rid of the string on the stack we just pushed on line 5
            lua_pushstring( L, cur_path.c_str()); // push the new one
            lua_setfield( L, -2, "path" ); // set the field "path" in table at -2 with value at top of stack
            lua_pop( L, 1 ); // get rid of package table from top of stack

            register_callback();
            is_load_ = true;
            return true;
        }

        bool lua_loadfile(std::string lua_file) {
            if (!is_load_) {
                log_wrap::lua().e("lua is not init");
                return false;
            }
            if (luaL_loadfile(L, lua_file.c_str()) || lua_pcall(L, 0, 0, 0)) {
                return false;
            }
            return false;

        }

        void register_callback() {
            lua_register(L, "__callback__", callback);
            lua_register(L, "__native_call__", nativecall);//注意:这里的第三个参数使用native_call名字的函数则编译错误
            lua_register(L, "__native_get_id__", getid);
        }

        void lua_callback(lua_State* L) {
            Json::Value result = table2json(L);
            std::string id = result["aux"]["id"].asString();
            auto callback = callback_map[id];
            if (callback != nullptr) {
                callback(result);
                callback_map.erase(id);
            }
        }

        void native_call(lua_State* L) {
            int args = lua_gettop(L);
            //lua的参数顺序和lua_state中顺序相反
            if (args == 4) {
                //有单独函数名的方式
                bool is_callback = false;
                if (lua_type(L, -1) == LUA_TBOOLEAN) {
                    is_callback = (bool)lua_toboolean(L, -1);
                    lua_pop(L, 1);
                }
                Json::Value param;
                if (lua_type(L, -1) == LUA_TTABLE) {
                    param = table2json(L);
                }
                std::string method;
                if (lua_type(L, -2) == LUA_TSTRING) {
                    method = lua_tostring(L, -2);
                }
                std::string callback_id;
                if (lua_isstring(L, -3)) {
                    callback_id = lua_tostring(L, -3);
                }
                if (is_callback) {
                    cmd_factory::instance().execute(method, param, [=](Json::Value result){
                        result["aux"]["callback_id"] = callback_id;
                        call("lua_c_bridge.callback_from_c", result, nullptr);
                    });
                } else {
                    cmd_factory::instance().execute(method, param);
                }
            }
        }

        static std::shared_ptr<std::vector<std::string>> get_call_level(std::string name) {
            std::shared_ptr<std::vector<std::string>> ret(new std::vector<std::string>());
            unsigned long pos = name.find(".");
            if (pos == std::string::npos) {
                ret->push_back(name);
            } else {
                std::string tmp = name.substr(0, pos);
                ret->push_back(tmp);

                unsigned long index = pos;
                pos = name.find(".", pos + 1);
                while (pos != std::string::npos) {
                    std::string tmp = name.substr(index + 1, pos - index - 1);
                    ret->push_back(tmp);
                    index = pos;

                    pos = name.find(".", pos + 1);
                }

                if (index < name.size()) {
                    std::string tmp = name.substr(index + 1, name.size() - index - 1);
                    ret->push_back(tmp);
                }
            }
            return ret;
        }

        /**
         * 将JSON对象转换成Lua中的table
         */
        void json2table(Json::Value json) {
            lua_newtable(L);
            Json::Value::iterator it = json.begin();
            while (it != json.end()) {
                std::string key = it.key().asString();
                Json::Value value = *it;
                if (value.isObject()) {
                    lua_pushstring(L, key.c_str());
                    json2table(value);
                    lua_settable(L, -3);
                } else if (value.isInt()) {
                    lua_pushstring(L, key.c_str());
                    lua_pushinteger(L, value.asInt());
                    lua_settable(L, -3);
                } else if (value.isInt64()) {
                    lua_pushstring(L, key.c_str());
                    lua_pushinteger(L, value.asInt64());
                    lua_settable(L, -3);
                } else if (value.isDouble()) {
                    lua_pushstring(L, key.c_str());
                    lua_pushnumber(L, value.asDouble());
                    lua_settable(L, -3);
                } else if (value.isString()) {
                    lua_pushstring(L, key.c_str());
                    lua_pushstring(L, value.asString().c_str());
                    lua_settable(L, -3);
                } else if (value.isBool()) {
                    lua_pushstring(L, key.c_str());
                    if (value.asBool()) {
                        lua_pushboolean(L, 1);
                    } else {
                        lua_pushboolean(L, 0);
                    }
                    lua_settable(L, -3);
                } else if (value.isArray()) {
                    lua_pushstring(L, key.c_str());
                    lua_newtable(L);
                    Json::ValueIterator it_arr = value.begin();
                    int index = 1;
                    while (it_arr != value.end()) {
                        Json::Value v = *it_arr;
                        lua_pushnumber(L, index);
                        if (v.isInt()) {
                            lua_pushinteger(L, v.asInt());
                            lua_settable(L, -3);
                        } else if (v.isInt64()) {
                            lua_pushinteger(L, v.asInt64());
                            lua_settable(L, -3);
                        } else if (v.isDouble()) {
                            lua_pushnumber(L, v.asDouble());
                            lua_settable(L, -3);
                        } else if (v.isString()) {
                            lua_pushstring(L, v.asString().c_str());
                            lua_settable(L, -3);
                        } else if (v.isBool()) {
                            if (v.asBool()) {
                                lua_pushboolean(L, 1);
                            } else {
                                lua_pushboolean(L, 0);
                            }
                            lua_settable(L, -3);
                        }
                        index ++;
                        it_arr ++;
                    }
                    lua_settable(L,-3);
                }
                it ++;
            }
        }

        /**
         * 将一个Lua的table转换成Json对象
         */
        Json::Value table2json(lua_State* L) {
            Json::Value ret;

            lua_pushnil(L);
            int type = lua_type(L, -2);
            if (type == LUA_TNIL) {
                lua_pop(L, 1);
                return ret;
            }
            if (type == LUA_TTABLE) {
                while (lua_next(L, -2)) {
                    /* 处理相应数据。此时栈上 -1 处为 value, -2 处为 key */
                    int key_type = lua_type(L, -2);
                    if (key_type == LUA_TSTRING) {
                        //为普通对象
                        std::string key = lua_tostring(L, -2);
                        int type = lua_type(L, -1);
                        if (type == LUA_TSTRING) {
                            //字符串
                            std::string value = lua_tostring(L, -1);
                            ret[key] = value;
                        } else if (type == LUA_TNUMBER) {
                            double value = lua_tonumber(L, -1);
                            ret[key] = value;
                        } else if (type == LUA_TBOOLEAN) {
                            bool value = (bool)lua_toboolean(L, -1);
                            ret[key] = value;
                        } else if (type == LUA_TTABLE) {
                            //数组和对象都是TABLE
                            ret[key] = table2json(L);
                        }
                    } else if (key_type == LUA_TNUMBER){
                        //为数据对象
                        int type = lua_type(L, -1);
                        if (type == LUA_TSTRING) {
                            //字符串
                            std::string value = lua_tostring(L, -1);
                            ret.append(value);
                        } else if (type == LUA_TNUMBER) {
                            double value = lua_tonumber(L, -1);
                            ret.append(value);
                        } else if (type == LUA_TBOOLEAN) {
                            bool value = (bool)lua_toboolean(L, -1);
                            ret.append(value);
                        }
                    }
                    lua_pop(L, 1);
                }
            } else {
                for (int i = 1; i < 10; ++i) {
                    int type = lua_type(L, i);
                    if (type == LUA_TSTRING) {
                        std::string value = lua_tostring(L, i);
                        ret.append(value);
                    } else if (type == LUA_TNUMBER) {
                        double value = lua_tonumber(L, i);
                        ret.append(value);
                    } else if (type == LUA_TBOOLEAN) {
                        bool value = (bool)lua_toboolean(L, i);
                        ret.append(value);
                    } else {
                        break;
                    }
                }
                lua_pop(L, 1);
            }

            return ret;
        }

        Json::Value wrap(Json::Value param) {
            Json::Value ret;
            std::string to;
            if (param.isMember("aux") && param["aux"].isMember("to")) {
                to = param["aux"]["to"].asString();
                param["aux"].removeMember("to");
                param.removeMember("aux");
            }

            if (!param.isMember("args")) {
                ret["args"] = param;
                ret["aux"] = Json::Value();
                ret["aux"]["id"] = UUID::id();
                ret["aux"]["to"] = to;
            } else {
                if (!param.isMember("aux")) {
                    param["aux"] = Json::Value();
                    param["aux"]["id"] = UUID::id();
                    ret["aux"]["to"] = to;
                } else if (!param["aux"].isMember("id")) {
                    param["aux"]["id"] = UUID::id();
                    ret["aux"]["to"] = to;
                }
                ret = param;
            }
            return ret;
        }

        Json::Value register_callback(Json::Value param, std::function<void(Json::Value)> callback) {
            Json::Value ret = wrap(param);
            if (callback != nullptr) {
                ret["aux"]["action"] = "callback";
                std::string id = ret["aux"]["id"].asString();
                callback_map[id] = callback;
            } else {
                ret["aux"]["action"] = "direct";
            }
            return ret;
        }

        bool call(std::string method, Json::Value param, std::function<void(Json::Value)> callback) {
            if (!is_load_) {
                return false;
            }
            bool ret = false;

            param = register_callback(param, callback);

            unsigned long pos = method.find(".");
            if (pos == std::string::npos) {
                //不包含 . 运算
                int type = lua_getglobal(L, method.c_str());//返回6表示函数,5表示table
                if (type == LUA_TFUNCTION) {
                    json2table(param);
                    ret = true;
                } else if (type == LUA_TNIL) {
                    ret = false;
                }
            } else {
                std::shared_ptr<std::vector<std::string>> calls = get_call_level(method);
                typedef std::vector<std::string>::iterator VStringIterator;
                VStringIterator end = calls->end();
                for (VStringIterator i = calls->begin(); i != end; ++i) {
                    std::string call = (*i);
                    int type = 0;
                    if (i == calls->begin()) {
                        type = lua_getglobal(L, call.c_str());
                    } else {
                        type = lua_getfield(L, -1, call.c_str());
                    }

                    if (type == LUA_TFUNCTION) {
                        json2table(param);
                        ret = true;
                        break;
                    } else if (type == LUA_TNIL) {
                        ret = false;
                    }
                }
            }
            if (ret) {
                ret = lua_pcall(L, 1, 0, 0) == 0;
                lua_pop(L, 1);
            }
            return ret;
        }


    private:
        bool is_load_;
        std::map<std::string, std::function<void(Json::Value)>> callback_map;
    };

    lua_bind lua_bind::instance() {
        static lua_bind lb;
        return lb;
    }

    bool lua_bind::lua_bind_init(std::string lua_path) {
        return impl->lua_init(lua_path);
    }

    bool lua_bind::lua_bind_loadfile(std::string lua_file) {
        return impl->lua_loadfile(lua_file);
    }

    bool lua_bind::call_lua(std::string method, Json::Value param) {
        return impl->call(method, param, nullptr);
    }

    bool lua_bind::call(std::string method, Json::Value param) {
        param["aux"]["to"] = method;
        return call_lua("lua_c_bridge.call_lua", param);
    }

    bool lua_bind::call_lua(std::string method, Json::Value param, std::function<void(Json::Value result)> callback) {
        return impl->call(method, param, callback);
    }

    bool lua_bind::call(std::string method, Json::Value param, std::function<void(Json::Value result)> callback) {
        param["aux"]["to"] = method;
        return call_lua("lua_c_bridge.call_lua", param, callback);
    }

    bool lua_bind::call(std::string method, std::function<void(Json::Value result)> callback) {
        return call(method, Json::Value(), callback);
    }

    bool lua_bind::call(std::string method) {
        return call(method, Json::Value());
    }

    lua_bind::lua_bind() : impl(new lua_bind_impl) {
    }

    void lua_bind::lua_callback(lua_State *L) {
        impl->lua_callback(L);
    }

    void lua_bind::native_call(lua_State *L) {
        impl->native_call(L);
    }

}