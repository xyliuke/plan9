
lua_c_bridge = {callback_map = {}, lua_function_map = {}}

function lua_c_bridge:get_id()
    return __native_get_id__()
end

-- lua调用c++函数的封装
function lua_c_bridge:call_native(method, param, callback)
    if callback then
        local id = lua_c_bridge:get_id()
        self.callback_map[id] = callback;
        __native_call__(id, method, param, true);
    else
        __native_call__(lua_c_bridge:get_id(), method, param, false);
    end
end

function lua_c_bridge:log_i(msg)
    lua_c_bridge:call_native("log", {level = "info", msg = msg, target = "lua"})
end
function lua_c_bridge:log_w(msg)
    lua_c_bridge:call_native("log", {level = "warn", msg = msg, target = "lua"})
end
function lua_c_bridge:log_e(msg)
    lua_c_bridge:call_native("log", {level = "error", msg = msg, target = "lua"})
end

-- c++调用lua的callback
function lua_c_bridge.callback_from_c(param)
    local id = param.aux.callback_id;
    local callback = lua_c_bridge.callback_map[id]
    if callback then
        param.aux.callback_id = nil
        callback(param)
    end
end

-- lua回调给c++
function lua_c_bridge:callback(result)
    if result and result.aux and result.aux.action == "callback" then
        __callback__(result);
    end
end

function lua_c_bridge:split(s, p)
    local rt= {}
    string.gsub(s, '[^'..p..']+', function(w) table.insert(rt, w) end )
    return rt
end

function lua_c_bridge:tostring(t)
    local print = print
    local tconcat = table.concat
    local tinsert = table.insert
    local srep = string.rep
    local type = type
    local pairs = pairs
    local tostring = tostring
    local next = next

    local function print_r(root)
        local cache = {  [root] = "." }
        local function _dump(t,space,name)
            local temp = {}
            for k,v in pairs(t) do
                local key = tostring(k)
                if cache[v] then
                    tinsert(temp,"+" .. key .. " {" .. cache[v].."}")
                elseif type(v) == "table" then
                    local new_key = name .. "." .. key
                    cache[v] = new_key
                    tinsert(temp,"+" .. key .. _dump(v,space .. (next(t,k) and "|" or " " ).. srep(" ",#key),new_key))
                else
                    tinsert(temp,"+" .. key .. " [" .. tostring(v).."]")
                end
            end
            return tconcat(temp,"\n"..space)
        end
        return _dump(root, "","")
    end
    return print_r(t)
end

function lua_c_bridge:split_function(s)
    return lua_c_bridge:split(s, ".")
end

function lua_c_bridge.call_lua(param)
    local m = param.aux.to
    local methods = lua_c_bridge:split_function(m)
    local count = #methods
    local func = lua_c_bridge.lua_function_map[methods[1]];
    for i = 2, count - 1 do
        local f = func[methods[i]]
        if (type(f) == "table") then
            func = f
        else
            lua_c_bridge:log_e("can not find lua function, param: \n" .. lua_c_bridge:tostring(param))
            return;
        end
    end
    local last = func
    func = func[methods[#methods]]
    if func ~= nil and type(func) == "function" then
        func(last, param, function(result)
            lua_c_bridge:callback(result);
        end)
    else
        lua_c_bridge:log_e("can not find lua function, param:\n " .. lua_c_bridge:tostring(param))
    end
end

function lua_c_bridge:register_lua_function(name, func)
    self.lua_function_map[name] = func
end

local biz = {}
function biz:test(param, callback)
    param.ret = {result = true, msg = "biz test callback"}
    callback(param)
end
lua_c_bridge:register_lua_function("biz", biz)