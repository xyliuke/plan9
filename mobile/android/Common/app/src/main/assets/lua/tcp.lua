require("bridge")

local tcp = {}
lua_c_bridge:register_lua_function("tcp", tcp)

function tcp:connect(param, callback)
    lua_c_bridge:call_native("tcp_connect", {ip = param.args.ip, port = param.args.port})
end

function tcp:to_notify_cpp(to, data)
    lua_c_bridge:notify(to, to, data);
end

--- tcp数据和事件通知
-- type_ 通过类型 分别为connected, disconnect, read, write
-- data_ 不同类型下的数据
function tcp:tcp_notify(param)
    self:to_notify_cpp(param.args.to, param.args.data)
    if "connected" == param.args.to then
        lua_c_bridge:call_native("tcp_write", {data = "hello world"})
    elseif "read" == param.args.to then

    end
end
