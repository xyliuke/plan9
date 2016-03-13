package cn.gocoding.server.config;

import cn.gocoding.server.base.BaseProtocolServer;
import cn.gocoding.server.base.BaseServerManagerImpl;
import cn.gocoding.server.base.ClientUnit;
import cn.gocoding.server.base.ServerUnit;

import java.util.List;

/**
 * 主配置服务器
 * Created by liuke on 16/3/11.
 */
public class MainConfigServer extends BaseProtocolServer {

    public MainConfigServer(int port, BaseServerManagerImpl manager, ServerUnit serverUnit) {
        super(port, manager, 1, serverUnit);
    }

}
