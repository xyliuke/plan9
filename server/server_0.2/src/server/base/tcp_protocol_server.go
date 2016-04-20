package base

import (
	"net"
	"common/timer"
	"time"
)

type TcpProtocolServer struct {
	*BaseServer
	op_map map[net.Conn]*tcpProtocolServerOperation
	id_map map[int]net.Conn
	operation_handler func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, rawData []byte)
}

func NewTcpProtocolServer(port int) *TcpProtocolServer {
	t := new(TcpProtocolServer)
	t.op_map = make(map[net.Conn]*tcpProtocolServerOperation)
	t.id_map = make(map[int]net.Conn)

	t.BaseServer = NewBaseServer(port)
	t.BaseServer.SetReadHandler(func(conn net.Conn, data []byte) {
		//设置读取数据处理
		op, ok := t.op_map[conn]
		if !ok {
			op = NewTcpProtocolServerOperation()
			op.setDisconnectedHandler(func(conn net.Conn, id int) {

				delete(t.op_map, conn)
				delete(t.id_map, id)

				if t.disconnected_handler != nil {
					t.disconnected_handler(conn)
				}

			})
			t.op_map[conn] = op
		}
		op.read(conn, data, t.operation_handler)
	})

	return t
}

func (this *TcpProtocolServer) SetOperationHandler(h func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, raw[]byte))  {
	this.operation_handler = h
}

type tcpProtocolServerOperation struct {
	*BaseTcpOperation

	disconnected_handler func(conn net.Conn, id int)
	conn net.Conn

	id int
	timer_id int64
	disconnected_time time.Duration
}

func NewTcpProtocolServerOperation() *tcpProtocolServerOperation {
	t := new(tcpProtocolServerOperation)
	t.BaseTcpOperation = NewBaseTcpOperation()
	t.disconnected_time = 10 * time.Minute
	return t
}

func (this *tcpProtocolServerOperation) setDisconnectedHandler(h func(conn net.Conn, id int)) {
	this.disconnected_handler = h
}

func (this *tcpProtocolServerOperation) read(conn net.Conn, data []byte, callback func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, rawData []byte)) {
	this.conn = conn

	timer.CancelInaccuracyTimer(this.timer_id)
	timer.NewInaccuracyTimerCallback(this.disconnected_time, func(id int64) {
		this.timer_id = id
	}, func(time int64) {
		if time != 0 {
			if this.disconnected_handler != nil {
				this.disconnected_handler(conn, this.id)
			}
		}
	})

	result, id, version, serverType, dataType, dataLen, protocolData, raw := this.OpData(conn, data)
	if result {
		//需要处理
		if callback != nil {
			callback(conn, id, version, serverType, dataType, dataLen, protocolData, raw)
		}
	}
}