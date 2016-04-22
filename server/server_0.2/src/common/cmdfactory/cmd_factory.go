package commander

import (
	"common/json"
	"common/util"
)

type function_callback func(json.JSONObject)

//注册函数的结构
var func_map = make(map[string]function_callback)

//调用函数的结构
var call_map = make(map[string]function_callback)

//注册命令;
//cmd为命令名;
//call 为具体执行函数，为func(*json.JSONObject)类型
func RegisterCmd(cmd string, call function_callback) {
	func_map[cmd] = call
}

func RemoveCmd(cmd string) {
	delete(func_map, cmd)
}

func ExecuteCmd(cmd string, param json.JSONObject, callback function_callback) {
	call, ok := func_map[cmd]
	if ok {
		p := json.NewJSONEmpty()
		if param.HasMember("aux") || param.HasMember("args") {
			p = param
		} else {
			p.Put("args", param)
		}

		id := util.CreateID()
		is, aux := p.GetJSON("aux")
		if is && aux.HasMember("id") {

		} else {
			aux = json.NewJSONEmpty()
			p.Put("aux", aux)
			aux.Put("id", id)
		}

		if callback != nil {
			call_map[id] = callback
			aux.AddToArray("from", id)
			aux.Put("action", "callback")
		} else {
			aux.Put("action", "direct")
		}

		aux.Put("to", cmd)
		call(p)
	}
}

func ExecuteCmdDirect(cmd string, json json.JSONObject) {
	ExecuteCmd(cmd, json, nil)
}

func Callback(param json.JSONObject) {
	if param != nil {
		is, aux := param.GetJSON("aux")
		if is {
			iss, arr := aux.GetArray("from")
			if iss {
				last, isss := arr.GetLast().(string)
				if isss {
					call_map[last](param)
					aux.RemoveLast("from")
				}
			}
		}
	}
}
