package cn.gocoding.server.biz.connect;

import cn.gocoding.server.base.BaseProtocolServer;
import cn.gocoding.server.base.BaseServerManagerImpl;
import cn.gocoding.server.base.ServerUnit;

/**
 * 连接服务器的tcp-server
 * Created by liuke on 16/3/18.
 */
public class ConnectTcpServer extends BaseProtocolServer {
    public ConnectTcpServer(int port, BaseServerManagerImpl manager, ServerUnit serverUnit) {
        super(port, manager, 4, serverUnit);
    }
}
