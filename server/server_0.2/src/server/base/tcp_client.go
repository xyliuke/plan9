package base

import (
	"common/log"
	"common/timer"
	"net"
	"time"
	"server/protocol"
	"errors"
)

type BaseClient struct {
	auto_connect            bool
	reconnect_interval_time time.Duration //检测与服务器连接的周期
	conn                    net.Conn
	addr                    string

	ping_timer_id int64
	ping_interval time.Duration
	id int

	read_handler func(net.Conn, []byte)
	disconnected_handler func()
	connected_handler func(net.Conn)
}

func NewBaseClient() *BaseClient {
	t := new(BaseClient)
	t.auto_connect = true
	t.reconnect_interval_time = 2 * time.Second
	t.ping_interval = 30 * time.Second
	return t
}

func (this *BaseClient) SetReadHandler(h func(net.Conn, []byte)) {
	this.read_handler = h
}

func (this *BaseClient) SetConnectedHandler(h func(net.Conn)) {
	this.connected_handler = h
}

func (this *BaseClient) SetDisconnectedHandler(h func()) {
	this.disconnected_handler = h
}

func (this *BaseClient) Connect(addr string) {
	this.addr = addr
	conn, err := net.Dial("tcp", addr)
	if err == nil {
		log.I_NET(conn.LocalAddr(), " connect to ", conn.RemoteAddr(), " success")
		this.conn = conn
		this.reconnect_interval_time = 2 * time.Second

		this.doRead(conn)
	} else {
		log.E_NET(" connect to ", addr, " failure")
		if this.disconnected_handler != nil {
			this.disconnected_handler()
		}
		this.delayReconnect()
	}
}

func (this *BaseClient) SetAutoConnect(auto bool) {
	this.auto_connect = auto
}

func (this *BaseClient) GetConnection() net.Conn {
	return this.conn
}

func (this *BaseClient) Write(data []byte, callback func(error, []byte)) {
	err := this.write(data)
	if callback != nil {
		callback(err, data)
	}
}

func (this BaseClient) write(data []byte) error {
	if this.conn != nil {
		log.D_NET(this.conn.LocalAddr(), "write to server", this.conn.RemoteAddr(), " data : ", data)
		read_n := 0
		for true {
			n, err := this.conn.Write(data[read_n:])
			if err == nil {
				if read_n >= len(data) {
					return nil
				}
				read_n += n
			} else {
				return err
			}
		}
	}
	return errors.New("the connection is nil")
}

func (this BaseClient) ReConnect() {
	if this.auto_connect && "" != this.addr {
		log.I_NET("try to reconnect ", this.addr)
		this.Connect(this.addr)
	}
}

func (this *BaseClient) delayReconnect() {
	if !this.auto_connect {
		return
	}
	timer.NewInaccuracyTimerCallback(this.reconnect_interval_time, func(id int64) {
		if this.reconnect_interval_time > (60 * time.Second) {
			this.reconnect_interval_time = 2 * time.Second
		} else {
			this.reconnect_interval_time *= 2
		}
	}, func(time int64) {
		if time != 0 {
			this.ReConnect()
		}
	})
}

func (this BaseClient) doRead(conn net.Conn) {
	this.Connected()

	defer this.Disconnected()

	data := make([]byte, 2048)
	for {
		d_len, err := conn.Read(data)
		if err != nil {
			break
		}
		temp := data[0:d_len]
		log.D_NET(this.conn.LocalAddr(), "read from server", conn.RemoteAddr(), ", data : ", temp)
		this.ReadData(temp)
	}
}

func (this *BaseClient) Connected() {
	if this.connected_handler != nil {
		this.connected_handler(this.conn)
	}
	this.doPing()
}

func (this *BaseClient) Disconnected() {
	if this.disconnected_handler != nil {
		this.disconnected_handler()
	}
	this.conn.Close()
	this.ReConnect()
}

func (this *BaseClient) ReadData(data []byte) {
	if this.read_handler != nil {
		this.read_handler(this.conn, data)
	}
	this.doPing()
}


func (this *BaseClient) doPing() {
	timer.CancelInaccuracyTimer(this.ping_timer_id)
	timer.NewInaccuracyTimerCallback(this.ping_interval, func(id int64) {
		this.ping_timer_id = id
	}, func(time int64) {
		if time != 0 {
			log.I_NET(this.conn.LocalAddr(), " write to ", this.conn.RemoteAddr(), ", data : ping")
			this.Write(protocol.NewPingProtocol(this.id), nil)
			this.doPing()
		}
	})
}

type BaseClientOperation struct  {
	buf []byte
	buf_len int
	conn net.Conn
}

func NewBaseClientOperation(conn net.Conn) *BaseClientOperation {
	b := new(BaseClientOperation)
	b.buf = make([]byte, 0x400)
	b.buf_len = 0
	b.conn = conn
	return b
}

func (this *BaseClientOperation) DisconnectedFromServer() {

}

func (this *BaseClientOperation) ConnectedToServer(conn net.Conn) {
	this.conn = conn
}

func (this *BaseClientOperation) ReadDataFromServer(data []byte) bool {
	copy(this.buf[this.buf_len:], data)
	this.buf_len += len(data)
	ret := false
	for true {
		is, _, _, _, dt, _, _, _, _ := protocol.ParseProtocol(this.buf, this.buf_len)
		if is {
			if protocol.IsPongByType(dt) {
				ret = true
				this.FinishFirstProtocol()
			} else {
				ret = false
				break
			}
		} else {
			ret = true
			break
		}
	}

	return ret
}

func (this *BaseClientOperation) FinishFirstProtocol() {
	this.buf_len = protocol.RemoveFristProtocol(this.buf, this.buf_len)
}

func (this *BaseClientOperation) Write(data []byte) {
	if data == nil {
		log.E_NET("can not write nil data")
		return
	}
	if this.conn == nil {
		log.E_NET("can not write, because conn is nil")
		return
	}
	this.conn.Write(data)
}