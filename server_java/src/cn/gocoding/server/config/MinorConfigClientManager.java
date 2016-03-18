package cn.gocoding.server.config;

import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.tuple.Tuple6;
import cn.gocoding.server.base.BaseClientManagerImpl;
import cn.gocoding.server.base.ClientUnit;

import java.nio.channels.AsynchronousSocketChannel;

/**
 * 配置服务tcp-client管理类
 * Created by liuke on 16/3/13.
 */
public class MinorConfigClientManager extends BaseClientManagerImpl {
    public MinorConfigClientManager(MinorClientUnit unit, byte managerServerType, int managerServerNum) {
        super(unit);
        this.managerServerNum = managerServerNum;
        this.managerServerType = managerServerType;
    }

    @Override
    public void connect(AsynchronousSocketChannel socketChannel) {
        super.connect(socketChannel);
        //连接成功后,发送身份协议
        write(ConfigProtocol.minor2MajorIdentification(unit.getClientID(), unit.getVersion(), managerServerType, managerServerNum));
    }

    @Override
    public boolean handle(Tuple6<ErrorCode, Integer, Integer, Byte, Integer, byte[]> item) {
        if (super.handle(item)) {
            return true;
        }

        return false;
    }

    private byte managerServerType;
    private int managerServerNum;
}
