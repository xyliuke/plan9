package cn.gocoding.server.config;

import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.tuple.Tuple6;
import cn.gocoding.common.tuple.Tuple7;
import cn.gocoding.server.base.BaseServerManagerImpl;
import cn.gocoding.server.protocol.Protocol;
import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.nio.channels.AsynchronousSocketChannel;
import java.util.HashMap;
import java.util.Map;

/**
 * 主配置服务器数据管理
 * Created by liuke on 16/3/11.
 */
public class MajorConfigServerManager extends BaseServerManagerImpl {

    @Override
    public void newConnection(AsynchronousSocketChannel socketChannel) {
        MajorConfigServerOperation op = new MajorConfigServerOperation(socketChannel, (MajorServerUnit) serverUnit);
        bindSocketAndOperation(socketChannel, op);
    }

    @Override
    public boolean handle(AsynchronousSocketChannel socketChannel, Tuple7<ErrorCode, Integer, Integer, Byte, Integer, byte[], byte[]> item) {
        if (super.handle(socketChannel, item)) {
            return true;
        }

        //manager处理

        int clientID = item._2().get();
        int version = item._3().get();
        byte type = item._4().get();

        byte serverType = Protocol.getServerType(type);
        byte dataType = Protocol.getDataType(type);


        if (serverType == Protocol.MAJOR_CONFIG_TYPE) {
            if (dataType == Protocol.NORMAL_JSON_DATA_TYPE) {
                String str = new String(item._6().get());
                try {
                    JSONObject data = JSON.parseObject(str);
                    logger.info("handle normal json data : {}", data);
                    if (data != null) {
                        if (data.containsKey(ConfigProtocol.MSG_TYPE)) {
                            if (ConfigProtocol.MSG_TYPE_IDENTIFICATION.equals(data.getString(ConfigProtocol.MSG_TYPE))
                                    && ConfigProtocol.IDENTIFY_MINOR_CONFIG.equals(data.getString(ConfigProtocol.IDENTITY))) {
                                //标识身份
                                byte clientManagerServerType = data.getByteValue(ConfigProtocol.MICRO_SERVER_TYPE);
                                int num = data.getInteger(ConfigProtocol.MICRO_SERVER_NUM);

                                MinorClientUnit clientUnit = new MinorClientUnit(clientID, (byte) version, clientManagerServerType, num);
                                clients.put(clientID, clientUnit);
                                logger.info("add minor config server success, the minor client data : {}", clientUnit);
                                return true;
                            }
                        }
                    }
                } catch (Exception e) {
                    logger.error("handle recv string data to json error : {}", e.getMessage());
                }
            }
        }

        return false;
    }

    @Override
    public void close(AsynchronousSocketChannel socketChannel) {
        int clientID = getClientIDBySocket(socketChannel);
        if (clientID >= 0) {
            //一个配置服务器失去连接
            handleCloseClientID(clientID);
        }
        super.close(socketChannel);
    }

    private void handleCloseClientID(int client) {
//        MinorClientUnit minorClientUnit = clients.get(client);
        clients.remove(client);
    }

    private Map<Integer, MinorClientUnit> clients = new HashMap<>();

    private static final Logger logger = LogManager.getLogger(MajorConfigServerManager.class);
}
