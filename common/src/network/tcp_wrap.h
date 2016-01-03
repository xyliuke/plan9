//
// Created by liuke on 16/1/2.
//

#ifndef COMMON_TCP_WRAP_H
#define COMMON_TCP_WRAP_H


#include <memory>
#include <string>
#include <functional>


/**
 *  网络包的格式:
 *  网络包固定包含6个字节,其中一个起始字符,以字符'^'开始,再加一个字节的类型位（备用）,后面有4个字节的整型值,表示这个报文的长度
 *  目前类型有:
 *  0x0001  字符串
 *
 */


namespace plan9 {

    class tcp_wrap {
    public:
        static tcp_wrap instance();
        /**
         * 连接服务器
         * @param ip 服务器的地址
         * @param port 服务器的端口号
         */
        void connect(std::string ip, int port);
        /**
         * 连接服务器
         * @param url 服务器的域名和端口  格式为: www.gocoding.cn:8080
         */
        void connect(std::string url);

        /**
         * 关闭连接
         */
        void close();

        void write(std::string msg);

        /**
         * 设置连接后的函数回调
         * @param function 连接成功或者失败后都会调用这个函数
         */
        void set_connect_handler(std::function<void(bool)> function);

        void set_read_handler(std::function<void(std::string msg)> function);

        void set_write_handler(std::function<void()> function);

    private:
        tcp_wrap();
        class tcp_wrap_impl;
        std::shared_ptr<tcp_wrap_impl> impl;
    };


}

#endif //COMMON_TCP_WRAP_H
