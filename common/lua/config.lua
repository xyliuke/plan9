----------------
-- 用来配置整个工程相关选项
-- @module config

require("bridge")

local config = {}
lua_c_bridge:register_lua_function("config", config)

config.log_level = "debug" --"debug","info","warn","error"
config.server = {ip = "127.0.0.1", port = 8081}

function config:get_config(param, callback)
    callback(param, true, config, nil)
end