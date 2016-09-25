package cn.gocoding.server.base;

/**
 * 消息通知接收者接口
 * Created by liuke on 16/3/10.
 */
public interface MessageNotifyRecevier {
    /**
     * 即可用于一个程序中的client向本身中的server发送数据,也可返过来发送
     * 发送给所有连接tcp server的client;或者发送给client连接的server
     * @param data 数据
     */
    void receiveMessage(int id, byte[] data);

    /**
     * 单向发送,只能是本程序中的client向本身中的server发送数据
     * 发送给连接tcp server中的某一个客户端
     * @param clientID 客户端id
     * @param data 数据
     */
    void receiveMessage(int id, int clientID, byte[] data);
}
