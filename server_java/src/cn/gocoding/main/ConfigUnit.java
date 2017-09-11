package cn.gocoding.main;

import java.util.ArrayList;
import java.util.List;

/**
 * 配置参数项
 * Created by liuke on 16/3/18.
 */
public class ConfigUnit {


    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
    }

    public String getMajorIP() {
        return majorIP;
    }

    public void setMajorIP(String majorIP) {
        this.majorIP = majorIP;
    }

    public int getMajorPort() {
        return majorPort;
    }

    public void setMajorPort(int majorPort) {
        this.majorPort = majorPort;
    }

    public int getVersion() {
        return version;
    }

    public void setVersion(int version) {
        this.version = version;
    }

    public int getClientID() {
        return clientID;
    }

    public void setClientID(int clientID) {
        this.clientID = clientID;
    }

    public byte getManagerServerType() {
        return managerServerType;
    }

    public void setManagerServerType(byte managerServerType) {
        this.managerServerType = managerServerType;
    }

    public int getManagerServerNum() {
        return managerServerNum;
    }

    public void setManagerServerNum(int managerServerNum) {
        this.managerServerNum = managerServerNum;
    }

    public void setCheckClientDisconnectPerios(int checkClientDisconnectPerios) {
        this.checkClientDisconnectPerios = checkClientDisconnectPerios;
    }

    public int getCheckClientDisconnectPerios() {
        return checkClientDisconnectPerios;
    }


    public String getManagerServerJarFile() {
        return managerServerJarFile;
    }

    public void setManagerServerJarFile(String managerServerJarFile) {
        this.managerServerJarFile = managerServerJarFile;
    }

    public List<String> getManagerServerJarConfigFile() {
        return managerServerJarConfigFile;
    }

    public void addManagerServerJarConfigFile(String managerServerJarConfigFile) {
        this.managerServerJarConfigFile.add(managerServerJarConfigFile);
    }

    private int port;//服务器

    private String majorIP;//major-server的ip
    private int majorPort;//major-server的port
    private int version = 1;//版本号
    private int clientID;//客户端的id
    private byte managerServerType;//minor-server管理的微服务类型
    private int managerServerNum;//minor-server管理的微服务数量
    int checkClientDisconnectPerios = 60 * 1000 * 10;
    private String managerServerJarFile;//minor-server管理的微服务的jar路径
    private List<String> managerServerJarConfigFile = new ArrayList<>();//minor-server管理的微服务的jar路径的配置文件
}
