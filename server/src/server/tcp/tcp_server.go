package tcp

import (
	"net"
	"fmt"
	"strconv"
	"server/protocol"
)

type TcpServer struct  {
	port int
}


func CreateTcpServer(port int) (*TcpServer)  {
	ser := new(TcpServer)
	ser.port = port
	return ser
}

func (server TcpServer) Listen()  {
	ip := ":" + strconv.Itoa(server.port);
	listen, err := net.Listen("tcp", ip)
	if err != nil {
		//监听错误
		fmt.Println("listen " + ip + " error")
		return
	}
	for {
		conn, err1 := listen.Accept()
		if err1 == nil {
			fmt.Println(conn.RemoteAddr().String() + " connect to server")
			go server.handleConnection(conn)
		}
	}
}

func (server TcpServer)handleConnection(conn net.Conn)  {
	go server.handleRead(conn)
}

func (server TcpServer) handleRead(conn net.Conn)  {
	connection := protocol.CreateConnection(conn)
	defer connection.Close()

	data := make([]byte, 2048)
	for {
		len, err := conn.Read(data)
		if err == nil {
			read_data := make([]byte, len)
			copy(read_data, data)
			connection.DealWith(read_data)
		} else {
			break
		}
	}
}