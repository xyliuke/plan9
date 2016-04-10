package tcp

import (
	"net"
	"common/log"
	"time"
	"common/timer"
)

type TcpClientInterface interface {
	Connect()
	Disconnect()
	ReadData([]byte)
}


type TcpClient struct {
	auto_connect bool
	reconnect_interval_time time.Duration//检测与服务器连接的周期
	manager TcpClientInterface
	conn net.Conn
	addr string
}

func NewTcpClient(m TcpClientInterface) *TcpClient {
	t := new(TcpClient)
	t.auto_connect = false
	t.reconnect_interval_time = 2 * time.Second
	return t
}

func (this *TcpClient) Connect(addr string) {
	this.addr = addr
	conn, err := net.Dial("tcp", addr)
	if err == nil {
		log.I_NET("connect to ", addr, " success")
		this.conn = conn
		this.reconnect_interval_time = 2 * time.Second
		this.doRead(conn)
	} else {
		log.E_NET("connect to ", addr, " failure")
		this.delayReconnect()
	}
}

func (this* TcpClient) SetAutoConnect(auto bool)  {
	this.auto_connect = auto
}

func (this TcpClient) Write(data []byte)  {
	if this.conn != nil {
		this.conn.Write(data)
	}
}

func (this TcpClient) ReConnect()  {
	if this.auto_connect && "" != this.addr {
		log.I_NET("try to reconnect ", this.addr)
		this.Connect(this.addr)
	}
}

func (this *TcpClient) delayReconnect() {
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

func (this TcpClient) doRead(conn net.Conn) {
	if this.manager != nil {
		this.manager.Connect()
	}
	data := make([]byte, 2048)
	defer conn.Close()
	for {
		len, err := conn.Read(data)
		if err != nil {
			break
		}
		temp := data[0:len]
		log.I("read data from ", conn.RemoteAddr(), ", the content : ", temp)
		if this.manager != nil {
			this.manager.ReadData(temp)
		}
	}

	if this.manager != nil {
		this.manager.Disconnect()
	}

	defer this.ReConnect()
}
