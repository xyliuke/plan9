package config

import (
	"plan9/server"
	"plan9/common/network/tcp"
	"net"
	"time"
)

type BaseConfig struct {
	entity *server.ServerEntity   //配置服务器的基本配置
	server *tcp.TcpCommonServer	  //服务器部分
}

func NewBaseConfig(serverID byte, serverType byte, version byte, port int, manager tcp.TcpCommonServerManagerInterface) *BaseConfig {
	bc := new(BaseConfig)
	bc.entity = server.NewServerEntity(serverID, serverType, version)
	bc.server = tcp.NewTcpCommonServer(port, manager)
	return bc
}

func (self *BaseConfig) Listen()  {
	self.server.Listen()
}

type ConfigCommonServerOperation struct {
	op *tcp.TcpCommonServerOperation
	serverID byte
	serverType byte
	version byte
}

func NewConfigCommonServerOperaion(serverID byte, serverType byte, version byte) *ConfigCommonServerOperation {
	c := new(ConfigCommonServerOperation)
	c.op = tcp.NewTcpCommonServerOperation()
	c.serverID = serverID
	c.serverType = serverType
	c.version = version
	return c
}

func (self* ConfigCommonServerOperation) GetID() string {
	return self.op.GetID()
}
//接收一个连接对象
func (self* ConfigCommonServerOperation) OpenConnection(conn net.Conn) {
	self.op.OpenConnection(conn)
}
//处理接收到的数据
func (self* ConfigCommonServerOperation) DealWithData(data []byte) bool {
	self.op.DealWithData(data)
	//执行config服务器能用操作
	err, tp := tcp.GetTypeValue4Protocol(self.op.GetBuf(), self.op.GetLen())
	if err == nil {
		ret := false
		if tcp.IsPingType4Protocol(tp) {
			self.op.Write(tcp.CreatePong4Protocol(self.serverID, self.version, self.serverType))
			ret = true
		}

		if ret {
			self.DeleteFinishProtocol()
		}
		return ret
	}
	return false
}
//关闭连接
func (self* ConfigCommonServerOperation) CloseConnection() {
	self.op.CloseConnection()
}

func (self* ConfigCommonServerOperation) DeleteFinishProtocol()  {
	l := self.op.GetLen()
	tcp.RemoveOneProtocol4Protocol(&l, self.op.GetBuf())
	self.op.SetLen(l)
}

type ConfigCommonClientOperation struct  {
	base *tcp.TcpCommonClientOperation
	timer *time.Timer
	duration time.Duration
	serverID, serverType, version byte
}

func NewConfigCommonClientOperation(serverID byte, serverType byte, version byte) *ConfigCommonClientOperation  {
	c := new(ConfigCommonClientOperation)
	c.base = tcp.NewTcpCommonClientOperation()
	c.duration = time.Second * 15
	c.serverID = serverID
	c.serverType = serverType
	c.version = version
	return c
}

func (self *ConfigCommonClientOperation) GetID() string {
	return self.base.GetID()
}

func (self *ConfigCommonClientOperation) OpenConnection(conn net.Conn)  {
	self.base.OpenConnection(conn)
	if self.timer != nil {
		self.timer.Stop()
	}
	self.timer = time.NewTimer(self.duration)
	go self.delayDoPing()
}

func (self* ConfigCommonClientOperation) delayDoPing()  {

	if self.timer == nil {
		self.timer = time.NewTimer(self.duration)
	} else {
		self.timer.Reset(self.duration)
	}

	select {
	case self.timer.C:
		self.Write(tcp.CreatePing4Protocol(self.serverID, self.serverType, self.version))
	}
}

func (self *ConfigCommonClientOperation) DealWithData(data []byte) bool  {
	ret := self.base.DealWithData(data)

	go self.delayDoPing()

	if !ret {
		//执行自己操作
	}
	return true
}

func (self *ConfigCommonClientOperation) Write(data []byte)  {
	self.base.Write(data)
}

func (self *ConfigCommonClientOperation) CloseConnection()  {
	self.base.CloseConnection()
}

































