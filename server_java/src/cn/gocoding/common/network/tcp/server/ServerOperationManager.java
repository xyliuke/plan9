package cn.gocoding.common.network.tcp.server;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.nio.channels.SocketChannel;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * 管理tcp服务端连接和数据处理类
 * Created by liuke on 16/3/4.
 */
public class ServerOperationManager {

    protected static Logger logger = LogManager.getLogger(ServerOperationManager.class);

    public void setThreadPoolNum(int num) {
        if (executorService == null) {
            executorService = Executors.newFixedThreadPool(num);
        }
    }

    public void createConnection(SocketChannel socketChannel) {
        logger.debug("create connection : {}", socketChannel);
    }

    /**
     * 将客户端编号和Socket绑定
     * @param clientID 客户端编号
     * @param socketChannel 客户端的Socket
     */
    public void bindClientIDAndSocket(int clientID, SocketChannel socketChannel) {
        if (connects.containsKey(socketChannel)) {
            ServerOperationInterface op = connects.get(socketChannel);
            userOperationMap.put(clientID, op);
        }
    }

    public void dealWithData(SocketChannel socketChannel, byte[] data) {
        logger.debug("deal with socket : {}", socketChannel);
        if (connects.containsKey(socketChannel)) {
            ServerOperationInterface op = connects.get(socketChannel);

            if (executorService != null) {
                executorService.execute(() -> op.dealWithData(data));
            } else {
                op.dealWithData(data);
            }
        }
    }

    public void write(SocketChannel socketChannel, byte[] data) {
        if (connects.containsKey(socketChannel)) {
            ServerOperationInterface op = connects.get(socketChannel);
            op.write(data);
        }
    }

    public void write(int clientID, byte[] data) {
        if (userOperationMap.containsKey(clientID)) {
            ServerOperationInterface op = userOperationMap.get(clientID);
            op.write(data);
        }
    }

    public void close(SocketChannel socketChannel) {
        try {
            socketChannel.close();
            connects.remove(socketChannel);
        } catch (IOException e) {
            try {
                logger.error("close client connection error, client address : {}", socketChannel.getRemoteAddress());
            } catch (IOException e1) {

            }
        }
    }

    private Map<SocketChannel, ServerOperationInterface> connects = new HashMap<>(); //socket和对应处理类的映射
    private Map<Integer, ServerOperationInterface> userOperationMap = new HashMap<>(); //客户端ID和对应处理类的映射
    private ExecutorService executorService;
}
