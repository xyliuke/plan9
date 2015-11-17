----------------
-- 数据检测模块
-- @module data_check_style


-- 例子样式介绍

-- 数据类型, lua中数组下标从1开始
local dt = {"number", "string", "boolean", "object", "array"}

--  样式控制的属性都是以__property_开头
example_style = {
	a = {
			__property_type__ = dt[4], --数据类型
			__property_require__ = true,  -- 是否必要
			__property_comment__ = "注释",
			-- 如果a为object类型，则需要包含其他的数据
			b = 
			{
				__property_type__ = dt[1],
				__property_require__ = false,
			}
		},
	c = {
			__property_type__ = dt[1], --数据类型
			__property_require__ = true,  -- 是否必要
		}
}
