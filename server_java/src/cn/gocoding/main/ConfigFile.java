package cn.gocoding.main;

import cn.gocoding.common.tuple.Tuple;
import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;

/**
 * 读取配置文件,设定应用的参数.
 * 使用JSON来作为配置文件
 * Created by liuke on 16/3/15.
 */
public class ConfigFile {
    public static JSONObject readConfigFile(String file) {
        File f = new File(file);
        if (f.exists()) {
            String text = getStringFromConfigFile(file);
            if (text != null) {
                try {
                    return JSONObject.parseObject(text);
                } catch (Exception e) {
                    logger.error("parse config json error, the reason : {}", e.getMessage());
                }

            }
            return null;
        } else {
            logger.error("config file : {} not exist", file);
        }
        return null;
    }


    private static String getStringFromConfigFile(String file) {
        FileReader reader = null;
        try {
            reader = new FileReader(file);
            char[] buf = new char[10 * 1024];
            int len = reader.read(buf);
            return new String(buf, 0, len);
        } catch (IOException e) {
            logger.error("read config file error, the reason : {}", e.getMessage());
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException e) {

                }
            }
        }
        return null;
    }


    private static final Logger logger = LogManager.getLogger(ConfigFile.class);
}
