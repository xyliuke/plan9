package cn.gocoding.server.biz.connect;

/**
 * Created by liuke on 16/3/21.
 */
class ConnectInstance {

    public static ConnectTcpClient4RouteManager getRouteManager() {
        return routeManager;
    }

    public static void setRouteManager(ConnectTcpClient4RouteManager routeManager) {
        ConnectInstance.routeManager = routeManager;
    }

    public static ConnectTcpServerManager getServerManager() {
        return serverManager;
    }

    public static void setServerManager(ConnectTcpServerManager serverManager) {
        ConnectInstance.serverManager = serverManager;
    }

    public static ConnectTcpClient4AuthManager getAuthManager() {
        return authManager;
    }

    public static void setAuthManager(ConnectTcpClient4AuthManager authManager) {
        ConnectInstance.authManager = authManager;
    }

    private static ConnectTcpClient4RouteManager routeManager;
    private static ConnectTcpServerManager serverManager;
    private static ConnectTcpClient4AuthManager authManager;
}
