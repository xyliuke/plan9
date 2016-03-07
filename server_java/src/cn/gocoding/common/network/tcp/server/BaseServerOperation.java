package cn.gocoding.common.network.tcp.server;

import cn.gocoding.common.util.IDUtil;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

/**
 * 实现
 * Created by liuke on 16/3/5.
 */
public class BaseServerOperation implements ServerOperationInterface {

    public static final Logger logger = LogManager.getLogger(BaseServerOperation.class);

    public BaseServerOperation() {
        id = IDUtil.createUniqueID();
    }


    @Override
    public String getID() {
        return id;
    }

    @Override
    public void createOperation(SocketChannel socketChannel) {
        socketChannelWeakReference = new WeakReference<>(socketChannel);
    }

    @Override
    public boolean dealWithData(byte[] data) {
        System.arraycopy(data, 0, buf, size, data.length);
        size += data.length;
        return false;
    }

    @Override
    public void write(byte[] data) {
        if (socketChannelWeakReference != null && socketChannelWeakReference.get() != null) {
            SocketChannel socketChannel = socketChannelWeakReference.get();
            ByteBuffer byteBuffer = ByteBuffer.allocate(data.length);
            byteBuffer.put(data);
            try {
                while (byteBuffer.hasRemaining()) {
                    socketChannel.write(byteBuffer);
                }
            } catch (IOException e) {
                try {
                    logger.error("write data to client error, the client address : {} , the data : {}",
                            socketChannel.getRemoteAddress(), data);
                } catch (IOException e1) {
                }
            }
        }
    }



    private String id;
    private WeakReference<SocketChannel> socketChannelWeakReference;
    private byte[] buf = new byte[0xFFFF];//64K容量
    private int size = 0;//buf中包含字节数
}
