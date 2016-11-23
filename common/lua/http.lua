require("bridge")


local http = {
    timeout = 60,
    download_timeout = 60, --下载超时
    upload_time = 60 --上传超时
}
lua_c_bridge:register_lua_function("http", http)

--- http的get方法
-- @param url_str 网址
-- @param header_ http头部参数，table值
-- @param callback 回调
function http:get(url_str, header_, callback)
    local p = {type = "get", model = "sync", url = url_str, header = header_}
    lua_c_bridge:call_native("http", p, callback)
end

--- http的post方法
-- @param url_str 网址
-- @param header_ http头部参数，table值
-- @param form_ post提交的表单
-- @param callback 回调
function http:post(url_str, header_, form_, callback)
    local p = {type = "post", model = "sync", url = url_str, header = header_, form = form_}
    lua_c_bridge:call_native("http", p, callback)
end

--- 下载文件
-- @param param 原始参数对象
-- @param url_str 网址
-- @param header_ http头部参数 table值
-- @param path_ 本地保存文件路径，字符串
-- @param process_ 是否回调下载进度 true/false
-- @param override_ 是否覆盖本地原有文件,如果为false,则断点处下载 true/false
function http:download(param, url_str, header_, path_, process_, override_, callback)
    local p = {model = "sync", url = url_str, header = header_, path = path_, process = process_, override = override_}
    lua_c_bridge:call_native("download", p, function(data)
        param.aux.once = data.aux.once
        lua_c_bridge:log_i("test download ".. lua_c_bridge:tostring(data.aux.once))
        callback(data)
    end)
end


function http:test_get(param, callback)
    self:get("http://www.baidu.com", nil, function(result)
        callback(param, result.result.success, result.result.data, result.result.error, result.result.reason, nil)
    end)
end

function http:test_download(param, callback)
    self:download(param, "http://www.baidu.com", nil, "./baidu.html", true, true, function(result)
        callback(param, result.result.success, result.result.data, result.result.error, result.result.reason, nil)
    end)
--    local p = {model = "sync", url = param.args.url, path = param.args.path}
--    lua_c_bridge:call_native("download", p, function(result)
--        param.aux.once = result.aux.once
--        callback(param, result.result.success, result.result.data, result.result.error, result.result.reason, nil)
--    end);
end




