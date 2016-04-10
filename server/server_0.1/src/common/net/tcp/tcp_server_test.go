package tcp

import (
	"testing"
	"time"
)

func Test_Server(t *testing.T)  {

	go func() {
		cli := NewTcpClient(nil)
		cli.SetAutoConnect(true)
		cli.Connect("127.0.0.1:9001")
	}()

	time.Sleep(10 * time.Second)

	tcp := NewTcpServer(nil, 9001)
	tcp.Listen()
}
