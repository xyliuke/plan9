package json

import (
	"log"
	"testing"
)


func Test_NewJSONObject(t *testing.T) {
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
	j, err := NewJSONObject(body)
	if err != nil {
		t.Error("new json object error")
	}
	str, err := j.Get("test").Get("string_array").StringArray()
	if err != nil {
		t.Error("new json object error")
	}
	log.Println(str)
}

func Test_NewJSONObjectByString(t *testing.T) {
	str := `{"from":"en","to":"zh"}"`
	j, err := NewJSONObjectByString(str)
	if err != nil {
		t.Error("error")
	}
	log.Println(j.data)
}

// func Test_ToJSON(t *testing.T) {
// 	j := NewJSONObject()
// 	// body := []byte(`{"from":"en","to":"zh"}"`)
// 	body := []byte(`{
// 		"test": {
// 			"string_array": ["asdf", "ghjk", "zxcv"],
// 			"array": [1, "2", 3],
// 			"arraywithsubs": [{"subkeyone": 1},
// 			{"subkeytwo": 2, "subkeythree": 3}],
// 			"int": 10,
// 			"float": 5.150,
// 			"bignum": 9223372036854775807,
// 			"string": "simplejson",
// 			"bool": true
// 		}
// 	}`)
// 	err := j.ToJSON(body)
// 	if err != nil {
// 		t.Error("解析错误")
// 		t.Error(err.Error())
// 	}
// 	// log.Println(j.Data)
// 	log.Println(j.Get("test").)
// }
