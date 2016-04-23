package auth

import (
	"server/base"
	"net"
	"common/log"
)

type AuthServerConfig struct {
	Port int
}

type AuthServer struct {
	*base.TcpProtocolServer
	config *AuthServerConfig
}

func NewAuthServer(config *AuthServerConfig) *AuthServer {
	a := new(AuthServer)
	a.config = config
	a.TcpProtocolServer = base.NewTcpProtocolServer(config.Port)
	a.TcpProtocolServer.SetOperationHandler(func(conn net.Conn, id int, version byte, serverType byte, dataType byte, dataLen int, data []byte, raw[]byte) {

	})

	a.TcpProtocolServer.SetConnectedHandler(func(conn net.Conn) {
		log.I_NET("connection server", conn.RemoteAddr(), "connnect to auth server success")
	})

	a.TcpProtocolServer.SetDisconnectedHandler(func() {
		log.W_NET("connection server disconnnect to auth server")
	})
	return a
}