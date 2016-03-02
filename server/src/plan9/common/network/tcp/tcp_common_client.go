//封装tcp客户端常用功能
package tcp
import "net"


type TcpCommonClient struct  {
	op TcpCommonClientOperationInterface
}

func NewTcpCommonClient(op TcpCommonClientOperationInterface) *TcpCommonClient  {
	t := new(TcpCommonClient)
	t.op = op
	return t
}

func (self *TcpCommonClient) Connect(addr string)  {
	conn, err := net.Dial("tcp", addr)
	if err != nil {

	}
	self.op.OpenConnection(conn)
	self.onRead(conn)
}

func (self *TcpCommonClient) onRead(conn net.Conn)  {
	if self.op != nil {
		defer self.op.CloseConnection()
	}

	data := make([]byte, 2048)
	for {
		len, err := conn.Read(data)
		if err != nil {
			break
		}

		temp := make([]byte, len)
		if self.op != nil {
			self.op.DealWithData(temp)
		}
	}

}

func (self *TcpCommonClient) Write(data []byte)  {
	if self.op != nil {
		self.op.Write(data)
	}
}

