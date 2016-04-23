package auth

import "common/json"

func ParseAuthServerConfig(jsonObject json.JSONObject) *AuthServerConfig {
	a := new(AuthServerConfig)
	port_ok, port := jsonObject.GetInt("port")
	if port_ok {
		a.Port = port
	}
	return a
}
