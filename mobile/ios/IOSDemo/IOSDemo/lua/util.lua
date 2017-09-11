-- 常用工具类
require("bridge")

local util = {}
lua_c_bridge:register_lua_function("util", util)

--- 获取文件路径中的文件名
-- @param 文件路径
function util:file_name(file_path)
    if type(file_path) == "string" then
        local bytes = {string.byte(file_path, 1, string.len(file_path))}
        local index = 1
        for i = #bytes, 1, -1 do
            local b = bytes[i]
            if b == 47 or b == 92 then
                index = i + 1
                break
            end
        end
        return string.sub(file_path, index)
    else
        return ""
    end
end

--- 获取文件路径中的文件扩展名
-- @param 文件路径或者文件名
function util:file_ext(file)
    if type(file) == "string" then
        local bytes = {string.byte(file, 1, string.len(file))}
        local index = 1
        for i = #bytes, 1, -1 do
            local b = bytes[i]
            if b == 47 or b == 92 then
                return ""
            end
            if b == 46 then
                index = i + 1
                break
            end
        end
        return string.sub(file, index)
    else
        return ""
    end
end

function util:replace(str, old, new)
    string.gsub(str, old, new)
end

