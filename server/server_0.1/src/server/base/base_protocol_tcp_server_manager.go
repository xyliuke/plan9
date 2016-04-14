package base

import (
	"net"
	"server/protocol"
	"common/timer"
	"time"
	"common/log"
)

const BUF_LEVEL_1 int = 2048 //默认2k的数据缓存
const BUF_LEVEL_2 int = BUF_LEVEL_1 * 8 //第二级20K的缓存
const BUF_LEVEL_3 int = BUF_LEVEL_1 * 16 //第三级40K的缓存
const BUF_LEVEL_4 int = BUF_LEVEL_1 * 32 //第四级64K的缓存

type BaseProtocolTcpServerOperationInterface interface  {
	GetID() int
	OpConnected(net.Conn)
	OpDisconnected()
	OpReadData(data []byte) bool
	OpWriteData(data []byte)
	GetProtocolBuf() []byte
	FinishOneProtocol()
}

type BaseProtocolTcpServerOperation struct {
	conn net.Conn
	buf []byte
	buf_len int
	timer_id int64
	id int
	time_for_disconnected time.Duration
	protocol_buf []byte
}

func NewBaseProtocolTcpServerOperation() *BaseProtocolTcpServerOperation {
	b := new(BaseProtocolTcpServerOperation)
	b.buf = make([]byte, BUF_LEVEL_1)
	b.buf_len = 0
	b.id = 0
	b.time_for_disconnected = 10 * time.Minute
	return b
}

func (this *BaseProtocolTcpServerOperation) SetTimeForDisconnected(t time.Duration)  {
	this.time_for_disconnected = t
}

func (this *BaseProtocolTcpServerOperation) OpConnected(conn net.Conn)  {
	this.conn = conn
}

func (this* BaseProtocolTcpServerOperation) GetID() int {
	return this.id
}

func (this *BaseProtocolTcpServerOperation) OpDisconnected() {
	this.conn = nil
}

func (this *BaseProtocolTcpServerOperation) OpReadData(data []byte) bool {

	this.copyBuf(data)

	ret := true
	for true {
		exist, id, _, _, dataType, _, _, flen, _ := protocol.ParseProtocol(this.buf, this.buf_len)
		if exist {
			this.id = id
			if protocol.IsPingByType(dataType) {
				log.I_NET(this.conn.LocalAddr(), "write to client", this.conn.RemoteAddr(), "pong")
				this.conn.Write(protocol.NewPongProtocol(id))
				this.FinishOneProtocol()
				ret = true
			} else {
				ret = false
				this.protocol_buf = this.buf[:(flen + protocol.PROTOCOL_HEADER_LEN)]
				break
			}
		} else {
			this.protocol_buf = nil
			break
		}
	}

	timer.CancelInaccuracyTimer(this.timer_id)
	timer.NewInaccuracyTimerCallback(this.time_for_disconnected, func(id int64) {
		this.timer_id = id
	}, func(time int64) {
		if time != 0 {
			this.OpDisconnected()
		}
	})

	return ret
}

func (this* BaseProtocolTcpServerOperation) OpWriteData(data []byte)  {
	this.conn.Write(data)
}

func (this *BaseProtocolTcpServerOperation) FinishOneProtocol() {
	this.buf_len = protocol.RemoveFristProtocol(this.buf, this.buf_len)
}

func (this *BaseProtocolTcpServerOperation) copyBuf(data []byte) {
	data_len := len(data) + this.buf_len
	if data_len > BUF_LEVEL_4 {
		log.E_NET("operation data error, the all buf size more than 64k")
		return
	}
	level := BUF_LEVEL_1
	if data_len < BUF_LEVEL_1 {
		level = BUF_LEVEL_1
	} else if data_len < BUF_LEVEL_2 {
		level = BUF_LEVEL_2
	} else if data_len < BUF_LEVEL_3 {
		level = BUF_LEVEL_3
	} else {
		level = BUF_LEVEL_4
	}
	if level != len(this.buf) {
		old := this.buf
		this.buf = make([]byte, level)
		copy(this.buf, old[0:this.buf_len])
	}
	copy(this.buf[this.buf_len:], data)
	this.buf_len += len(data)
}

func (this *BaseProtocolTcpServerOperation) GetProtocolBuf() []byte {
	return this.protocol_buf
}