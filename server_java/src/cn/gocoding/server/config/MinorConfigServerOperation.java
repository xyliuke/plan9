package cn.gocoding.server.config;

import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.tuple.Tuple6;
import cn.gocoding.server.base.BaseServerOperation;
import cn.gocoding.server.base.ServerUnit;

import java.nio.channels.AsynchronousSocketChannel;

/**
 * 次配置服务的tcp-server处理类
 * Created by liuke on 16/3/13.
 */
public class MinorConfigServerOperation extends BaseServerOperation {

    public MinorConfigServerOperation(AsynchronousSocketChannel socketChannel, ServerUnit unit) {
        super(socketChannel, unit);
    }

    @Override
    public boolean handle(Tuple6<ErrorCode, Integer, Integer, Byte, Integer, byte[]> item) {
        if (super.handle(item)) {
            return true;
        }

        //执行处理操作

        return false;
    }
}
