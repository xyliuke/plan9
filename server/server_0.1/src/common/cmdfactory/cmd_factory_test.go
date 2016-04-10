package commander

import (
	"common/json"
	"testing"
	"fmt"
)

func Test_ExecuteCmd(t *testing.T) {
	RegisterCmd("hello", func(param json.JSONObject) {
		fmt.Println("run hello", param.ToString())
		param.Put("ret", 1)
		Callback(param)
	})

	RegisterCmd("world", func(param json.JSONObject) {
		fmt.Println("run hello", param.ToString())
		param.Put("ret", 1)
		Callback(param)
	})

	js := json.NewJSONEmpty()
	js.Put("a", "a1")
	ExecuteCmdDirect("hello", js)
	ExecuteCmd("hello", js, func(value json.JSONObject) {
		fmt.Println("callback ", value.ToString())
	})
}
