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
 *
 *    }
 *
 * }
 *
 */


#include <iosfwd>
#include <string>
#include <json/json.h>

namespace plan9
{
    class cmd_factory {
    public:
        static cmd_factory instance();
        void register_cmd(std::string cmd, std::function<void(Json::Value)> function);
        void execute(std::string cmd, Json::Value param, std::function<void(Json::Value)> callback);
        void callback(Json::Value json);

    private:
        cmd_factory();
        class cmd_factory_impl;
        std::shared_ptr<cmd_factory_impl> impl_;
    };

}

#endif //COMMON_CMD_FACTORY_H
