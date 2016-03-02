//封装tcp服务器端常用功能
package tcp
import (
	"net"
	"strconv"
)

type TcpCommonServer struct  {
	port int	//监听的端口号
	manager TcpCommonServerManagerInterface //处理接口
}

func NewTcpCommonServer(port int, manager TcpCommonServerManagerInterface) *TcpCommonServer {
	t := new(TcpCommonServer)
	t.port = port
	t.manager = manager
	return t
}

func (self *TcpCommonServer) Listen()  {
	ip := ":" + strconv.Itoa(self.port)
	listen, err := net.Listen("tcp", ip)
	if err != nil {
		//监听错误
	}
	conn, err_conn := listen.Accept()
	if err_conn != nil {
		//连接错误
	}
	go self.doConnect(conn)
}

func (self *TcpCommonServer) doConnect(conn net.Conn)  {
	if self.manager != nil {
		conn_op := self.manager.CreateConnection()
		defer conn_op.CloseConnection()

		data := make([]byte, 2048)
		for {
			len, err := conn.Read(data)
			if err == nil {
				read_data := make([]byte, len)
				copy(read_data, data)
				conn_op.DealWithData(read_data)
			} else {
				break
			}
		}
	}
}


