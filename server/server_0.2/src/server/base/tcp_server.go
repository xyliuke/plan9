package base

import (
	"common/log"
	"net"
	"strconv"
	"errors"
)

type BaseServer struct {
	port      int

	connected_handler func(net.Conn)
	disconnected_handler func(net.Conn)
	read_handler func(net.Conn, []byte)
}

func NewBaseServer(port int) *BaseServer {
	b := new(BaseServer)
	b.port = port
	return b
}

func (this *BaseServer) SetConnectedHandler(h func(net.Conn)) {
	this.connected_handler = h
}

func (this *BaseServer) SetDisconnectedHandler(h func(net.Conn)) {
	this.disconnected_handler = h
}

func (this *BaseServer) SetReadHandler(h func(net.Conn, []byte)) {
	this.read_handler = h
}

func (this *BaseServer) GetPort() int {
	return this.port
}

func (this *BaseServer) Listen(callback func(result bool, reason string)) {
	ip := ":" + strconv.Itoa(this.port)
	listen, err := net.Listen("tcp", ip)
	if err == nil {
		log.I_NET("tcp server", ip, "listening")
		callback(true, "")
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
		callback(false, err.Error())
	}
}

func (this *BaseServer) doConnect(conn net.Conn) {
	this.Connected(conn)
	data := make([]byte, 2048)

	defer func() {
		this.Disconnected(conn)
	}()

	for {
		len, err := conn.Read(data)
		if err == nil {
			read_data := make([]byte, len)
			copy(read_data, data)
			log.D_NET(conn.LocalAddr(), "read from client", conn.RemoteAddr(), "data :", read_data)
			this.Read(conn, read_data)
		} else {
			log.E_NET("read from client", conn.RemoteAddr(), "error, reason :", err)
			break
		}
	}
}

func (this *BaseServer) Connected(conn net.Conn) {
	log.I_NET("client:", conn.RemoteAddr(), "connect to server", conn.LocalAddr(), "success")
	if this.connected_handler != nil {
		this.connected_handler(conn)
	}
}

func (this *BaseServer) Disconnected(conn net.Conn) {
	log.I_NET("client", conn.RemoteAddr(), "close")
	if this.disconnected_handler != nil {
		this.disconnected_handler(conn)
	}
}

func (this *BaseServer) Read(conn net.Conn, data []byte) {
	if this.read_handler != nil {
		this.read_handler(conn, data)
	}
}

func (this *BaseServer) Write(id int, data []byte, callback func(int, []byte)) {
	
}

func (this *BaseServer) write(conn net.Conn, data []byte) error {
	if conn != nil {
		read_n := 0
		for true {
			n, err := conn.Write(data[read_n:])
			if err == nil {
				if read_n >= len(data) {
					return nil
				}
				read_n += n
			} else {
				return err
			}
		}
	}
	return errors.New("the connection is nil")
}
