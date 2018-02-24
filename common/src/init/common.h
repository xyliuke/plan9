//
// Created by liuke on 15/12/18.
//

#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

#include <string>
#include <functional>
#include <memory>
#include <json/JSONObject.h>

namespace plan9
{
    class common {
    public:
        /**
         * 初始化库
         * @param path 数据目录,库函数会将所有数据放在这个目录下
         * @param lua_path lua文件的目录,库函数会在这个目录寻找可以执行的lua文件
         */
        static void init(std::string path, std::string lua_path);
        /**
         * 初始化库,
         * @param path
         * @param lua_path
         * @param tmp_path 临时文件的目录，common会将一些临时文件在这个文件下生成
         */
        static void init(std::string path, std::string lua_path, std::string tmp_path);

        static void init(std::string path, std::string lua_path, std::string tmp_path, std::function<void(void)> callback);

        /**
         * 调用库函数的统一接口,这个函数可以是c++中注册的函数,也可以是lua中的函数.但会优先查找lua中的函数来执行,如果找不到再去查找c++中
         * @param method 函数名
         * @param param 参数
         * @param callback 调用的结果callback
         */
        static void call_(std::string method, JSONObject param, std::function<void(JSONObject result)> callback);
        static void call_(std::string method, std::string param, std::function<void(JSONObject result)> callback);
        static void call(std::string method, std::string param, std::function<void(std::string result)> callback);
        /**
         * 函数功能同上,只是没有回调
         */
        static void call(std::string method, JSONObject param);
        static void call(std::string method, std::string param);
        /**
         * 函数功能void call(std::string method, std::string param, std::function<void(Json::Value result)> callback);
         * 只是不需要传入参数
         */
        static void call_(std::string method, std::function<void(JSONObject result)> callback);
        /**
         * 函数功能void call(std::string method, std::string param, std::function<void(Json::Value result)> callback);
         * 只是不需要传入参数和回调
         */
        static void call(std::string method);

        /**
         * 设置通知的接口,底层将通过这个传入的函数向上传递数据
         * 通知数据为json格式,协议如下:
         * aux : {
         *     to : "" //表示事件类型
         *     type : "" //表示事件通知的方式
         * }
         * result : {//数据内容
         *
         * }
         *
         *
         * @param notify 向上传递数据的lambda
         */
        static void set_notify_function(std::function<void(std::string)> notify);

        /**
         * 判断result值是否为成功
         * @param result callback返回的值
         * @param success 是否result返回成功,result[result][success] = true,则true,否则false
         * @return 如果失败返回error_code,否则返回空字符
         */
        static std::string success(JSONObject result, bool* success);

        static void stop();

        /**
         * 设置平台为ios
         */
        static void set_ios_platform();
        /**
         * 设置平台为android
         */
        static void set_android_platform();
        /**
         * 设置平台为windows
         */
        static void set_win_platform();
        /**
         * 设置平台为macosx
         */
        static void set_macosx_platform();

        /**
         * 数据目录
         * @return
         */
        static std::string get_data_path();
        /**
         * 临时文件目录
         * @return
         */
        static std::string get_tmp_path();
        /**
         * 缓存目录
         * @return
         */
        static std::string get_cache_path();

        static std::string get_tcp_notify_function();

    private:
        static void init_function();
        static void init_log();
        static void init_lua();
        static void init_network();
        static void send_notify_msg(std::string msg);
        static void init_config();

    private:
        static std::function<void(std::string)> notify_function;
        static std::function<void(bool)> connect_function;
        static std::function<void(std::string)> read_function;
        static std::function<void(std::string)> write_function;
    };

}



#endif //COMMON_COMMON_H
