package protocol

import (
	"server"
	"fmt"
	"net"
	"common/util"
	"strconv"
)

type Connection struct {
	id string		//连接id
	conn net.Conn   //tcp连接
	buf []byte		//数据缓存
	len int			//缓存数据的长度
}

func log(msg string)  {
	fmt.Println(msg)
}

func CreateConnection(conn net.Conn) (*Connection)  {
	connection := new(Connection)
	connection.id = util.CreateID()
	connection.buf = make([]byte, 10240)
	connection.len = 0
	connection.conn = conn

	log("create connection from client ip " + conn.RemoteAddr().String() + " , the connection id : " + connection.id)
	return connection;
}

func CreateConnectionWithData(conn net.Conn, data []byte) (*Connection)  {
	connection := CreateConnection(conn)
	copy(connection.buf[connection.len:], data)
	return connection
}

func (connection *Connection) DealWith(data []byte)  {
	connection.dealWith(data)
}


func (connection *Connection) Close()  {
	log("close connection to client ip " + connection.conn.RemoteAddr().String())
	connection.conn.Close()
}


func (connection *Connection) dealWith(data []byte)  {
	if connection.opPacket(data) {
		//处理数据
		connection.opMsg()
	}
}

//处理粘包问题,返回true可以处理数据,false表示数据不全
func (connection *Connection) opPacket(data []byte) bool {
	copy(connection.buf[connection.len:], data)
	connection.len += len(data)
	return true
}

//从buf中取数据进行处理,可处理数据可能有多条
func (connection *Connection) opMsg()  {
	for {
		data_len := getDataLength(connection.buf)
		if connection.buf[0] == '^' && (data_len + 6) <= connection.len {
			//说明数据头部正确,数据完整
			e, tp := getTypeValue(connection.buf)
			if e == nil {
				if isPingType(tp) {
					log("recv type ping")
					connection.opPing()
				} else if isStringType(tp) {
					log("recv type string")
					tmp := connection.buf[6 : tp + 6]
					connection.opOneMsg(tmp)
				} else {
					log("recv type null")
				}
			}
			connection.clearFirstComplateData()
		} else {
			log("deal with op msg finish")
			return
		}
	}
}

//清除缓存第一条完整的数据,即数据头部和后面数据完整
func (connection *Connection) clearFirstComplateData()  {
	if connection.buf[0] == '^' {
		data_len := getDataLength(connection.buf)
		if data_len > 0 {
			copy(connection.buf[0 : ], connection.buf[data_len + 6 : ])
			connection.len -= (data_len + 6)
		}
	}
}

//处理一个字符串数据
func (connection* Connection) opOneMsg(data []byte)  {
	msg := toString(data)
	log("deal with data : " + msg)
}

func (connection *Connection) opPing()  {
	log("deal with ping")
	connection.conn.Write([]byte{'^', 3, 0, 0, 0, 4, 'p', 'o', 'n', 'g'})
}

func (connection *Connection) writeString(data string)  {
	len := len(data)
	header := []byte{'^', 0x01, byte(len & 0xFF000000), byte(len & 0x00FF0000), byte(len & 0x0000FF00), byte(len & 0x000000FF)}
	tmp := make([]byte, len + 6)
	copy(tmp, header)
	copy(tmp[6:], []byte(data))
	connection.conn.Write(tmp)
}


//对网络协议的解析类
func getTypeValue(data []byte ) (*server.NetworkError, byte)  {
	if len(data) >= 6 && data[0] == '^'{
//		头部有6个字节
		return nil, data[1]
	}
	return server.CreateNetworkError(server.NETWORK_ERROR_ILLEGAL_PROTOCOL), '^'
}


//判断是否为ping包类型
func isPingType(tp byte) bool {
	if tp == 0x02 {
		return true
	}
	return false
}


func isStringType(t byte) bool  {
	if t == 0x01 {
		return true
	}
	return false
}
//
func getDataLength(data []byte) int {
	if data[0] == '^' && len(data) >= 6 {
		a1 := int(data[2]);
		a2 := int(data[3]);
		a3 := int(data[4]);
		a4 := int(data[5]);
		return (a1 << 24) + (a2 << 16) + (a3 << 8) + a4
	}
	return -1
}

func toString(data []byte) string {
	len := len(data)
	if len > 2 {
		if data[len - 2] == 13 && data[len - 1] == 10 {
			return string(data[ : len - 2])
		}
		return string(data)
	} else {
		return string(data)
	}
}