----------------
-- 访问服务器的接口
-- @module server

require("bridge")

local server = {
    server_type = {SERVER_CONNECT = 0x00, SERVER_ROUTE = 0x10, SERVER_SESSION = 0x20, SERVER_DATABASE = 0x30}
}
lua_c_bridge:register_lua_function("server", server)

local config = lua_c_bridge:get_module("config")

--- 封装向服务器发送的数据
-- @param method 方法名
-- @param param json格式参数
-- @param server_type 服务器种类
-- @param timeout 超时
function server:wrap(method, param, server_type, timeout)
    local msg = param
    msg.timeout = timeout
    msg.server = server_type
    msg.to = method
    return msg
end

function server:connect()
    lua_c_bridge:call_native("connect", {ip = config.server.ip, port = config.server.port})
end

function server:send(param, callback)
    self:send_server_connect("function", param, function(result)
        if result.result then
            local suc = false;
            if result.result.success ~= nil then
                suc = result.result.success
            end
            param.result = result.result
            callback(param, suc, result.result.data, nil)
        else
            callback(param, false, nil, nil)
        end
    end)
end

--- 向连接服务器发送数据
function server:send_server_connect(method, param, callback)
    local p = server:wrap(method, param.args, self.server_type.SERVER_CONNECT, config.server.timeout)
    lua_c_bridge:call_native("send", p, function(result)
        callback(result)
    end)
end

--- 向路由服务器发送数据
function server:send_server_route(method, param, callback)
    local p = server:wrap(method, param.args, self.server_type.SERVER_ROUTE, config.server.timeout)
    lua_c_bridge:call_native("send", p, function(result)
        callback(result)
    end)
end

--- 向事务服务器发送数据
function server:send_server_session(method, param, callback)
    local p = server:wrap(method, param.args, self.server_type.SERVER_SESSION, config.server.timeout)
    lua_c_bridge:call_native("send", p, function(result)
        callback(result)
    end)
end

--- 向数据库服务器发送数据
function server:send_server_database(method, param, callback)
    local p = server:wrap(method, param.args, self.server_type.SERVER_DATABASE, config.server.timeout)
    lua_c_bridge:call_native("send", p, function(result)
        callback(result)
    end)
end