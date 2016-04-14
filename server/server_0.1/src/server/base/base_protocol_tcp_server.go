package base

import (
	"common/net/tcp"
	"net"
)

type BaseProtocolTcpServer struct {
	server *tcp.TcpServer
	op_map map[net.Conn]BaseProtocolTcpServerOperationInterface
	id_conn_map map[int]net.Conn
	message *MessageManager
	manager tcp.TcpServerInterface
}

func NewBaseProtocolTcpServer(port int, manager tcp.TcpServerInterface) *BaseProtocolTcpServer {
	b := new(BaseProtocolTcpServer)
	b.server = tcp.NewTcpServer(b, port)
	b.op_map = make(map[net.Conn]BaseProtocolTcpServerOperationInterface)
	b.id_conn_map = make(map[int]net.Conn)
	b.manager = manager
	return b
}

func (this *BaseProtocolTcpServer) Listen() {
	this.server.Listen()
}

func (this *BaseProtocolTcpServer) Write(id int, data []byte) {
	this.server.Write(id, data)
}

func (this *BaseProtocolTcpServer) ReceiveMessage(data []byte)  {
	for id, _ := range this.id_conn_map {
		this.Write(id, data)
	}
}

func (this *BaseProtocolTcpServer) ReceiveMessageID(id int, data []byte)  {
	this.Write(id, data)
}

func (this *BaseProtocolTcpServer) GetMessageType() byte {
	return MESSAGE_TYPE_SERVER
}

func (this *BaseProtocolTcpServer) Connected(conn net.Conn)  {
	if this.manager != nil {
		this.manager.Connected(conn)
	} else {
		op := NewBaseProtocolTcpServerOperation()
		this.op_map[conn] = op
		op.OpConnected(conn)
	}
}

func (this *BaseProtocolTcpServer) NewOperation(conn net.Conn, op BaseProtocolTcpServerOperationInterface)  {
	this.op_map[conn] = op
}

func (this BaseProtocolTcpServer) Disconnected(conn net.Conn)  {
	op, ok := this.op_map[conn]
	if ok {
		op.OpDisconnected()
	}
	if this.manager != nil {
		this.manager.Disconnected(conn)
	}
}

func (this* BaseProtocolTcpServer) ReadData(conn net.Conn, data []byte)  {
	op, ok := this.op_map[conn]
	if ok {
		deal := op.OpReadData(data)
		if deal {
			this.bindID(op.GetID(), conn)
		} else {
			if this.manager != nil {
				if op.GetProtocolBuf() != nil {
					this.manager.ReadData(conn, op.GetProtocolBuf())
					op.FinishOneProtocol()
				}
			}
		}
	}
}

func (this* BaseProtocolTcpServer) WriteData(id int, data []byte)  {
	co, ok := this.id_conn_map[id]
	if ok {
		op, ook := this.op_map[co]
		if ook {
			op.OpWriteData(data)
		}
	}
}

func (this *BaseProtocolTcpServer) bindID(id int, conn net.Conn)  {
	if id != 0 {
		this.id_conn_map[id] = conn
		if this.message != nil {
			this.message.Add(id, this)
		}
	}
}

func (this *BaseProtocolTcpServer) SetMessageManager(m *MessageManager)  {
	this.message = m
}

func (this *BaseProtocolTcpServer) GetMessageManager() *MessageManager  {
	return this.message
}