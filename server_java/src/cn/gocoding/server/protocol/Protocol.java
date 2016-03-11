package cn.gocoding.server.protocol;

import cn.gocoding.common.tuple.Tuple;
import cn.gocoding.common.tuple.Tuple2;
import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.tuple.Tuple6;

/**
 * 服务器的协议处理类
 * Created by liuke on 16/3/5.
 */
public class Protocol {
    //协议起始标志
    public static final byte PROTOCOL_FIRST_LETTER = (byte) 94;
    //协议头部总长度
    public static final int PROTOCOL_HEADER_LEN = 9;
    //数据客户端id的下标
    public static final int PROTOCOL_ID_INDEX = 1;
    //数据版本的下标
    public static final int PROTOCOL_VERSION_INDEX = 5;
    //数据类型的下标
    public static final int PROTOCOL_TYPE_INDEX = 6;
    //数据长度的下标
    public static final int PROTOCOL_DATA_LEN_INDEX = 7;
    //数据实体的下标
    public static final int PROTOCOL_DATA_INDEX = 9;


    //前4bit的服务器类型协议
    public static final byte LOGIN_SERVER_TYPE = (byte) 0x00;
    public static final byte VERIFY_SERVER_TYPE = (byte) 0x10;
    public static final byte SESSION_SERVER_TYPE = (byte) 0x20;
    public static final byte DATABASE_SERVER_TYPE = (byte) 0x30;

    public static final byte MAIN_CONFIG_TYPE = (byte) 0xE0;
    public static final byte OTHER_CONFIG_TYPE = (byte) 0xF0;


    //后4bit的数据类型协议
    public static final byte NORMAL_STRING_DATA_TYPE = (byte) 0x00;
    public static final byte NORMAL_JSON_DATA_TYPE = (byte) 0x01;
    public static final byte PING_DATA_TYPE = (byte) 0x02;
    public static final byte PONG_DATA_TYPE = (byte) 0x03;
    public static final byte COMPRESS_JSON_DATA_TYPE = (byte) 0x04;
    public static final byte ENCRYPT_JSON_DATA_TYPE = (byte) 0x05;
    public static final byte ENCRYPT_COMPRESS_JSON_DATA_TYPE = (byte) 0x06;

    /**
     * 判断数据包中是否包含至少一条完整协议
     * @param data 原始数据包
     * @param len 原始数据包中有效字节长度
     * @return 返回二元组,第一个元素为错误类型,也可以不是错误,NOT_ERROR;当第一个元素为NOT_ERROR时,第二个元素为协议中数据实体部分长度（不包括协议头部）
     */
    public static Tuple2<ErrorCode, Integer> isLegalProtocol(byte[] data, int len) {
        if (len > PROTOCOL_HEADER_LEN && data.length >= len && data[0] == PROTOCOL_FIRST_LETTER) {
            int size = getDataLen(data);
            if (len >= size + PROTOCOL_HEADER_LEN) {
                return Tuple.of(ErrorCode.NOT_ERROR, size);
            }
        }
        return Tuple.of(ErrorCode.PROTOLOC_FORMAT_ILLEGAL_ERROR, 0);
    }

    /**
     * 获取给定数据中第一条协议包的各项值,分别为协议包是否错误, 客户端ID/协议版本号/类型字节/数据长度/数据实体
     * @param data 原始数据包
     * @param len 原始数据包中有效字节长度
     * @return 返回五元组,如果出错,第一个值返回错误类型,其他值忽略
     */
    public static Tuple6<ErrorCode, Integer, Integer, Byte, Integer, byte[]> getProtocolItem(byte[] data, int len) {
        Tuple2<ErrorCode, Integer> ret = isLegalProtocol(data, len);
        if (ret._1().isPresent()) {
            if (ret._1().get() == ErrorCode.NOT_ERROR) {
                int cid = getClientID(data);
                int ver = getVersion(data);
                byte type = getType(data);
                byte[] d = getData(data);
                return Tuple.of(ret._1().get(), cid, ver, type, ret._2().get(), d);
            } else {
                return Tuple.of(ret._1().get(), 0, 0, (byte)0, 0, null);
            }
        }
        return Tuple.of(ErrorCode.UNKNOW_ERROR, 0, 0, (byte)0, 0, null);
    }

    /**
     * 是否为Ping类型包
     * @param type 类型字节
     * @return
     */
    public static boolean isPingType(byte type) {
        return (type & 0x0F) == PING_DATA_TYPE;
    }

    /**
     * 是否为Pong类型包
     * @param type 类型字节
     * @return
     */
    public static boolean isPongType(byte type) {
        return (type & 0x0F) == PONG_DATA_TYPE;
    }

    /**
     * 获取类型字节中的服务器类型,可直接与XXX_SERVER_TYPE比较
     * @param type 类型字节
     * @return
     */
    public static byte getServerType(byte type) {
        return (byte) (type & 0xF0);
    }

    /**
     * 获取类型字节中的数据类型,可直接与XXX_DATA_TYPE比较
     * @param type 类型字节
     * @return
     */
    public static byte getDataType(byte type) {
        return (byte) (type & 0x0F);
    }

