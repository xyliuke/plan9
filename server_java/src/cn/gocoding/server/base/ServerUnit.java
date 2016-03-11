package cn.gocoding.server.base;

/**
 * 每个服务器的基本数据
 * Created by liuke on 16/3/9.
 */
public class ServerUnit {

    public ServerUnit(int serverID, byte serverType, byte version) {
        this.serverID = serverID;
        this.serverType = serverType;
        this.version = version;
    }

    public int getServerID() {
        return serverID;
    }

    public byte getServerType() {
        return serverType;
    }

    public byte getVersion() {
        return version;
    }

    private int serverID;
    private byte serverType;
    private byte version;
}
