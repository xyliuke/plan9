package cn.gocoding.server.biz.auth;

import cn.gocoding.common.command.CmdFactory;
import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.tuple.Tuple7;
import cn.gocoding.server.base.BaseServerOperation;
import cn.gocoding.server.base.ServerUnit;
import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

import java.nio.channels.AsynchronousSocketChannel;

/**
 *
 * Created by liuke on 16/3/21.
 */
public class AuthTcpServerOperation extends BaseServerOperation {

    public AuthTcpServerOperation(AsynchronousSocketChannel socketChannel, ServerUnit unit) {
        super(socketChannel, unit);
    }


    @Override
    public boolean handle(Tuple7<ErrorCode, Integer, Integer, Byte, Integer, byte[], byte[]> item) {
        boolean ret = super.handle(item);
        if (ret) {
            return true;
        }

        //处理登录验证
        String str = new String(item._7().get());
        JSONObject param = JSON.parseObject(str);

        CmdFactory.execute("login", param, (JSONObject result) -> {
            String retString = result.toJSONString();
            write(item._2().get(), item._3().get().byteValue(), item._4().get(), (byte)1, retString.length(), retString.getBytes());
        });

        return true;
    }
}
