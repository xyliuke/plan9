package cn.gocoding.server.base;

import cn.gocoding.common.network.tcp.client.BaseClient;
import cn.gocoding.common.network.tcp.client.ClientManager;

/**
 * 基本的客户端
 * Created by liuke on 16/3/9.
 */
public class BaseProtocolClient extends BaseClient implements BaseClientManagerImplDelegate{

    public BaseProtocolClient(ClientUnit clientUnit, BaseClientManagerImpl manager) {
        super(manager);
        manager.setDelegate(this);
        this.clientUnit = clientUnit;
    }


    @Override
    public void reconnect() {
        super.reconnect();
    }

    protected ClientUnit clientUnit;
}
