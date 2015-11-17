----------------
-- 数据检测模块
-- @module table_check

require "data_check_style"

--- 检测所给的json数据是否与指定的样式符合
-- @param data 数据
-- @param style 样式结构，见data_check_style提示
-- @return 样式符合返回true；否则返回false
function json_check(data, style)
	if data == nil or style == nil then
		return false
	end
	for k,v in pairs(style) do
		print(k, ",type:", type(k), "\tvalue", v, ",type:", type(v))
		local sk = style[k]
		if sk then

		end
		return false
	end
	return false
end

--- 判断val的类型，返回类型
-- @prarm val 数据
-- @return 返回值为string/boolean/number/object/array中其一
function json_type(val)
	t = type(val)
	if t == "table" then
		for k,v in pairs(val) do
			if type(k) ~= "number" then
				return "object"
			end
		end
		return "array"
	else
		return t
	end
end

function json_match(val, style)
	if json_type(val) == style.__property_type__ then

	end
end

local d = {a = "va", b = "vb", c = 1, d = 1.2, e = {e1 = "ve"}, f = {1,2,3,4}, g = true}
print(d["a"])
-- json_check(d)

-- local arr = {1,2,3,4,5, a = "a"}
-- local x = 1
-- local y = 1.2
-- local z = "hello"
-- local g = true
-- -- print(json_check(arr))
-- print(json_type(arr))
-- print(json_type(x))
-- print(json_type(y))
-- print(json_type(z))
-- print(json_type(g))






