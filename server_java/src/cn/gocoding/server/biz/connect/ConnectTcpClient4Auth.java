package cn.gocoding.server.biz.connect;

import cn.gocoding.server.base.BaseClientManagerImpl;
import cn.gocoding.server.base.BaseProtocolClient;
import cn.gocoding.server.base.ClientUnit;

/**
 * Created by liuke on 16/3/21.
 */
public class ConnectTcpClient4Auth extends BaseProtocolClient {
    public ConnectTcpClient4Auth(ClientUnit clientUnit, BaseClientManagerImpl manager) {
        super(clientUnit, manager);
    }
}
