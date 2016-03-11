package cn.gocoding.common.network.tcp.server;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.AsynchronousChannelGroup;
import java.nio.channels.AsynchronousServerSocketChannel;
import java.nio.channels.AsynchronousSocketChannel;
import java.nio.channels.CompletionHandler;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

/**
 * 使用异步IO方式实现基本的TCP服务器
 * Created by liuke on 16/3/8.
 */
public class BaseServer {
    private static final Logger logger = LogManager.getLogger(BaseServer.class);


    public BaseServer(int port, ServerManager manager, int threadNum) {
        try {
            this.manager = manager;
            group = AsynchronousChannelGroup.withThreadPool(Executors.newFixedThreadPool(threadNum));
            serverSocketChannel = AsynchronousServerSocketChannel.open(group).bind(new InetSocketAddress(port));
        } catch (IOException e) {
            logger.error("create tcp server error, the reason : {}", e.getStackTrace());
        }
    }

    public void listen() {
        serverSocketChannel.accept(null, new CompletionHandler<AsynchronousSocketChannel, Object>() {
            @Override
            public void completed(AsynchronousSocketChannel result, Object attachment) {
                serverSocketChannel.accept(null, this);
                try {
                    logger.info("one client connect to server, client address {}", result.getRemoteAddress());
                    if (manager != null) {
                        manager.newConnection(result);
                    }

                    ByteBuffer byteBuffer = ByteBuffer.allocate(BUF_SIZE);
                    ReadCompletionHandler readCompletionHandler = new ReadCompletionHandler(result, manager);
                    result.read(byteBuffer, byteBuffer, readCompletionHandler);

                } catch (IOException e) {
                    try {
                        logger.error("tcp server accept client address {} error", result.getRemoteAddress());
                    } catch (IOException e1) {

                    }
                }
            }

            @Override
            public void failed(Throwable exc, Object attachment) {
                logger.error("tcp server accept client connection error, the reason : {}", exc.getStackTrace());
            }
        });

        try {
            group.awaitTermination(Long.MAX_VALUE, TimeUnit.SECONDS);
        } catch (InterruptedException e) {
            logger.error("tcp server listen error, the reason : {}", e.getMessage());
        }

    }






    private AsynchronousServerSocketChannel serverSocketChannel;
    private AsynchronousChannelGroup group;
    private ServerManager manager;
    private static final int BUF_SIZE = 1024 * 2;
}


class ReadCompletionHandler implements CompletionHandler<Integer, ByteBuffer> {
    private static final Logger logger = LogManager.getLogger(BaseServer.class);

    public ReadCompletionHandler(AsynchronousSocketChannel socketChannel, ServerManager manager) {
        socketChannelWeakReference = new WeakReference<>(socketChannel);
        if (manager != null) {
            managerWeakReference = new WeakReference<>(manager);
        }
    }

    @Override
    public void completed(Integer result, ByteBuffer attachment) {if (socketChannelWeakReference != null && socketChannelWeakReference.get() != null && result != null) {
            try {
                if (result.intValue() < 0) {
                    logger.info("the client close connection, client address : {}", socketChannelWeakReference.get().getRemoteAddress());
                    if (managerWeakReference != null && managerWeakReference.get() != null) {
                        managerWeakReference.get().close(socketChannelWeakReference.get());
                    }
                } else if (result.intValue() > 0){
                    byte[] data = new byte[result];
                    attachment.flip();
                    attachment.get(data);
                    logger.info("read data from client : {} , data : {}", socketChannelWeakReference.get().getRemoteAddress(), data);
                    if (managerWeakReference != null && managerWeakReference.get() != null) {
                        managerWeakReference.get().handle(socketChannelWeakReference.get(), data);
                    }
                    attachment.flip();
                }
            } catch (IOException e) {
                logger.info("read data from client error, the reason : ", e.getStackTrace());
            } finally {
                if (result.intValue() > 0) {
                    socketChannelWeakReference.get().read(attachment, attachment, this);
                }
            }
        } else {
            logger.info("read data from client, data : {}, but the client is null", attachment);
        }
    }

    @Override
    public void failed(Throwable exc, ByteBuffer attachment) {
        try {
            logger.info("read error from client : {} , the reason : {}", socketChannelWeakReference.get().getRemoteAddress(), exc.getStackTrace());
            if (managerWeakReference != null && managerWeakReference.get() != null) {
                if (managerWeakReference != null && managerWeakReference.get() != null) {
                    managerWeakReference.get().close(socketChannelWeakReference.get());
                }
            }
        } catch (Exception e) {
        }

    }

    private WeakReference<AsynchronousSocketChannel> socketChannelWeakReference;
    private WeakReference<ServerManager> managerWeakReference;
}