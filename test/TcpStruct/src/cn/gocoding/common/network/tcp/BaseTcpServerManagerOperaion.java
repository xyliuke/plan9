package cn.gocoding.common.network.tcp;

/**
 * 处理每个连接的数据
 * Created by liuke on 16/3/16.
 */
public interface BaseTcpServerManagerOperaion {
    boolean handle(byte[] data);
}
