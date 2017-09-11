package cn.gocoding.server.biz.auth;

import cn.gocoding.common.command.CmdFactory;
import cn.gocoding.common.command.CmdFunction;
import cn.gocoding.main.ConfigUnit;
import com.alibaba.fastjson.JSONObject;

import static cn.gocoding.common.error.ErrorCode.PARAMTER_ERROR;

/**
 * Created by liuke on 16/3/21.
 */
public class AuthServer {

    public static void run(ConfigUnit configUnit) {
        CmdFactory.registerCmd("login", (JSONObject param) -> {
            if (param != null) {
                if (param.containsKey("args")) {
                    JSONObject args = param.getJSONObject("args");
                    String phone = args.getString("phone");
                    int code = args.getIntValue("code");
                    if (phone != null) {
                        JSONObject data = new JSONObject();
                        CmdFactory.callback(data);
                        return;
                    }
                }
            }
            CmdFactory.callback(param, PARAMTER_ERROR);
        });
    }
}
