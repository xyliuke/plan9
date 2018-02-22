//
// Created by ke liu on 30/01/2018.
// Copyright (c) 2018 ke liu. All rights reserved.
//

#include <assert.h>
#include <set>
#include <iostream>
#include <sstream>
#include "ahttp1.h"
#include "state/state_machine.h"
#include "uvwrapper/uv_wrapper.hpp"
#include "proxy/local_proxy.h"
#include "common/common_callback_err_wrapper.h"

namespace plan9
{
    class mutex_wrap {
    public:
        void lock() {
            mutex.lock();
        }

        void unlock() {
            mutex.unlock();
        }
    private:
        std::recursive_mutex mutex;
    };

    static int dns_cache_time = 1000 * 60;
    static void http_default_cache_resolve(std::string url, int port, std::function<void(std::shared_ptr<common_callback>,
            std::shared_ptr<std::vector<std::string>>)> callback) {
        static mutex_wrap mutex;
        class ip_cache {
        public:
            ip_cache() : time(std::chrono::system_clock::now().time_since_epoch().count() / 1000),
                         callbacks(std::make_shared<std::vector<std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>>>()){
            }

            bool is_too_long(long t) {
                if (dns_cache_time > 0) {
                    return t - time > dns_cache_time;
                }
                return true;
            }

            bool is_cache() {
                return dns_cache_time > 0;
            }

            void push_callback(std::function<void(std::shared_ptr<common_callback>,
                    std::shared_ptr<std::vector<std::string>>)> cb) {
                if (cb) {
                    mutex.lock();
                    callbacks->push_back(cb);
                    mutex.unlock();
                }
            }

            void callback(std::shared_ptr<common_callback> ccb) {
                mutex.lock();
                auto it = callbacks->begin();
                while (it != callbacks->end()) {
                    if (*it) {
                        (*it)(ccb, ips);
                    }
                    it ++;
                }
                callbacks->clear();
                mutex.unlock();
            }

            bool has_callback() {
                mutex.lock();
                bool ret = callbacks->size() > 0;
                mutex.unlock();
                return ret;
            }

            void reset() {
                ips = nullptr;
                time = std::chrono::system_clock::now().time_since_epoch().count() / 1000;
            }
            std::shared_ptr<std::vector<std::string>> ips;
        private:
            long time;
            std::shared_ptr<std::vector<std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>>> callbacks;
        };
        static std::map<std::string, std::shared_ptr<ip_cache>> cache;
        std::stringstream ss;
        ss << url;
        ss << ":";
        ss << port;
        long time = std::chrono::system_clock::now().time_since_epoch().count() / 1000;

        std::string key = ss.str();
        if (cache.find(key) != cache.end()) {
            auto ip = cache[key];
            if (ip->is_cache() && ip->is_too_long(time)) {
                ip->reset();
                ip->push_callback(callback);
            } else {
                if (ip->ips) {
                    if (callback) {
                        callback(common_callback::get(), ip->ips);
                    }
                    return;
                } else {
                    bool has = ip->has_callback();
                    ip->push_callback(callback);
                    if (has) {
                        return;
                    }
                }
            }
        } else {
            auto ip = std::make_shared<ip_cache>();
            ip->push_callback(callback);
            cache[key] = ip;
        }

        uv_wrapper::resolve(url, port, [=](std::shared_ptr<common_callback> ccb, std::shared_ptr<std::vector<std::string>> ips) {
            if (cache.find(key) != cache.end()) {
                auto ip = cache[key];
                if (ccb->success) {
                    ip->ips = ips;
                }
                ip->callback(ccb);
            }
        });
    }

    class http_info {
    public:

        http_info() : info(std::make_shared<std::map<std::string, std::string>>()) {

        }

        void set_proxy(bool used) {
            push("proxy", used ? "1" : "0");
        }

        void set_reused_tcp(bool reused) {
            push("reused_tcp", reused ? "1" : "0");
        }

        void set_fetch_time() {
            fetch = get_current_time();
            std::stringstream ss;
            ss << (fetch / 1000);
            push("fetch", ss.str());
        }

        void set_remote_ip_port(std::string ip, int port) {
            std::stringstream ss;
            ss << ip;
            ss << ":";
            ss << port;
            push("remote", ss.str());
        }

        void set_remote_ip_port(std::string ip, std::string port) {
            std::stringstream ss;
            ss << ip;
            ss << ":";
            ss << port;
            push("remote", ss.str());
        }

        void set_dns_start_time() {
            push("dns_start", get_current_time_string());
        }

        void set_dns_end_time() {
            push("dns_end", get_current_time_string());
        }

        void set_connect_start_time() {
            push("connect_start", get_current_time_string());
        }

        void set_connect_end_time() {
            push("connect_end", get_current_time_string());
        }

        void set_ssl_start_time() {
            push("ssl_start", get_current_time_string());
        }
        void set_ssl_end_time() {
            push("ssl_end", get_current_time_string());
        }

        void set_request_start_time() {
            push("request_start", get_current_time_string());
        }
        void set_request_end_time() {
            push("request_end", get_current_time_string());
        }
        void set_response_start_time() {
            push("response_start", get_current_time_string());
        }

        void set_response_end_time() {
            long end = get_current_time();
            std::stringstream ss;
            ss << (end / 1000);
            push("response_end", ss.str());
            std::stringstream sss;
            sss << ((end - fetch) / 1000);
            push("total", sss.str());
        }

