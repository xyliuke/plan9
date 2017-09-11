package cn.gocoding.common.network.tcp;

import java.nio.channels.AsynchronousSocketChannel;

/**
 * 基本tcp-server数据管理接口
 * Created by liuke on 16/3/16.
 */
public interface BaseTcpServerManager {
    void newConnection(AsynchronousSocketChannel socketChannel);
    boolean handle(byte[] data);
    void close();
}
