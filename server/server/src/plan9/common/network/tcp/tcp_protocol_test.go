package tcp
import (
	"testing"
	"fmt"
)


func Test_Ping(t *testing.T)  {
//	data := wrapPing4Protocol(LOGIN_SERVER_TYPE)
//	data := wrapPong4Protocol(LOGIN_SERVER_TYPE)

	data := CreateString4Protocol(byte(0x01), byte(0x02), MAIN_CONFIG_TYPE, "hello world xxxxaaa")
	SetProtocolVersion(data, 0x10)
	SetProtocolType(data, OTHER_CONFIG_TYPE, NORMAL_JSON_DATA_TYPE)
	SetProtocolData(data, []byte("123"), 3)
	d := append(data, 0xAA, 0xAB)
	fmt.Println("data byte : ", d)
	fmt.Println("one data : ", GetOneFullProtocol(d, len(d)))
	_, stp := GetServerType4Protocol(d, len(d))
	_, dtp := GetDataType4Protocol(d, len(d))
	fmt.Print("server type : ", stp, "\t")
	fmt.Print("data type : ", dtp, "\n")
	l := len(d)
	RemoveOneProtocol4Protocol(&l, d)
	fmt.Println("remove one protocol : ", d, "\t len : ", l)
}
