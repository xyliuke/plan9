package route

import (
	"server/base"
	"common/log"
	"net"
	"common/cmdfactory"
	"common/json"
	"server/protocol"
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
		r.execute(conn, id, version, serverType, dataType, dataLen, data, raw)
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

	initRouteRegister()

	this.Listen(func(result bool, reason string) {
		if result {
			log.I_NET("route server listen", this.GetPort(), "success")
		} else {
			log.I_NET("route server listen", this.GetPort(), "error, reason :", reason)
		}
	})
}

func (this *RouteServer) execute(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, raw []byte) {
	js := json.NewJSON(data)

	commander.ExecuteCmd1(js, func(result json.JSONObject) {
		log.I_NET("callback from commander, data :", result.ToString())
		put := []byte(result.ToString())
		r, d := protocol.NewProtocol(id, version, serverType, dataType, len(put), put)
		if r {
			conn.Write(d)
		}
	})
}