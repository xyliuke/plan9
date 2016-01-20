----------------
-- 访问服务器的接口
-- @module server

require("bridge")

local server = {
    server_type = {SERVER_CONNECT = 0x00, SERVER_ROUTE = 0x10, SERVER_SESSION = 0x20, SERVER_DATABASE = 0x30}
}
lua_c_bridge:register_lua_function("server", server)

local config = lua_c_bridge:get_module("config")

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
--    local p = server:wrap("function", param.args, self.server_type.SERVER_CONNECT, 10000)
--    lua_c_bridge:call_native("send", p, function(result)
--        callback(param, true, result.result, nil)
--    end)
    self:send_server_connect("function", param, function(result)
        callback(param, true, result.result, nil)
    end)
end

function server:send_server_connect(method, param, callback)
    local p = server:wrap(method, param.args, self.server_type.SERVER_CONNECT, config.server.timeout)
    lua_c_bridge:call_native("send", p, function(result)
        callback(result)
    end)
end