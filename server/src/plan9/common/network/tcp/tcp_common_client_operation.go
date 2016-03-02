//处理tcp客户端常用操作
package tcp
import (
	"net"
	"plan9/common/util"
)

type TcpCommonClientOperationInterface interface  {
	GetID() string
	OpenConnection(conn net.Conn)
	DealWithData(data []byte) bool
	Write([]byte)
	CloseConnection()
}

type TcpCommonClientOperation struct  {
	id string	//唯一标示
	conn net.Conn	//tcp连接
	buf  []byte   //数据缓存
	len  int      //缓存数据的长度
}

func NewTcpCommonClientOperation() *TcpCommonClientOperation  {
	t := new(TcpCommonClientOperation)
	t.id = util.CreateID()
	t.buf = make([]byte, 10240)
	t.len = 0
	return t
}

func (self *TcpCommonClientOperation) GetID() string  {
	return self.id
}

func (self *TcpCommonClientOperation) OpenConnection(conn net.Conn)  {
	self.conn = conn
}

func (self *TcpCommonClientOperation) DealWithData(data []byte) bool  {
	copy(self.buf[self.len:], data)
	self.len += len(data)
	return false
}

func (self *TcpCommonClientOperation) Write(data []byte)  {
	if self.conn != nil {
		self.conn.Write(data)
	}
}

func (self *TcpCommonClientOperation) CloseConnection()  {
	if self.conn != nil {
		self.conn.Close()
	}
}
