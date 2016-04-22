package connection

import (
	"server/base"
	"server/protocol"
	"common/log"
	"net"
)

type ConnectionServerConfig struct {
	Port int//客户端监听端口
	RouteAddress string
	AuthAddress string
}

type ConnectionServer struct {
	*base.TcpProtocolServer
	route *base.TcpProtocolClient
	serverType byte
	config *ConnectionServerConfig
}

func NewConnectionServer(config *ConnectionServerConfig) *ConnectionServer {
	c := new(ConnectionServer)
	c.config = config
	c.serverType = protocol.PROTOCOL_CONNECTION_SERVER_TYPE

	if config.RouteAddress != "" {
		c.route = base.NewTcpProtocolClient()
		go c.route.Connect(config.RouteAddress)
		c.route.SetRawOperationHandler(func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, raw []byte) {
			//处理从route server接收的数据
			log.I_NET("connection server read from route ", conn.RemoteAddr(), " , data :", raw)
			result, d := protocol.NewRawProtocol(id, version, serverType, dataType, dataLen, raw)
			if result {
				c.TcpProtocolServer.Write(id, d, func(e error) {
					if e == nil {
						log.I_NET("connection server write to client ", conn.RemoteAddr(), " success, data :", raw)
					} else {
						log.E_NET("connection server write to client ", conn.RemoteAddr(), " error, reason :", e.Error(), " , data :", raw)
					}
				})
				if c.isTempID(id) {
					c.RemoveIDAndConnection(id)
				}
			}
		})
		log.I_NET("route address is", c.route)
		c.route.SetConnectedHandler(func(conn net.Conn) {
			log.I_NET("connection server connect to route server", conn,"success")
		})

		c.route.SetDisconnectedHandler(func() {
			log.E_NET("connection server disconnect to route server")
		})
	}

	c.TcpProtocolServer = base.NewTcpProtocolServer(config.Port)
	c.TcpProtocolServer.SetRawOperationHandler(func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, raw []byte) {
		//处理第一个连接的发送的数据
		log.I_NET("connection server read data from", conn.RemoteAddr().String(), ", id :", id, ", version :", version, ", server type", serverType,
		", data type :", dataType, ", data len:", dataLen, ", data :", raw)

		if serverType == protocol.PROTOCOL_VERIFY_SERVER_TYPE {
			//需要验证登录
			ok, d := protocol.NewRawProtocol(id, version, serverType, dataType, len(raw), raw)
			if ok {
				log.I_NET("send data to verify server")
				c.send2Verify(d)
			} else {
				log.E_NET("send data to verify server error, the protocol illegal")
			}

		} else {
			//剩下全部发送给route server
			new_id := id
			if id == 0 {
				new_id = c.createTempID()
			}
			c.BindIDAndConnection(new_id, conn)
			ok, d := protocol.NewRawProtocol(new_id, version, serverType, dataType, len(raw), raw)
			if ok {
				log.I_NET("send data to route server")
				c.send2Route(d)
			} else {
				log.E_NET("send data to route server error, the protocol illegal")
			}
		}

	})
	c.SetConnectedHandler(func(conn net.Conn) {
		//处理每一个客户端连接成功后的操作
		log.I_NET("client", conn.RemoteAddr(), "connect to connection server success")
	})

	c.SetDisconnectedHandler(func(conn net.Conn) {
		log.W_NET("client", conn.RemoteAddr(), "disconnect to connection server")
	})
	return c
}

func (this *ConnectionServer) send2Route(data []byte) {
	if this.route == nil {
		log.E_NET("connection server send data to route server error, reason : can not connect to route server")
		return
	}
	log.I_NET("send to route data , the connection is", this.route.GetConnection())
	this.route.Write(data, func(e error, d []byte) {
		if e == nil {
			log.I_NET("connection server send data to route server", this.route.GetConnection().RemoteAddr(), "success, the data : ", data)
		} else {
			log.E_NET("connection server send data to route server error, reason : ", e.Error())
		}
	})
}

func (this *ConnectionServer) send2Verify(data []byte) {

}

func (this *ConnectionServer) Run() {
	log.I_NET("run conncection server")
	go this.Listen(func(result bool, reason string) {
		if result {
			log.I_NET("connection server listen", this.GetPort(), "success")
		} else {
			log.E_NET("connection server listen", this.GetPort(), "error, reason :", reason)
		}
	})
}

const min_temp_id int = 0x70000000
const max_temp_id int = 0x7FFFFFFF
var temp_id int = min_temp_id
//为没有登录的用户生成一个临时id
func (this *ConnectionServer) createTempID() int {
	if temp_id > max_temp_id {
		temp_id = min_temp_id
	} else {
		temp_id ++
	}
	return temp_id
}

func (this *ConnectionServer) isTempID(id int) bool {
	if id >= min_temp_id && id <= max_temp_id {
		return true
	}
	return false
}
