----------------
-- 数据检测模块
-- @module table_check

require "data_check_style"


local function is_style_property(key)
	local index, len = string.find(key, "__property_")
	if index == 1 then
		return true
	else
		return false
	end
end

--- 判断val的类型，返回类型
-- @prarm val 数据
-- @return 返回值为string/boolean/number/object/array中其一
local function json_type(val)
	t = type(val)
	if t == "table" then
		if #val == 0 then
			return "object"
		end
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


--- 检测json数据是否符合style标准，如果json数据是style的超集（即style中所有数据都在json中），也判断为符合
-- @param data 数据
-- @param style 样式结构，见data_check_style提示
-- @return 样式符合返回true,nil；否则返回false,"reason"
local function json_check_json_from_style(data, style)
	for k,v in pairs(style) do
		if not is_style_property(k) then
			if v["__property_require__"] == nil then
				return false, k .. " value must has __property_require__ in style"
			end
			if v["__property_type__"] == nil then
				return false, k .. " must has __property_type__ in style"
			end
			if v["__property_require__"] then
				local d_value = data[k]
				if d_value == nil then
					return false, k .. " is not exist in json"
				end
				local j_type = json_type(d_value)
				local s_type = v["__property_type__"]
				if j_type ~= s_type then
					return false, k .. " value type " .. j_type .. " is not match style type " .. s_type
				end
				if s_type == "object" then
					local result, reason = json_check_json_from_style(d_value, v)
					if not result then
						return result, reason
					end
				end
			else
				local d_value = data[k]
				if d_value then
					local j_type = json_type(d_value)
					local s_type = v["__property_type__"]
					if j_type ~= s_type then
						return false, k .. " value type " .. j_type .. " is not match style type " .. s_type
					end
				end
			end
		end
	end
	return true, "success"
end

--- 和json_check_json_from_style相反，检测json中是否有数据不在style标准中的，如果存在则判断为不符合
-- @param data 数据
-- @param style 样式结构，见data_check_style提示
-- @return 样式符合返回true；否则返回false
local function json_check_style_from_json(data, style)
	for k,v in pairs(data) do
		local d_type = json_type(v)
		local s_value = style[k]
		if s_value == nil then
			return false, k .. " is not define in style"
		end
		if d_type == "object" then
			local result, reason = json_check_style_from_json(v, s_value)
			if not result then
				return result, reason
			end
		end
	end
	return true, "success"
end



--- 检测所给的json数据是否与指定的样式符合
-- @param data 数据
-- @param style 样式结构，见data_check_style提示
-- @return 样式符合返回true,nil；否则返回false,"reason"
function json_check(data, style)
	if data == nil or style == nil then
		return false
	end
	local ret, reason = json_check_json_from_style(data, style)
	if ret then
		ret, reason = json_check_style_from_json(data, style)
	end
	return ret, reason
end

-- 测试数据
local d = {
	key_object = {
		key_string = "string", 
		key_object = {
			key_arr1 = {"1", "a", "b"},
			-- key_test = ""
		}, 
	},
	key_arr = {1,3,4},
	key_bool = true,
	key_number = 1.2,
	-- key_a = "d"
}

-- 测试调用
local result, reason = json_check(d, example_style)
print(result, reason)







