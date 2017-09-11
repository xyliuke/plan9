package cn.gocoding.server.biz.auth;

import cn.gocoding.server.base.BaseProtocolServer;
import cn.gocoding.server.base.BaseServerManagerImpl;
import cn.gocoding.server.base.ServerUnit;

/**
 * Created by liuke on 16/3/21.
 */
public class AuthTcpServer extends BaseProtocolServer {
    public AuthTcpServer(int port, BaseServerManagerImpl manager, ServerUnit serverUnit) {
        super(port, manager, 1, serverUnit);
    }
}
