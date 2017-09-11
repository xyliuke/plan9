package cn.gocoding.common.command;

import com.alibaba.fastjson.JSONObject;

/**
 * 执行cmd命令后的回调接口
 * Created by liuke on 16/3/12.
 */
public interface CmdCallback {
    void callback(JSONObject result);
}
