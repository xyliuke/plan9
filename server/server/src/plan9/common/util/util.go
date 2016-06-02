package util

import (
	"fmt"
	"strconv"
	"time"
	"crypto/rand"
)

const alp = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" //字表表
var i = 0                                //第一个字母当前在alp中位置
var j = 0                                //第二个字母当前在alp中位置
var randomBuf []byte	//生成2个字节的随机数
// 生成一个唯一的ID,组成为:"ID" + 当前时间纳秒 + 2个26字母的校验去重
// 如ID1451984038172094167AA 和 ID1451984038172094169AB
func CreateID() string {
	nano := time.Now().UnixNano()
	nanoStr := strconv.FormatInt(nano, 10)
	r := fmt.Sprintf("%04d", random() % 10000)
	ret := "ID-" + nanoStr + "-" + r + "-" + string(alp[i]) + string(alp[j])
	if j > 24 {
		j = 0
		if i > 24 {
			i = 0
		} else {
			i++
		}
	} else {
		j++
	}
	return ret
}

var step int = 0
func CreateDataID(serverType byte) int {
	var st = int(serverType) << 24
	if step > 0x00FFFFFF {
		step = 0
	} else {
		step += 1
	}

	return st | (step & 0x00FFFFFF)
}


func random() int {
	if randomBuf == nil {
		randomBuf = make([]byte, 2)
	}

	rand.Read(randomBuf)
	var ret int = int(randomBuf[0]) << 8 + int(randomBuf[1])
	return ret
}