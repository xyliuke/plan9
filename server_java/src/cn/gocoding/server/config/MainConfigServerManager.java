package cn.gocoding.server.config;

import cn.gocoding.server.base.BaseServerManagerImpl;

import java.nio.channels.AsynchronousSocketChannel;

/**
 * 主配置服务器数据管理
 * Created by liuke on 16/3/11.
 */
public class MainConfigServerManager extends BaseServerManagerImpl {

    @Override
    public void newConnection(AsynchronousSocketChannel socketChannel) {
        MainConfigServerOperation op = new MainConfigServerOperation(socketChannel, serverUnit);
        
        bindSocketAndOperation(socketChannel, op);
    }
}
