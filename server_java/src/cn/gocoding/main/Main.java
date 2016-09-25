package cn.gocoding.main;


import cn.gocoding.server.biz.connect.ConnectServer;
import cn.gocoding.server.config.MajorConfig;
import cn.gocoding.server.config.MinorConfig;
import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.apache.logging.log4j.core.net.Priority;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;


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

        runConnectServer(null);

//        logger.info("read config file from {}", configFile);
//        JSONObject config = ConfigFile.readConfigFile(configFile);
//        if (config != null) {
//            logger.info("config json : {}", config);
//            if (isMajorServer(config)) {
//                logger.info("major-server run");
//                runMajorServer(config);
//            } else if (isMinorServer(config)) {
//                logger.info("minor-server run");
//                runMinorServer(config);
//            }
//        } else {
//            logger.error("config file {} not exist or config content format error", configFile);
//        }
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
            int checkPeriod = -1;
            if (json.containsKey("check-client-disconnect-period")) {
                checkPeriod = json.getInteger("check-client-disconnect-period");
            }

            ConfigUnit configUnit = new ConfigUnit();
            configUnit.setVersion(version);
            configUnit.setPort(port);

            if (checkPeriod > 10000) {
                configUnit.setCheckClientDisconnectPerios(checkPeriod);
            }

            MajorConfig.run(configUnit);
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
            int checkPeriod = -1;
            if (json.containsKey("check-client-disconnect-period")) {
                checkPeriod = json.getInteger("check-client-disconnect-period");
            }

            String managerServerJarFile = json.getString("manager-server-jar-path");

            ConfigUnit configUnit = new ConfigUnit();
            configUnit.setClientID(clientID);
            configUnit.setVersion(version);
            configUnit.setMajorIP(major_ip);
            configUnit.setMajorPort(major_port);
            configUnit.setManagerServerType(managerServerType);
            configUnit.setManagerServerNum(managerServerNum);
            configUnit.setPort(port);
            configUnit.setManagerServerJarFile(managerServerJarFile);

            if (json.containsKey("manager-server-config-file")) {
                JSONArray jarConfigArray = json.getJSONArray("manager-server-config-file");
                for (int i = 0; i < jarConfigArray.size(); i ++) {
                    configUnit.addManagerServerJarConfigFile(jarConfigArray.getString(i));
                }
            }

            if (checkPeriod > 10000) {
                configUnit.setCheckClientDisconnectPerios(checkPeriod);
            }

            MinorConfig.run(configUnit);
        }
    }

    private static void runConnectServer(JSONObject jsonObject) {
        ConnectServer.run(null);
    }


    private static String configFile = "./config.json";
}
