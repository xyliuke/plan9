package base

import (
	"net"
	"common/timer"
	"time"
	"errors"
	"fmt"
)

type TcpProtocolServer struct {
	*BaseServer
	op_map map[net.Conn]*tcpProtocolServerOperation
	id_map map[int]net.Conn
	operation_handler func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, rawData []byte)
	raw_operation_handler func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, rawData []byte)
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

		op.read(conn, data, t.operation_handler != nil, func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, rawData []byte) {
			if t.operation_handler != nil {
				t.operation_handler(conn, id, version, serverType, dataType, dataLen, data, rawData)
			}
			if t.raw_operation_handler != nil {
				t.raw_operation_handler(conn, id, version, serverType, dataType, dataLen, rawData)
			}
		})
	})

	return t
}

//设置协议数据处理后的handler,如果解压/加密后,该处理会将数据解析,将最终数据返回
func (this *TcpProtocolServer) SetOperationHandler(h func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, raw[]byte))  {
	this.operation_handler = h
}
//设置协议数据基本处理的handler,无论数据是否解压/加密,都将最原始的数据返回
func (this *TcpProtocolServer) SetRawOperationHandler(h func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, raw[]byte))  {
	this.raw_operation_handler = h
}

func (this *TcpProtocolServer) BindIDAndConnection(id int, conn net.Conn) {
	this.id_map[id] = conn
}

func (this *TcpProtocolServer) RemoveIDAndConnection(id int) {
	delete(this.id_map, id)
}

func (this *TcpProtocolServer) Write(id int, data []byte, callback func(error))  {
	conn, ok := this.id_map[id]
	if ok {
		err := this.write(conn, data)
		if callback != nil {
			callback(err)
		}
	} else {
		if callback != nil {
			callback(errors.New(fmt.Sprintf("write to client id error, the client id : %d  is not exist", id)))
		}
	}
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

func (this *tcpProtocolServerOperation) read(conn net.Conn, data []byte, parse bool, callback func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, rawData []byte)) {
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
	if parse {
		result, id, version, serverType, dataType, dataLen, protocolData, raw := this.OpData(conn, data)
		if result {
			//需要处理
			if callback != nil {
				callback(conn, id, version, serverType, dataType, dataLen, protocolData, raw)
			}
		}
	} else {
		result, id, version, serverType, dataType, dataLen, raw := this.OpRawData(conn, data)
		if result {
			//需要处理
			if callback != nil {
				callback(conn, id, version, serverType, dataType, dataLen, nil, raw)
			}
		}
	}
}