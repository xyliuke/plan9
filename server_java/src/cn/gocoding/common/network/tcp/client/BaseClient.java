package cn.gocoding.common.network.tcp.client;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.AsynchronousChannelGroup;
import java.nio.channels.AsynchronousSocketChannel;
import java.nio.channels.CompletionHandler;
import java.util.concurrent.Executors;

/**
 * 基本的TCP客户端
 * Created by liuke on 16/3/9.
 */
public class BaseClient {

    public BaseClient(ClientManager manager) {
        try {
            this.manager = manager;
            group = AsynchronousChannelGroup.withThreadPool(Executors.newSingleThreadExecutor());
        } catch (IOException e) {
        }
    }

    public void connect(String ip, int port) {
        this.ip = ip;
        this.port = port;
        try {
            socketChannel = AsynchronousSocketChannel.open(group);
            socketChannel.connect(new InetSocketAddress(ip, port), socketChannel, new CompletionHandler<Void, AsynchronousSocketChannel>() {
                @Override
                public void completed(Void result, AsynchronousSocketChannel attachment) {
                    logger.info("connect to server");
                    manager.connect(socketChannel);
                    ByteBuffer byteBuffer = ByteBuffer.allocate(BUF_SIZE);
                    ReadCommpleteHandler readCommpleteHandler = new ReadCommpleteHandler(socketChannel, manager);
                    socketChannel.read(byteBuffer, byteBuffer, readCommpleteHandler);
                }

                @Override
                public void failed(Throwable exc, AsynchronousSocketChannel attachment) {
                    logger.error("connect to server error, the reason : {}", exc.getMessage());
                    try {
                        socketChannel.close();
                    } catch (IOException e) {

                    }

                    manager.close();
                }
            });
        } catch (IOException e) {
            logger.error("connect to ip : {} , port : {} error, the reason : {}", ip, port, e.getMessage());
        }

    }

    public void reconnect() {
        logger.info("reconnet to server : {} : {}", ip, port);
        connect(this.ip, this.port);
    }

    public void write(byte[] data) {
        if (data != null) {
            ByteBuffer byteBuffer = ByteBuffer.allocate(data.length);
            byteBuffer.put(data);
            byteBuffer.flip();
            socketChannel.write(byteBuffer, null, new CompletionHandler<Integer, Object>() {
                @Override
                public void completed(Integer result, Object attachment) {
                    logger.info("write the data success, the data : {}", data);
                }

                @Override
                public void failed(Throwable exc, Object attachment) {
                    logger.info("write the data error, data : {} ", data);
                }
            });
        } else {
            logger.error("write error, the data can not be null");
        }
    }

    private AsynchronousChannelGroup group;
    private AsynchronousSocketChannel socketChannel;
    private static final Logger logger = LogManager.getLogger(BaseClient.class);
    private static final int BUF_SIZE = 1024 * 2;
    private ClientManager manager;
    private String ip;
    private int port;
}


class ReadCommpleteHandler implements CompletionHandler<Integer, ByteBuffer> {

    public ReadCommpleteHandler(AsynchronousSocketChannel socketChannel, ClientManager manager) {
        if (socketChannel != null) {
            socketChannelWeakReference = new WeakReference<>(socketChannel);
        }
        if (manager != null) {
            managerWeakReference = new WeakReference<>(manager);
        }
    }

    @Override
    public void completed(Integer result, ByteBuffer attachment) {
        if (socketChannelWeakReference != null && socketChannelWeakReference.get() != null) {
            try {
                if (result > 0) {
                    byte[] bytes = new byte[result];
                    attachment.flip();
                    attachment.get(bytes);
                    logger.info("read data from server : {}, the data : {}", socketChannelWeakReference.get().getRemoteAddress(), bytes);
                    attachment.flip();
                    if (managerWeakReference != null && managerWeakReference.get() != null) {
                        managerWeakReference.get().handle(bytes);
                    }
                } else if (result < 0){
                    logger.info("disconnect by server : {}", socketChannelWeakReference.get().getRemoteAddress());
                    socketChannelWeakReference.get().close();
                    managerWeakReference.get().close();
                }
            } catch (IOException e) {

            } finally {
                if (result > 0) {
                    socketChannelWeakReference.get().read(attachment, attachment, this);
                }
            }
        }
    }

    @Override
    public void failed(Throwable exc, ByteBuffer attachment) {
        if (socketChannelWeakReference != null && socketChannelWeakReference.get() != null) {
            try {
                logger.error("read data from server error, server address : {}, reason : {}", socketChannelWeakReference.get().getRemoteAddress(), exc.getMessage());
                managerWeakReference.get().close();
            } catch (IOException e) {
            }
        }
    }

    private static final Logger logger = LogManager.getLogger(ReadCommpleteHandler.class);
    private WeakReference<AsynchronousSocketChannel> socketChannelWeakReference;
    private WeakReference<ClientManager> managerWeakReference;
}