    /**
     * 从原始数据中删除第一条完整协议,如果没有完整协议包,则不删除
     * @param data 原始数据
     * @param len 原始数据的长度
     * @return 操作后的长度
     */
    public static int removeFirstProtocol(byte[] data, int len) {
        Tuple2<ErrorCode, Integer> is = isLegalProtocol(data, len);
        if (is._1().isPresent() && is._1().get() == ErrorCode.NOT_ERROR) {
            int flen = getDataLen(data) + PROTOCOL_HEADER_LEN;
            System.arraycopy(data, flen, data, 0, len - flen);
            return len - flen;
        }
        return len;
    }

    /**
     * 创建一条ping的数据协议
     * @param id 客户端id
     * @param version 客户端版本号
     * @param serverType 接收端服务器类型
     * @return 返回ping协议数据
     */
    public static byte[] createPingProtocol(int id, byte version, byte serverType) {
        return createProtocol(id, version, serverType, PING_DATA_TYPE, 4, "ping".getBytes())._2().get();
    }

    /**
     * 创建一条pong的数据协议
     * @param id 服务器id
     * @param version 服务器版本号
     * @return 返回pong协议数据
     */
    public static byte[] createPongProtocol(int id, byte version) {
        return createProtocol(id, version, (byte)0, PONG_DATA_TYPE, 4, "pong".getBytes())._2().get();
    }


    public static Tuple2<ErrorCode, byte[]> createProtocol(int id, byte version, byte serverType, byte dataType, int len, byte[] data) {

        if (data.length < len) {
            return Tuple.of(ErrorCode.PARAMTER_ERROR, null);
        }

        byte[] ret = new byte[PROTOCOL_HEADER_LEN + len];
        ret[0] = PROTOCOL_FIRST_LETTER;
        ret[PROTOCOL_ID_INDEX] = (byte)((id & 0xFF000000) >> 24);
        ret[PROTOCOL_ID_INDEX + 1] = (byte)((id & 0x00FF0000) >> 16);
        ret[PROTOCOL_ID_INDEX + 2] = (byte)((id & 0x0000FF00) >> 8);
        ret[PROTOCOL_ID_INDEX + 3] = (byte)(id & 0x000000FF);

        ret[PROTOCOL_VERSION_INDEX] = version;

        ret[PROTOCOL_TYPE_INDEX] = (byte)(serverType | dataType);

        ret[PROTOCOL_DATA_LEN_INDEX] = (byte) ((len & 0x0000FF00) >> 8);
        ret[PROTOCOL_DATA_LEN_INDEX + 1] = (byte) ((len & 0x000000FF));

        System.arraycopy(data, 0, ret, PROTOCOL_DATA_INDEX, len);

        return Tuple.of(ErrorCode.NOT_ERROR, ret);
    }


    /**
     * 获取数据协议中数据实体长度值, 假设数据包为合法协议
     * @param data 原始数据
     * @return 返回数据实体的长度
     */
    private static int getDataLen(byte[] data) {
        if (data.length > PROTOCOL_HEADER_LEN) {
            return (int) (Byte.toUnsignedInt(data[PROTOCOL_DATA_LEN_INDEX]) << 8) + (int) (Byte.toUnsignedInt(data[PROTOCOL_DATA_LEN_INDEX + 1]));
        }
        return -1;
    }

    /**
     * 获取数据协议中的客户端ID字段,假设数据包为合法协议
     * @param data 原始数据
     * @return 返回客户端ID
     */
    private static int getClientID(byte[] data) {
        if (data.length > PROTOCOL_HEADER_LEN) {
            int a = Byte.toUnsignedInt(data[PROTOCOL_ID_INDEX]);
            int b = Byte.toUnsignedInt(data[PROTOCOL_ID_INDEX + 1]);
            int c = Byte.toUnsignedInt(data[PROTOCOL_ID_INDEX + 2]);
            int d = Byte.toUnsignedInt(data[PROTOCOL_ID_INDEX + 3]);

            return (a << 24) + (b << 16) + (c << 8) + d;
        }
        return -1;
    }

    /**
     * 获取数据协议中的版本号
     * @param data 原始数据
     * @return 版本号
     */
    private static int getVersion(byte[] data) {
        if (data.length > PROTOCOL_HEADER_LEN) {
            return Byte.toUnsignedInt(data[PROTOCOL_VERSION_INDEX]);
        }
        return -1;
    }

    /**
     * 获取协议中类型字节
     * @param data 原始数据
     * @return 协议类型
     */
    private static byte getType(byte[] data) {
        if (data.length > PROTOCOL_HEADER_LEN) {
            return data[PROTOCOL_TYPE_INDEX];
        }
        return -1;
    }

    /**
     * 获取协议包中数据实体部分
     * @param data 原始数据
     * @return 数据实体
     */
    private static byte[] getData(byte[] data) {
        int len = getDataLen(data);
        if (len >= 0) {
            byte[] ret = new byte[len];
            System.arraycopy(data, PROTOCOL_DATA_INDEX, ret, 0, len);
            return ret;
        }
        return null;
    }

}
