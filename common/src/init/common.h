//
// Created by liuke on 15/12/18.
//

#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

#include <string>
#include <json/json.h>
#include <functional>
#include <memory>

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
         * 调用库函数的统一接口,这个函数可以是c++中注册的函数,也可以是lua中的函数.但会优先查找lua中的函数来执行,如果找不到再去查找c++中
         * @param method 函数名
         * @param param 参数
         * @param callback 调用的结果callback
         */
        static void call_(std::string method, Json::Value param, std::function<void(Json::Value result)> callback);
        static void call_(std::string method, std::string param, std::function<void(Json::Value result)> callback);
        static void call(std::string method, std::string param, std::function<void(std::string result)> callback);
        /**
         * 函数功能同上,只是没有回调
         */
        static void call(std::string method, Json::Value param);
        static void call(std::string method, std::string param);
        /**
         * 函数功能void call(std::string method, std::string param, std::function<void(Json::Value result)> callback);
         * 只是不需要传入参数
         */
        static void call_(std::string method, std::function<void(Json::Value result)> callback);
        /**
         * 函数功能void call(std::string method, std::string param, std::function<void(Json::Value result)> callback);
         * 只是不需要传入参数和回调
         */
        static void call(std::string method);



        /**
         * 判断result值是否为成功
         * @param result callback返回的值
         * @param success 是否result返回成功,result[result][success] = true,则true,否则false
         * @return 如果失败返回error_code,否则返回空字符
         */
        static std::string success(Json::Value result, bool* success);

        static void stop();

    private:
        static void init_function();
        static void init_log();
        static void init_lua();

    private:
        static std::string path;
        static std::string lua_path;
    };

}



#endif //COMMON_COMMON_H
