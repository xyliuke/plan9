package commander

import (
	"common/json"
)

type function_callback func(*json.JSONObject)

var func_map = make(map[string]function_callback)

//注册命令;
//cmd为命令名;
//call 为具体执行函数，为func(*json.JSONObject)类型
func RegisterCmd(cmd string, call function_callback) {
	func_map[cmd] = call
}

func RemoveCmd(cmd string) {
	delete(func_map, cmd)
}

func ExecuteCmd(cmd string, json *json.JSONObject) {
	call, ok := func_map[cmd]
	if ok {
		call(json)
	}
}
