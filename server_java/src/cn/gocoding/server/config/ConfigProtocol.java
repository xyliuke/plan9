package cn.gocoding.server.config;

import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.tuple.Tuple2;
import cn.gocoding.server.protocol.Protocol;
import com.alibaba.fastjson.JSONObject;

/**
 * 配置服务器的协议类
 * Created by liuke on 16/3/15.
 */
public class ConfigProtocol {

    //协议消息类型
    public static final String MSG_TYPE = "msg_type";
    //msg_type可能的值
    public static final String MSG_TYPE_IDENTIFICATION = "identification";


    //标识服务种类
    public static final String IDENTITY = "identity";
    //identity可能的值
    public static final String IDENTIFY_MINOR_CONFIG = "minor_config";


    //微服务类型的key
    public static final String MICRO_SERVER_TYPE = "micro_server_type";
    //微服务的个数
    public static final String MICRO_SERVER_NUM = "micro_server_num";









    /**
     * minor-server向major-server发送表明身价的协议
     * @param clientID 客户端id
     * @param version 客户端版本
     * @param serverType minor-server下的管理微服务类型
     * @param serverNum minor-server下的管理微服务个数
     * @return 返回协议包
     */
    public static byte[] minor2MajorIdentification(int clientID, byte version, byte serverType, int serverNum) {
        JSONObject jsonObject = new JSONObject();

        jsonObject.put(MSG_TYPE, MSG_TYPE_IDENTIFICATION);
        jsonObject.put(IDENTITY, IDENTIFY_MINOR_CONFIG);
        jsonObject.put(MICRO_SERVER_TYPE, serverType);
        jsonObject.put(MICRO_SERVER_NUM, serverNum);

        byte[] data = jsonObject.toJSONString().getBytes();

        Tuple2<ErrorCode, byte[]> ret = Protocol.createProtocol(clientID, version, Protocol.MAJOR_CONFIG_TYPE, Protocol.NORMAL_JSON_DATA_TYPE, data.length, data);
        if (ret._1().isPresent() && !ErrorCode.isError(ret._1().get())) {
            return ret._2().get();
        }
        return null;
    }
}
