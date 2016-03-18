package cn.gocoding.server.base;

import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.network.tcp.server.ServerManager;
import cn.gocoding.common.tuple.Tuple6;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.AsynchronousSocketChannel;
import java.nio.channels.CompletionHandler;
import java.util.HashMap;
import java.util.Map;

/**
 * 实现服务器管理基本类
 * Created by liuke on 16/3/10.
 */
public class BaseServerManagerImpl implements ServerManager, MessageNotifyRecevier {

    public void setServerUnit(ServerUnit unit) {
        serverUnit = unit;
    }

    public void bindClient(int clientID, AsynchronousSocketChannel socketChannel) {
        if (socketChannel != null) {
            try {
                socketChannelMap.put(clientID, socketChannel);
                logger.info("bind client id and socket, client id is : {}, socket is : {}", clientID, socketChannel.getRemoteAddress());
            } catch (IOException e) {
            }
        }
    }

    public int getClientIDBySocket(AsynchronousSocketChannel socketChannel) {
        for (int id : socketChannelMap.keySet()) {
            if (socketChannelMap.get(id).equals(socketChannel)) {
                return id;
            }
        }
        return -1;
    }

    @Override
    public void newConnection(AsynchronousSocketChannel socketChannel) {
        if (socketChannel != null) {
            operationMap.put(socketChannel, new BaseServerOperation(socketChannel, serverUnit));
        }
    }

    @Override
    public void close(AsynchronousSocketChannel socketChannel) {
        if (socketChannel == null) {
            return;
        }
        try {
            socketChannel.close();
            operationMap.remove(socketChannel);

            for (int id : socketChannelMap.keySet()) {
                AsynchronousSocketChannel s = socketChannelMap.get(id);
                if (s.equals(socketChannel)) {
                    socketChannelMap.remove(id);
                    break;
                }
            }

        } catch (IOException e) {
            try {
                logger.error("close client {} error, the reason : {}", socketChannel.getRemoteAddress(), e.getStackTrace());
            } catch (Exception e1) {

            }
        }
    }

    @Override
    public boolean handle(AsynchronousSocketChannel socketChannel, byte[] data) {
        if (socketChannel != null && data != null) {
            try {
                logger.info("handle client {} data {}", socketChannel.getRemoteAddress(), data);
                if (operationMap.containsKey(socketChannel)) {
                    BaseServerOperation serverOperation = operationMap.get(socketChannel);
                    Tuple6<ErrorCode, Integer, Integer, Byte, Integer, byte[]> item = serverOperation.parse(data);
                    if (item._1().isPresent() && !ErrorCode.isError(item._1().get())) {
                        boolean suc = handle(item);

                        if (suc) {
                            serverOperation.handleSuccess();
                            return true;
                        } else {
                            return serverOperation.handle(item);
                        }
                    }

                }
            } catch (IOException e) {
                logger.error("handle data from client error, the reason : {}", e.getMessage());
            } finally {
                return false;
            }
        } else {
            logger.info("handle client or data is null");
        }

        return false;
    }

    public boolean handle(Tuple6<ErrorCode, Integer, Integer, Byte, Integer, byte[]> item) {
        return false;
    }

    @Override
    public void write(AsynchronousSocketChannel socketChannel, byte[] data) {
        if (operationMap.containsKey(socketChannel)) {
            BaseServerOperation serverOperation = operationMap.get(socketChannel);
            serverOperation.write(data);
        }
    }

    @Override
    public void sendMessage(byte[] data) {
        for (AsynchronousSocketChannel socketChannel : operationMap.keySet()) {
            write(socketChannel, data);
        }
    }

    @Override
    public void sendMessage(int clientID, byte[] data) {
        if (socketChannelMap.containsKey(clientID)) {
            AsynchronousSocketChannel socketChannel = socketChannelMap.get(clientID);
            write(socketChannel, data);
        }
    }


    protected void bindSocketAndOperation(AsynchronousSocketChannel socketChannel, BaseServerOperation operation) {
        if (socketChannel != null && operation != null) {
            operationMap.put(socketChannel, operation);
        }
    }

    private Map<AsynchronousSocketChannel, BaseServerOperation> operationMap = new HashMap<>();
    private Map<Integer, AsynchronousSocketChannel> socketChannelMap = new HashMap<>();
    private static final Logger logger = LogManager.getLogger(BaseServerManagerImpl.class);
    protected ServerUnit serverUnit;
}
