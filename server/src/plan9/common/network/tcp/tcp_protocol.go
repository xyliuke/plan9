//自定义协议封装功能
package tcp

import (
	"plan9/common/network"
	"plan9/common/util"
)

//数据报首字符^
const PROTOCOL_FIRST_LETTER = 94
//数据报头部字节数长度
const PROTOCOL_HEADER_LEN = 9

//数据id的下标
const PROTOCOL_ID_INDEX = 1
//数据版本的下标
const PROTOCOL_VERSION_INDEX = 5
//数据类型的下标
const PROTOCOL_TYPE_INDEX = 6
//数据长度的下标
const PROTOCOL_DATA_LEN_INDEX = 7
//数据实体的下标
const PROTOCOL_DATA_INDEX = 9

const (
	//前4bit的服务器类型协议
	LOGIN_SERVER_TYPE = 0x00
	VERIFY_SERVER_TYPE = 0x10
	SESSION_SERVER_TYPE = 0x20
	DATABASE_SERVER_TYPE = 0x30

	MAIN_CONFIG_TYPE = 0xE0
	OTHER_CONFIG_TYPE = 0xF0



	//后4bit的数据类型协议
	NORMAL_STRING_DATA_TYPE = 0x00
	NORMAL_JSON_DATA_TYPE = 0x01
	PING_DATA_TYPE = 0x02
	PONG_DATA_TYPE = 0x03
	COMPRESS_JSON_DATA_TYPE = 0x04
	ENCRYPT_JSON_DATA_TYPE = 0x05
	ENCRYPT_COMPRESS_JSON_DATA_TYPE = 0x06
)

//---------------------------------------基本功能-----------------------------------------------------

func MaxProtocolLength() int  {
	return 0xFFFFFFFF + PROTOCOL_HEADER_LEN
}

//判断数据协议是否合法,至少包含一条完整数据报
//如果合法,则返回第一条数据报的总长度,包括报头
//如果不合法,则返回err和0
func IsLegalProtocol(data []byte, dataLen int) (*network.NetworkError, int)   {
	if dataLen >= PROTOCOL_HEADER_LEN && data[0] == PROTOCOL_FIRST_LETTER {
		//头部完整
		a1 := int(data[PROTOCOL_DATA_LEN_INDEX])
		a2 := int(data[PROTOCOL_DATA_LEN_INDEX + 1])
		dLen := (a1 << 8) + a2
		if dataLen >= (dLen + PROTOCOL_HEADER_LEN) {
			return nil, dLen + PROTOCOL_HEADER_LEN
		}
	}
	return network.CreateNetworkError(network.NETWORK_ERROR_ILLEGAL_PROTOCOL), 0
}

//删除给定字节数组中第一个完整协议包,并将字节数组重置成从0开始
//len为数据长度,data为数据
//运算后len为删除后的数据长度
func RemoveOneProtocol4Protocol(len *int, data []byte)  {
	err, protocolLen := IsLegalProtocol(data, *len)
	if err == nil {
		copy(data, data[protocolLen:*len])
		*len -= protocolLen

		bc := -1
		for i := 0; i < *len; i ++ {
			if data[i] == PROTOCOL_FIRST_LETTER {
				bc = i
			}
		}

		if bc == 0 {
			//紧接一条协议
		} else if bc > 0{
			//中间存在非协议数据
			copy(data, data[bc:])
			*len -= bc
		} else {
			//其他数据非协议
			*len = 0
		}
	}
}

//得到数据包的类型字符
func GetTypeValue4Protocol(data []byte, dataLen int) (*network.NetworkError, byte) {
	err, _ := IsLegalProtocol(data, dataLen)
	if err == nil {
		return nil, data[PROTOCOL_TYPE_INDEX]
	}
	return err, 0x0
}

//得到数据的长度
func GetDataLength4Protocol(data []byte, dataLen int) int {
	err, oneDataLen := IsLegalProtocol(data, dataLen)
	if err == nil {
		return oneDataLen - PROTOCOL_HEADER_LEN
	}
	return -1
}


//得到一个完整的数据协议包
func GetOneFullProtocol(data []byte, dataLen int) []byte {
	err, protocolLen := IsLegalProtocol(data, dataLen)
	if err == nil {
		return data[:(protocolLen)]
	}
	return nil
}

//得到数据的字节数组
func GetData4Protocol(data []byte, packetLen int) []byte  {
	err, protocolLen := IsLegalProtocol(data, packetLen)
	if err == nil {
		return data[PROTOCOL_HEADER_LEN:protocolLen]
	}
	return nil
}

