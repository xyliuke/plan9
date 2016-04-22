package route

import "common/json"

func PaserRouteServerConfig(jsonObject json.JSONObject) *RouteServerConfig {
	r := new(RouteServerConfig)

	port_ok, port := jsonObject.GetInt("port")
	if port_ok {
		r.Port = port
	}

	return r
}