package cn.gocoding.server.base;

/**
 * 客户端基本数据单元
 * Created by liuke on 16/3/9.
 */
public class ClientUnit {

    public ClientUnit(int clientID, byte version) {
        this.clientID = clientID;
        this.version = version;
    }

    public int getClientID() {
        return clientID;
    }

    public byte getVersion() {
        return version;
    }

    private int clientID;
    private byte version;
}
