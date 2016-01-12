package protocol

import (
	"testing"
	_"server"
	"fmt"
)


func Test_GetTypeValue(t *testing.T) {
	body := []byte{'^', 0x01, 0x0, 0x0, 0x0, 0x05, 'h', 'e', 'l', 'l', 'o'}
	err, _ := GetTypeValue(body)
	if err != nil {
		fmt.Println(err.ErrorString)
	} else {
		err1, str := GetStringData(body)
		if err1 == nil {
			fmt.Println("the data : ", str)
		} else {
			fmt.Println("the err : ", err1.ErrorString)
		}
	}
}
