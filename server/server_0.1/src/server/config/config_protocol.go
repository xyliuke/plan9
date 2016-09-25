package config

import (
	"common/json"
)

const MSG_TYPE string = "msg_type"
const IDENTITY string = "identity"
const CLIENT_ID string = "client_id"
const MICRO_SERVER_TYPE string = "micro_server_type"
const MICRO_SERVER_NUM string = "micro_server_num"
const MICRO_SERVER_ADDRESS string = "micro_server_address"

const MSG_TYPE_VALUE_IDENTIFICATION string = "identification"
const MSG_TYPE_VALUE_UPDATE_IDENTIFICATION string = "update_identification"
const IDENTITY_VALUE_MINOR_CONFIG string = "minor_config"
const IDENTITY_VALUE_MICRO_SERVER string = "micro_server"

func Minor2MajorIdentify(minor_id int, server_type byte) []byte {
	j := json.NewJSONEmpty()

	j.Put(MSG_TYPE, MSG_TYPE_VALUE_IDENTIFICATION)
	j.Put(IDENTITY, IDENTITY_VALUE_MINOR_CONFIG)
	j.Put(CLIENT_ID, minor_id)
	j.Put(MICRO_SERVER_TYPE, server_type)

	str := j.ToString()
	return []byte(str)
}



func Minor2MajorUpdateMicroServerNum(minor_id int, server_num int, address []string) []byte {
	j := json.NewJSONEmpty()

	j.Put(MSG_TYPE, MSG_TYPE_VALUE_UPDATE_IDENTIFICATION)
	j.Put(IDENTITY, IDENTITY_VALUE_MINOR_CONFIG)
	j.Put(CLIENT_ID, minor_id)
	j.Put(MICRO_SERVER_NUM, server_num)
	for _, val := range address {
		j.AddToArray(MICRO_SERVER_ADDRESS, val)
	}

	str := j.ToString()
	return []byte(str)
}

func MicroServer2MinorIndentify(id int, serverType byte) []byte {
	j := json.NewJSONEmpty()

	j.Put(MSG_TYPE, MSG_TYPE_VALUE_IDENTIFICATION)
	j.Put(IDENTITY, IDENTITY_VALUE_MICRO_SERVER)
	j.Put(CLIENT_ID, id)
	j.Put(MICRO_SERVER_TYPE, serverType)

	str := j.ToString()
	return []byte(str)
}