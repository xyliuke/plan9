----------------
-- 用来配置整个工程相关选项
-- @module config

require("bridge")

local config = {}
lua_c_bridge:register_lua_function("config", config)

--- 日志配置级别
config.log_level = "debug" --"debug","info","warn","error"

--- 服务器相关配置
config.server = {ip = "127.0.0.1", port = 9099, timeout = 50000}

--- 获取配置项
function config:get_config(param, callback)
    callback(param, true, {
        log_level = config.log_level,
    }, nil)
end