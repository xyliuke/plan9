package cn.gocoding.common.command;

import com.alibaba.fastjson.JSONObject;

/**
 * 执行命令的方法体,使用接口来替代java中没有闭馆的问题
 * Created by liuke on 16/3/12.
 */
public interface CmdFunction {
    void run(JSONObject param);
}
