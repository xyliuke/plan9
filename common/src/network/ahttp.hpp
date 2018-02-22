//
//  ahttp.hpp
//  anet
//
//  Created by ke liu on 22/10/2017.
//  Copyright © 2017 ke liu. All rights reserved.
//

#ifndef ahttp_hpp
#define ahttp_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <functional>
#include "common/common_callback.hpp"
#include "common/char_array.h"

namespace plan9 {

    class ahttp_request {
    public:

        static const std::string METHOD_GET;
        static const std::string METHOD_POST;
        static const std::string METHOD_HEAD;//暂不支持
        static const std::string METHOD_OPTIONS;//暂不支持
        static const std::string METHOD_PUT;//暂不支持
        static const std::string METHOD_DELETE;//暂不支持
        static const std::string METHOD_TRACE;//暂不支持
        static const std::string METHOD_CONNECT;//暂不支持

        ahttp_request();

        void set_uesd_proxy(bool uesd);
        /**
         * 向header中添加数据
         * @param key header中key
         * @param value header中value
         */
        void append_header(std::string key, std::string value);

        std::string get_header(std::string key);
        /**
         * 向header中添加数据
         * @param key header中key
         * @param value header中value
         */
        void append_header(std::string key, int value);
        /**
         * 向header中添加一组数据
         * @param headers header集合
         */
        void append_header(std::shared_ptr<std::map<std::string, std::string>> headers);
        /**
         * 设置HTTP方法，目前支持GET/POST,默认为GET方法
         * @param method GET/POST
         */
        void set_method(std::string method);
        /**
         * 设置HTTP版本号，目前只支持1.1，默认为1.1
         * @param version 版本号
         */
        void set_http_version(std::string version);
        /**
         * 设置请求的url
         * @param url 请求链接
         */
        void set_url(std::string url);
        /**
         * 向body中添加数据，主要用于POST请求时
         * @param data 一组数据
         */
        void append_body_data(std::shared_ptr<std::map<std::string, std::string>> data);
        /**
         * 向body中添加数据，主要用于POST请求时
         * @param key key值
         * @param value value值
         */
        void append_body_data(std::string key, std::string value);

        void append_body_data_from_file(std::string key, std::string file);

        /**
         * 设置是否复用TCP，如果能复用的话
         * @param reused true为复用，默认为复用
         */
        void set_reused_tcp(bool reused);
        bool is_reused_tcp();
        /**
         * 设置超时时间，单位为秒，默认为30s
         * @param seconds 秒
         */
        void set_timeout(int seconds);
        int get_timeout();

        void set_keep_alive(bool keep_alive);
        bool is_keep_alive();

        /**
         * 是否为HTTPS协议
         * @return true为使用ssl协议
         */
        bool is_use_ssl();

        bool is_ip_format_host();

        /**
         * 给定的字符串是否为ip格式，包括ip4和ip6
         * @param str 字符串参数
         * @return true表示str为ip格式
         */
        static bool is_ip_format(std::string str);

        /**
         * 将对象转化为字符串
         * @return 字符串结果
         */
        std::string to_string();

        //以下方法主要为内部使用
        std::string get_http_method_string();
        std::string get_http_header_string();
        std::string get_http_string();
        void get_http_data(std::function<void(std::shared_ptr<char> data, int len, int sent, int total)>);

        std::string get_domain();
        int get_port();
        static inline int get_default_timeout() {
            return 30;
        }
    private:
        class ahttp_request_impl;
        std::shared_ptr<ahttp_request_impl> impl;
    };

    class ahttp_response {
    public:
        ahttp_response();
        /**
         * 通过key值获取header中数据，可以指定返回值转换的类型
         * @param key header中的key值，不区分大小写
         * @return header中key对应的值
         */
        template <typename T>
        T get_header(std::string key);
        /**
         * 获取header中的数据，返回字符串格式
         * @param key header中的key值，不区分大小写
         * @return header中key对应的值
         */
        std::string get_header(std::string key);
        /**
         * 获取所有header中的值
         * @return header集合
         */
        std::shared_ptr<std::map<std::string, std::string>> get_headers();

        /**
         * 设置数据写入文件
         * @param file 文件路径
         */
        void set_response_data_file(std::string file);
        /**
         * 获取HTTP 状态值
         * @return 状态值
         */
        int get_response_code();
        /**
         * 获取数据的body部分数据长度
         * @return body部分长度
         */
        long get_response_data_length();
        /**
         * 获取数据的header部分长度
         * @return header长度
         */
        long get_response_header_length();
        /**
         * 获取数据的总长度
         * @return 报文长度
         */
        long get_response_length();
        /**
         * 获取数据长度，通过header中的Content-Length来获取
         * @return 字节长度
         */
        long get_content_length();
        /**
         * 获取body的字符串
         * @return body部分的字符串
         */
        std::string get_body_string();
        /**
         * 将对象转化为字符串
         * @return 字符串
         */
        std::string to_string();
        //内部使用
        bool append_response_data(std::shared_ptr<char> data, int len);
    private:
        class ahttp_response_impl;
        std::shared_ptr<ahttp_response_impl> impl;
    };

