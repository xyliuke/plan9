require("bridge")


local http = {
    timeout = 60,
    download_timeout = 60, --下载超时
    upload_time = 60 --上传超时
}
lua_c_bridge:register_lua_function("http", http)
local util = lua_c_bridge:get_module("util")
--- http的get方法
-- @param url_str 网址
-- @param header_ http头部参数，table值
-- @param callback 回调
function http:get(url_str, header_, callback)
    local p = {type = "get", url = url_str, header = header_}
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
        if callback then
            callback(data)
        end
    end)
end

function http:upload(param, url_str, path_, header_, form_, process_, callback)
    local p = {model = "sync", url = url_str, header = header_, form = form_, process = process_, path = path_ }
    lua_c_bridge:call_native("upload", p, function(data)
        param.aux.once = data.aux.once
        if callback then
            callback(data)
        end
    end)

end

function http:test_get(param, callback)
    local u = "https://api.chesupai.cn/customer/index/data/?idfa=01D4B665-11AB-45FA-A8EB-2172CA06A566&osv=iOS11.2&net=data&screenWH=750%2C1334&deviceId=FDAB131D-04B6-475A-8C82-A48FC5EA4FEE&city_id=12&platform=1&dpi=326&versionId=2.9.0&deviceModel=iPhone&page_size=20&model=x86_64&sign=4b1a809279223c733a2acd097d34a1eb"
    u = "https://www.baidu.com"
    u = "http://api.chesupai.cn/customer/detail/info?id=1429449&idfa=11BFBC7A-98EF-4B37-A216-E8DAF0ABAB8B&osv=iOS8.1&net=wifi&screenWH=750%252C1334&deviceId=3200A4C2-C469-469D-A42A-920B1A5A0216&deviceModel=iPhoneSimulator&platform=1&dpi=326&versionId=2.7.3&model=x86_64&pushTYpe=0&sign=9102c932d5e96cd5129b1c35f9baee28";
-- u = "https://api.chesupai.cn"
    self:get(u, nil, function(result)
        callback(param, result.result.success, result.result.data, result.result.error, result.result.reason, nil)
    end)
end

function http:test_download(param, callback)
    self:download(param, "http://www.baidu.com", nil, "./baidu.html", true, true, function(result)
        callback(param, result.result.success, result.result.data, result.result.error, result.result.reason, nil)
    end)
end

function http:test_upload(param, callback)
    local file_name = util:file_name(param.args.file)
    local h = {type = "img" }
    h["file_list[0]"] = file_name
    self:post("http://storage.guazi.com/newsign.php", nil, h, function(sign_result)
        if sign_result.result.success and sign_result.result.data and sign_result.result.data.data.signs then
            local sign = sign_result.result.data.data.signs[1]
            local url = string.gsub(sign_result.result.data.data.upload_url, "\\/", "/")
            self:upload(sign_result, url, param.args.file, nil, {key = sign.key, token = sign.token}, true, function(upload_result)
                lua_c_bridge:log_d("upload result " .. lua_c_bridge:tostring(upload_result))
                callback(param, upload_result.result.success, upload_result.result.data, upload_result.result.error, upload_result.result.reason, nil)
            end)
        end
    end)
end




