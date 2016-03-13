package cn.gocoding.common.command;

import cn.gocoding.common.util.IDUtil;
import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.util.HashMap;
import java.util.Map;

/**
 * 以命令模式调用函数功能的类,记录命令,执行命令
 *
 * JSON对象的格式为:
 * {
 *    aux:  (辅助参数)
 *    {
 *         id:id                    (一个JSON对象调用的唯一标识)
 *         action:callback/direct   (callback,表示需要执行完成后最终执行要回到最初调用者的callback中,而direct则执行完成后不再回到之前的callback中)
 *         once:true/false          (true表示这个函数的callback会执行多次,false表示一次)
 *         from:[]                  (from是个数组,表示执行来源,最后根据这个来源再callback回去)
 *         to:cmd_name              (执行的命令名)
 *    }
 *    args:  (调用参数,这个结构中所有参数都是绑定函数所使用的)
 *    {
 *
 *    }
 *    result:   (调用函数最终返回的数据结构,为调用者使用)
 *    {
 *        success : true/false      (函数调用成功或者失败)
 *        error   :  error_code     这个错误码定义在lua的error_code文件中
 *        reason  :  失败原因        错误原因的文字说明,主要是UI使用
 *        data    : {}              (函数返回的结果数据封装在这个字段中)
 *    }
 *
 * }
 *
 * Created by liuke on 16/3/11.
 */
public class CmdFactory {

    /**
     * 注册一个函数名和执行的方法体
     * @param cmd 函数名
     * @param function 方法体
     */
    public static void registerCmd(String cmd, CmdFunction function) {
        cmdFunctionMap.put(cmd, function);
    }

    /**
     * 执行一个方法,并传入相应的参数.参数格式参考注释
     * @param cmd 函数名
     * @param param 参数
     * @param callback 回调
     */
    public static void execute(String cmd, JSONObject param, CmdCallback callback) {
        if (cmd == null || param == null) {
            throw new NullPointerException("执行命令名或者参数不能为空");
        } else {
            JSONObject aux = null;
            if (param.containsKey("aux")) {
                aux = param.getJSONObject("aux");
            } else {
                aux = new JSONObject();
                param.put("aux", aux);
            }

            aux.put("to", cmd);
            aux.put("id", IDUtil.createUniqueID());

            execute(param, callback);
        }

    }

    /**
     * 执行一个方法,参数中包含方法名和函数中使用的参数
     * @param param 参数
     */
    public static void execute(JSONObject param, CmdCallback callback) {
        if (param == null || !param.containsKey("aux") || !param.getJSONObject("aux").containsKey("to")) {
            throw new NullPointerException("执行的命令参数为空");
        } else {
            JSONObject aux = param.getJSONObject("aux");
            String to = aux.getString("to");
            String id = null;
            if (aux.containsKey("id")) {
                id = aux.getString("id");
            } else {
                id = IDUtil.createUniqueID();
                aux.put("id", id);
            }
            if (callback != null) {
                if (!aux.containsKey("action")) {
                    aux.put("action", "callback");
                }
                callbackMap.put(id, callback);
                if (aux.containsKey("from")) {
                    JSONArray from = aux.getJSONArray("from");
                    from.add(id);
                } else {
                    JSONArray from = new JSONArray();
                    from.add(id);
                    aux.put("from", from);
                }
            } else {
                if (!aux.containsKey("action")) {
                    aux.put("action", "direct");
                }
            }

            CmdFunction function = cmdFunctionMap.get(to);
            if (function != null) {
                function.run(param);
            } else {
                logger.error("can not execute the paramter : {} ,because of not find {} function, this cmd is not register", param, to);
            }
        }
    }

    /**
     * 回调调用者传入的callback接口函数,这个函数由命令实现函数体调用
     * @param result json格式数据
     */
    public static void callback(JSONObject result) {
        if (result == null) {
            throw new NullPointerException("回调参数不能为空");
        } else {
            if (result.containsKey("aux")) {
                JSONObject aux = result.getJSONObject("aux");
                JSONArray from = null;
                String action = null;
                if (aux.containsKey("from")) {
                    from = aux.getJSONArray("from");
                }
                if (aux.containsKey("action")) {
                    action = aux.getString("action");
                }

                if (from != null && action != null) {
                    if ("callback".equals(action) && !from.isEmpty()) {
                        aux.put("id", from.getString(0));
                        String lastID = from.getString(from.size() - 1);
                        CmdCallback callback = callbackMap.get(lastID);
                        if (callback != null) {
                            from.remove(from.size() - 1);
                            callback.callback(result);
                        }
                    }
                } else {
                    logger.error("parameter error, the from or action is null, result json : {}", result);
                }
            }
        }
    }

    /**
     * 返回结果成功的json数据
     * @param param 原始参数
     * @param data 成功后的数据实体,需要callback回调用者
     */
    public static void callback(JSONObject param, JSONObject data) {
        if (param == null) {
            throw new NullPointerException("回调的结果数据不能为空");
        } else {
            JSONObject result = null;
            if (param.containsKey("result")) {
                result = param.getJSONObject("result");
            } else {
                result = new JSONObject();
                param.put("result", result);
            }

            result.put("success", true);
            if (data == null) {
                param.put("data", new JSONObject());
            } else {
                param.put("data", data);
            }
            callback(param);
        }
    }

    /**
     * 返回失败结果的json数据
     * @param param 原始参数
     * @param errorCode 失败码
     * @param reason 失败原因描述
     */
    public static void callback(JSONObject param, int errorCode, String reason) {
        if (param == null) {
            throw new NullPointerException("回调的结果数据不能为空");
        } else {
            JSONObject result = null;
            if (param.containsKey("result")) {
                result = param.getJSONObject("result");
            } else {
                result = new JSONObject();
                param.put("result", result);
            }

            result.put("success", false);
            result.put("error", errorCode);
            result.put("reason", reason);

            callback(param);
        }
    }


    private static CmdFactory cmdFactory = new CmdFactory();
    private static Map<String, CmdFunction> cmdFunctionMap = new HashMap<>();
    private static Map<String, CmdCallback> callbackMap = new HashMap<>();

    private static final Logger logger = LogManager.getLogger(CmdFactory.class);
}
