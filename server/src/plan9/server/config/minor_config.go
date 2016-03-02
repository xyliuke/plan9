package config

import (
	"plan9/common/network/tcp"
	"net"
	"strconv"
)

type MinorConfigServer struct  {
	base *BaseConfig
	client *tcp.TcpCommonClient
}

func NewMinorConfigServer(serverID byte, serverType byte, version byte, port int, majorIP string, majorPort int) *MinorConfigServer {
	m := new(MinorConfigServer)
	mg := newMinorConfigServerManager(serverID, serverType, version)
	m.base = NewBaseConfig(serverID, serverType, version, port, mg)
	m.client = tcp.NewTcpCommonClient(newMinorConfigClientOperation(serverID, serverType, version))
	m.client.Connect(majorIP + ":" + strconv.Itoa(majorPort))
	return m
}

func (self* MinorConfigServer) Listen()  {
	self.base.Listen()
}



type minorConfigServerOperation struct {
	config *ConfigCommonServerOperation
}

func newMinorConfigServerOperation(serverID byte, serverType byte, version byte) *majorConfigServerOperation {
	m := new(majorConfigServerOperation)
	m.config = NewConfigCommonServerOperaion(serverID, serverType, version)
	return m
}

func (self* minorConfigServerOperation) GetID() string {
	return self.config.GetID()
}
//接收一个连接对象
func (self* minorConfigServerOperation) OpenConnection(conn net.Conn) {
	self.config.OpenConnection(conn)
}
//处理接收到的数据
func (self* minorConfigServerOperation) DealWithData(data []byte) bool {
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
func (self* minorConfigServerOperation) CloseConnection() {
	self.config.CloseConnection()
}


type minorConfigServerManager struct {
	manager *tcp.TcpCommonServerManager
	serverID byte
	serverType byte
	version byte
}

func newMinorConfigServerManager(serverID byte, serverType byte, version byte) *majorConfigServerManager {
	m := new(majorConfigServerManager)
	m.serverID = serverID
	m.serverType = serverType
	m.version = version
	m.manager = tcp.NewTcpCommonServerManager()
	return m
}

func (self *minorConfigServerManager) CreateConnection() tcp.TcpCommonServerOperationInterface {
	op := newMajorConfigServerOperation(self.serverID, self.serverType, self.version)
	return op
}

func (self *minorConfigServerManager) GetConnection(id string) tcp.TcpCommonServerOperationInterface {
	return self.manager.GetConnection(id)
}
//销毁一个连接处理对象
func (self *minorConfigServerManager) DistroyConnection(id string) {
	self.manager.DistroyConnection(id)
}


//从配置服务器中连接其他服务器操作
type minorConfigClientOperation struct {
	base *tcp.TcpCommonClientOperation
	serverID byte
	serverType byte
	version byte
}

func newMinorConfigClientOperation(serverID byte, serverType byte, version byte) *minorConfigClientOperation {
	m := new(minorConfigClientOperation)
	m.base = tcp.NewTcpCommonClientOperation()
	m.serverID =serverID
	m.serverType = serverType
	m.version = version
	return m
}

func (self *minorConfigClientOperation) GetID() string  {
	return self.base.GetID()
}

func (self *minorConfigClientOperation) OpenConnection(conn net.Conn)  {
	self.base.OpenConnection(conn)
}

func (self *minorConfigClientOperation) DealWithData(data []byte) bool  {
	ret := self.base.DealWithData(data)
	if !ret {
		//执行自己操作

	}
	return true
}

func (self *minorConfigClientOperation) Write(data []byte)  {
	self.base.Write(data)
}

func (self *minorConfigClientOperation) CloseConnection()  {
	self.base.CloseConnection()
}
