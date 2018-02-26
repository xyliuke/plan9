//
// Created by ke liu on 30/01/2018.
// Copyright (c) 2018 ke liu. All rights reserved.
//

#ifndef ANET_AHTTP1_H
#define ANET_AHTTP1_H

#include <memory>
#include "ahttp.hpp"

namespace plan9
{
    class ahttp1 {

    public:
        ahttp1();

        virtual ~ahttp1();

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
         * HTTP请求最基本的方法
         * @param request 请求参数的对象
         * @param callback 回调
         */
        void exec(std::shared_ptr<ahttp_request> request, std::function<void(std::shared_ptr<common_callback>ccb, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback);

        void get(std::string url, int timeout, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(std::shared_ptr<common_callback>ccb, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback);
        void post(std::string url, int timeout, std::shared_ptr<std::map<std::string, std::string>> header, std::shared_ptr<std::map<std::string, std::string>> form, std::function<void(std::shared_ptr<common_callback>ccb, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback);

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
         * 取消请求
         */
        void cancel();

        /**
         * 设置dns解析的替换方案，默认使用libuv解析
         * @param callback 结果回调
         */
        void set_dns_resolve(std::function<void(std::string url, int port, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>)> callback);

        /**
         * 设置调试模式，在调试模式下，会收集网络各阶段信息，状态机的迁移路径，默认不是debug模式
         * @param debug true表示debug模式
         */
        void set_debug_mode(bool debug, std::function<void(std::string)> callback);

        /**
         * 获取请求过程中网络相关数据信息
         * @return 返回kv格式的请求数据
         */
        std::shared_ptr<std::map<std::string, std::string>> get_network_info();

        std::string get_network_info_string();

    private:
        class ahttp_impl;
        std::shared_ptr<ahttp_impl> impl;
    };
}


#endif //ANET_AHTTP1_H
