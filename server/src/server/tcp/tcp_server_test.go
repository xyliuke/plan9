package tcp


import (
	"testing"
	_ "fmt"
)

//var i int

func Test_CreateTcpServer(t *testing.T)  {
//	data := make([]byte, 20)
//	data[0] = 50
//	data[1] = '3'
//
//	data[2] = 60
//	data[3] = 61
//	data[1] = '\r'
//	data[2] = '\n'

//	data1 := make([]byte, 200)
//	data1[0] = 'a'
//	data1[1] = 'b'

//	data1 = append(data1, data...)

//	l := 2
//	tmp := data[2:3]
//	fmt.Println(tmp)
//
//	fmt.Println(data)
//	copy(data[0:], data[2:])
//	fmt.Println(data)



//	fmt.Println(i)
//	i ++
//	fmt.Println(i)
	tp := CreateTcpServer(8081)
	tp.Listen()
}