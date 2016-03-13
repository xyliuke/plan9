package cn.gocoding.server.base;

import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.tuple.Tuple2;
import cn.gocoding.common.tuple.Tuple6;
import cn.gocoding.server.protocol.Protocol;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.nio.channels.AsynchronousSocketChannel;
import java.nio.channels.CompletionHandler;

/**
 * 最基本的处理服务器每个连接数据的类
 * Created by liuke on 16/3/10.
 */
public class BaseServerOperation {

    public BaseServerOperation(AsynchronousSocketChannel socketChannel, ServerUnit unit) {
        this.unit = unit;
        if (socketChannel != null) {
            try {
                socketChannelWeakReference = new WeakReference<>(socketChannel);
                addr = socketChannel.getRemoteAddress().toString();
            } catch (IOException e) {
            }
        }
    }

    public boolean handle(byte[] data) {
        if (data != null) {
            System.arraycopy(data, 0, this.buf, size, data.length);
            size += data.length;

            Tuple6<ErrorCode, Integer, Integer, Byte, Integer, byte[]> item = Protocol.getProtocolItem(buf, size);
            if (item._1().isPresent() && !ErrorCode.isError(item._1().get())) {
                if (Protocol.isPingType(item._4().get())) {
                    logger.info("handle data from client {}, client id : {}, version : {}, ping type",
                            addr,
                            item._2().get(),
                            item._3().get());
                    logger.info("write pong data to client {}", addr);
                    byte[] pong = Protocol.createPongProtocol(item._2().get(), unit.getVersion());
                    write(pong);
                    return true;
                } else {
                    handle(item);
                }
            }
        }
        return false;
    }

    /**
     * 子类需要重写这个方法来处理
     * @param item 一个协议的所有项
     * @return 处理完成返回true,否则返回false
     */
    public boolean handle(Tuple6<ErrorCode, Integer, Integer, Byte, Integer, byte[]> item) {
        return false;
    }

    public void write(byte[] data) {
        if (socketChannelWeakReference != null && data != null) {
            AsynchronousSocketChannel socketChannel = socketChannelWeakReference.get();
            ByteBuffer byteBuffer = ByteBuffer.allocate(data.length);
            byteBuffer.put(data);
            byteBuffer.flip();
            if (socketChannel != null) {
                socketChannel.write(byteBuffer, null, new CompletionHandler<Integer, Object>() {
                    @Override
                    public void completed(Integer result, Object attachment) {
                        try {
                            logger.info("write to client {} data success, the data : {}", socketChannel.getRemoteAddress(), data);
                        } catch (IOException e) {

                        }
                    }

                    @Override
                    public void failed(Throwable exc, Object attachment) {
                        try {
                            logger.info("write to client {} data error, the data : {}", socketChannel.getRemoteAddress(), data);
                        } catch (IOException e) {

                        }
                    }
                });
            }
        }
    }


    protected void write(int clientID, byte version, byte serverType, byte dataType, int len, byte[] data) {
        Tuple2<ErrorCode, byte[]> protocol = Protocol.createProtocol(clientID, version, serverType, dataType, len, data);
        if (protocol._1().isPresent() && !ErrorCode.isError(protocol._1().get())) {
            write(protocol._2().get());
        } else {
            logger.error("write data to client error, the reason : {}", protocol._1().get());
        }
    }


    private WeakReference<AsynchronousSocketChannel> socketChannelWeakReference;
    private static final Logger logger = LogManager.getLogger(BaseServerOperation.class);
    private byte[] buf = new byte[BUF_SIZE];
    private int size = 0;
    public static final int BUF_SIZE = 64 * 2014;
    private ServerUnit unit;

    protected String addr = null;//记录客户端的地址,用于写log
}
