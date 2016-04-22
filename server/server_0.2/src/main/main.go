package main

import (
	"common/log"
	"common/json"
	"os"
	"io/ioutil"
	"server/connection"
	"server/route"
)

func main() {

	log.I_COM(os.Args)
	if len(os.Args) <= 1 {
		log.E_COM("command error, can not find config file")
	} else {
		id, jsonObject := getIdentity(os.Args[1])
		log.I_COM("identity is", id)
		if "connection_server" == id {
			log.I_COM("run connection server")
			go func() {
				result, cs := connection.ParseConnectionServerConfig(jsonObject)
				if result {
					connection.NewConnectionServer(cs).Run()
				} else {
					log.E_COM("read connection config error")
				}
			}()
		} else if "route_server" == id {
			log.I_COM("run route server")
			go func() {
				rs := route.PaserRouteServerConfig(jsonObject)
				route.NewRouteServer(rs).Run()
			}()
		}
	}

	ch := make(chan int)
	<- ch
}

func getIdentity(file string) (string, json.JSONObject) {
	js := getJSONFromFileConfig(file)
	ok, id := js.GetString("identity")
	if ok {
		return id, js
	}
	return "", js
}


func getJSONFromFileConfig(file string) json.JSONObject {
	f, err := os.Open(file)
	if err == nil {
		data, e := ioutil.ReadAll(f)
		if e == nil {
			js := json.NewJSON(data)
			return js
		}
	}
	return nil
}