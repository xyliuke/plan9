package cn.gocoding.server.config;

import cn.gocoding.common.error.ErrorCode;
import cn.gocoding.common.local.RunJar;
import cn.gocoding.common.tuple.Tuple6;
import cn.gocoding.common.tuple.Tuple7;
import cn.gocoding.server.base.BaseClientManagerImpl;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.IOException;
import java.nio.channels.AsynchronousSocketChannel;
import java.util.List;

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

        whenConnectMajorServer();
    }

    @Override
    public boolean handle(Tuple7<ErrorCode, Integer, Integer, Byte, Integer, byte[], byte[]> item) {
        if (super.handle(item)) {
            return true;
        }

        return false;
    }

    public void setMircoServerFile(String jarFile, List<String> configFile) {
        this.jarFile = jarFile;
        this.configFile = configFile;
    }



    private void whenConnectMajorServer() {
        //连接成功后,发送身份协议
        write(ConfigProtocol.minor2MajorIdentification(unit.getClientID(), unit.getVersion(), managerServerType, managerServerNum));

        if (jarFile != null && configFile != null) {
            try {
                for (String config : configFile) {
                    RunJar.run(jarFile, new String[]{config});
                }
            } catch (IOException e) {
                logger.error("run micro server jar file error, the reason : {}", e.getMessage());
            }
        }
    }

    private byte managerServerType;
    private int managerServerNum;
    private String jarFile;
    private List<String> configFile;
    private static final Logger logger = LogManager.getLogger(MinorConfigClientManager.class);
}
