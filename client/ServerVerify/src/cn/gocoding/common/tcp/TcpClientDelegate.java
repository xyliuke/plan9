package cn.gocoding.common.tcp;

/**
 * Created by liuke on 16/3/21.
 */
public interface TcpClientDelegate {
    void connect();
    void read(byte[] data);
    void write(byte[] data);
    void close();
}
