package cn.gocoding.common.network.tcp.server;

import java.nio.channels.AsynchronousSocketChannel;

/**
 * 处理服务器连接的接口
 * Created by liuke on 16/3/9.
 */
public interface ServerManager {

    /**
     * 创建一个新处理连接的对象
     * @param socketChannel 客户端的socket
     */
    void newConnection(AsynchronousSocketChannel socketChannel);

    /**
     * 关闭连接
     */
    void close(AsynchronousSocketChannel socketChannel);

    /**
     * 处理接受的数据
     * @param data 数据
     * @return 处理完成返回true,未处理返回false
     */
    boolean handle(AsynchronousSocketChannel socketChannel, byte[] data);

    /**
     * 写入客户端数据
     * @param data 写入数据
     */
    void write(AsynchronousSocketChannel socketChannels, byte[] data);
}
