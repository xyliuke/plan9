package cn.gocoding.common;


import android.content.Context;
import android.util.JsonReader;

import org.json.JSONObject;
import org.json.JSONStringer;

import java.util.HashMap;
import java.util.Map;


/**
 * 与C++交互接口
 * Created by liuke on 15/12/25.
 */
public class Bizlayer {
    static {
        System.loadLibrary("common");
    }

    /**
     * 生成一个id
     * @return
     */
    private static String createID() {
        StringBuffer sb = new StringBuffer();
        sb.append("ID");
        sb.append(System.currentTimeMillis());
        sb.append(ALP[i]);
        sb.append(ALP[j]);

        if (j > 24) {
            j = 0;
            if (i > 24) {
                i = 0;
            } else {
                i ++;
            }
        } else {
            j ++;
        }

        return sb.toString();
    }

    /**
     * 初始化common库
     * @param context android的context
     * @param assetLua 在asset中的lua的文件夹
     * @param rootPath 数据路径
     */
    public static void initBiz(Context context, String assetLua, String rootPath) {
        try {
            String luaPath = rootPath + "/lua";
            FileUtils.copyAssetDirToFiles(context, assetLua, luaPath);
            Bizlayer.init(rootPath, luaPath);
        } catch (Exception e) {
        }
    }

    /**
     * 调用biz的函数接口
     * @param method 方法名
     * @param param 参数，为json格式字符串
     * @param bizlayerCallback 回调，可以null
     */
    public static void call(String method, String param, BizlayerCallback bizlayerCallback) {
        try {
            String id = createID();

            StringBuffer sb = new StringBuffer();
            sb.append("{\"aux\":{\"id\":\"");
            sb.append(id);
            sb.append("\",\"to\":\"");
            sb.append(method);
            sb.append("\"}");

            if (param != null) {
                sb.append(",\"args\":{");
                sb.append(param);
                sb.append("}");
            }
            sb.append("}");

            if (bizlayerCallback != null) {
                callbackMap.put(id, bizlayerCallback);
                call(method, sb.toString(), true);
            } else {
                call(method, sb.toString(), false);
            }

        } catch (Exception e) {

        }
    }

    /**
     * 被jni调用的回调
     * @param data 回调的数据，为json格式的数据
     */
    private static void callback(String data){
        try {
            JSONObject jsonObject = new JSONObject(data);
            if (jsonObject.has("aux")) {
                JSONObject aux = jsonObject.getJSONObject("aux");
                if (aux.has("id")) {
                    String id = aux.getString("id");
                    BizlayerCallback callback = callbackMap.get(id);
                    if (callback != null) {
                        callback.callback(data);
                    }
                }
            }
        } catch (Exception e) {

        }
    }


    private static native void init(String rootPath, String luaSrcPath);
    public static native void stop();
    private static native void call(String method, String param, boolean isCallback);
    public static native void logi(String msg);
    public static native void logw(String msg);
    public static native void loge(String msg);

    private static Map<String, BizlayerCallback> callbackMap = new HashMap<>();
    private static int i = 0, j = 0;
    private static char[] ALP = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W','X', 'Y', 'Z'};
}
