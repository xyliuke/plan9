package protocol

import (
	"common/compress"
	"fmt"
)

const PROTOCOL_FRIST_LETTER byte = '^'
const PROTOCOL_HEADER_LEN int = 9
const PROTOCOL_ID_INDEX int = 1
const PROTOCOL_VERSION_INDEX int = 5
const PROTOCOL_TYPE_INDEX int = 6
const PROTOCOL_LEN_INDEX int = 7
const PROTOCOL_DATA_INDEX int = 9

const PROTOCOL_CONNECTION_SERVER_TYPE byte = 0x00
const PROTOCOL_VERIFY_SERVER_TYPE byte = 0x10
const PROTOCOL_SESSION_SERVER_TYPE byte = 0x20

const PROTOCOL_NORMAL_STRING_DATA_TYPE byte = 0x00
const PROTOCOL_NORMAL_JSON_DATA_TYPE byte = 0x01
const PROTOCOL_PING_DATA_TYPE byte = 0x02
const PROTOCOL_PONG_DATA_TYPE byte = 0x03
const PROTOCOL_COMPRESS_JSON_DATA_TYPE byte = 0x04
const PROTOCOL_ENCRYPT_JSON_DATA_TYPE byte = 0x05
const PROTOCOL_ENCRYPT_COMPRESS_JSON_DATA_TYPE byte = 0x06

/*
 * 生成一条新的协议
 */
func NewProtocol(id int, version byte, serverType byte, dataType byte, dataLen int, data []byte) (bool, []byte) {

	if (data != nil && dataLen <= len(data)) || (dataLen == 0 && data == nil) {

		var ret []byte = nil
		realDataLen := dataLen

		if data != nil {
			if dataType == PROTOCOL_COMPRESS_JSON_DATA_TYPE {
				//压缩
				comp := compress.Compress(data, dataLen)
				realDataLen = len(comp)
				ret = make([]byte, realDataLen + PROTOCOL_HEADER_LEN)
				copy(ret[PROTOCOL_DATA_INDEX:], comp)

			} else {
				ret = make([]byte, realDataLen + PROTOCOL_HEADER_LEN)
				copy(ret[PROTOCOL_DATA_INDEX:], data)
			}
		} else {
			ret = make([]byte, PROTOCOL_HEADER_LEN)
		}



		ret[0] = PROTOCOL_FRIST_LETTER

		ret[PROTOCOL_ID_INDEX] = byte((id & 0xFF000000) >> 24)
		ret[PROTOCOL_ID_INDEX + 1] = byte((id & 0x00FF0000) >> 16)
		ret[PROTOCOL_ID_INDEX + 2] = byte((id & 0x0000FF00) >> 8)
		ret[PROTOCOL_ID_INDEX + 3] = byte(id & 0x000000FF)

		ret[PROTOCOL_VERSION_INDEX] = version

		ret[PROTOCOL_TYPE_INDEX] = (serverType & 0xF0) | (dataType & 0x0F)

		ret[PROTOCOL_LEN_INDEX] = byte((realDataLen & 0x0000FF00) >> 8)
		ret[PROTOCOL_LEN_INDEX + 1] = byte(realDataLen & 0x000000FF)

		return true, ret
	}

	return false, nil
}

func NewPingProtocol(id int) []byte {
	_, ret := NewProtocol(id, 0, 0, PROTOCOL_PING_DATA_TYPE, 0, nil)
	return ret
}

func NewPongProtocol(id int) []byte {
	_, ret := NewProtocol(id, 0, 0, PROTOCOL_PONG_DATA_TYPE, 0, nil)
	return ret
}

func RemoveFristProtocol(oriData []byte, oriDataLen int) (destDataLen int)  {
	is, _, _, _, _, plen, _, _, _ := ParseProtocol(oriData, oriDataLen)
	fmt.Println("remove ", is, plen)
	if is {
		allLen := plen + PROTOCOL_HEADER_LEN
		copy(oriData[:], oriData[allLen:oriDataLen])
		newLen := oriDataLen - allLen
		for newLen > 0{
			if oriData[0] == PROTOCOL_FRIST_LETTER {
				break
			}
			copy(oriData[:], oriData[0:1])
			newLen --
		}
		return newLen
	}
	return oriDataLen
}

/**
	解析协议报文,如果能解析出一条协议,则第一个参数返回true;
	其他参数依次为协议中客户端id,版本号,服务器类型,数据类型,协议中原始数据实体长度,协议中原始数据实体,最终数据长度,最终数据实体
	最终数据实体是根据协议中数据类型来生成,如果数据被压缩,则最终数据实体是解压后的数据
 */
func ParseProtocol(data []byte, dataLen int) (result bool, id int, version byte, serverType byte, dataType byte, oriDataLen int, oriData []byte, finalDataLen int, finalData []byte) {
	if len(data) >= PROTOCOL_HEADER_LEN && dataLen >= PROTOCOL_HEADER_LEN && data[0] == PROTOCOL_FRIST_LETTER {
		pLen := getDataLength(data);
		if (pLen + PROTOCOL_HEADER_LEN) <= dataLen {
			tp := data[PROTOCOL_TYPE_INDEX]
			oriData := data[PROTOCOL_DATA_INDEX:]
			dt := tp & 0x0F
			if dt == PROTOCOL_COMPRESS_JSON_DATA_TYPE {
				deCom := compress.DeCompress(oriData, pLen)
				return true, getID(data), data[PROTOCOL_VERSION_INDEX], tp & 0xF0, tp & 0x0F, pLen, oriData, len(deCom), deCom
			} else {
				return true, getID(data), data[PROTOCOL_VERSION_INDEX], tp & 0xF0, tp & 0x0F, pLen, oriData, pLen, oriData
			}

		}
	}
	return false, 0, 0, 0, 0, 0, nil, 0, nil
}

func IsPing(data []byte, dataLen int) bool {
	result, _, _, _, dt, _, _, _, _ := ParseProtocol(data, dataLen)
	if result {
		return IsPingByType(dt)
	}
	return false
}

func IsPingByType(tp byte) bool {
	return (tp & 0x0F) == PROTOCOL_PING_DATA_TYPE
}

func IsPong(data []byte, dataLen int) bool {
	result, _, _, _, dt, _, _, _, _ := ParseProtocol(data, dataLen)
	if result {
		return IsPongByType(dt)
	}
	return false
}

func IsPongByType(tp byte) bool {
	return (tp & 0xF0) == PROTOCOL_PONG_DATA_TYPE
}

func getID(data []byte) int {
	a := int(data[PROTOCOL_ID_INDEX]) << 24
	b := int(data[PROTOCOL_ID_INDEX + 1]) << 16
	c := int(data[PROTOCOL_ID_INDEX + 2]) << 8
	d := int(data[PROTOCOL_ID_INDEX + 3])
	return a + b + c + d
}

func getDataLength(data []byte) int {
	a := int(data[PROTOCOL_LEN_INDEX]) << 8
	b := int(data[PROTOCOL_LEN_INDEX + 1])
	return a + b
}