    //TODO 失败的错误类型返回，如SSL握手错误等
    //TODO 内部状态使用状态机来实现
    class ahttp {
    public:
        ahttp();
        ~ahttp();

        /**
         * 设置是否自动使用代理，默认为false
         * @param auto_use  true 表示请求时会自动获取代理服务器，并使用代理服务器；
         * false 表示不自动获取最新的代理服务器，而是依赖set_proxy的配置
         * @note 当auto_use由true修改为false时，自动清空proxy设置
         */
        static void set_auto_proxy(bool auto_use);

        /**
         * 设置代理功能，当设置set_auto_proxy由true变成false时，要重新设置
         * @param host ip或者域名
         * @param port 端口号
         */
        static void set_proxy(std::string host, int port);

        /**
         * 设置相同ip和端口号下的最大同时连接数
         * 未达到最大连接数数的请求，在已存在的tcp不空闲时，创建新的连接；存在空闲tcp时，优先复用原来tcp
         * 达到最大连接数时，等待tcp空闲，再复用tcp进行请求
         *
         * @param max 最大连接数
         */
        static void set_max_connection(int max);

        /**
         * 设置低优先级，默认为高优先级
         * 设置低优先级后，会优先考虑复用tcp的情况，只有在没有tcp连接情况下，才会创建新的连接
         */
        void set_low_priority();

        /**
         * 设置高优先级
         * 默认为高优先级，默认情况下，
         * 未达到最大连接数数的请求，在已存在的tcp不空闲时，创建新的连接；存在空闲tcp时，优先复用原来tcp
         * 达到最大连接数时，等待tcp空闲，再复用tcp进行请求
         */
        void set_high_priority();
        /**
         * 是否验证域名
         * @param validate true 验证 false 不验证
         */
        void is_validate_domain(bool validate);
        /**
         * 是否验证证书
         * @param validate validate true 验证 false 不验证
         */
        void is_validate_cert(bool validate);

        /**
         * 取消请求
         */
        void cancel();
        /**
         * HTTP请求最基本的方法
         * @param request 请求参数的对象
         * @param callback 回调
         */
        void exec(std::shared_ptr<ahttp_request> request, std::function<void(std::shared_ptr<common_callback>ccb, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback);
        /**
         * 封装GET请求
         * @param url url字符串
         * @param header 请求头
         * @param callback 回调
         */
        void get(std::string url, std::shared_ptr<std::map<std::string, std::string>>header, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback);
        void get(std::string url, int timeout, std::shared_ptr<std::map<std::string, std::string>>header, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback);
        /**
         * 封装POST请求
         * @param url url字符串
         * @param header 请求头
         * @param data post的数据
         * @param callback 回调
         */
        void post(std::string url, std::shared_ptr<std::map<std::string, std::string>>header, std::shared_ptr<std::map<std::string, std::string>> data, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback);
        void post(std::string url, int timeout, std::shared_ptr<std::map<std::string, std::string>>header, std::shared_ptr<std::map<std::string, std::string>> data, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback);
        /**
         * 封装下载请求
         * @param url url字符串
         * @param file 本地的文件
         * @param header 请求头
         * @param process_callback 下载进度回调
         * @param callback 完成回调
         */
        void download(std::string url, std::string file, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(long current, long total)> process_callback, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback);

        void upload(std::string url, std::string file, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(long current, long total)> process_callback, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback);
        /**
         * 设置dns解析的替换方案，默认使用libuv解析
         * @param callback 结果回调
         */
        void set_dns_resolve(std::function<void(std::string url, int port, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>)> callback);
        //各个时间段事件回调

        /**
         * 解析DNS后事件
         * @param callback 回调
         */
        void set_dns_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback);

        /**
         * 连接服务器后事件
         * @param callback 回调
         */
        void set_connected_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback);

        /**
         * ssl握手成功后的事件
         * @param callback 回调
         */
        void set_ssl_connected_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback);

        /**
         * 客户端发送数据后事件
         * @param callback 回调
         */
        void set_send_event_callback(std::function<void(std::shared_ptr<common_callback>, long sent, long total)> callback);

        /**
         * 读取数据事件，每次读取数据都会触发，调用download函数时不要使用这个事件
         * @param callback 回调
         */
        void set_read_event_callback(std::function<void(std::shared_ptr<common_callback>, long size)> callback);

        /**
         * 第一次读到数据的事件
         * @param callback 回调
         */
        void set_read_begin_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback);

        /**
         * 最后一次读到数据的事件
         * @param callback 回调
         */
        void set_read_end_event_callback(std::function<void(std::shared_ptr<common_callback>, long)> callback);

        /**
         * 获取请求过程中网络相关数据信息
         * @return 返回kv格式的请求数据
         */
        std::shared_ptr<std::map<std::string, std::string>> get_network_info();

    private:
        class ahttp_impl;
        std::shared_ptr<ahttp_impl> impl;
    };
}

#endif /* ahttp_hpp */
