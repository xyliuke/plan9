package cn.gocoding.server.config;

import cn.gocoding.main.ConfigUnit;
import cn.gocoding.server.base.ServerUnit;
import cn.gocoding.server.protocol.Protocol;

import java.util.List;

/**
 * 次配置服务,包含一个tcp-server和一个tcp-client,tcp-server被其他微服务连接,
 * tcp-client连接主配置服务器
 * Created by liuke on 16/3/13.
 */
public class MinorConfig {

    public static void run(ConfigUnit configUnit) {
        MinorConfig.majorServerIP = configUnit.getMajorIP();
        MinorConfig.majorServerPort = configUnit.getMajorPort();
        MinorConfig.minorServerPort = configUnit.getPort();
        MinorConfig.version = (byte)configUnit.getVersion();
        MinorConfig.clientID = configUnit.getClientID();
        MinorConfig.managerServerType = configUnit.getManagerServerType();
        MinorConfig.managerServerNum = configUnit.getManagerServerNum();
        checkClientDisconnectPeriod = configUnit.getCheckClientDisconnectPerios();
        jarFile = configUnit.getManagerServerJarFile();
        jarConfigFile = configUnit.getManagerServerJarConfigFile();
        initClient();
        initServer();
    }

    private static void initServer() {
        new Thread(() -> {
            serverUnit = new ServerUnit(serverID, serverType, version);
            MinorConfigServerManager manager = new MinorConfigServerManager();
            manager.setCheckDisconnectPeriod(checkClientDisconnectPeriod);
            server = new MinorConfigServer(minorServerPort, manager, serverUnit);
            server.listen();
        }).start();
    }

    private static void initClient() {
        new Thread(() -> {
            clientUnit = new MinorClientUnit(clientID, version, managerServerType, managerServerNum);
            MinorConfigClientManager manager = new MinorConfigClientManager(clientUnit, managerServerType, managerServerNum);
            manager.setMircoServerFile(jarFile, jarConfigFile);
            client = new MinorConfigClient(clientUnit, manager);
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
    private static int checkClientDisconnectPeriod;

    private static int clientID = 0;
    private static byte version = 0;
    private static int serverID = 1;
    private static byte serverType = Protocol.MINOR_CONFIG_TYPE;

    private static byte managerServerType = Protocol.CONNECTION_SERVER_TYPE;
    private static int managerServerNum = 1;
    private static String jarFile;
    private static List<String> jarConfigFile;
}
