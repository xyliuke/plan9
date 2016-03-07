package cn.gocoding.common.network.tcp.server;

import java.nio.channels.SocketChannel;

/**
 * 处理tcp服务器端数据的统一接口
 * Created by liuke on 16/3/4.
 */
public interface ServerOperationInterface {
    String getID();

    void createOperation(SocketChannel socketChannel);

    /**
     * 处理服务器端接收的数据
     *
     * @param data 接收的数据
     * @return 如果在当前函数中处理过, 则返回true, 否则返回false
     */
    boolean dealWithData(byte[] data);

    void write(byte[] data);
}
