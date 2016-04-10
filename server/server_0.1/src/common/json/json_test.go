package json

import (
	"testing"
	"fmt"
)

//import (
//	"log"
//	"testing"
//)
//
//
//func Test_NewJSONObject(t *testing.T) {
//	body := []byte(`{
//		"test": {
//			"string_array": ["asdf", "ghjk", "zxcv"],
//			"array": [1, "2", 3],
//			"arraywithsubs": [{"subkeyone": 1},
//			{"subkeytwo": 2, "subkeythree": 3}],
//			"int": 10,
//			"float": 5.150,
//			"bignum": 9223372036854775807,
//			"string": "simplejson",
//			"bool": true
//		}
//	}`)
//	j, err := NewJSONObject(body)
//	if err != nil {
//		t.Log("new json object error")
//	}
//
//	if j.HasMember("test") {
//		t.Log("has test key")
//	}
//
//	str, err := j.Get("test").Get("string_array").StringArray()
//	if err != nil {
//		t.Log("new json object error")
//	}
//	log.Println(str)
//}
//
//func Test_NewJSONObjectByString(t *testing.T) {
//	str := `{"from":"en","to":"zh"}"`
//	j, err := NewJSONObjectByString(str)
//	if err != nil {
//		t.Error("error")
//	}
////	log.Println(j.data)
//	log.Println(j.Get("from").String())
//}

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


func Test_JSON(t *testing.T)  {
	body := []byte(`{
 		"test": {
 			"string_array": ["asdf", "ghjk", "zxcv"],
 			"array": [1, "2", 3, true],
 			"arraywithsubs": [{"subkeyone": 1},
 			{"subkeytwo": 2, "subkeythree": 3}],
 			"int": 10,
 			"float": 5.150,
 			"bignum": 9223372036854775807,
 			"string": "simplejson",
 			"bool": true
 		}
 	}`)
	m := NewJSON(body)
	fmt.Println(m)

	s1, s2 := m.GetJSON("test")
	fmt.Println(s1, s2)

	fmt.Println("exist test : ", m.HasMember("test"))

	fmt.Println(s2.GetString("string"))
	fmt.Println(s2.GetInt("int"))
	fmt.Println(s2.GetFloat("float"))
	fmt.Println(s2.GetBool("bool"))
	fmt.Println(s2.GetArray("array"))
	fmt.Println(s2.GetStringByIndex("array", 1))
	fmt.Println(s2.GetDoubleByIndex("array", 0))
	fmt.Println(s2.GetBoolByIndex("array", 3))

	s2.Put("string", "new string")
	fmt.Println(s2.GetString("string"))

	s2.AddToArray("array", 3.14)
	fmt.Println(s2.GetArray("array"))

	s2.Remove("string")
	s2.RemoveIndex("array", 1)
	fmt.Println(s2)

	is, arr := s2.GetArray("array")
	if is {
		fmt.Println("array enum ...")
		arr.Enumerate(func(index int, value interface{}) {
			fmt.Print(index, " - ", value, " ")
		})
		fmt.Println("\narray enum ...")
	} else {
		fmt.Println("array not enum ...")
	}

	s2.RemoveLast("array")

	s2.Enumerate(func(key string, value interface{}) {
		fmt.Println(key, value)
	})

	s2.RemoveFirst("array")

	fmt.Println(s2.ToString())

	s2.AddToArray("new_arr", 2.33)
	fmt.Println(s2.ToString())


	fmt.Println("new json")

	ss := NewJSONEmpty()
	fmt.Println(ss.ToString())
	ss.Put("a", "a")
	ss.AddToArray("b", "b1")
	fmt.Println(ss.ToString())
	//is, arr := s2.GetArray("array")
	//if is {
		//fmt.Println("array : ", arr)
		//fmt.Println("array : ", arr)
		//fmt.Println(s2)
	//}

	//slice := make([]int, 10);
	//for i := 0; i < 10; i ++ {
	//	slice[i] = i
	//}
	//fmt.Println(slice)
	//delete_index := 5;
	//slice = append(slice[0:delete_index], slice[delete_index + 1:]...)
	//fmt.Println(slice)
	//test, _ := m["test"].(map[string]interface{})
	//arr := test["array"]
	//fmt.Println(reflect.TypeOf(test))
	//fmt.Println(reflect.TypeOf(arr))
}