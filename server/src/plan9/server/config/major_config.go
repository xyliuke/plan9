package config

import (
	"net"
	"plan9/common/network/tcp"
)

type MajorConfigServer struct {
	base *BaseConfig
}

func NewMajorConfigServer(serverID byte, serverType byte, version byte, port int) *MajorConfigServer {
	m := new(MajorConfigServer)
	mg := newMajorConfigServerManager(serverID, serverType, version)
	m.base = NewBaseConfig(serverID, serverType, version, port, mg)
	return m
}

func (self* MajorConfigServer) Listen()  {
	self.base.Listen()
}



type majorConfigServerOperation struct {
	config *ConfigCommonServerOperation
}

func newMajorConfigServerOperation(serverID byte, serverType byte, version byte) *majorConfigServerOperation {
	m := new(majorConfigServerOperation)
	m.config = NewConfigCommonServerOperaion(serverID, serverType, version)
	return m
}

func (self* majorConfigServerOperation) GetID() string {
	return self.config.GetID()
}
//接收一个连接对象
func (self* majorConfigServerOperation) OpenConnection(conn net.Conn) {
	self.config.OpenConnection(conn)
}
//处理接收到的数据
func (self* majorConfigServerOperation) DealWithData(data []byte) bool {
	ret := self.config.DealWithData(data)
	if !ret {
		//需要自己处理

		if ret {
			self.config.DeleteFinishProtocol()
		}
		return ret
	}
	return true
}
//关闭连接
func (self* majorConfigServerOperation) CloseConnection() {
	self.config.CloseConnection()
}

type majorConfigServerManager struct {
	manager *tcp.TcpCommonServerManager
	serverID byte
	serverType byte
	version byte
}

func newMajorConfigServerManager(serverID byte, serverType byte, version byte) *majorConfigServerManager {
	m := new(majorConfigServerManager)
	m.serverID = serverID
	m.serverType = serverType
	m.version = version
	m.manager = tcp.NewTcpCommonServerManager()
	return m
}

func (self *majorConfigServerManager) CreateConnection() tcp.TcpCommonServerOperationInterface {
	op := newMajorConfigServerOperation(self.serverID, self.serverType, self.version)
	return op
}

func (self *majorConfigServerManager) GetConnection(id string) tcp.TcpCommonServerOperationInterface {
	return self.manager.GetConnection(id)
}
//销毁一个连接处理对象
func (self *majorConfigServerManager) DistroyConnection(id string) {
	self.manager.DistroyConnection(id)
}
