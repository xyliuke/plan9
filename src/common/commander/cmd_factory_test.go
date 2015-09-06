package commander

import (
	"common/json"
	"common/logger"
	"testing"
)

func Test_ExecuteCmd(t *testing.T) {
	RegisterCmd("hello", func(json *json.JSONObject) {
		// glog
	})
	body := []byte(`{
		"test": {
			"string_array": ["asdf", "ghjk", "zxcv"],
			"array": [1, "2", 3],
			"arraywithsubs": [{"subkeyone": 1},
			{"subkeytwo": 2, "subkeythree": 3}],
			"int": 10,
			"float": 5.150,
			"bignum": 9223372036854775807,
			"string": "simplejson",
			"bool": true
		}
	}`)
	json, _ := json.NewJSONObject(body)
	ExecuteCmd("hello", json)
	RemoveCmd("hello")
	ExecuteCmd("hello", json)
}