//通过数据包得到服务器类型
func GetServerType4Protocol(data []byte, packetLen int) (*network.NetworkError, byte) {
	err, tp := GetTypeValue4Protocol(data, packetLen)
	if err == nil {
		return nil, tp & 0xF0
	}
	return err, 0XFF
}

//通过类型字节得到服务器类型
func GetServerTypeByByte4Protocol(data byte) byte  {
	return data & 0xF0
}

//通过数据包得到数据类型
func GetDataType4Protocol(data []byte, packetLen int) (*network.NetworkError, byte)  {
	err, tp := GetTypeValue4Protocol(data, packetLen)
	if err == nil {
		return nil, tp & 0x0F
	}
	return err, 0XFF
}

//通过类型字节得到数据类型
func GetDataTypeByByte4Protocol(data byte) byte  {
	return data & 0xF0
}

//填充数据包
//serverType 服务器类型,传递服务器类型常量
//dataType 数据类型,传递数据类型常量
//size	数据长度
//data	数据
//返回填充后的数据包
func CreateData4Protocol(serverID byte, version byte, serverType byte, dataType byte, size int, data []byte) []byte  {
	if size > 0 {
		id := util.CreateDataID(serverID)
		header := []byte{
			PROTOCOL_FIRST_LETTER,
			byte((id & 0xFF000000) >> 24),
			byte((id & 0x00FF0000) >> 16),
			byte((id & 0x0000FF00) >> 8),
			byte(id & 0x000000FF),
			version,
			serverType | dataType,
			byte((size & 0x0000FF00) >> 8),
			byte(size & 0x000000FF)}
		ret := make([]byte, size + PROTOCOL_HEADER_LEN)
		copy(ret, header)
		if size <= len(data) {
			copy(ret[PROTOCOL_HEADER_LEN:], data[:size])
		} else {
			copy(ret[PROTOCOL_HEADER_LEN:], data)
		}
		return ret
	}
	return nil
}

//修改数据包的数据实体内容和数据长度,data必须能够容纳newData的数据
func SetProtocolData(data []byte, newData []byte, dataLen int) bool {
	if dataLen >= 0 && len(data) >= PROTOCOL_HEADER_LEN + dataLen{
		copy(data[PROTOCOL_HEADER_LEN:PROTOCOL_HEADER_LEN + dataLen], newData)
		return SetProtocolDataLen(data, dataLen)
	}
	return false
}

func SetProtocolDataLen(data []byte, dataLen int) bool {
	if len(data) >= PROTOCOL_HEADER_LEN {
		data[PROTOCOL_DATA_LEN_INDEX] = byte(dataLen >> 8)
		data[PROTOCOL_DATA_LEN_INDEX + 1] = byte(dataLen)
		return true
	}
	return false
}

//设置数据包的类型字段
func SetProtocolType(data []byte, serverType byte, dataType byte) bool  {
	if len(data) >= PROTOCOL_HEADER_LEN {
		data[PROTOCOL_TYPE_INDEX] = serverType | dataType
		return true
	}
	return false
}
//设置数据包的版本字段
func SetProtocolVersion(data []byte, version byte) bool  {
	if len(data) >= PROTOCOL_HEADER_LEN {
		data[PROTOCOL_VERSION_INDEX] = version
		return true
	}
	return false
}

//---------------------------辅助功能-----------------------------------------------------

//判断是否为Ping包
func IsPingType4Protocol(tp byte) bool {
	return GetDataTypeByByte4Protocol(tp) == PING_DATA_TYPE
}

//判断是否pong包
func IsPongType4Protocol(tp byte) bool {
	return GetDataTypeByByte4Protocol(tp) == PONG_DATA_TYPE
}


//封装ping数据包
//@param t 服务器类型
func CreatePing4Protocol(serverID byte, version byte, serverType byte) []byte {
	data := CreateData4Protocol(serverID, version, serverType, PING_DATA_TYPE, 4, []byte{'p', 'i', 'n', 'g'})
	return data
}

//封装pong数据包
func CreatePong4Protocol(serverID byte, version byte, serverType byte) []byte {
	data := CreateData4Protocol(serverID, version, serverType, PONG_DATA_TYPE, 4, []byte{'p', 'o', 'n', 'g'})
	return data
}

func CreateString4Protocol(serverID byte, version byte, serverType byte, data string) []byte {
	size := len(data)
	return CreateData4Protocol(serverID, version, serverType, NORMAL_STRING_DATA_TYPE, size, []byte(data))
}
