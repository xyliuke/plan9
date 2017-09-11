package cn.gocoding.common.tcp;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.AsynchronousChannelGroup;
import java.nio.channels.AsynchronousSocketChannel;
import java.nio.channels.CompletionHandler;
import java.util.concurrent.Executors;

/**
 * tcp客户端
 * Created by liuke on 16/3/21.
 */
public class TcpClient {


    public TcpClient() {
        try {
            group = AsynchronousChannelGroup.withThreadPool(Executors.newSingleThreadExecutor());
        } catch (IOException e) {

        }
    }

    public void connect(String ip, int port) {
        try {
            socketChannel = AsynchronousSocketChannel.open(group);
            socketChannel.connect(new InetSocketAddress(ip, port), socketChannel, new CompletionHandler<Void, AsynchronousSocketChannel>() {
                @Override
                public void completed(Void result, AsynchronousSocketChannel attachment) {
                    if (dele != null) {
                        dele.connect();
                    }
                    ByteBuffer byteBuffer = ByteBuffer.allocate(1024);
                    socketChannel.read(byteBuffer, byteBuffer, new CompletionHandler<Integer, ByteBuffer>() {
                        @Override
                        public void completed(Integer result, ByteBuffer attachment) {
                            byte[] bytes = new byte[result];
                            attachment.flip();
                            attachment.get(bytes);
                            if (dele != null) {
                                dele.read(bytes);
                            }
                        }

                        @Override
                        public void failed(Throwable exc, ByteBuffer attachment) {

                        }
                    });
                }

                @Override
                public void failed(Throwable exc, AsynchronousSocketChannel attachment) {
                    if (dele != null) {
                        dele.close();
                    }
                    try {
                        socketChannel.close();
                    } catch (IOException e) {

                    }

                }
            });
        } catch (IOException e) {

        }
    }

    public void disconnect() {
        try {
            if (socketChannel != null) {
                socketChannel.close();
                if (dele != null) {
                    dele.close();
                }
            }
            socketChannel = null;
        } catch (IOException e) {

        }
    }

    public void write(byte[] data) {
        if (data != null && socketChannel != null && socketChannel.isOpen()) {
            ByteBuffer byteBuffer = ByteBuffer.allocate(data.length);
            byteBuffer.put(data);
            byteBuffer.flip();
            socketChannel.write(byteBuffer, null, new CompletionHandler<Integer, Object>() {
                @Override
                public void completed(Integer result, Object attachment) {
                    if (dele != null) {
                        dele.write(data);
                    }
                }

                @Override
                public void failed(Throwable exc, Object attachment) {
                }
            });
        }
    }


    public void setDelegate(TcpClientDelegate delegate) {
        dele = delegate;
    }

    private AsynchronousSocketChannel socketChannel;
    private AsynchronousChannelGroup group;
    private TcpClientDelegate dele;
}
