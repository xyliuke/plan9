//
// Created by liuke on 16/1/2.
//

#ifndef COMMON_TCP_H
#define COMMON_TCP_H


#include <memory>
#include <string>
#include <functional>


/**
 *  网络包的格式:
 *  网络包固定包含6个字节,其中一个起始字符,以字符'^'开始,再加一个字节的类型位,后面有4个字节的整型值,表示这个报文的长度
 *  目前类型有:
 *  0x01  字符串
 *  0x02  ping包标志位
 *  0x03  pong包标志位
 */


namespace plan9 {

    class tcp {
    public:
        tcp();
        /**
         * 连接服务器
         * @param ip 服务器的地址
         * @param port 服务器的端口号
         */
        void connect(std::string ip, int port);

        static void resolver(std::string url, int port, std::function<void(std::shared_ptr<std::vector<std::tuple<std::string, int>>>)> callback);
        /**
         * 重新连接服务器
         */
        void reconnect();

        /**
         * 关闭连接
         */
        void close();

        /**
         * 向服务器发送数据
         * @param data 数据数组
         * @param len 数据长度
         */
        void write(const char data[], int len);

        /**
         * 设置连接后的函数回调
         * @param function 连接成功或者失败后都会调用这个函数
         */
        void set_connect_handler(std::function<void(bool)> function);

        void set_read_handler(std::function<void(char data[], int len)> function);

        void set_write_handler(std::function<void(char data[], int len)> function);

    private:
        class tcp_impl;
        std::shared_ptr<tcp_impl> impl;
    };


}

#endif //COMMON_TCP_H
