package base

import (
	"common/net/tcp"
	"common/timer"
	"time"
	"server/protocol"
	"common/log"
	"net"
)

type BaseClientEntity struct {
	ID      int
	Version byte
}

type BaseProtocolTcpClient struct {
	client *tcp.TcpClient
	ping_timer_id int64
	ping_interval time.Duration
	manager tcp.TcpClientInterface
	buf []byte
	buf_len int
	messageManger *MessageManager

	entity *BaseClientEntity
}

func NewBaseProtocolTcpClient(entry *BaseClientEntity, manager tcp.TcpClientInterface) *BaseProtocolTcpClient {
	c := new(BaseProtocolTcpClient)
	c.client = tcp.NewTcpClient(c)
	c.client.SetAutoConnect(true)
	c.entity = entry
	c.ping_interval = 30 * time.Second
	c.buf = make([]byte, 0x400)
	c.buf_len = 0
	c.manager = manager
	return c
}

func (this *BaseProtocolTcpClient) GetConnection() net.Conn {
	return this.client.GetConnection()
}

func (this *BaseProtocolTcpClient) Connect(addr string)  {
	this.client.Connect(addr)
}

func (this *BaseProtocolTcpClient) Write(data []byte) {
	this.client.Write(data)
}

func (this *BaseProtocolTcpClient) doPing() {
	timer.CancelInaccuracyTimer(this.ping_timer_id)
	timer.NewInaccuracyTimerCallback(this.ping_interval, func(id int64) {
		this.ping_timer_id = id
	}, func(time int64) {
		if time != 0 {
			log.I_NET(this.client.GetConnection().LocalAddr(), " write to ", this.client.GetConnection().RemoteAddr(), ", data : ping")
			this.Write(protocol.NewPingProtocol(this.entity.ID))
		}
	})
}


func (this *BaseProtocolTcpClient) ClientConnect() {
	this.doPing()
	if this.messageManger != nil {
		this.messageManger.Add(this.entity.ID, this)
	}
	if this.manager != nil {
		this.manager.ClientConnect()
	}
}
func (this *BaseProtocolTcpClient) ClientDisconnect() {
	if this.messageManger != nil {
		this.messageManger.Remove(this.entity.ID)
	}
	if this.manager != nil {
		this.manager.ClientDisconnect()
	}
}
func (this *BaseProtocolTcpClient) ClientReadData(data []byte, data_len int) {
	this.doPing()

	copy(this.buf[this.buf_len:], data[0:data_len])
	this.buf_len += data_len

	if protocol.IsPong(this.buf, this.buf_len) {
		log.I_NET(this.client.GetConnection().LocalAddr(), "read from server", this.client.GetConnection().RemoteAddr(), ", data : pong")
		this.FinishOneProtocol()
	} else {
		if this.manager != nil {
			this.manager.ClientReadData(this.buf, this.buf_len)
		}
	}
}

func (this* BaseProtocolTcpClient) FinishOneProtocol()  {
	this.buf_len = protocol.RemoveFristProtocol(this.buf, this.buf_len)
}

//message接口
func (this *BaseProtocolTcpClient) ReceiveMessage(data []byte)  {
	this.Write(data)
}

func (this *BaseProtocolTcpClient) ReceiveMessageID(id int, data []byte) {
	this.Write(data)
}

func (this *BaseProtocolTcpClient) GetMessageType() byte {
	return MESSAGE_TYPE_CLIENT
}

func (this *BaseProtocolTcpClient) SetMessageManager(m *MessageManager)  {
	this.messageManger = m
}

func (this *BaseProtocolTcpClient) GetMessageManager() *MessageManager  {
	return this.messageManger
}