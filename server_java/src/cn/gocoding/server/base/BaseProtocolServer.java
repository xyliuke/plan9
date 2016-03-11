package cn.gocoding.server.base;

import cn.gocoding.common.network.tcp.server.BaseServer;

/**
 * 最基本的添加协议的tcp服务器端
 * Created by liuke on 16/3/9.
 */
public class BaseProtocolServer extends BaseServer{
    public BaseProtocolServer(int port, BaseServerManagerImpl manager, int threadNum, ServerUnit serverUnit) {
        super(port, manager, threadNum);
        manager.setServerUnit(serverUnit);
        this.serverUnit = serverUnit;
    }

    protected ServerUnit serverUnit;
}
