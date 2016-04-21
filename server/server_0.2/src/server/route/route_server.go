package route

import (
	"server/base"
	"common/log"
	"net"
)

type RouteServerConfig struct {
	Port int
}

type RouteServer struct {
	*base.TcpProtocolServer
	config *RouteServerConfig
}


func NewRouteServer(config *RouteServerConfig) *RouteServer {
	r := new(RouteServer)
	r.config = config

	r.TcpProtocolServer = base.NewTcpProtocolServer(config.Port)
	r.TcpProtocolServer.SetOperationHandler(func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, raw []byte) {
		log.I_NET("reoute server recv from", conn.RemoteAddr(), ", data id :", id, ", version :", version,
		", server type :", serverType, ", data type :", dataType, ", data :", string(data))
		//js := json.NewJSON(data)
		//
		//result := json.NewJSONEmpty()
		//result.Put("success", true)
		//
		//result_data := json.NewJSONEmpty()
		//result_data.Put("hello", "from go lang server")
		//result.Put("data", result_data)
		//
		//js.Put("result", result)
		//
		//
		//
		//put := []byte(js.ToString())
		//r, d := protocol.NewProtocol(id, version, serverType, dataType, len(put), put)
		//if r {
		//	conn.Write(d)
		//}
	})

	r.TcpProtocolServer.SetConnectedHandler(func(conn net.Conn) {
		log.I_NET("client", conn.RemoteAddr().String(), "connect to route server", conn.LocalAddr().String(), "success")
	})

	r.TcpProtocolServer.SetDisconnectedHandler(func(conn net.Conn) {
		log.I_NET("client", conn.RemoteAddr().String(), "disconnect to route server", conn.LocalAddr().String())
	})
	return r
}

func (this *RouteServer) Run() {
	this.Listen(func(result bool, reason string) {
		if result {
			log.I_NET("route server listen", this.GetPort(), "success")
		} else {
			log.I_NET("route server listen", this.GetPort(), "error, reason :", reason)
		}
	})
}
