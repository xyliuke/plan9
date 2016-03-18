package cn.gocoding.server.config;

import cn.gocoding.server.base.BaseProtocolServer;
import cn.gocoding.server.base.BaseServerManagerImpl;
import cn.gocoding.server.base.ServerUnit;

/**
 * 次配置服务中的tcp-server
 * Created by liuke on 16/3/13.
 */
public class MinorConfigServer extends BaseProtocolServer {
    public MinorConfigServer(int port, BaseServerManagerImpl manager, ServerUnit serverUnit) {
        super(port, manager, 1, serverUnit);
    }
}
