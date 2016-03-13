package cn.gocoding.server.base;

import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.network.tcp.client.ClientManager;
import cn.gocoding.common.tuple.Tuple2;
import cn.gocoding.server.protocol.Protocol;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.nio.channels.AsynchronousSocketChannel;
import java.nio.channels.CompletionHandler;
import java.util.Timer;
import java.util.TimerTask;

interface BaseClientManagerImplDelegate {
    void reconnect();
}

/**
 * 基本协议的tcp客户端管理类
 * Created by liuke on 16/3/10.
 */
public class BaseClientManagerImpl implements ClientManager, MessageNotifyRecevier {

    public BaseClientManagerImpl(ClientUnit unit) {
        this.unit = unit;

    }

    public void setDelegate(BaseClientManagerImplDelegate delegate) {
        delegateWeakReference = new WeakReference<>(delegate);
    }

    @Override
    public void connect(AsynchronousSocketChannel socketChannel) {
        if (socketChannel != null) {
            try {
                socketChannelWeakReference = new WeakReference<>(socketChannel);
                addr = socketChannel.getLocalAddress().toString();
                serverAddr = socketChannel.getRemoteAddress().toString();
                sendPing();
                checkDisconnect();
                stopReconnect();
            } catch (IOException e) {
            }
        }
    }

    @Override
    public boolean handle(byte[] data) {
        sendPing();
        checkDisconnect();
        return false;
    }

    @Override
    public void write(byte[] data) {
        if (data != null && socketChannelWeakReference != null) {
            logger.info("client write data to server, client address : {} server address : {}, data : {}", addr, serverAddr, data);
            AsynchronousSocketChannel socketChannel = socketChannelWeakReference.get();
            if (socketChannel != null) {
                ByteBuffer byteBuffer = ByteBuffer.allocate(data.length);
                byteBuffer.put(data);
                byteBuffer.flip();
                socketChannel.write(byteBuffer, null, new CompletionHandler<Integer, Object>() {
                    @Override
                    public void completed(Integer result, Object attachment) {
                        logger.info("client write data to server success, client address : {} server address : {}, data : {}", addr, serverAddr, data);
                    }

                    @Override
                    public void failed(Throwable exc, Object attachment) {
                        logger.info("client write data to server error, client address : {} server address : {}, data : {}, error reason : {}", addr, serverAddr, data, exc.getStackTrace());
                    }
                });
            }
        }
    }

    @Override
    public void close() {
        stopPing();
        reconnect();
    }

    @Override
    public void sendMessage(byte[] data) {
        write(data);
    }

    @Override
    public void sendMessage(int clientID, byte[] data) {
        //在客户端不需要实现
    }

    private void sendPing() {
        if (pingTimerTask != null) {
            pingTimerTask.cancel();
            pingTimerTask = null;
        }

        pingTimerTask = new TimerTask() {
            @Override
            public void run() {
                writePing();
            }
        };

        pingTimer.schedule(pingTimerTask, pingInterval);
    }

    private void stopPing() {
        if (pingTimerTask != null) {
            pingTimerTask.cancel();
            pingTimerTask = null;
        }
        if (disconnectTimerTask != null) {
            disconnectTimerTask.cancel();
            disconnectTimerTask = null;
        }
    }

    private void checkDisconnect() {
        if (disconnectTimerTask != null) {
            disconnectTimerTask.cancel();
            disconnectTimerTask = null;
        }
        disconnectTimerTask = new TimerTask() {
            @Override
            public void run() {
                logger.error("disconnect to server");
                if (delegateWeakReference != null && delegateWeakReference.get() != null) {
                    delegateWeakReference.get().reconnect();
                }
            }
        };
        pingTimer.schedule(disconnectTimerTask, disconnetInterval);
    }

    private void reconnect() {
        if (reconnectTimer != null) {
            reconnectTimer.cancel();
            reconnectTimer = null;

        }
        reconnectTimer = new Timer();
        reconnectTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                if (delegateWeakReference != null && delegateWeakReference.get() != null) {
                    delegateWeakReference.get().reconnect();
                }
            }
        }, pingInterval);
    }

    private void stopReconnect() {
        if (reconnectTimer != null) {
            reconnectTimer.cancel();
            reconnectTimer = null;
        }
    }

    private void writePing() {
        byte st = 0;
        if (serverUnit != null) {
            st = serverUnit.getServerType();
        }
        write(st, Protocol.PING_DATA_TYPE, 4, "ping".getBytes());
    }

    private void write(byte serverTYpe, byte dataType, int len, byte[] data) {
        Tuple2<ErrorCode, byte[]> protocol = Protocol.createProtocol(unit.getClientID(), unit.getVersion(), serverTYpe, dataType, len, data);
        if (protocol._1().isPresent() && !ErrorCode.isError(protocol._1().get())) {
            write(protocol._2().get());
        } else {
            logger.error("write data to server : {} error, the reason : {}", addr, protocol._1());
        }
    }

    private WeakReference<BaseClientManagerImplDelegate> delegateWeakReference;
    private ClientUnit unit;
    private ServerUnit serverUnit;//连接成功后的服务器基本数据
    private Timer pingTimer = new Timer();
    private TimerTask pingTimerTask = null;
    private TimerTask disconnectTimerTask = null;

    private Timer reconnectTimer;


    private WeakReference<AsynchronousSocketChannel> socketChannelWeakReference;
    private int pingInterval = 1000 * 15;
    private int disconnetInterval = pingInterval * 2;
    private static final Logger logger = LogManager.getLogger(BaseClientManagerImpl.class);
    private String addr;
    private String serverAddr;
}
