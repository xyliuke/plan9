package cn.gocoding.server.biz.auth;

import cn.gocoding.server.base.BaseServerManagerImpl;

import java.nio.channels.AsynchronousSocketChannel;

/**
 * Created by liuke on 16/3/21.
 */
public class AuthTcpServerManager extends BaseServerManagerImpl {
    @Override
    public void newConnection(AsynchronousSocketChannel socketChannel) {
        AuthTcpServerOperation op = new AuthTcpServerOperation(socketChannel, serverUnit);
        bindSocketAndOperation(socketChannel, op);
        checkClientDisconnect();
    }


}
