package cn.gocoding.server.biz.connect;

import cn.gocoding.server.base.BaseClientManagerImpl;
import cn.gocoding.server.base.BaseProtocolClient;
import cn.gocoding.server.base.ClientUnit;

/**
 * 连接服务器中需要连接路由服务的tcp-client
 * Created by liuke on 16/3/18.
 */
public class ConnectTcpClient4Route extends BaseProtocolClient {
    public ConnectTcpClient4Route(ClientUnit clientUnit, BaseClientManagerImpl manager) {
        super(clientUnit, manager);
    }
}
