package cn.gocoding.common.network.tcp.server;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.Set;

/**
 * tcp服务器端的基类
 * Created by liuke on 16/3/3.
 */
public class BaseServer {

    public static final int TCP_BUF_SIZE = 1024 * 2;

    public BaseServer(int port, ServerOperationManager manager) throws IOException{
        this.port = port;
        this.manager = manager;
        serverSocketChannel.bind(new InetSocketAddress(port));
        serverSocketChannel.configureBlocking(false);
        logger.debug("init server socket with port {}", port);

        selector = Selector.open();
        serverSocketChannel.register(selector, SelectionKey.OP_ACCEPT);
    }

    public void listen() {
        new Thread(() -> {
            try {
                while (true) {
                    if (selector.select() == 0) continue;

                    Set<SelectionKey> selectionKeys = selector.selectedKeys();
                    Iterator<SelectionKey> iterator = selectionKeys.iterator();
                    while (iterator.hasNext()) {
                        SelectionKey selectionKey = iterator.next();
                        handleKey(selectionKey);
                        iterator.remove();
                    }
                }
            } catch (IOException e) {

            }

        }).start();

    }

    public void close() throws IOException{
        serverSocketChannel.close();
    }


    private void handleKey(SelectionKey key) throws IOException {
        if (key.isAcceptable()) {
            SocketChannel socketChannel = serverSocketChannel.accept();//(SocketChannel)key.channel();
            socketChannel.configureBlocking(false);
            logger.debug("accept client address : {}, {}", socketChannel.socket().getInetAddress(), socketChannel.socket().getPort());

            if (manager != null) {
                manager.createConnection(socketChannel);
            }

            socketChannel.register(selector, SelectionKey.OP_READ);
        } else if (key.isReadable()) {
            ByteBuffer byteBuffer = ByteBuffer.allocate(TCP_BUF_SIZE);
            SocketChannel socketChannel = (SocketChannel) key.channel();

            int readSize = 0;
            byte[] bytes;
            while ((readSize = socketChannel.read(byteBuffer)) > 0) {
                byteBuffer.flip();
                bytes = new byte[readSize];
                byteBuffer.get(bytes);
                byteBuffer.clear();
                logger.debug("read from client address : {}, {}, data : {}", socketChannel.socket().getInetAddress(),
                        socketChannel.socket().getPort(), bytes);
                if (manager != null) {
                    manager.dealWithData(socketChannel, bytes);
                }
                socketChannel.register(selector, SelectionKey.OP_READ);
            }
            if (readSize < 0) {
                if (manager != null) {
                    manager.close(socketChannel);
                } else {
                    socketChannel.close();
                }
                logger.debug("close client address : {}, {} connection", socketChannel.socket().getInetAddress(),
                        socketChannel.socket().getPort());
            }
        }
    }

    protected void write(SocketChannel socketChannel, byte[] data) {
        if (manager != null) {
            manager.write(socketChannel, data);
        }
    }


    private ServerSocketChannel serverSocketChannel = ServerSocketChannel.open();
    private Selector selector;
    private int port;
    private static Logger logger = LogManager.getLogger(BaseServer.class);
    private ServerOperationManager manager;
}
