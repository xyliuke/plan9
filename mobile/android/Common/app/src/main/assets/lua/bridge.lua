----------------
-- lua和c的桥接层,用来完成c和lua的交互
-- @module lua_c_bridge

require("error_code")
require("table_check")
require("data_check_style")

lua_c_bridge = {
    callback_map = {},--调用C++函数后回调给lua时使用
    lua_function_map = {}, -- lua和所有可用函数集合
    check_json = true, -- 是否检测json的合法性
    platform = "none", -- 平台,字符串分别为: ios/android/win/macosx/none
}

--- 调用c++函数,获取唯一的id
-- @return 返回一个唯一的id值
function lua_c_bridge:get_id()
    return __native_get_id__()
end

function lua_c_bridge:notify(to_, type_, data_)
    local n = {aux = {to = to_, type = type_}}
    if type(data_) == "object" then
        n.result = {data = data_}
    else
        n.result = {data = {data = data_}}
    end

    lua_c_bridge:log_i("lua notify data : " .. lua_c_bridge:tostring(n))
    __notify__(n)
end

--- lua调用c++函数的封装
-- @param method c++中注册的函数名
-- @param param 参数,为table值
-- @param callback 回调,可为nil值
function lua_c_bridge:call_native(method, param, callback)
    if callback then
        local id = lua_c_bridge:get_id()
        self.callback_map[id] = callback;
        if "log" ~= method then
            lua_c_bridge:log_i("lua call c++ function , id : " .. id .. ", method : " .. method .. ", param : " .. lua_c_bridge:tostring(param) .. ", callback : yes")
        end
        __native_call__(id, method, param, true);
    else
        local id = lua_c_bridge:get_id()
        if "log" ~= method then
            lua_c_bridge:log_i("lua call c++ function , id : " .. id .. ", method : " .. method .. ", param : " .. lua_c_bridge:tostring(param) .. ", callback : no")
        end
        __native_call__(id, method, param, false);
    end
end
--- lua中写log函数,INFO级别
-- @param msg 日志内容
function lua_c_bridge:log_d(msg)
    lua_c_bridge:call_native("log", {level = "debug", msg = msg, target = "lua"})
end
--- lua中写log函数,INFO级别
-- @param msg 日志内容
function lua_c_bridge:log_i(msg)
    lua_c_bridge:call_native("log", {level = "info", msg = msg, target = "lua"})
end
--- lua中写log函数,WARN级别
-- @param msg 日志内容
function lua_c_bridge:log_w(msg)
    lua_c_bridge:call_native("log", {level = "warn", msg = msg, target = "lua"})
end
--- lua中写log函数,ERROR级别
-- @param msg 日志内容
function lua_c_bridge:log_e(msg)
    lua_c_bridge:call_native("log", {level = "error", msg = msg, target = "lua"})
end

--- 这个函数是供c++调用的, 以此来达到lua调用c++函数的而callback的结果
-- @param param c++传递的json格式数据
function lua_c_bridge.callback_from_c(param)
    lua_c_bridge:log_i("callback from c++ : " .. lua_c_bridge:tostring(param))
    local id = param.aux.callback_id;
    local callback = lua_c_bridge.callback_map[id]
    if callback then
        param.aux.action = "callback"
        callback(param)
        if param == nil or param.aux == nil or param.aux.once == nil or param.aux.once then
            param.aux.callback_id = nil
            lua_c_bridge.callback_map[id] = nil
        end

    end
end

--- 这个函数供lua调用,lua回调给c++
-- @param result 将回调的值整合成一个table后传递给C++
function lua_c_bridge:callback_direct(result)
    if result and result.aux and result.aux.action == "callback" then
        lua_c_bridge:log_i("callback from lua : " .. lua_c_bridge:tostring(result))
        __callback__(result);
    end
end
--- 这个函数供lua调用,用来将值回调给c++
-- @param param 由c++传递来的参数
-- @param result 回调的成功或失败,值必须为boolean值或者nil值,如果为nil则为false
-- @param data 回调给C++的数据,必须为table值或nil值
-- @param data_style 检测data是否符合data_style定义的格式,可以为nil
function lua_c_bridge:callback(param, result, data, error, reason, data_style)
    if param then
        if param.result == nil then
            param.result = {}
        end
        if result == nil then
            param.result.success = false
        else
            if type(result) == "boolean" then
                param.result.success = result
            else
                lua_c_bridge:log_e("callback from lua param error, result must be boolean")
                return
            end
        end
        if not param.result.success then
            param.result.error = error
            param.result.reason = reason
        end
        if data then
            if type(data) == "table" then
                param.result.data = data;
            else
                param.result.data = {data = data}
                lua_c_bridge:log_w("callback from lua param warning, data should be table, but is not. we change to it")
            end
        else
            param.result.data = data;
        end

        if self.check_json and result and data_style then
            local s,r = json_check(param.result.data, data_style)
            if not s then
                param.result.reason = r
                param.result.success = false
                param.result.error = error_code.JSON_FORMAT_ERROR
            end
        end

        lua_c_bridge:callback_direct(param)
    else
        lua_c_bridge:log_e("callback from lua param error, the param must not be nil")
    end
