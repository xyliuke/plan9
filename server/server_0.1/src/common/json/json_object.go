package json

import (
	"encoding/json"
)

type JSONObject map[string]interface{}
type JSONArray  []interface{}

func NewJSONEmpty() JSONObject {
	return make(JSONObject)
}

func NewJSON(data []byte) JSONObject {
	var m JSONObject
	json.Unmarshal(data, &m)
	return m
}

func NewJSONByString(str string) JSONObject {
	return NewJSON([]byte(str))
}

func (this JSONObject) ToString() string {
	b, e := json.Marshal(this)
	if e == nil {
		return string(b)
	}
	return ""
}
//获取成员相关操作

func (this JSONObject) HasMember(key string) bool {
	return this[key] != nil
}

func (this JSONObject) GetJSON(key string) (bool, JSONObject) {
	j, is := (this)[key].(map[string]interface{})
	if is {
		a := JSONObject(j)
		return is, a
	}
	j, is = this[key].(JSONObject)
	return is, j
}

func (this JSONObject) Get(key string) interface{}  {
	return this[key]
}

func (this JSONObject) GetString(key string) (bool, string) {
	str, err := this[key].(string)
	return err, str
}

func (this JSONObject) GetInt(key string) (bool, int)  {
	is, i := this.GetDouble(key)
	return is, int(i)
}

func (this JSONObject) GetFloat(key string) (bool, float32)  {
	is, i := this.GetDouble(key)
	return is, float32(i)
}

func (this JSONObject) GetDouble(key string) (bool, float64)  {
	str, err := this[key].(float64)
	return err, str
}

func (this JSONObject) GetBool(key string) bool  {
	is, b := this[key].(bool)
	if is {
		return b
	}
	return is
}

//获取数据相关操作

func (this JSONObject) GetArray(key string) (bool, JSONArray)  {
	a, is := this[key].([]interface{})
	if is {
		return is, JSONArray(a)
	}
	a, is = this[key].(JSONArray)
	if is {
		return is, a
	}
	return is, nil
}

func (this JSONObject) Enumerate(callback func(key string, value interface{})())  {
	for i, v := range this {
		callback(i, v)
	}
}

func (this JSONObject) GetStringByIndex(key string, index int) (bool, string) {
	a, is := this[key].([]interface{})
	if is {
		if index < len(a) {
			item, ok := a[index].(string)
			return ok, item
		}
	}
	return false, ""
}

func (this JSONObject) GetDoubleByIndex(key string, index int) (bool, float64) {
	a, is := this[key].([]interface{})
	if is {
		if index < len(a) {
			item, ok := a[index].(float64)
			return ok, item
		}
	}
	return false, 0
}


func (this JSONObject) GetBoolByIndex(key string, index int) (bool) {
	a, is := this[key].([]interface{})
	if is {
		if index < len(a) {
			item, ok := a[index].(bool)
			if ok {
				return item
			}
			return false
		}
	}
	return false
}

//添加数据相关操作

func (this JSONObject) Put(key string, value interface{})  {
	this[key] = value
}
//向数组中添加元素
func (this JSONObject) AddToArray(key string, value interface{})  {
	if this[key] == nil {
		this[key] = make([]interface{}, 0)
	}
	i, is := this[key].([]interface{})
	if is {
		i = append(i, value)
		this[key] = i
	}
}

//删除普通key值
func (this JSONObject) Remove(key string) {
	delete(this, key)
}

//删除数组中元素,需要给出数组的key值和元素在数组中的索引
func (this JSONObject) RemoveIndex(key string, index int)  {
	is, arr := this.GetArray(key)
	if is {
		a := append(arr[0:index], arr[index + 1 :]...)
		this[key] = a
	}
}

func (this JSONObject) RemoveLast(key string)  {
	is, arr := this.GetArray(key)
	if is {
		a := append(arr[0:len(arr) - 1])
		this[key] = a
	}
}

func (this JSONObject) RemoveFirst(key string)  {
	is, arr := this.GetArray(key)
	if is {
		a := append(arr[1:])
		this[key] = a
	}
}

func (this JSONArray) GetString(index int) (bool, string) {
	if index >= len(this){
		return false, ""
	}
	item, ok := this[index].(string)
	return ok, item
}

func (this JSONArray) GetBool(index int) bool {
	if index >= len(this){
		return false
	}
	item, ok := this[index].(bool)
	if ok {
		return item
	}
	return false
}

func (this JSONArray) GetInt(index int) (bool, int) {
	if index >= len(this){
		return false, 0
	}
	item, ok := this[index].(float64)
	return ok, int(item)
}

func (this JSONArray) GetFloat(index int) (bool, float32) {
	if index >= len(this){
		return false, 0
	}
	item, ok := this[index].(float64)
	return ok, float32(item)
}

func (this JSONArray) GetDouble(index int) (bool, float64) {
	if index >= len(this){
		return false, 0
	}
	item, ok := this[index].(float64)
	return ok, item
}

func (this JSONArray) GetJSON(index int) (bool, JSONObject)  {
	if index >= len(this){
		return false, nil
	}
	item, ok := this[index].(map[string]interface{})
	if ok {
		return ok, JSONObject(item)
	}
	return false, nil
}

func (this JSONArray) Get(index int) interface{}  {
	if index >= len(this){
		return nil
	}
	return this[index]
}

func (this JSONArray) GetFirst() interface{}  {
	return this.Get(0)
}

func (this JSONArray) GetLast() interface{} {
	return this.Get(len(this) - 1)
}

func (this JSONArray) Enumerate(callback func(int, interface{})())  {
	for i, v := range this {
		callback(i, v)
	}
}