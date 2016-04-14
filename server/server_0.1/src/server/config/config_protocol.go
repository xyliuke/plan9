package config

import (
	"common/json"
)

const MSG_TYPE string = "msg_type"
const IDENTITY string = "identity"
const CLIENT_ID string = "client_id"
const MICRO_SERVER_TYPE string = "micro_server_type"
const MICRO_SERVER_NUM string = "micro_server_num"

const MSG_TYPE_VALUE_IDENTIFICATION string = "identification"
const MSG_TYPE_VALUE_UPDATE_IDENTIFICATION string = "update_identification"
const IDENTITY_VALUE_MINOR_CONFIG string = "minor_config"

func Minor2MajorIdentify(minor_id int, server_type byte, server_num int) []byte {
	j := json.NewJSONEmpty()

	j.Put(MSG_TYPE, MSG_TYPE_VALUE_IDENTIFICATION)
	j.Put(IDENTITY, IDENTITY_VALUE_MINOR_CONFIG)
	j.Put(CLIENT_ID, minor_id)
	j.Put(MICRO_SERVER_TYPE, server_type)
	j.Put(MICRO_SERVER_NUM, server_num)

	str := j.ToString()
	return []byte(str)
}



func Minor2MajorUpdateMicroServerNum(minor_id int, server_num int) []byte {
	j := json.NewJSONEmpty()

	j.Put(MSG_TYPE, MSG_TYPE_VALUE_UPDATE_IDENTIFICATION)
	j.Put(IDENTITY, IDENTITY_VALUE_MINOR_CONFIG)
	j.Put(CLIENT_ID, minor_id)
	j.Put(MICRO_SERVER_NUM, server_num)

	str := j.ToString()
	return []byte(str)
}
