package connection

import (
	"common/json"
)

func ParseConnectionServerConfig(jsonObject json.JSONObject) (bool, *ConnectionServerConfig) {
	config := new(ConnectionServerConfig)

	port_ok, port := jsonObject.GetInt("port")
	if port_ok {
		config.Port = port
	}
	route_ok, route := jsonObject.GetString("route_address")
	if route_ok {
		config.RouteAddress = route
	}
	auth_ok, auth := jsonObject.GetString("auth_address")
	if auth_ok {
		config.AuthAddress = auth
	}

	return true, config
}
