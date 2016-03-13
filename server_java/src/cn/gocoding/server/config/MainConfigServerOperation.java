package cn.gocoding.server.config;

import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.tuple.Tuple6;
import cn.gocoding.server.base.BaseServerOperation;
import cn.gocoding.server.base.MessageNotify;
import cn.gocoding.server.base.MessageNotifyRecevier;
import cn.gocoding.server.base.ServerUnit;
import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

import java.nio.channels.AsynchronousSocketChannel;

/**
 * 处理主配置服务器数据项
 * Created by liuke on 16/3/11.
 */
public class MainConfigServerOperation extends BaseServerOperation {
    public MainConfigServerOperation(AsynchronousSocketChannel socketChannel, ServerUnit unit) {
        super(socketChannel, unit);
    }

    public void setRecvMessageDelegate(MessageNotifyRecevier recevier) {
        messageNotify.addRecevier(recevier);
    }

    @Override
    public boolean handle(Tuple6<ErrorCode, Integer, Integer, Byte, Integer, byte[]> item) {
        if (super.handle(item)) {
            return true;
        }

        //处理次配置服务器发送的数据
        JSONObject data = JSON.parseObject(new String(item._6().get()));
        handle(item._2().get(), item._3().get(), item._4().get(), data);

        return false;
    }


    private boolean handle(int clientID, int version, byte type, JSONObject data) {

        return false;
    }




    private MessageNotify messageNotify = new MessageNotify();
}
