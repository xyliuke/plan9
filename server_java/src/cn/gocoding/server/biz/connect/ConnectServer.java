package cn.gocoding.server.biz.connect;

import cn.gocoding.main.ConfigUnit;
import cn.gocoding.server.base.ClientUnit;
import cn.gocoding.server.base.ServerUnit;
import cn.gocoding.server.protocol.Protocol;

/**
 * 供客户端连接的服务器
 * Created by liuke on 16/3/18.
 */
public class ConnectServer {


    public static void run(ConfigUnit configUnit) {

//        ClientUnit clientUnit = new ClientUnit(2, (byte) 1);
//        routeManager = new ConnectTcpClient4RouteManager(clientUnit);
//        client4Route = new ConnectTcpClient4Route(clientUnit, routeManager);
//        client4Route.connect(routeIP, routePort);

//        ConnectInstance.setRouteManager(routeManager);

        ServerUnit serverUnit = new ServerUnit(1, Protocol.CONNECTION_SERVER_TYPE, (byte) 1);
        serverManager = new ConnectTcpServerManager();
        tcpServer = new ConnectTcpServer(9099, serverManager, serverUnit);
        tcpServer.listen();

    }


    private static ConnectTcpClient4Route client4Route;
    private static ConnectTcpServer tcpServer;

    private static ConnectTcpClient4RouteManager routeManager;
    private static ConnectTcpServerManager serverManager;
    private static String routeIP = "";
    private static int routePort = 8095;
}
