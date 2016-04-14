package config

import (
	"server/base"
	"net"
	"common/log"
	"server/protocol"
	"common/json"
)

type MajorConfig struct {
	server *base.BaseProtocolTcpServer
	minors map[int]*minorInfo //minor服务器列表
}

func NewMajorConfig(port int) *MajorConfig {
	m := new(MajorConfig)
	m.server = base.NewBaseProtocolTcpServer(port, m)
	m.minors = make(map[int]*minorInfo)
	return m
}

func (this *MajorConfig) Run()  {
	go this.Listen()
}

func (this *MajorConfig) Listen() {
	this.server.Listen()
}

func (this *MajorConfig) Connected(conn net.Conn)  {
	op := newMajorConfigServerOperation()
	this.server.NewOperation(conn, op)
	op.OpConnected(conn)
}

func (this *MajorConfig) Disconnected(conn net.Conn)  {

}

func (this *MajorConfig) ReadData(conn net.Conn, data []byte)  {
	ret, id, _, _, dt, _, _, l, d := protocol.ParseProtocol(data, len(data))
	if ret {
		if dt == protocol.PROTOCOL_NORMAL_JSON_DATA_TYPE {
			str := string(d[:l])
			log.I_NET("major read from", conn.RemoteAddr(), ", data :", str)
			j := json.NewJSONByString(str)
			msg_type := j.Get(MSG_TYPE)
			if msg_type == MSG_TYPE_VALUE_IDENTIFICATION {
				//minor上报身份
				is, micro_server_type := j.GetInt(MICRO_SERVER_TYPE)
				iss, micro_server_num := j.GetInt(MICRO_SERVER_NUM)

				if is && iss {
					mi := new(minorInfo)
					mi.id = id
					mi.micro_server_num = micro_server_num
					mi.micro_server_type = byte(micro_server_type)

					this.addMinor(mi)
					log.I_NET("add minor identification id:", id, " , type:", micro_server_type, " , num", micro_server_num, " success")
				}

			} else if msg_type == MSG_TYPE_VALUE_UPDATE_IDENTIFICATION {
				//minor更新身份
				is, micro_server_num := j.GetInt(MICRO_SERVER_NUM)

				if is {
					mi := this.getMinor(id)
					mi.micro_server_num = micro_server_num
					log.I_NET("update minor identification id:", id, " , num", micro_server_num, " success")
				}
			}

			this.check()
		}
	} else {
		log.E_NET("major read from", conn.RemoteAddr(), "error, data :", data)
	}
}

func (this *MajorConfig) WriteData(id int, data []byte) {

}

func (this *MajorConfig) addMinor(m *minorInfo) {
	this.minors[m.id] = m
}

func (this *MajorConfig) removeMinor(id int)  {
	delete(this.minors, id)
}

func (this *MajorConfig) getMinor(id int) *minorInfo {
	return this.minors[id]
}

func (this *MajorConfig) check() {

}

type majorConfigServerOperation struct {
	*base.BaseProtocolTcpServerOperation
}

func newMajorConfigServerOperation() *majorConfigServerOperation {
	m := new(majorConfigServerOperation)
	m.BaseProtocolTcpServerOperation = base.NewBaseProtocolTcpServerOperation()
	return m
}

type minorInfo struct  {
	id int
	micro_server_type byte
	micro_server_num int
}














