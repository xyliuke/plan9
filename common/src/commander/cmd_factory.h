//
// Created by liuke on 15/11/6.
//

#ifndef COMMON_CMD_FACTORY_H
#define COMMON_CMD_FACTORY_H



/**
 * cmd_factory功能说明
 * 这是一个注册命令名和函数的类,函数的参数为JSON对象,调用相应的命令,即可执行所绑定的函数.
 * 一般情况下不会直接调用命令名,而是将命令名放在JSON对象里,再将这个JSON对象作为参数传出去.
 * JSON对象的格式为:
 * {
 *    aux:  (辅助参数)
 *    {
 *         id:id                    (一个JSON对象调用的唯一标识)
 *         action:callback/direct   (callback,表示需要执行完成后最终执行要回到最初调用者的callback中,而direct则执行完成后不再回到之前的callback中)
 *         once:true/false          (false表示这个函数的callback会执行多次,true表示一次;没有该字段，则默认为true)
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
 */


#include <string>
#include <json/json.h>
#include <functional>
#include <memory>
#include <json/JSONObject.h>


namespace plan9
{
    class cmd_factory {
    public:
        /**
         * 单例对象
         */
        static cmd_factory instance();
        /**
         * 注册一个命令和对应的执行代码.当调用这个命令时,对应的注册函数会被执行,参数为一个JSON对象.
         * 如果调用者需要callback返回结果,则必须在注册函数执行完成后,调用callback(json)函数.
         * @param cmd 命令名
         * @param function 注册的执行函数,参数为json对象
         */
        void register_cmd(std::string cmd, std::function<void(JSONObject)> function);
        /**
         * 执行一个命令,并传递一个JSON参数,执行完成后结果可以callback返回,但这取决于被调用函数是否在结尾调用了callback(json)函数
         * @param cmd 命令名
         * @param param json格式参数
         * @param callback 回调
         */
        void execute(std::string cmd, JSONObject param, std::function<void(JSONObject)> callback);
        /**
         * 函数功能同void execute(std::string cmd, Json::Value param, std::function<void(Json::Value)> callback);
         * 只是函数名保存在param["aux"]["to"]中
         */
        void execute(JSONObject param, std::function<void(JSONObject)> callback);

        /**
         * 功能同execute(std::string cmd, Json::Value param, std::function<void(Json::Value)> callback);但没有回调
         * @param cmd 命令名
         * @param param json格式参数
         */
        void execute(std::string cmd, JSONObject param);
        /**
         * 函数功能同void execute(std::string cmd, Json::Value param);
         * 只是函数名保存在param["aux"]["to"]中
         */
        void execute(JSONObject param);

        /**
         * 将调用的结果封装到原始的json中,格式如上面注释所示
         * result:   (调用函数最终返回的数据结构,为调用者使用)
         *    {
         *        success : true/false      (函数调用成功或者失败)
         *        data : {}                 (函数返回的结果数据封装在这个字段中)
         *    }
         * @param json 调用参数时传入json
         * @param result 函数调用成功或者失败
         * @param data 调用函数的结果数据
         * @return 返回封装好的数据
         */
//        Json::Value wrap_callback_data(Json::Value json, bool result, Json::Value data);

        /**
         * 这个函数用于注册函数内部使用,如果需要callback,则调用.
         * 但是需要按照指定结构来传递值,不清楚情况下不要直接使用,而是调用其他两个重载的函数
         * @param json 参数,包括了指定返回值结构
         */
        void callback(JSONObject json);

        /**
         * 这个函数同void callback(Json::Value json);区别在于返回的数据封装在result字段由谁来封装,自己手动或者程序填充
         * @param param 调用时传递的参数
         * @param result 操作成功或失败
         * @param data 需要返回的数据
         */
        void callback(JSONObject param, bool result, JSONObject data);
        /**
         * 同void callback(Json::Value param, bool result, Json::Value data);区别在于是否需要传递数据
         */
        void callback(JSONObject param, bool result);

        void callback(JSONObject param, int error, std::string reason);

        void callback_multi(JSONObject param, bool result, int error, std::string reason, JSONObject data);
        /**
         * 命令是否被注册
         * @param cmd 命令名字符串
         * @return
         */
        bool is_register(std::string cmd);

    private:
        cmd_factory();
        class cmd_factory_impl;
        std::shared_ptr<cmd_factory_impl> impl_;
    };

}

#endif //COMMON_CMD_FACTORY_H
