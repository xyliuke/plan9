package cn.gocoding.server.config;

import cn.gocoding.server.base.ServerUnit;
import cn.gocoding.server.protocol.Protocol;

/**
 * 主配置应用程序,包括
 * Created by liuke on 16/3/11.
 */
public class MajorConfig {

    public static void run(int port, int version) {
        MajorConfig.port = port;
        MajorConfig.version = (byte) version;
        initServer();
    }

    private static void initServer() {
        new Thread(() -> {
            serverUnit = new MajorServerUnit(serverID, serverType, version);
            server = new MajorConfigServer(port, new MajorConfigServerManager(), serverUnit);
            server.listen();
        }).start();
    }


    private static int port = 8081;
    private static MajorConfigServer server;
    private static MajorServerUnit serverUnit;
    private static int serverID = 1;
    private static byte serverType = Protocol.MAJOR_CONFIG_TYPE;
    private static byte version = 0;
}
