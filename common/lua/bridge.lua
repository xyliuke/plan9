----------------
-- lua和c的桥接层,用来完成c和lua的交互
-- @module lua_c_bridge

require("error_code")

lua_c_bridge = {callback_map = {}, lua_function_map = {}}

--- 调用c++函数,获取唯一的id
-- @return 返回一个唯一的id值
function lua_c_bridge:get_id()
    return __native_get_id__()
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
        param.aux.callback_id = nil
        callback(param)
        lua_c_bridge.callback_map[id] = nil
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
-- @param data 回调给C++的数据圣贤,必须为table值或nil值
function lua_c_bridge:callback(param, result, data)
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
        if data then
            if type(data) == "table" then
                param.result.data = data;
            else
                lua_c_bridge:log_e("callback from lua param error, data must be table")
                return
            end
        else
            param.result.data = data;
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
            tmp[#tmp] = nil
            tmp[#tmp + 1] = "]"
        else
            tmp[#tmp + 1] = "{"
            for k,v in pairs(obj) do
                tmp[#tmp + 1] = tostring(k)
                tmp[#tmp + 1] = ":"
                tmp[#tmp + 1] = lua_c_bridge:tostring(v)
                tmp[#tmp + 1] = ","
            end
            tmp[#tmp] = nil
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
        func(last, param, function(param, result, data)
            lua_c_bridge:callback(param, result, data);
        end)
    else
        lua_c_bridge:log_e("can not find lua function, param:\n " .. lua_c_bridge:tostring(param))
        lua_c_bridge:callback_error(param, error_code.LUA_FUNCTION_NOT_EXSIT, nil)
        return
    end
end

--- 注册lua中可供c++调用的函数
-- @param 函数的命名空间,即函数的包装table名
-- @param 函数实体
function lua_c_bridge:register_lua_function(name, func)
    self.lua_function_map[name] = func
end

local native = {}
function native:get_error_code(param, callback)
    callback(param, true, error_code)
end
lua_c_bridge:register_lua_function("native", native)