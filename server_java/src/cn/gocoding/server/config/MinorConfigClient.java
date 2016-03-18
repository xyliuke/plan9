package cn.gocoding.server.config;

import cn.gocoding.server.base.BaseClientManagerImpl;
import cn.gocoding.server.base.BaseProtocolClient;
import cn.gocoding.server.base.ClientUnit;

/**
 * 次配置服务的tcp-client
 * Created by liuke on 16/3/13.
 */
public class MinorConfigClient extends BaseProtocolClient {
    public MinorConfigClient(MinorClientUnit clientUnit, BaseClientManagerImpl manager) {
        super(clientUnit, manager);
    }
}
