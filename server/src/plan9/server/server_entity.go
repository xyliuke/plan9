package server

//服务器实体类,包含服务器相关配置项
type ServerEntity struct  {
	serverID byte //服务器id
	serverType byte	//服务器类型
	version byte	//服务器版本
}

func NewServerEntity(serverID byte, serverType byte, version byte) *ServerEntity {
	s := new(ServerEntity)
	s.serverID = serverID
	s.serverType = serverType
	s.version = version
	return s
}

func (self ServerEntity) GetServerID() byte {
	return self.serverID
}

func (self ServerEntity) GetServerType() byte {
	return self.serverType
}

func (self ServerEntity) GetServerVersion() byte {
	return self.version
}