        void set_request_data_size(int size) {
            std::stringstream ss;
            ss << size;
            push("request_bytes", ss.str());
        }

        void set_response_data_size(int size) {
            std::stringstream ss;
            ss << size;
            push("response_bytes", ss.str());
        }

        void set_local_ip_port(std::string ip, int port) {
            std::stringstream ss;
            ss << ip;
            ss << ":";
            ss << port;
            push("local", ss.str());
        }

        void set_local_ip_port(std::string ip, std::string port) {
            std::stringstream ss;
            ss << ip;
            ss << ":";
            ss << port;
            push("local", ss.str());
        }

        std::shared_ptr<std::map<std::string, std::string>> get_info() {
            return info;
        }

    private:
        std::shared_ptr<std::map<std::string, std::string>> info;
        std::string get_current_time_string() {
            std::stringstream ss;
            ss << (get_current_time() / 1000);
            return ss.str();
        }
        long get_current_time() {
            auto tp = std::chrono::system_clock::now();
            return tp.time_since_epoch().count();
        }
        void push(std::string key, std::string value) {
            if (info) {
                (*info)[key] = value;
            }
        }
        long fetch;
    };

    class ahttp1::ahttp_impl : public state_machine {
    public:
        ahttp_impl() : tcp_id(-1), timer_id(-1), validate_domain(false), validate_cert(false), low_priority(false),
                       dns_resolve_callback(nullptr), debug_mode(false), info(std::make_shared<http_info>()) {
            static int count = 0;
            id = count;
            count ++;
            //1
            STATE_MACHINE_ADD_ROW(this, init_state, PUSH_WAITING_QUEUE, wait_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //2
            STATE_MACHINE_ADD_ROW(this, init_state, FETCH, begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //3
            STATE_MACHINE_ADD_ROW(this, wait_state, FETCH, begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //4
            STATE_MACHINE_ADD_ROW(this, begin_state, READY_DNS, dns_begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //5
            STATE_MACHINE_ADD_ROW(this, dns_begin_state, DNS_RESOLVE, dns_ing_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //6
            STATE_MACHINE_ADD_ROW(this, dns_ing_state, DNS_RESOLVE_OK, dns_end_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //7
            STATE_MACHINE_ADD_ROW(this, dns_end_state, READY_CONNECT, connect_begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //8
            STATE_MACHINE_ADD_ROW(this, connect_begin_state, OPEN, connecting_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //9
            STATE_MACHINE_ADD_ROW(this, connecting_state, OPEN_SUCCESS, connected_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //10
            STATE_MACHINE_ADD_ROW(this, connected_state, SSL_CONNECT, ssl_ing_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //11
            STATE_MACHINE_ADD_ROW(this, ssl_ing_state, SSL_CONNECT_SUCCESS, ssl_end_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //12
            STATE_MACHINE_ADD_ROW(this, ssl_end_state, READY_SEND, send_begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //13
            STATE_MACHINE_ADD_ROW(this, send_begin_state, SEND, send_ing_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //14
            STATE_MACHINE_ADD_ROW(this, send_ing_state, SEND_FINISH, send_end_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //15
            STATE_MACHINE_ADD_ROW(this, send_end_state, READY_RECV, read_begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //16
            STATE_MACHINE_ADD_ROW(this, read_begin_state, RECV, read_ing_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //17
            STATE_MACHINE_ADD_ROW(this, read_ing_state, RECV_FINISH, read_end_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //18
            STATE_MACHINE_ADD_ROW(this, read_end_state, FINISH, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            //19
            STATE_MACHINE_ADD_ROW(this, dns_ing_state, DNS_RESOLVE_ERROR, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            //20
            STATE_MACHINE_ADD_ROW(this, begin_state, READY_CONNECT, connect_begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //21
            STATE_MACHINE_ADD_ROW(this, connecting_state, CLOSE, disconnect_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //22
            STATE_MACHINE_ADD_ROW(this, disconnect_state, SWITCH_IP, dns_end_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //23
            STATE_MACHINE_ADD_ROW(this, disconnect_state, RETRY, connect_begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //24
            STATE_MACHINE_ADD_ROW(this, connected_state, READY_SEND, send_begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //25
            STATE_MACHINE_ADD_ROW(this, ssl_ing_state, CLOSE, disconnect_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //26
            STATE_MACHINE_ADD_ROW(this, begin_state, READY_SEND, send_begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //27
            STATE_MACHINE_ADD_ROW(this, read_end_state, REDIRECT_INNER, send_begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //28
            STATE_MACHINE_ADD_ROW(this, read_end_state, FORWARD, send_begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //29
            STATE_MACHINE_ADD_ROW(this, read_end_state, REDIRECT_OUTER, begin_state, [=](state_machine* fsm) -> bool {
                if (fsm->is_current_state<end_state>()) {
                    return false;
                }
                return true;
            });
            //30
            STATE_MACHINE_ADD_ROW(this, disconnect_state, GIVE_UP, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, wait_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, begin_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, dns_begin_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, dns_ing_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, dns_end_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, connect_begin_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, connecting_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, connected_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, ssl_ing_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, ssl_end_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, send_begin_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, send_ing_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, send_end_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, read_begin_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, read_ing_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, read_end_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, disconnect_state, TIME_OUT, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, init_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, wait_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, begin_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, dns_begin_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, dns_ing_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, dns_end_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, connect_begin_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, connecting_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, connected_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, ssl_ing_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, ssl_end_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, send_begin_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, send_ing_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, send_end_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, read_begin_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, read_ing_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, read_end_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });
            STATE_MACHINE_ADD_ROW(this, disconnect_state, CANCEL, end_state, [=](state_machine* fsm) -> bool {
                return true;
            });

            set_init_state<init_state>();
            start();
        }

        void exec(std::shared_ptr<ahttp_request> request, std::function<void(std::shared_ptr<common_callback>ccb, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
            this->request = request;
            this->response = std::make_shared<ahttp_response>();
            this->callback = callback;
            mgr->push_task(this);
        }

        static void set_max_connection(int max) {
            mgr->set_max_connection(max);
        }


        static void set_proxy(std::string host, int port) {
            if (proxy_host != host || port != proxy_port) {
                proxy_host = host;
                proxy_port = port;
            }
        }

        static void set_auto_proxy(bool auto_use) {
            if (auto_use_proxy && !auto_use) {
                set_proxy("", -1);
            }
            auto_use_proxy = auto_use;
        }

        void is_validate_domain(bool validate) {
            validate_domain = validate;
        }

        void is_validate_cert(bool validate) {
            validate_cert = validate;
        }

        void set_high_priority() {
            low_priority = false;
        }

        void set_low_priority() {
            low_priority = true;
        }

        void cancel() {
            if (!is_current_state<end_state>()) {
                remove_http();
                process_event(CANCEL);
            }
        }

        void set_dns_resolve(std::function<void(std::string url, int port, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>)> callback) {
            this->dns_resolve_callback = callback;
        }

        std::shared_ptr<std::map<std::string, std::string>> get_debug_info() {
            return info->get_info();
        };

        void set_debug_mode(bool debug) {
            debug_mode = debug;
            set_trace(debug, [=](std::string trace){
                std::cout << std::chrono::system_clock::now().time_since_epoch().count() / 1000 << "\tid : " << id << "\t" << trace << std::endl;
            });
        }
    private:
        static const std::string FETCH;//开始执行请求数据操作
        static const std::string PUSH_WAITING_QUEUE;//压入请求队列
        static const std::string READY_DNS;//
        static const std::string DNS_RESOLVE;//dns解析
        static const std::string DNS_RESOLVE_OK;//dns解析成功
        static const std::string DNS_RESOLVE_ERROR;//dns解析失败
        static const std::string READY_CONNECT;
        static const std::string OPEN;//连接TCP
        static const std::string OPEN_SUCCESS;//连接成功
        static const std::string CLOSE;//关闭TCP
        static const std::string SSL_CONNECT;//SSL握手开始
        static const std::string SSL_CONNECT_SUCCESS;//SSL握手成功
        static const std::string READY_SEND;//
        static const std::string SEND;//发送数据
        static const std::string SEND_FINISH;//发送数据完成
        static const std::string READY_RECV;
        static const std::string RECV;//接收数据
        static const std::string RECV_FINISH;//接收数据完成
        static const std::string GIVE_UP;//没有连接上服务器，直接放弃连接
        static const std::string RETRY;//重试连接服务器
        static const std::string REDIRECT_INNER;//重定向内部
        static const std::string REDIRECT_OUTER;//重定向外部
        static const std::string FORWARD;//
        static const std::string FINISH;//请求结束
        static const std::string SWITCH_IP;//换IP重新连接
        static const std::string TIME_OUT;
        static const std::string CANCEL;

        // HTTP的初始状态
        struct init_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
            }

            void on_exit(std::string event, state_machine *fsm) override {
                ahttp_impl* http = (ahttp_impl*)fsm;
                //添加超时功能
                if (http->request->get_timeout() > 0) {
                    http->timer_id = uv_wrapper::post_timer([http](){
                        http->time_out();
                    }, http->request->get_timeout() * 1000, 0);
                }
            }
        };

        //开始执行请求
        struct begin_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* impl = (ahttp_impl*)fsm;
                impl->set_fetch_time();
                auto next = [=]() {
                    impl->request->set_uesd_proxy(ahttp_impl::is_use_proxy());
                    impl->set_used_proxy();
                    if (event == REDIRECT_OUTER) {
                        //重定向
                        //                    impl->process_event(SEND);
                    } else {
                        if (impl->is_reused_tcp()) {
                            impl->set_reused_tcp(true);
                            impl->process_event(READY_SEND);
                        } else {
                            impl->set_reused_tcp(false);

                            if (impl->request->is_ip_format_host() || (is_use_proxy() && ahttp_request::is_ip_format(proxy_host))) {
                                //ip直连
                                impl->process_event(READY_CONNECT);
                            } else {
                                impl->process_event(READY_DNS);
                            }

                        }
                    }
                };
                if (impl->auto_use_proxy) {
                    ahttp_impl::get_auto_proxy([=](){
                        next();
                    });
                } else {
                    next();
                }
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //在请求队列中等待
        struct wait_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        struct dns_begin_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* http = (ahttp_impl*)fsm;
                http->set_dns_start_time();
                http->process_event(DNS_RESOLVE);
                http->resolve();
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //dns解析中
        struct dns_ing_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //dns解析完成
        struct dns_end_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* impl = (ahttp_impl*)fsm;
                if (event == SWITCH_IP) {
                    //换下一个IP
                    impl->change_ip();
                } else {
                    impl->set_dns_end_time();
                }
                impl->process_event(READY_CONNECT);
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //连接中
        struct connect_begin_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* http = (ahttp_impl*)fsm;
                http->set_connect_start_time();
                http->process_event(OPEN);
                http->connect();
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //连接中
        struct connecting_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //连接完成
        struct connected_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* http = (ahttp_impl*)fsm;
                http->set_connect_end_time();
                http->set_local_info();
                if (http->is_ssl_connect()) {
                    //HTTPS
                    auto ssl = uv_wrapper::get_ssl_impl_by_tcp_id(http->tcp_id);
                    if (ssl) {
                        ssl->validate_domain(http->validate_domain);
                        ssl->validate_cert(http->validate_cert);
                    }
                    http->process_event(SSL_CONNECT);
                } else {
                    //HTTP
                    http->process_event(READY_SEND);
                }
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //断开连接
        struct disconnect_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* http = (ahttp_impl*)fsm;
                http->process_event(GIVE_UP);
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //ssl ing
        struct ssl_ing_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* http = (ahttp_impl*)fsm;
                http->set_ssl_start_time();
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //ssl end
        struct ssl_end_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* http = (ahttp_impl*)fsm;
                http->set_ssl_end_time();
                http->process_event(READY_SEND);
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //发送数据中
        struct send_begin_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* http = (ahttp_impl*)fsm;
                http->set_request_start_time();
                http->process_event(SEND);
                http->send();
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //发送数据中
        struct send_ing_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //发送数据结束
        struct send_end_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* http = (ahttp_impl*)fsm;
                http->set_request_end_time();
                http->process_event(READY_RECV);
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //读取数据中
        struct read_begin_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* http = (ahttp_impl*)fsm;
                http->set_response_start_time();
                http->process_event(RECV);
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //读取数据中
        struct read_ing_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //读取数据结束
        struct read_end_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* http = (ahttp_impl*)fsm;
                //TODO 判断Redirect/Forward情况
                if (event == REDIRECT_OUTER || event == REDIRECT_INNER || event == FORWARD) {

                } else {
                    http->set_response_data_size();
                    http->set_response_end_time();
                    http->process_event(FINISH);
                }
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };
        //请求结束状态
        struct end_state : public state {
            void on_entry(std::string event, state_machine *fsm) override {
                ahttp_impl* http = (ahttp_impl*)fsm;
                if (event != TIME_OUT) {
                    http->cancel_timer();
                }
                http->remove_http();
                http->send_callback();
                //执行下一个任务
                http->exec_next();
            }

            void on_exit(std::string event, state_machine *fsm) override {
            }
        };

        void no_transition(std::shared_ptr<state> begin, std::string event) override {
        }

        std::shared_ptr<ahttp_request> request;
        std::shared_ptr<ahttp_response> response;
        std::function<void(std::shared_ptr<common_callback>ccb, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback;
        std::shared_ptr<common_callback> ccb;
        int tcp_id;
        int timer_id;
        std::function<void(std::string url, int port, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>)> dns_resolve_callback;
        bool validate_domain;
        bool validate_cert;
        bool low_priority;
        std::shared_ptr<http_info> info;
        bool debug_mode;
        static std::string proxy_host;
        static int proxy_port;
        static bool auto_use_proxy;
        int id;

        class ahttp_mgr {
        public:
            ahttp_mgr() : url_ips(std::make_shared<std::map<std::string, std::shared_ptr<std::vector<std::string>>>>()),
                          url_tcp(std::make_shared<std::map<std::string, std::shared_ptr<std::set<int>>>>()),
                          tcp_http(std::make_shared<std::map<int, std::shared_ptr<std::vector<ahttp_impl*>>>>()),
                          url_http_unconnect(std::make_shared<std::map<std::string, std::shared_ptr<std::vector<ahttp_impl*>>>>()),
                          max_connection_num(4) {
            }

            void push_task(ahttp_impl* impl) {
                int size;
                bool reused = is_reused_tcp(impl, &size);
                bool exist = true;
                if (impl->low_priority && !reused) {
                    exist = is_exist_http_in_unconnect_queue(impl);
                }
                if ((impl->low_priority && (reused || exist)) || is_exceed_max_connection_num(impl)) {
                    //超过最大连接数或者优先级低
                    if (reused) {
                        assign_reused_tcp(impl);
                    } else {
                        push_unconnect_queue(impl);
                    }
                    impl->process_event(PUSH_WAITING_QUEUE);
                } else {
                    if (reused && size == 0) {
                        assign_reused_tcp(impl);
                    } else {
                        push_unconnect_queue(impl);
                    }
                    impl->process_event(FETCH);
                }
            }

            void push_ips(ahttp_impl* http, std::shared_ptr<std::vector<std::string>> ips) {
                if (http) {
                    (*url_ips)[http->get_uni_domain()] = ips;
                }
            }

            std::string get_ip(ahttp_impl* http) {
                if (http) {
                    if (url_ips->find(http->get_uni_domain()) != url_ips->end()) {
                        //TODO 设置IP的选择性
                        return (*(*url_ips)[http->get_uni_domain()])[0];
                    } else {
                        if (http->is_use_proxy() && http->request->is_ip_format(ahttp_impl::proxy_host)) {
                            return ahttp_impl::proxy_host;
                        }
                    }
                }
                return "";
            }

            int get_port(ahttp_impl* http) {
                if (http) {
                    if (ahttp_impl::is_use_proxy()) {
                        return ahttp_impl::proxy_port;
                    } else {
                        return http->request->get_port();
                    }
                }
                return -1;
            }

            void switch_ip(ahttp_impl* http) {}

            bool is_reused_tcp(ahttp_impl *http, int* size) {
                *size = 0;
                if (http && url_tcp->find(http->get_uni_domain()) != url_tcp->end()) {
                    auto list = (*url_tcp)[http->get_uni_domain()];
                    if (list->size() > 0) {
                        *size = (int)(list->size());
                        return true;
                    }
                }
                return false;
            }

            void connect(ahttp_impl* impl) {
                int tcp_id = uv_wrapper::connect(get_ip(impl), get_port(impl), impl->is_ssl_connect(), impl->request->get_domain(),
                        [=](std::shared_ptr<common_callback> ccb, int tcp_id) {
                            impl->ccb = ccb;
                            if (ccb->success) {
                                impl->tcp_id = tcp_id;
                                impl->process_event(OPEN_SUCCESS);
                            } else {
                                impl->process_event(CLOSE);
                            }
                        }, [=](std::shared_ptr<common_callback> ccb, int tcp_id) {
                            impl->ccb = ccb;
                            if (ccb->success) {
                                impl->process_event(SSL_CONNECT_SUCCESS);
                            } else {
                                impl->process_event(CLOSE);
                            }
                        }, [=](int tcp_id, std::shared_ptr<char> data, int len) {
                            ahttp_impl* http = get_http(tcp_id);
                            if (http) {
                                bool finish = http->response->append_response_data(data, len);
                                if (finish) {
                                    http->process_event(RECV_FINISH);
                                }
                            }
                        }, [=](std::shared_ptr<common_callback> ccb, int tcp_id) {
                            impl->ccb = ccb;
                            close(tcp_id);
                        });
                impl->tcp_id = tcp_id;
                push(tcp_id, impl);
                remove_from_unconnected_queue(impl);
            }

            void send(ahttp_impl* impl) {
                //找到对应的TCP_ID
                const int tcp_id = get_tcp_id(impl);
                if (tcp_id > 0) {
                    impl->request->get_http_data([=](std::shared_ptr<char> data, int len, int sent, int total){
                        uv_wrapper::write(tcp_id, data, len, [=](std::shared_ptr<common_callback> write_callback){
                            if ((sent + len) >= total) {
                                //发送完成
                                impl->set_request_data_size(total);
                                impl->process_event(SEND_FINISH);
                            }
                        });
                    });
                }
            }

            void remove_top_http(int tcp_id) {
                if (tcp_http->find(tcp_id) != tcp_http->end()) {
                    auto list = (*tcp_http)[tcp_id];
                    if (list->size() > 0) {
                        list->erase(list->begin());
                    }
                }
            }

            void remove_http(ahttp_impl* http) {
                auto it = tcp_http->begin();
                while (it != tcp_http->end()) {
                    auto list = it->second;
                    auto itt = list->begin();
                    while (itt != list->end()){
                        if ((*itt) == http) {
                            list->erase(itt);
                            return;
                        }
                        itt ++;
                    }
                    it ++;
                }
            }
            void set_max_connection(int max) {
                if (max > 0 && max < 16) {
                    max_connection_num = max;
                }
            }
            void exec_next(int tcp_id) {
                mutex.lock();
                ahttp_impl* http = nullptr;
                if (tcp_http->find(tcp_id) != tcp_http->end()) {
                    auto list = (*tcp_http)[tcp_id];
                    if (list->size() > 0) {
                        auto h = (*list)[0];
                        if (h) {
                            http = h;
                        }
                    } else {
                        //去未连接队列中查找
                        std::string host = get_host(tcp_id);
                        if (host != "") {
                            if (url_http_unconnect->size() > 0 && url_http_unconnect->find(host) != url_http_unconnect->end()) {
                                auto list = (*url_http_unconnect)[host];
                                if (list->size() > 0) {
                                    auto it = list->begin();
                                    bool find = false;
                                    while (it != list->end()) {
                                        if (!((*it)->low_priority)) {
                                            http = *it;
                                            list->erase(it);
                                            find = true;
                                            break;
                                        }
                                        it ++;
                                    }
                                    if (!find) {
                                        http = *(list->begin());
                                        list->erase(list->begin());
                                    }
                                }
                            }
                        }
                    }
                }
                mutex.unlock();
                if (http && !(http->is_current_state<end_state>())) {
                    push(tcp_id, http);
                    http->process_event(FETCH);
                }
            }
            bool is_exceed_max_connection_num(ahttp_impl* http) {
                int num = 0;
                int max = max_connection_num;
                if (ahttp_impl::is_use_proxy()) {
                    //使用代理，则只有一个连接
                    max = 1;
                }
                mutex.lock();
                if (url_http_unconnect->find(http->get_uni_domain()) != url_http_unconnect->end()) {
                    auto list = (*url_http_unconnect)[http->get_uni_domain()];
                    if (list) {
                        num += list->size();
                    }
                }
                if (url_tcp->find(http->get_uni_domain()) != url_tcp->end()) {
                    auto list = (*url_tcp)[http->get_uni_domain()];
                    if (list) {
                        num += list->size();
                    }
                }
                mutex.unlock();
                return num >= max;
            }
            void assign_reused_tcp(ahttp_impl* http) {
                auto tcp_ids = (*url_tcp)[http->get_uni_domain()];
                if (tcp_ids->size() > 0) {
                    auto it = tcp_ids->begin();
                    int tcp_id_task_min_num = -1;
                    int tcp_id_ret = -1;
                    while (it != tcp_ids->end()) {
                        int tcp_id = *it;
                        if (tcp_http->find(tcp_id) != tcp_http->end()) {
                            auto list = (*tcp_http)[tcp_id];
                            if (list->size() < tcp_id_task_min_num) {
                                tcp_id_task_min_num = (int)list->size();
                                tcp_id_ret = tcp_id;
                            }
                        } else {
                            if (tcp_id_task_min_num > 0) {
                                tcp_id_task_min_num = 0;
                                tcp_id_ret = tcp_id;
                            }
                        }
                        it ++;
                    }
                    if (tcp_id_ret > 0) {
                        push(tcp_id_ret, http);
                    }
                }
            }

        private:

            int get_tcp_id(ahttp_impl* impl) {
                mutex.lock();
                int ret = -1;
                auto it = tcp_http->begin();
                while (it != tcp_http->end()) {
                    auto list = it->second;
                    auto itt = list->begin();
                    while (itt != list->end()){
                        if ((*itt) == impl) {
                            ret = it->first;
                            break;
                        }
                        itt ++;
                    }
                    if (ret > 0) {
                        break;
                    }
                    it ++;
                }
                mutex.unlock();
                return ret;
            }

            ahttp_impl* get_http(int tcp_id) {
                if (tcp_http->find(tcp_id) != tcp_http->end()) {
                    auto list = (*tcp_http)[tcp_id];
                    if (list->size() > 0) {
                        return (*list)[0];
                    }
                }
                return nullptr;
            }
            void push(int tcp_id, ahttp_impl* http) {
                mutex.lock();
                http->tcp_id = tcp_id;
                if (url_tcp->find(http->get_uni_domain()) != url_tcp->end()) {
                    auto list = (*url_tcp)[http->get_uni_domain()];
                    list->insert(tcp_id);
                } else {
                    auto list = std::make_shared<std::set<int>>();
                    list->insert(tcp_id);
                    (*url_tcp)[http->get_uni_domain()] = list;
                }

                std::shared_ptr<std::vector<ahttp_impl*>> list;
                if (tcp_http->find(tcp_id) != tcp_http->end()) {
                    list = (*tcp_http)[tcp_id];
                } else {
                    list = std::make_shared<std::vector<ahttp_impl*>>();
                    (*tcp_http)[tcp_id] = list;
                }
                list->push_back(http);
                mutex.unlock();
            }

            void close(int tcp_id) {
                mutex.lock();
                std::shared_ptr<std::vector<ahttp_impl*>> list;
                if (tcp_http->find(tcp_id) != tcp_http->end()) {
                    list = (*tcp_http)[tcp_id];
                }

                tcp_http->erase(tcp_id);
                auto it = url_tcp->begin();
                while (it != url_tcp->end()) {
                    auto list = it->second;
                    auto itt = list->begin();
                    while (itt != list->end()) {
                        if (*itt == tcp_id) {
                            list->erase(itt);
                            break;
                        }
                        itt ++;
                    }
                    it ++;
                }
                mutex.unlock();

                auto itt = list->begin();
                while (itt != list->end()) {
                    (*itt)->process_event(CLOSE);
                    itt ++;
                }
            }

            void push_unconnect_queue(ahttp_impl* http) {
                mutex.lock();
                if (url_http_unconnect->find(http->get_uni_domain()) != url_http_unconnect->end()) {
                    auto list = (*url_http_unconnect)[http->get_uni_domain()];
                    list->push_back(http);
                } else {
                    auto list = std::make_shared<std::vector<ahttp_impl*>>();
                    list->push_back(http);
                    (*url_http_unconnect)[http->get_uni_domain()] = list;
                }
                mutex.unlock();
            }

            bool is_exist_http_in_unconnect_queue(ahttp_impl* http) {
                mutex.lock();
                bool ret = url_http_unconnect->find(http->get_uni_domain()) != url_http_unconnect->end();
                mutex.unlock();
                return ret;
            }

            void remove_from_unconnected_queue(ahttp_impl *http) {
                mutex.lock();
                if (url_http_unconnect->find(http->get_uni_domain()) != url_http_unconnect->end()) {
                    auto list = (*url_http_unconnect)[http->get_uni_domain()];
                    auto it = list->begin();
                    while (it != list->end()) {
                        if (*it == http) {
                            list->erase(it);
                            break;
                        }
                        it ++;
                    }
                }
                mutex.unlock();
            }

            std::string get_host(int tcp_id) {
                auto it = url_tcp->begin();
                while (it != url_tcp->end()) {
                    auto set = it->second;
                    auto itt = set->begin();
                    while (itt != set->end()) {
                        if (*itt == tcp_id) {
                            return it->first;
                        }
                        itt ++;
                    }
                    it ++;
                }
                return "";
            }

            std::shared_ptr<std::map<std::string, std::shared_ptr<std::vector<std::string>>>> url_ips;
            std::shared_ptr<std::map<int, std::shared_ptr<std::vector<ahttp_impl*>>>> tcp_http;
            std::shared_ptr<std::map<std::string, std::shared_ptr<std::set<int>>>> url_tcp;
            std::shared_ptr<std::map<std::string, std::shared_ptr<std::vector<ahttp_impl*>>>> url_http_unconnect;
            mutex_wrap mutex;
            int max_connection_num;
        };

        static std::shared_ptr<ahttp_mgr> mgr;


        //http private method
        std::function<void(std::string url, int port, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>)> get_resolver() {
            if (dns_resolve_callback) {
                return dns_resolve_callback;
            }
            static std::function<void(std::string url, int port, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>)> default_dns_resolve = nullptr;
            if (!default_dns_resolve) {
                default_dns_resolve = std::bind(&uv_wrapper::resolve, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            }
            return default_dns_resolve;
        }

        void push_ips(std::shared_ptr<std::vector<std::string>> ips) {
            mgr->push_ips(this, ips);
        }

        void change_ip() {
            mgr->switch_ip(this);
        }

        void send() {
            mgr->send(this);
        }

        void send_callback() {
            if (callback) {
                if (!ccb) {
                    ccb = common_callback::get();
                }
                callback(ccb, request, response);
            }
        }

        bool is_reused_tcp() {
            int size;
            return mgr->is_reused_tcp(this, &size) && size == 0;
        }

        void remove_http() {
            mgr->remove_http(this);
        }
        void resolve() {
            get_resolver()(request->get_domain(), request->get_port(), [=](std::shared_ptr<common_callback> ccb, std::shared_ptr<std::vector<std::string>> ips) {
                if (!is_current_state<end_state>()) {
                    if (ccb->success) {
                        push_ips(ips);
                        process_event(DNS_RESOLVE_OK);
                    } else {
                        this->ccb = ccb;
                        process_event(DNS_RESOLVE_ERROR);
                    }
                }
            });
        }
        void connect() {
            mgr->connect(this);
        }

        void exec_next() {
            mgr->exec_next(tcp_id);
        }

        void time_out() {
            mgr->remove_http(this);
            ccb = common_callback_err_wrapper::get(E_TIME_OUT);
            process_event(TIME_OUT);
        }
        void cancel_timer() {
            uv_wrapper::cancel_timer(timer_id);
        }

        void set_fetch_time() {
            if (debug_mode) {
                info->set_fetch_time();
            }
        }

        void set_local_info() {
            if (debug_mode) {
                auto tcp_info = uv_wrapper::get_info(tcp_id);
                info->set_local_ip_port((*tcp_info)["local_ip"], (*tcp_info)["local_port"]);
                info->set_remote_ip_port((*tcp_info)["remote_ip"], (*tcp_info)["remote_port"]);
            }
        }

        void set_dns_start_time() {
            if (debug_mode) {
                info->set_dns_start_time();
            }
        }

        void set_dns_end_time() {
            if (debug_mode) {
                info->set_dns_end_time();
            }
        }

        void set_connect_start_time() {
            if (debug_mode) {
                info->set_connect_start_time();
            }
        }

        void set_connect_end_time() {
            if (debug_mode) {
                info->set_connect_end_time();
            }
        }

        void set_ssl_start_time() {
            if (debug_mode) {
                info->set_ssl_start_time();
            }
        }
        void set_ssl_end_time() {
            if (debug_mode) {
                info->set_ssl_end_time();
            }
        }

        void set_request_start_time() {
            if (debug_mode) {
                info->set_request_start_time();
            }
        }
        void set_request_end_time() {
            if (debug_mode) {
                info->set_request_end_time();
            }
        }
        void set_response_start_time() {
            if (debug_mode) {
                info->set_response_start_time();
            }
        }

        void set_response_end_time() {
            if (debug_mode) {
                info->set_response_end_time();
            }
        }

        void set_request_data_size(int size) {
            if (debug_mode) {
                info->set_request_data_size(size);
            }
        }

        void set_response_data_size() {
            if (debug_mode) {
                info->set_response_data_size((int)response->get_response_length());
            }
        }

        void set_reused_tcp(bool reused) {
            if (debug_mode) {
                info->set_reused_tcp(reused);
            }
        }

        void set_used_proxy() {
            if (debug_mode) {
                info->set_proxy(is_use_proxy());
            }
        }

        std::string get_uni_domain() {
            std::stringstream ss;
            ss << request->get_domain();
            ss << ":";
            ss << request->get_port();
            return ss.str();
        }

        static bool is_use_proxy() {
            return proxy_port > 0 && "" != proxy_host;
        }

        bool is_ssl_connect() {
            return request->is_use_ssl() && !(is_use_proxy());
        }

        static void get_auto_proxy(std::function<void()> callback) {
            static long time = 0;
            long cur = std::chrono::system_clock::now().time_since_epoch().count() / 1000;
            if (cur - time > 2000) {
                time = cur;
                local_proxy::get_proxy([=](std::shared_ptr<std::map<std::string, std::string>> proxy){
                    if (proxy->find("HTTPPort") != proxy->end() && proxy->find("HTTPProxy") != proxy->end()) {
                        std::string port = proxy->at("HTTPPort");
                        proxy_host = proxy->at("HTTPProxy");
                        proxy_port = atoi(port.c_str());
                    } else {
                        proxy_host = "";
                        proxy_port = -1;
                    }
                    if (callback) {
                        callback();
                    }
                });
            } else {
                if (callback) {
                    callback();
                }
            }

        }
    };

    const std::string ahttp1::ahttp_impl::FETCH("FETCH");//开始执行请求数据操作
    const std::string ahttp1::ahttp_impl::PUSH_WAITING_QUEUE("PUSH_WAITING_QUEUE");//压入请求队列
    const std::string ahttp1::ahttp_impl::READY_DNS("READY_DNS");//
    const std::string ahttp1::ahttp_impl::DNS_RESOLVE("DNS_RESOLVE");//dns解析
    const std::string ahttp1::ahttp_impl::DNS_RESOLVE_OK("DNS_RESOLVE_OK");//dns解析成功
    const std::string ahttp1::ahttp_impl::DNS_RESOLVE_ERROR("DNS_RESOLVE_ERROR");//dns解析失败
    const std::string ahttp1::ahttp_impl::READY_CONNECT("READY_CONNECT");
    const std::string ahttp1::ahttp_impl::OPEN("OPEN");//连接TCP
    const std::string ahttp1::ahttp_impl::OPEN_SUCCESS("OPEN_SUCCESS");//连接成功
    const std::string ahttp1::ahttp_impl::CLOSE("CLOSE");//关闭TCP
    const std::string ahttp1::ahttp_impl::SSL_CONNECT("SSL_CONNECT");//SSL握手开始
    const std::string ahttp1::ahttp_impl::SSL_CONNECT_SUCCESS("SSL_CONNECT_SUCCESS");//SSL握手成功
    const std::string ahttp1::ahttp_impl::READY_SEND("READY_SEND");//
    const std::string ahttp1::ahttp_impl::SEND("SEND");//发送数据
    const std::string ahttp1::ahttp_impl::SEND_FINISH("SEND_FINISH");//发送数据完成
    const std::string ahttp1::ahttp_impl::READY_RECV("READY_RECV");
    const std::string ahttp1::ahttp_impl::RECV("RECV");//接收数据
    const std::string ahttp1::ahttp_impl::RECV_FINISH("RECV_FINISH");//接收数据完成
    const std::string ahttp1::ahttp_impl::GIVE_UP("GIVE_UP");//没有连接上服务器，直接放弃连接
    const std::string ahttp1::ahttp_impl::RETRY("RETRY");//重试连接服务器
    const std::string ahttp1::ahttp_impl::REDIRECT_INNER("REDIRECT_INNER");//重定向内部
    const std::string ahttp1::ahttp_impl::REDIRECT_OUTER("REDIRECT_OUTER");//重定向外部
    const std::string ahttp1::ahttp_impl::FORWARD("FORWARD");//
    const std::string ahttp1::ahttp_impl::FINISH("FINISH");//请求结束
    const std::string ahttp1::ahttp_impl::SWITCH_IP("SWITCH_IP");//换IP重新连接
    const std::string ahttp1::ahttp_impl::TIME_OUT("TIME_OUT");
    const std::string ahttp1::ahttp_impl::CANCEL("CANCEL");
    std::string ahttp1::ahttp_impl::proxy_host("");
    int ahttp1::ahttp_impl::proxy_port = -1;
    bool ahttp1::ahttp_impl::auto_use_proxy = false;


    std::shared_ptr<ahttp1::ahttp_impl::ahttp_mgr> ahttp1::ahttp_impl::mgr = std::make_shared<ahttp1::ahttp_impl::ahttp_mgr>();

    ahttp1::ahttp1() : impl(std::make_shared<ahttp1::ahttp_impl>()) {

    }

    ahttp1::~ahttp1() {
        cancel();
    }
    //TODO 重构完成代理功能
    void ahttp1::set_auto_proxy(bool auto_use) {
        ahttp_impl::set_auto_proxy(auto_use);
    }

    void ahttp1::set_proxy(std::string host, int port) {
        ahttp_impl::set_proxy(host, port);
    }

    void ahttp1::set_max_connection(int max) {
        ahttp_impl::set_max_connection(max);
    }

    void ahttp1::exec(std::shared_ptr<ahttp_request> request, std::function<void(std::shared_ptr<common_callback> ccb, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
        impl->exec(request, callback);
    }

    void ahttp1::is_validate_cert(bool validate) {
        impl->is_validate_cert(validate);
    }

    void ahttp1::is_validate_domain(bool validate) {
        impl->is_validate_domain(validate);
    }

    void ahttp1::set_low_priority() {
        impl->set_low_priority();
    }

    void ahttp1::set_high_priority() {
        impl->set_high_priority();
    }

    void ahttp1::cancel() {
        impl->cancel();
    }

    void ahttp1::set_dns_resolve(std::function<void(std::string url, int port, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>)> callback) {
        impl->set_dns_resolve(callback);
    }

    std::shared_ptr<std::map<std::string, std::string>> ahttp1::get_network_info() {
        return impl->get_debug_info();
    }

    void ahttp1::set_debug_mode(bool debug) {
        impl->set_debug_mode(debug);
    }
}