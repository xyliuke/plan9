package connection

import (
	"testing"
)

func Test_Conn(t *testing.T)  {
	cs := new(ConnectionServerConfig)
	cs.Port = 9099
	NewConnectionServer(cs).Run()

	ch := make(chan int)
	<- ch
}
