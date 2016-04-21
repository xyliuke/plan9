package base

import (
	"common/log"
	"net"
	"server/protocol"
)

const BUF_LEVEL_1 int = 4 * 1048  //默认4k的数据缓存
const BUF_LEVEL_2 int = 16 * 1024 //第二级16K的缓存
const BUF_LEVEL_3 int = 32 * 1024 //第三级32K的缓存
const BUF_LEVEL_4 int = 64 * 1024 //第四级64K的缓存

type BaseTcpOperation struct {
	buf                   []byte
	buf_len               int
}

func NewBaseTcpOperation() *BaseTcpOperation {
	b := new(BaseTcpOperation)
	b.buf = make([]byte, BUF_LEVEL_1)
	b.buf_len = 0
	return b
}

func (this *BaseTcpOperation) FinishOneProtocol() {
	this.buf_len = protocol.RemoveFristProtocol(this.buf, this.buf_len)
}

func (this *BaseTcpOperation) copyBuf(data []byte) {
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

func (this *BaseTcpOperation) OpData(conn net.Conn, data []byte) (result bool, id int, version byte, serverType byte, dataType byte, dataLen int, protocolData []byte, rawData []byte) {
	this.copyBuf(data)

	for true {
		exist, id, version, serverType, dataType, oLen, rawData, protocolLen, protocolData := protocol.ParseProtocol(this.buf, this.buf_len)
		if exist {
			if protocol.IsPingByType(dataType) {
				log.I_NET(conn.LocalAddr(), "write to client", conn.RemoteAddr(), "pong")
				conn.Write(protocol.NewPongProtocol(id))
				this.FinishOneProtocol()
			} else if protocol.IsPongByType(dataType) {
				log.I_NET("client", conn.LocalAddr(), "read from server", conn.RemoteAddr(), " pong")
				this.FinishOneProtocol()
			} else {

				d := make([]byte, protocolLen)
				copy(d, protocolData)

				np := make([]byte, oLen)
				copy(np, rawData)

				this.FinishOneProtocol()
				return true, id, version, serverType, dataType, protocolLen, d, np
			}
		} else {
			return false, 0, 0, 0, 0, 0, nil, nil
		}
	}

	return false, 0, 0, 0, 0, 0, nil, nil
}

func (this *BaseTcpOperation) OpRawData(conn net.Conn, data []byte) (result bool, id int, version byte, serverType byte, dataType byte, dataLen int, rawData []byte) {
	this.copyBuf(data)

	for true {

		exist, id, version, serverType, dataType, oLen, rawData := protocol.ParseRawProtocol(this.buf, this.buf_len)
		if exist {
			if protocol.IsPingByType(dataType) {
				log.I_NET(conn.LocalAddr(), "write to client", conn.RemoteAddr(), "pong")
				conn.Write(protocol.NewPongProtocol(id))
				this.FinishOneProtocol()
			} else if protocol.IsPongByType(dataType) {
				log.I_NET("client", conn.LocalAddr(), "read from server", conn.RemoteAddr(), " pong")
				this.FinishOneProtocol()
			} else {

				d := make([]byte, oLen)
				copy(d, rawData)

				this.FinishOneProtocol()
				return true, id, version, serverType, dataType, oLen, d
			}
		} else {
			return false, 0, 0, 0, 0, 0, nil
		}
	}
	return false, 0, 0, 0, 0, 0, nil
}