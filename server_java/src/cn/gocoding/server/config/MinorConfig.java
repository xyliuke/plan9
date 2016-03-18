package cn.gocoding.server.config;

import cn.gocoding.server.base.ClientUnit;
import cn.gocoding.server.base.ServerUnit;
import cn.gocoding.server.protocol.Protocol;

/**
 * 次配置服务,包含一个tcp-server和一个tcp-client,tcp-server被其他微服务连接,
 * tcp-client连接主配置服务器
 * Created by liuke on 16/3/13.
 */
public class MinorConfig {

    public static void run(String majorServerIP, int majorServerPort, int port, int version,
                           int clientID, byte managerServerType, int managerServerNum) {
        MinorConfig.majorServerIP = majorServerIP;
        MinorConfig.majorServerPort = majorServerPort;
        MinorConfig.minorServerPort = port;
        MinorConfig.version = (byte)version;
        MinorConfig.clientID = clientID;
        MinorConfig.managerServerType = managerServerType;
        MinorConfig.managerServerNum = managerServerNum;
        initClient();
        initServer();
    }

    private static void initServer() {
        new Thread(() -> {
            serverUnit = new ServerUnit(serverID, serverType, version);
            server = new MinorConfigServer(minorServerPort, new MinorConfigServerManager(), serverUnit);
            server.listen();
        }).start();
    }

    private static void initClient() {
        new Thread(() -> {
            clientUnit = new MinorClientUnit(clientID, version, managerServerType, managerServerNum);
            client = new MinorConfigClient(clientUnit, new MinorConfigClientManager(clientUnit, managerServerType, managerServerNum));
            client.connect(majorServerIP, majorServerPort);
        }).start();
    }

    private static String majorServerIP = "127.0.0.1";
    private static int majorServerPort = 8081;
    private static int minorServerPort = 8090;
    private static MinorConfigServer server;
    private static MinorConfigClient client;
    private static MinorClientUnit clientUnit;
    private static ServerUnit serverUnit;

    private static int clientID = 0;
    private static byte version = 0;
    private static int serverID = 1;
    private static byte serverType = Protocol.MINOR_CONFIG_TYPE;

    private static byte managerServerType = Protocol.CONNECTION_SERVER_TYPE;
    private static int managerServerNum = 1;
}
