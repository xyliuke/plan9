--
-- Created by IntelliJ IDEA.
-- User: liuke
-- Date: 18/11/2016
-- Time: 5:56 PM
-- To change this template use File | Settings | File Templates.
--
require("bridge")
local http = {}
lua_c_bridge:register_lua_function("http", http)

function http:get(param, callback)
    local p = {type = "get", model = "sync", url = param.args.url }
    lua_c_bridge:call_native("http", p, function(result)
        callback(param, result.result.success, result.result.data)
    end);
end




