package cn.gocoding.server.config;

import cn.gocoding.server.base.BaseProtocolServer;
import cn.gocoding.server.base.BaseServerManagerImpl;


/**
 * 主配置服务器
 * Created by liuke on 16/3/11.
 */
public class MajorConfigServer extends BaseProtocolServer {

    public MajorConfigServer(int port, BaseServerManagerImpl manager, MajorServerUnit serverUnit) {
        super(port, manager, 1, serverUnit);
    }

}
