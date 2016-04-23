package route

import (
	"common/cmdfactory"
	"common/json"
)

func initRouteRegister() {

	commander.RegisterCmd("function", func(json_object json.JSONObject) {
		result_data := json.NewJSONEmpty()
		result_data.Put("hello", "from go lang server")
		commander.CallbackWithSuccessResult(json_object, result_data)
	})




}
