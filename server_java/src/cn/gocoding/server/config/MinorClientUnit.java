package cn.gocoding.server.config;

import cn.gocoding.server.base.ClientUnit;

/**
 * minor客户端数据
 * Created by liuke on 16/3/16.
 */
public class MinorClientUnit extends ClientUnit {
    public MinorClientUnit(int clientID, byte version, byte managerServerType, int managerServerNum) {
        super(clientID, version);

        this.managerServerType = managerServerType;
        this.managerServerNum = managerServerNum;
    }

    public byte getManagerServerType() {
        return managerServerType;
    }

    public int getManagerServerNum() {
        return managerServerNum;
    }


    @Override
    public String toString() {
        return new StringBuffer().append("client id : ").append(getClientID()).append(" , version : ").append(getVersion())
                .append(" , manager server type : ").append(getManagerServerType()).append(" , manager server num : ").append(getManagerServerNum()).toString();
    }

    private byte managerServerType;
    private int managerServerNum;
}
