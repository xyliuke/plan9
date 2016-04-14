package base

import "testing"

func Test_ProtocolTCP(t *testing.T)  {
	go func() {
		entity := new(BaseClientEntity)
		entity.ID = 0
		client := NewBaseProtocolTcpClient(entity, nil)
		client.Connect("127.0.0.1:9001")
	}()


	ser := NewBaseProtocolTcpServer(9001, nil)
	ser.Listen()
}