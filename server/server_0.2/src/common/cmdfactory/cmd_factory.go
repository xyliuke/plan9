package commander

import (
	"common/json"
	"common/util"
	"common/log"
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

func ExecuteCmd1(param json.JSONObject, callback function_callback) {
	if param != nil {
		aux_ok, aux := param.GetJSON("aux")
		if aux_ok {
			name_ok, name := aux.GetString("to")
			if name_ok {
				ExecuteCmd(name, param, callback)
			} else {
				log.E_COM("execute cmd error, reason : aux has not name paramter")
			}
		} else {
			log.E_COM("execute cmd error, reason : param has not aux paramter")
		}
	} else {
		log.E_COM("execute cmd error, reason : param is nil")
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
					aux.RemoveLast("from")
					call_map[last](param)
				}
			}
		}
	}
}

func CallbackWithFailResult(param json.JSONObject, reason string) {
	if param != nil {
		if !param.HasMember("result") {
			result := json.NewJSONEmpty()
			param.Put("result", result)
		}

		ok, result := param.GetJSON("result")
		if ok {
			result.Put("success", false)
			result.Put("reason", reason)
		}
		Callback(param)
	}
}

func CallbackWithSuccessResult(param json.JSONObject, data json.JSONObject) {
	if param != nil {
		if !param.HasMember("result") {
			result := json.NewJSONEmpty()
			param.Put("result", result)
		}

		ok, result := param.GetJSON("result")
		if ok {
			result.Put("success", true)
			if data != nil {
				result.Put("data", data)
			} else {
				result.Put("data", json.NewJSONEmpty())
			}
		}
		Callback(param)
	}
}