package cn.gocoding.server.config;

import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.tuple.Tuple6;
import cn.gocoding.common.tuple.Tuple7;
import cn.gocoding.server.base.BaseServerOperation;
import cn.gocoding.server.base.MessageNotify;
import cn.gocoding.server.base.MessageNotifyRecevier;
import cn.gocoding.server.base.ServerUnit;
import cn.gocoding.server.protocol.Protocol;
import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.nio.channels.AsynchronousSocketChannel;

/**
 * 处理主配置服务器数据项
 * Created by liuke on 16/3/11.
 */
public class MajorConfigServerOperation extends BaseServerOperation {
    public MajorConfigServerOperation(AsynchronousSocketChannel socketChannel, MajorServerUnit unit) {
        super(socketChannel, unit);
    }

    public void setRecvMessageDelegate(MessageNotifyRecevier recevier) {
        messageNotify.addRecevier(recevier);
    }

    @Override
    public boolean handle(Tuple7<ErrorCode, Integer, Integer, Byte, Integer, byte[], byte[]> item) {
        if (super.handle(item)) {
            return true;
        }

        //处理次配置服务器发送的数据
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
                        return handle(item._2().get(), item._3().get(), data);
                    }
                } catch (Exception e) {
                    logger.error("handle recv string data to json error : {}", e.getMessage());
                }
            }
        }

        return false;
    }


    private boolean handle(int clientID, int version, JSONObject data) {

        return false;
    }




    private MessageNotify messageNotify = new MessageNotify();
    private static final Logger logger = LogManager.getLogger(MajorConfigServerOperation.class);
}
