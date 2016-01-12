package util

import (
	"strconv"
	"time"
)

const alp = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"//字表表
var i = 0//第一个字母当前在alp中位置
var j = 0//第二个字母当前在alp中位置

// 生成一个唯一的ID,组成为:"ID" + 当前时间纳秒 + 2个26字母的校验去重
// 如ID1451984038172094167AA 和 ID1451984038172094169AB
func CreateID() (string)  {
	nano := time.Now().UnixNano()
	nanoStr := strconv.FormatInt(nano, 10)
	ret := "ID" + nanoStr + string(alp[i]) + string(alp[j])
	if j > 24 {
		j = 0
		if i > 24 {
			i = 0
		} else {
			i ++
		}
	} else {
		j ++
	}
	return ret
}


