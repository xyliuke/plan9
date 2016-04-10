package tcp

import (
	"net"
	"strconv"
	"common/log"
)

type TcpServerInterface interface  {
	Connected(net.Conn)
	Disconnected(net.Conn)
	ReadData(net.Conn, []byte)
	WriteData(int, []byte)
}


type TcpServer struct {
	port int
	manager TcpServerInterface
}

func NewTcpServer(manager TcpServerInterface, port int) *TcpServer {
	t := new(TcpServer)
	t.port = port
	t.manager = manager
	return t
}

func (this *TcpServer) Listen() {
	ip := ":" + strconv.Itoa(this.port)
	listen, err := net.Listen("tcp", ip)
	if err == nil {
		log.I_NET("tcp server listening")
		for true {
			conn, err_conn := listen.Accept()
			if err_conn == nil {
				go this.doConnect(conn)
			} else {
				log.E_NET("accept tcp error, the reason : ", err)
			}
		}
	} else {
		log.E_NET("listen tcp error, the reason : ", err)
	}
}

func (this* TcpServer) Write(id int, data []byte)  {
	if this.manager != nil {
		this.manager.WriteData(id, data)
	}
}

func (this *TcpServer) doConnect(conn net.Conn)  {
	log.I_NET(conn.RemoteAddr(), " connect to server success")
	if this.manager != nil {
		this.manager.Connected(conn)
	}
	data := make([]byte, 2048)
	defer conn.Close()
	for {
		len, err := conn.Read(data)
		if err == nil {
			read_data := make([]byte, len)
			copy(read_data, data)
			log.D_NET("read data from client ", conn.RemoteAddr(), " the content : ", read_data)
			if this.manager != nil {
				this.manager.ReadData(conn, read_data)
			}
		} else {
			break
		}
	}
	log.I_NET("client ", conn.RemoteAddr(), " close")
	if this.manager != nil {
		this.manager.Disconnected(conn)
	}
}
