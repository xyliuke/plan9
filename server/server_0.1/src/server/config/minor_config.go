package config

import (
	"server/base"
	"net"
	"common/log"
	"server/protocol"
)

type MinorConfigEntity struct {
	clientID        int
	microServerType byte
	microServerNum int
	serverPort      int
	version         byte
	majorAddress    string
}

type MinorConfig struct {
	server *base.BaseProtocolTcpServer
	client *base.BaseProtocolTcpClient
	config *MinorConfigEntity
}

func NewMinorConfig(config *MinorConfigEntity) *MinorConfig {
	m := new(MinorConfig)
	m.server = base.NewBaseProtocolTcpServer(config.serverPort, m)
	clientEntity := new(base.BaseClientEntity)
	clientEntity.ID = config.clientID
	clientEntity.Version = config.version
	m.client = base.NewBaseProtocolTcpClient(clientEntity, m)
	m.config = config
	return m
}

func (this* MinorConfig) Run() {
	go this.Connect()
	go this.Listen()
}

func (this *MinorConfig) Listen() {
	this.server.Listen()
}

func (this *MinorConfig) Connect() {
	this.client.Connect(this.config.majorAddress)
}

//服务器处理接口
func (this *MinorConfig) Connected(conn net.Conn)  {

}

func (this *MinorConfig) Disconnected(conn net.Conn)  {

}

func (this *MinorConfig) ReadData(conn net.Conn, data []byte)  {

}

func (this *MinorConfig) WriteData(id int, data []byte)  {

}

//客户端处理接口
func (this *MinorConfig) ClientConnect()  {
	log.I_NET("minor id", this.config.clientID, "connect to major", this.client.GetConnection().RemoteAddr(), "success")
	this.write2MajorIdentification()

	this.write2MajorUpdateMicroServerNum(10)
}

func (this *MinorConfig) ClientDisconnect()  {
	log.E_NET("minor id", this.config.clientID, "disconnect to major", this.client.GetConnection().RemoteAddr())
}

func (this *MinorConfig) ClientReadData(data []byte, data_len int)  {
	log.I_NET("minor id", this.config.clientID, "read from major", this.client.GetConnection().RemoteAddr(), ", data :", data)
}

func (this *MinorConfig) write2Major(data []byte) {
	ret, p := protocol.NewProtocol(this.config.clientID,
		this.config.version,
		protocol.PROTOCOL_MAJOR_SERVER_TYPE,
		protocol.PROTOCOL_NORMAL_JSON_DATA_TYPE,
		len(data),
		data)
	if ret {
		this.client.Write(p)
	} else {
		log.E_NET("minor id", this.config.clientID, "write to major error, id:", this.config.clientID, ",version:", this.config.version,
		",server type:", protocol.PROTOCOL_MAJOR_SERVER_TYPE, ",data type:", protocol.PROTOCOL_NORMAL_JSON_DATA_TYPE, ",data len:", len(data),
		",data:", data)
	}
}

func (this *MinorConfig) write2MajorIdentification()  {
	this.write2Major(Minor2MajorIdentify(this.config.clientID, this.config.microServerType, this.config.microServerNum))
}

func (this *MinorConfig) write2MajorUpdateMicroServerNum(num int) {
	this.write2Major(Minor2MajorUpdateMicroServerNum(this.config.clientID, num))
}