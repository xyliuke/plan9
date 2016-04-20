package base

import (
	"testing"
	"common/log"
)

func Test_Tcp(t *testing.T) {
	go func() {
		NewTcpProtocolClient().Connect("127.0.0.1:9001")
	}()

	NewTcpProtocolServer(9001).Listen(func(result bool, reason string) {
		log.I_NET("listen success")
	})
}
