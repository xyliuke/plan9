//
// Created by liuke on 15/9/21.
//

#include "lua_bind.h"

#include <json/json_wrap.h>

#include <iostream>
#include <map>
#include <sstream>
#include <vector>




namespace plan9 {

    static lua_State *L;

    static int callback(lua_State *L) {
        lua_bind::instance().lua_callback(L);
        return 0;
    }

    class lua_bind::lua_bind_impl {
    public:

        lua_bind_impl() : is_load_(false) {

        }

        bool lua_init(std::string lua_file) {
            L = luaL_newstate();
            luaopen_base(L);
            luaopen_io(L);
            luaopen_string(L);
            luaopen_math(L);
            if (luaL_loadfile(L, lua_file.c_str()) || lua_pcall(L, 0, 0, 0)) {
                return false;
            }
            register_callback();
            is_load_ = true;
            return true;
        }

        void register_callback() {
            lua_register(L, "callback", callback);
        }

        Json::Value lua_callback(lua_State* L) {
            return table2json(L);
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
            if (lua_type(L, -2) == LUA_TTABLE) {
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

        bool call(std::string method, Json::Value param, std::function<void(Json::Value)> callback) {
            if (!is_load_) {
                return false;
            }
            bool ret = false;

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
    };

    lua_bind lua_bind::instance() {
        static lua_bind lb;
        return lb;
    }

    bool lua_bind::lua_bind_init(std::string lua_file) {
        return impl->lua_init(lua_file);
    }

    bool lua_bind::call(std::string method, Json::Value param) {
        return impl->call(method, param, nullptr);
    }

    bool lua_bind::call(std::string method, Json::Value param, std::function<void(Json::Value result)> callback) {
        return impl->call(method, param, callback);
    }

    lua_bind::lua_bind() : impl(new lua_bind_impl) {
    }

    void lua_bind::lua_callback(lua_State *L) {
        Json::Value json = impl->lua_callback(L);
//        std::cout << "callback: " << json_wrap::toString(json) << std::endl;
    }

}