----------------
-- 数据检测模块
-- @module data_check_style


-- 例子样式介绍

-- 数据类型, lua中数组下标从1开始
local dt = {"number", "string", "boolean", "object", "array"}

--  样式控制的属性都是以__property_开头
example_style = {
	key_object = {
			__property_type__ = dt[4], --数据类型
			__property_require__ = true,  -- 是否必要
			__property_comment__ = "注释",
			-- 如果为object类型，则需要包含其他的数据
			key_string = {
				__property_type__ = dt[2],
				__property_require__ = true,
			},
			key_object = {
				__property_type__ = dt[4],
				__property_require__ = true,
				key_arr1 = {
					__property_type__ = dt[5],
					__property_require__ = true,
				}
			}
		},
	key_arr = {
			__property_type__ = dt[5],
			__property_require__ = true,
		},
	key_bool = {
			__property_type__ = dt[3],
			__property_require__ = true
	},
	key_number = {
			__property_require__ = false,
			__property_type__ = dt[1]
	}

}
