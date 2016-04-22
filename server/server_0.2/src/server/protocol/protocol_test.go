package protocol

import (
	"fmt"
	"testing"
)

func Test_Protocolo(t *testing.T) {
	b := []byte("hello world hello world hello world")
	ret, p := NewProtocol(0x12345678, 9, 0x20, PROTOCOL_COMPRESS_JSON_DATA_TYPE, len(b), b)
	fmt.Println(ret, p)

	ret, id, ver, st, dt, l, d, fl, fd := ParseProtocol(p, len(p))
	fmt.Println(ret, id, ver, st, dt, l, d, fl, fd)

	ping := NewPingProtocol(0x01)
	fmt.Println("ping : ", ping)
	fmt.Println("is ping ", IsPing(ping, len(ping)))

	np := append(ping, '^')
	fmt.Println("np : ", np, len(np))
	fmt.Println("remove protocol : ", np, RemoveFristProtocol(np, len(np)))
}
