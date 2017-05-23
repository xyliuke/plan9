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
config.server = {ip = "127.0.0.1", port = 9099, timeout = 5000}

--- 获取配置项,内部使用
function config:get_config(param, callback)
    callback(param, true, {
        log_level = config.log_level,
        log_days = 14, -- 日志保存天数
        tcp_notify_function = "tcp.tcp_notify", -- tcp 连接、读取、写入数据事件处理函数，使用前提是必须在lib中有tcp项
--        compress = true, -- tcp包是否压缩
--        encrypt = false,  -- tcp包是否加密,如果需要加密,则必须要压缩
        ssl_verify_peer = false,
        ssl_verify_host = false,
        lib = {"http", "algo", "database", "file", "tcp"}  --加载的库名
    }, nil)
end