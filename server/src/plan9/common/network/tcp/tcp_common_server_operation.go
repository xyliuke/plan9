package tcp
import (
	"net"
	"plan9/common/util"
)

//处理tcp服务器数据管理器的通用接口
type TcpCommonServerManagerInterface interface  {
	//创建一个新的连接处理对象
	CreateConnection() TcpCommonServerOperationInterface
	//通过id找到相应的连接处理对象
	GetConnection(id string) TcpCommonServerOperationInterface
	//销毁一个连接处理对象
	DistroyConnection(id string)
}

//处理tcp服务器数据的接口
type TcpCommonServerOperationInterface interface {
	//获取id
	GetID() string
	//接收一个连接对象
	OpenConnection(conn net.Conn)
	//处理接收到的数据,处理完成返回true,否则返回false
	DealWithData(data []byte) bool
	//关闭连接
	CloseConnection()
}

//处理tcp服务器管理器的基本对象结构体
type TcpCommonServerManager struct  {
	servers map[string]TcpCommonServerOperationInterface
}

//实例化一个tcp处理管理器
func NewTcpCommonServerManager() *TcpCommonServerManager  {
	manager := new(TcpCommonServerManager)
	manager.servers = make(map[string]TcpCommonServerOperationInterface, 20)
	return manager
}

func (self *TcpCommonServerManager) CreateConnection() TcpCommonServerOperationInterface  {
	op := NewTcpCommonServerOperation()
	self.servers[op.GetID()] = op
	return op
}

func (self *TcpCommonServerManager) GetConnection (id string) TcpCommonServerOperationInterface  {
	return self.servers[id]
}

func (self *TcpCommonServerManager) DistroyConnection(id string)  {
	op := self.servers[id]
	if op != nil {
		op.CloseConnection()
		delete(self.servers, id)
	}
}


//处理tcp服务器数据的基本操作类
type TcpCommonServerOperation struct {
	id   string   //唯一标示
	conn net.Conn //tcp连接
	buf  []byte   //数据缓存
	len  int      //缓存数据的长度
}

func NewTcpCommonServerOperation() *TcpCommonServerOperation  {
	t := new(TcpCommonServerOperation)
	t.id = util.CreateID()
	t.buf = make([]byte, MaxProtocolLength())
	t.len = 0
	return t
}

func (self *TcpCommonServerOperation) GetID() string  {
	return self.id
}

func (self *TcpCommonServerOperation) OpenConnection(conn net.Conn)  {
	self.conn = conn
}

func (self *TcpCommonServerOperation) DealWithData(data []byte) bool  {
	copy(self.buf[self.len:], data)
	self.len += len(data)
	return false
}

func (self *TcpCommonServerOperation) CloseConnection()  {
	if self.conn != nil {
		self.conn.Close()
	}
}

func (self* TcpCommonServerOperation) GetBuf() []byte {
	return self.buf
}

func (self* TcpCommonServerOperation) GetLen() int {
	return self.len
}

func (self* TcpCommonServerOperation) Write(data []byte) {
	self.conn.Write(data)
}

func (self* TcpCommonServerOperation) SetLen(l int)  {
	self.len = l
}