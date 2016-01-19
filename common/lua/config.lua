----------------
-- 用来配置整个工程相关选项
-- @module config

require("bridge")

local config = {}
lua_c_bridge:register_lua_function("config", config)

--- 日志配置级别
config.log_level = "debug" --"debug","info","warn","error"

--- 服务器相关配置
-- @param ip 服务器的ip地址
-- @param port 服务器监听端口号
-- @param timeout 连接服务器超时
config.server = {ip = "127.0.0.1", port = 8081, timeout = 5000}

--- 获取配置项,内部使用
function config:get_config(param, callback)
    callback(param, true, {
        log_level = config.log_level,
    }, nil)
end