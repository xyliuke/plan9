package base

import (
	"net"
	"server/protocol"
)

type TcpProtocolClient struct  {
	*BaseClient

	operation_handler  func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, raw[]byte)
	*BaseTcpOperation
}

func NewTcpProtocolClient() *TcpProtocolClient {
	c := new(TcpProtocolClient)
	c.BaseTcpOperation = NewBaseTcpOperation()

	c.BaseClient = NewBaseClient()
	c.BaseClient.SetAutoConnect(true)
	c.BaseClient.SetReadHandler(func(conn net.Conn, data []byte) {
		c.OpData(conn, data)
	})

	return c
}

func (this *TcpProtocolClient) SetOperationHandler(h func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, raw[]byte))  {
	this.operation_handler = h
}

func (this *TcpProtocolClient) read(data []byte)  {
	result, id, version, serverType, dataType, dataLen, protocolData, raw := this.OpData(this.conn, data)
	if result {
		//需要处理
		if this.operation_handler != nil {
			this.operation_handler(this.conn, id, version, serverType, dataType, dataLen, protocolData, raw)
		}
	}
}

func (this *TcpProtocolClient) FinishFirstProtocol() {
	this.buf_len = protocol.RemoveFristProtocol(this.buf, this.buf_len)
}


