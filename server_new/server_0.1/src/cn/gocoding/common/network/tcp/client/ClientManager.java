package cn.gocoding.common.network.tcp.client;

import java.nio.channels.AsynchronousSocketChannel;

/**
 * 处理客户端的数据管理
 * Created by liuke on 16/3/9.
 */
public interface ClientManager {
    void connect(AsynchronousSocketChannel socket);
    boolean handle(byte[] data);
    void write(byte[] data);
    void close();
}
