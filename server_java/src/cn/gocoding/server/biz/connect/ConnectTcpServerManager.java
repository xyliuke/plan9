package cn.gocoding.server.biz.connect;

import cn.gocoding.common.algo.Compress;
import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.tuple.Tuple;
import cn.gocoding.common.tuple.Tuple2;
import cn.gocoding.common.tuple.Tuple6;
import cn.gocoding.common.tuple.Tuple7;
import cn.gocoding.server.base.BaseServerManagerImpl;
import cn.gocoding.server.base.MessageNotify;
import cn.gocoding.server.protocol.Protocol;
import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

import java.nio.channels.AsynchronousSocketChannel;

/**
 * 连接服务器管理类
 * Created by liuke on 16/3/18.
 */
class ConnectTcpServerManager extends BaseServerManagerImpl {

    @Override
    public boolean handle(AsynchronousSocketChannel socketChannel, Tuple7<ErrorCode, Integer, Integer, Byte, Integer, byte[], byte[]> item) {
        boolean ret = super.handle(socketChannel, item);
        if (ret) {
            return true;
        }

        String str = new String(item._6().get());
        if (Protocol.getDataType(item._4().get()) == Protocol.COMPRESS_JSON_DATA_TYPE) {
            str = new String(Compress.decompress(item._6().get()));
        }

        if (!"".equals(str)) {
            JSONObject jsonObject = JSON.parseObject(str);

            JSONObject result = new JSONObject();
            result.put("success", true);

            JSONObject data = new JSONObject();
            data.put("a", "data from server");

            result.put("data", data);
            jsonObject.put("result", result);

            String string = jsonObject.toString();
            byte[] compressBuf = Compress.compress(string.getBytes());
            Tuple2<ErrorCode, byte[]> p = Protocol.createProtocol(item._2().get(), item._3().get().byteValue(), (byte) (item._4().get().byteValue() & 0xF0), Protocol.COMPRESS_JSON_DATA_TYPE, compressBuf.length, compressBuf);
            write(socketChannel, p._2().get());
            return true;
        }


        //连接服务器处理数据
//        int clientID = item._2().get();
//        if (clientID == 0) {
//            //用户没有登录,去登录
//            int tempID = getTempClientID();
//
//            bindClient(tempID, socketChannel);
//            handle2Auth(Protocol.modifyID(item, tempID));
//        } else {
//            bindClient(clientID, socketChannel);
//            handle2Route(item);
//        }
        return false;
    }

    private boolean handle2Route(Tuple7<ErrorCode, Integer, Integer, Byte, Integer, byte[], byte[]> item) {
        if (item._1().isPresent() && !ErrorCode.isError(item._1().get()) && item._7().isPresent()) {
            ConnectInstance.getRouteManager().write(item._7().get());
            return true;
        }
        return false;
    }

    private boolean handle2Auth(Tuple7<ErrorCode, Integer, Integer, Byte, Integer, byte[], byte[]> item) {
        if (item._1().isPresent() && !ErrorCode.isError(item._1().get()) && item._7().isPresent()) {
            ConnectInstance.getAuthManager().write(item._7().get());
            return true;
        }
        return false;
    }

    private int getTempClientID() {
        int ret = tempClientID;
        if (tempClientID == maxTempClientID) {
            tempClientID = minTempClientID;
        } else {
            tempClientID ++;
        }
        return ret;
    }

    private static final int minTempClientID = 0x70000000;
    private static final int maxTempClientID = 0x7FFFFFFF;
    private int tempClientID =  minTempClientID;
}
