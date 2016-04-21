package base

import (
	"net"
	"server/protocol"
)

type TcpProtocolClient struct  {
	*BaseClient

	operation_handler  func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, raw[]byte)
	raw_operation_handler  func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, raw[]byte)
	*BaseTcpOperation
}

func NewTcpProtocolClient() *TcpProtocolClient {
	c := new(TcpProtocolClient)
	c.BaseTcpOperation = NewBaseTcpOperation()

	c.BaseClient = NewBaseClient()
	c.BaseClient.SetAutoConnect(true)
	c.BaseClient.SetReadHandler(func(conn net.Conn, data []byte) {
		c.read(data)
	})

	return c
}

func (this *TcpProtocolClient) SetOperationHandler(h func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, raw[]byte))  {
	this.operation_handler = h
}

func (this *TcpProtocolClient) SetRawOperationHandler(h func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, raw[]byte))  {
	this.raw_operation_handler = h
}

func (this *TcpProtocolClient) read(data []byte) {
	if this.operation_handler != nil {
		result, id, version, serverType, dataType, dataLen, protocolData, raw := this.OpData(this.conn, data)
		if result {
			//需要处理
			this.operation_handler(this.conn, id, version, serverType, dataType, dataLen, protocolData, raw)
			if this.raw_operation_handler != nil {
				this.raw_operation_handler(this.conn, id, version, serverType, dataType, dataLen, raw)
			}
		}
	} else {
		result, id, version, serverType, dataType, dataLen, raw := this.OpRawData(this.conn, data)
		if result {
			//需要处理
			if this.raw_operation_handler != nil {
				this.raw_operation_handler(this.conn, id, version, serverType, dataType, dataLen, raw)
			}
		}
	}
}

func (this *TcpProtocolClient) FinishFirstProtocol() {
	this.buf_len = protocol.RemoveFristProtocol(this.buf, this.buf_len)
}