end
function lua_c_bridge:callback_error(param, error, reason)
    if param and error then
        if param.result == nil then
            param.result = {}
        end
        param.result.success = false
        param.result.error = error
        param.result.reason = reason
        param.result.data = {}
        lua_c_bridge:callback_direct(param)
    else
        lua_c_bridge:log_e("callback from lua param error, the param must not be nil, error must not be nil")
    end
end

--- 将字符串使用某一字符分隔,如a.b 使用.号分隔结果就是a  b
-- @param s 字符串
-- @param p 分隔符
-- @return 返回分隔后字符串数组
function lua_c_bridge:split(s, p)
    local rt= {}
    string.gsub(s, '[^'..p..']+', function(w) table.insert(rt, w) end )
    return rt
end

--- 将lua中的table数据转换成json格式的字符串
-- @param obj 数据
-- @return 返回字符串表示
function lua_c_bridge:tostring(obj)
    if obj == nil then
        return ""
    end
    local obj_type = type(obj)
    local tmp = {}
    if obj_type == "table" then
        local is_array = false
        for k,v in pairs(obj) do
            if type(k) == "number" then
                is_array = true
                break
            end
        end

        if is_array then
            tmp[#tmp + 1] = "["
            for k,v in pairs(obj) do
                tmp[#tmp + 1] = lua_c_bridge:tostring(v)
                tmp[#tmp + 1] = ","
            end
            if tmp[#tmp] == "," then
                tmp[#tmp] = nil
            end
            tmp[#tmp + 1] = "]"
        else
            tmp[#tmp + 1] = "{"
            for k,v in pairs(obj) do
                tmp[#tmp + 1] = tostring(k)
                tmp[#tmp + 1] = ":"
                tmp[#tmp + 1] = lua_c_bridge:tostring(v)
                tmp[#tmp + 1] = ","
            end
            if tmp[#tmp] == "," then
                tmp[#tmp] = nil
            end
            tmp[#tmp + 1] = "}"
        end

    else
        if obj_type == "string" then
            tmp[#tmp + 1] = "\"" .. tostring(obj) .. "\""
        else
            tmp[#tmp + 1] = tostring(obj)
        end
    end
    local ret = table.concat(tmp, '')
    return ret
end

--- 分隔函数名,函数名使用.点运算来表示
-- @param s 函数名
function lua_c_bridge:split_function(s)
    return lua_c_bridge:split(s, ".")
end

--- c++调用lua的统一入口
-- @param param 传入参数,为json格式数据
function lua_c_bridge.call_lua(param)
    lua_c_bridge:log_i("c++ call lua function : " .. lua_c_bridge:tostring(param))
    local m = param.aux.to
    local methods = lua_c_bridge:split_function(m)
    local count = #methods
    local func = lua_c_bridge.lua_function_map[methods[1]];
    if func == nil then
        lua_c_bridge:log_e("can not find lua function, param: \n" .. lua_c_bridge:tostring(param))
        lua_c_bridge:callback_error(param, error_code.LUA_FUNCTION_NOT_EXSIT, nil)
        return
    end
    for i = 2, count - 1 do
        local f = func[methods[i]]
        if (type(f) == "table") then
            func = f
        else
            lua_c_bridge:log_e("can not find lua function, param: \n" .. lua_c_bridge:tostring(param))
            lua_c_bridge:callback_error(param, error_code.LUA_FUNCTION_NOT_EXSIT, nil)
            return
        end
    end
    local last = func
    func = func[methods[#methods]]
    if func ~= nil and type(func) == "function" then
        func(last, param, function(param, result, data, error, reason, data_style)
            lua_c_bridge:callback(param, result, data, error, reason, data_style);
        end)
    else
        lua_c_bridge:log_e("can not find lua function, param:\n " .. lua_c_bridge:tostring(param))
        lua_c_bridge:callback_error(param, error_code.LUA_FUNCTION_NOT_EXSIT, nil)
        return
    end
end

--- 判断当前宿主是否ios
-- @return 是则返回true,否则返回false
function lua_c_bridge:is_ios()
    return self.platform == "ios"
end

--- 判断当前宿主是否android
-- @return 是则返回true,否则返回false
function lua_c_bridge:is_android()
    return self.platform == "android"
end

--- 判断当前宿主是否windows
-- @return 是则返回true,否则返回false
function lua_c_bridge:is_windows()
    return self.platform == "win"
end

--- 判断当前宿主是否macosx
-- @return 是则返回true,否则返回false
function lua_c_bridge:is_macosx()
    return self.platform == "macosx"
end



--- 注册lua中可供c++调用的函数
-- @param name 函数的命名空间,即函数的包装table名
-- @param func 函数实体
function lua_c_bridge:register_lua_function(name, func)
    self.lua_function_map[name] = func
end
--- 得到在lua_c_bridge注册的函数
-- @param name 函数名
-- @return 返回注册的table值
function lua_c_bridge:get_module(name)
    return self.lua_function_map[name]
end

local native = {}
function native:get_error_code(param, callback)
    callback(param, true, error_code, nil)
end

--- 设置当前平台,供内部使用
function native:set_platform(param)
    lua_c_bridge.platform = param.args.platform
    lua_c_bridge:log_i("platform set " .. lua_c_bridge.platform)
    lua_c_bridge:call_native("tcp_connect", {ip = "127.0.0.1", port = 8880})
end

lua_c_bridge:register_lua_function("native", native)