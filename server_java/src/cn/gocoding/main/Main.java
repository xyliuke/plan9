package cn.gocoding.main;


import cn.gocoding.server.config.MajorConfig;
import cn.gocoding.server.config.MinorConfig;
import com.alibaba.fastjson.JSONObject;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;


/**
 * 入口函数
 * Created by liuke on 16/3/3.
 */
public class Main {
    private static Logger logger = LogManager.getLogger(Main.class);
    public static void main(String[] args) {
        if (args.length > 0) {
            configFile = args[0];
        }

        logger.info("read config file from {}", configFile);
        JSONObject config = ConfigFile.readConfigFile(configFile);
        if (config != null) {
            logger.info("config json : {}", config);
            if (isMajorServer(config)) {
                runMajorServer(config);
            } else if (isMinorServer(config)) {
                runMinorServer(config);
            }
        }
    }


    private static boolean isMajorServer(JSONObject json) {
        if (json.containsKey("identity")) {
            if ("major-config-server".equals(json.getString("identity"))) {
                return true;
            }
        }
        return false;
    }

    private static boolean isMinorServer(JSONObject json) {
        if (json.containsKey("identity")) {
            if ("minor-config-server".equals(json.getString("identity"))) {
                return true;
            }
        }
        return false;
    }

    private static void runMajorServer(JSONObject json) {
        if (json.containsKey("port") && json.containsKey("version")) {
            int port = json.getInteger("port");
            int version = json.getInteger("version");
            MajorConfig.run(port, version);
        }
    }

    private static void runMinorServer(JSONObject json) {
        if (json.containsKey("client-id") && json.containsKey("major-config-server-ip")
                && json.containsKey("major-config-server-port") && json.containsKey("version")) {
            int clientID = json.getInteger("client-id");
            String major_ip = json.getString("major-config-server-ip");
            int major_port = json.getInteger("major-config-server-port");
            int port = json.getInteger("port");
            int version = json.getInteger("version");
            byte managerServerType = json.getByteValue("manager-server-type");
            int managerServerNum = json.getByteValue("manager-server-num");

            MinorConfig.run(major_ip, major_port, port, version, clientID, managerServerType, managerServerNum);
        }
    }


    private static String configFile = "./config.json";
}
