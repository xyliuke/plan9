//
//  thread_wrap.cpp
//  libuv_test
//
//  Created by ke liu on 11/10/2017.
//  Copyright © 2017 ke liu. All rights reserved.
//

#include "uv_wrapper.hpp"
#include <common/common_callback_err_wrapper.h>
#include <map>
#include <sstream>
#include <list>
#include <vector>
#include <uv.h>

namespace plan9 {

    struct uv_content_s {
        uv_content_s() : tcp_id(-1), connect_callback(nullptr), ssl_connect_callback(nullptr),
                         read_callback(nullptr), close_callback(nullptr), read_buf(nullptr),
                         write_callback(nullptr), ssl_enable(false), raw_read_bytes(0) {
        }

        ~uv_content_s() {
            if (read_buf != nullptr) {
                if (read_buf->base != nullptr) {
                    delete read_buf->base;
                }
                delete read_buf;
                read_buf = nullptr;
            }
        }

        void append_read_bytes(unsigned long bytes) {
            raw_read_bytes += bytes;
        }

        unsigned long get_read_bytes() {
            return raw_read_bytes;
        }

        void clear_read_bytes() {
            raw_read_bytes = 0;
        }

        std::string remote_ip;
        int remote_port;
        std::string local_ip;
        int local_port;
        int tcp_id;
        bool ssl_enable;
        std::shared_ptr<ssl_interface> ssl_impl;
        uv_tcp_t* tcp;
        std::function<void(std::shared_ptr<common_callback>, int)> connect_callback;
        std::function<void(std::shared_ptr<common_callback>, int tcp_id)> ssl_connect_callback;
        std::function<void(int tcp_id, std::shared_ptr<char> data, int len, unsigned long total_raw_len)> read_callback;
        std::function<void(std::shared_ptr<common_callback>, int tcp_id)> close_callback;
        std::function<void(std::shared_ptr<common_callback>)> write_callback;
        uv_buf_t* read_buf;
        unsigned long raw_read_bytes;
    };

    static std::map<int, std::shared_ptr<uv_content_s>> tcp_id_2_content;
    static uv_thread_t *thread = nullptr;
    static uv_loop_t *loop = nullptr;
    static std::map<int, uv_timer_t *> timer_map;
    static const int TIMER_EVER_LOOP = -1234;
    static std::map<int, uv_work_t *> concurrent_map;
    static std::list<uv_work_t *> reuse_concurrent_array;
    static std::list<uv_idle_t *> reuse_idle_array;
    static std::list<uv_timer_t *> reuse_timer_array;
    static std::function<std::shared_ptr<ssl_interface>()> ssl_callback = nullptr;

    template<typename T>
    class function_wrap {
    public:
        function_wrap(T func) : function(func), ssl(false) {
        }

        function_wrap(T func, int times) : function(func), ssl(false) {
            if (times > 0 || times == TIMER_EVER_LOOP) {
                max_times = times;
                current_times = times;
            }
        }

        ~function_wrap() {
            if (function) {
                function = nullptr;
            }
        }

        bool minus() {
            if (current_times == 1) {
                return true;
            } else if (current_times == TIMER_EVER_LOOP) {
                return false;
            }
            current_times--;
            return false;
        }

        int id;
        bool is_canceled;
        T function;
        bool ssl;
        std::shared_ptr<ssl_interface> ssl_impl;
    private:
        int max_times;
        int current_times;
    };

    static std::map<int, function_wrap<std::function<void(void)>> *> serial_map;

    static uv_mutex_t *get_timer_mutex() {
        static uv_mutex_t *timer_map_mutex = nullptr;
        if (timer_map_mutex == nullptr) {
            timer_map_mutex = new uv_mutex_t;
            uv_mutex_init(timer_map_mutex);
        }
        return timer_map_mutex;
    }

    static uv_mutex_t *get_serial_mutex() {
        static uv_mutex_t *mutex = nullptr;
        if (mutex == nullptr) {
            mutex = new uv_mutex_t;
            uv_mutex_init(mutex);
        }
        return mutex;
    }

    static uv_mutex_t *get_concurrent_mutex() {
        static uv_mutex_t *mutex = nullptr;
        if (mutex == nullptr) {
            mutex = new uv_mutex_t;
            uv_mutex_init(mutex);
        }
        return mutex;
    }

    static void idle_callback(uv_idle_t *handle) {
        if (handle->data != nullptr) {
            auto func = (function_wrap<std::function<void(void)>> *) (handle->data);
            if (func->function != nullptr) {
                func->function();
            }
            delete func;
            handle->data = nullptr;
            reuse_idle_array.push_back(handle);
        }
        uv_close((uv_handle_t *) handle, nullptr);
    }

    static void idle_loop_callback(uv_idle_t *handle) {
    }

    static void thread_cb(void *arg) {
        loop = new uv_loop_t;
        uv_loop_init(loop);
        if (arg != nullptr) {
            auto func = (function_wrap<std::function<void(void)>> *) arg;
            if (func->function != nullptr) {
                func->function();
            }
            delete (func);
        }
        uv_idle_t *idle = new uv_idle_t;
        uv_idle_init(loop, idle);
        uv_idle_start(idle, idle_loop_callback);
        uv_run(loop, UV_RUN_DEFAULT);
        uv_loop_close(loop);
        delete thread;
        thread = nullptr;
        delete loop;
        loop = nullptr;
    }

    static void async_callback(uv_async_t *async) {
        if (async->data != nullptr) {
            auto func = (function_wrap<std::function<void(void)>> *) async->data;
            if (!func->is_canceled) {
                if (func->function != nullptr) {
                    func->function();
                }
            }
            uv_mutex_trylock(get_serial_mutex());
            serial_map.erase(func->id);
            uv_mutex_unlock(get_serial_mutex());
            delete func;
        }
        uv_close((uv_handle_t *) async, nullptr);
    }

    static void init_loop(std::function<void(void)> callback) {
        if (thread == nullptr && loop == nullptr) {
            thread = new uv_thread_t;
            uv_thread_create(thread, thread_cb, new function_wrap<std::function<void(void)>>(callback));
        } else {
            if (callback) {
                callback();
            }
        }
    }

    static void timer_callback(uv_timer_t *handle) {
        if (handle->data != nullptr) {
            auto func = (function_wrap<std::function<void(void)>> *) handle->data;
            if (func->minus()) {
                if (func->function != nullptr) {
                    func->function();
                }
                uv_timer_stop(handle);
                uv_mutex_trylock(get_timer_mutex());
                timer_map.erase(func->id);
                reuse_timer_array.push_back(handle);
                uv_mutex_unlock(get_timer_mutex());
                delete func;
                handle->data = nullptr;
            } else {
                if (func->function != nullptr) {
                    func->function();
                }
            }
        }
    }

    static void queue_callback(uv_work_t *work) {
        if (work->data != nullptr) {
            auto func = (function_wrap<std::function<void(void)>> *) (work->data);
            if (func->function != nullptr) {
                func->function();
            }
            work->data = nullptr;
            uv_mutex_trylock(get_concurrent_mutex());
            concurrent_map.erase(func->id);
            reuse_concurrent_array.push_back(work);
            uv_mutex_unlock(get_concurrent_mutex());
            delete (func);
        }
    }

    void uv_wrapper::init(std::function<void(void)> callback) {
        init_loop(callback);
    }

    void uv_wrapper::set_ssl_impl(std::function<std::shared_ptr<ssl_interface>()> callback) {
        ssl_callback = callback;
        if (ssl_callback) {
            ssl_callback();//预初始化
        }
    }

    std::shared_ptr<ssl_interface> uv_wrapper::get_ssl_impl_by_tcp_id(int tcp_id) {
        if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
            return tcp_id_2_content[tcp_id]->ssl_impl;
        }
        std::shared_ptr<ssl_interface> ret;
        return ret;
    }

    int uv_wrapper::post_serial_queue(std::function<void()> callback) {
        static int count = 0;
        count++;
        uv_async_t *async = new uv_async_t;
        auto f = new function_wrap<std::function<void(void)>>(callback);
        f->id = count;
        f->is_canceled = false;
        async->data = (void *) f;
        uv_async_init(loop, async, async_callback);
        uv_mutex_trylock(get_serial_mutex());
        serial_map[count] = f;
        uv_mutex_unlock(get_serial_mutex());
        uv_async_send(async);
        return count;
    }

    void uv_wrapper::cancel_serial(int id) {
        uv_mutex_trylock(get_serial_mutex());
        if (serial_map.find(id) != serial_map.end()) {
            auto f = serial_map[id];
            f->is_canceled = true;
        }
        uv_mutex_unlock(get_serial_mutex());
    }

    void uv_wrapper::set_concurrent_pool_size(int size) {
        std::stringstream ss;
        ss << size;
        std::string s = ss.str();
//        uv_os_setenv("UV_THREADPOOL_SIZE", s.c_str());
    }

    int uv_wrapper::post_concurrent_queue(std::function<void()> callback) {
        static int count = 0;
        count++;

        uv_mutex_trylock(get_concurrent_mutex());

        uv_work_t *work = nullptr;
        if (reuse_concurrent_array.size() > 0) {
            work = *(reuse_concurrent_array.begin());
            reuse_concurrent_array.erase(reuse_concurrent_array.begin());
        } else {
            work = new uv_work_t;
        }

        auto f = new function_wrap<std::function<void(void)>>(callback);
        f->id = count;
        work->data = (void *) (f);
        concurrent_map[count] = work;

        uv_mutex_unlock(get_concurrent_mutex());
        uv_queue_work(loop, work, queue_callback, nullptr);

        return count;
    }

    void uv_wrapper::cancel_concurrent(int id) {
        uv_mutex_trylock(get_concurrent_mutex());
        if (concurrent_map.find(id) != concurrent_map.end()) {
            auto f = concurrent_map[id];
            uv_cancel((uv_req_t *) f);
        }
        uv_mutex_unlock(get_concurrent_mutex());
    }

    void uv_wrapper::post_idle(std::function<void()> callback) {
        uv_idle_t *idle_handle = nullptr;
        if (reuse_idle_array.size() > 0) {
            idle_handle = *(reuse_idle_array.begin());
            reuse_idle_array.erase(reuse_idle_array.begin());
        } else {
            idle_handle = new uv_idle_t;
        }
        idle_handle->data = new function_wrap<std::function<void(void)>>(callback);
        uv_idle_init(loop, idle_handle);
        uv_idle_start(idle_handle, idle_callback);
    }

    int uv_wrapper::post_timer(std::function<void()> callback, long time, long repeat) {
        return post_timer(callback, time, repeat, TIMER_EVER_LOOP);
    }

    int uv_wrapper::post_timer(std::function<void()> callback, long time, long repeat, int times) {
        static int timer_id = 0;
        timer_id++;

        uv_mutex_trylock(get_timer_mutex());
        uv_timer_t *timer = nullptr;
        if (reuse_timer_array.size() > 0) {
            timer = *(reuse_timer_array.begin());
            reuse_timer_array.erase(reuse_timer_array.begin());
        } else {
            timer = new uv_timer_t;
        }
        timer_map[timer_id] = timer;
        int t = times;
        if (repeat <= 0) {
            t = 1;
        }
        auto func = new function_wrap<std::function<void(void)>>(callback, t);
        func->id = timer_id;
        timer->data = (void *) (func);
        uv_mutex_unlock(get_timer_mutex());
        uv_timer_init(loop, timer);
        uv_timer_start(timer, timer_callback, time, repeat);

        return timer_id;
    }

    void uv_wrapper::cancel_timer(int timer_id) {
        uv_mutex_trylock(get_timer_mutex());
        if (timer_map.find(timer_id) != timer_map.end()) {
            auto t = timer_map[timer_id];
            uv_timer_stop(t);
            timer_map.erase(timer_id);
            if (t->data != nullptr) {
                auto f = static_cast<function_wrap<std::function<void(void)>>*>(t->data);
                delete f;
                t->data = nullptr;
            }
            reuse_timer_array.push_back(t);
        }
        uv_mutex_unlock(get_timer_mutex());
    }

    static void on_resolved(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res) {
        if (status >= 0) {
            std::shared_ptr<std::vector<std::string>> ret = std::make_shared<std::vector<std::string>>();// = res->ai_addrlen
            struct addrinfo *next = res;
            while (next != nullptr) {
                if (next->ai_family == PF_INET) {
                    char addr[17] = {'\0'};
                    uv_ip4_name((struct sockaddr_in *) next->ai_addr, addr, 16);
                    std::string ip = std::string(addr);
                    ret->push_back(ip);
                } else if (next->ai_family == PF_INET6) {
                    char addr[17] = {'\0'};
                    uv_ip6_name((struct sockaddr_in6 *) next->ai_addr, addr, 16);
                    std::string ip = std::string(addr);
                    ret->push_back(ip);
                }
                next = next->ai_next;
            }
            if (resolver != nullptr && resolver->data != nullptr) {
                auto func = (function_wrap<std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>> *) resolver->data;
                func->function(common_callback::get(), ret);
                delete func;
                resolver->data = nullptr;
            }
        } else {
            std::string reason = std::string(uv_err_name(status));
            if (resolver != nullptr && resolver->data != nullptr) {
                auto func = (function_wrap<std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>> *) resolver->data;
                func->function(common_callback_err_wrapper::get(E_DNS_RESOLVE), nullptr);
                delete func;
                resolver->data = nullptr;
            }
        }
        if (resolver) {
            delete (resolver);
        }
        if (res) {
            delete (res);
        }
    }

    void uv_wrapper::resolve(std::string url, int port, std::function<void(std::shared_ptr<common_callback>,
            std::shared_ptr<std::vector<std::string>>)> callback) {
        if (loop == nullptr) {
            if (callback) {
                callback(common_callback::get(false, -1, "loop must be init"), nullptr);
            }
            return;
        }
        uv_getaddrinfo_t *resolver = new uv_getaddrinfo_t;
        auto func = new function_wrap<std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>>(callback);
        resolver->data = func;
        struct addrinfo hints;// = new addrinfo;
        hints.ai_family = PF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = 0;
        std::stringstream ss;
        ss << port;
        std::string p = ss.str();
        int ret = uv_getaddrinfo(loop, resolver, on_resolved, url.c_str(), p.c_str(), &hints);
        if (ret) {
            if (callback) {
                callback(common_callback::get(false, -1, "call getadrinfo fail"), nullptr);
            }
        }
    }



    static void read_callback(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
        if (handle != nullptr && handle->data != nullptr) {
            uv_content_s* content = (uv_content_s*)(handle->data);
            if (content->ssl_enable && content->ssl_impl) {
                if (nread > 0) {
                    content->append_read_bytes(nread);
                    content->ssl_impl->on_read(content->tcp_id, buf->base, nread, [=](std::shared_ptr<plan9::common_callback> ccb, std::shared_ptr<char> data, long len) mutable {
                        if (ccb->success) {
                            if (len == 0) {
                                //disconnected

                            } else if (len < 0) {
                                //ssl connected
                                if (content->ssl_connect_callback) {
                                    content->ssl_connect_callback(common_callback::get(), content->tcp_id);
                                }
                            } else {
                                if (content->read_callback) {
                                    content->read_callback(content->tcp_id, data, len, content->get_read_bytes());
                                }
                            }
                        } else {

                        }
                    });
                } else {
                    uv_wrapper::close(content->tcp_id);
                }
            } else {
                if (nread > 0) {
                    content->append_read_bytes(nread);
                    if (content->read_callback) {
                        std::shared_ptr<char> data(new char[buf->len]{});
                        memcpy(data.get(), buf->base, buf->len);
                        content->read_callback(content->tcp_id, data, nread, content->get_read_bytes());
                    }
                } else {
                    uv_wrapper::close(content->tcp_id);
                }
            }
        }
    }

    static void alloc_callback(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
//        if (handle != nullptr && handle->data != nullptr) {
//            uv_content_s* content = (uv_content_s*)handle->data;
//            static int default_size = 64 * 1024;
//            if (content->read_buf == nullptr) {
//                uv_buf_t* b = new uv_buf_t;
//                b->base = (char*)malloc(default_size);
//                b->len = default_size;
//                content->read_buf = b;
//            }
//            *buf = *(content->read_buf);
//        } else {
            *buf = uv_buf_init((char*)malloc(suggested_size), suggested_size);
//        }
    }


    static void connect_event_callback(uv_connect_t* handle, int status) {
        if (handle != nullptr && handle->data != nullptr) {
            uv_content_s* content = (uv_content_s*)handle->data;
            struct sockaddr addr;
            memset(&addr, -1, sizeof addr);
            int addr_len = sizeof addr;

            uv_tcp_getsockname(content->tcp, &addr, &addr_len);
            struct sockaddr_in *in = (sockaddr_in*)&addr;
            char ip[30];
            uv_ip4_name(in, ip, 30);
            content->local_ip = std::string(ip);
            content->local_port = ntohs(in->sin_port);
            if (handle->type == UV_CONNECT) {
                if (status >= 0) {
                    auto tcp_handle = content->tcp;
                    uv_read_start((uv_stream_t*)tcp_handle, alloc_callback, read_callback);

                    if (content->connect_callback != nullptr) {
                        content->connect_callback(common_callback::get(), content->tcp_id);
                    }

                    if (content->ssl_enable && content->ssl_impl) {
                        content->ssl_impl->on_connect(content->tcp_id, [=](std::shared_ptr<common_callback> ccb) {
                        });
                    }
                } else {
                    std::string reason = std::string(uv_err_name(status));
                    if (content->connect_callback != nullptr) {
                        content->connect_callback(common_callback::get(false, status, reason), content->tcp_id);
                    }
                }
            } else if (handle->type == UV_DISCONNECT) {
                if (content->close_callback) {
                    content->close_callback(common_callback::get(), content->tcp_id);
                }
            }
            free(handle);
        }
    }

    int uv_wrapper::connect(std::string ip, int port, std::function<void(std::shared_ptr<common_callback>, int tcp_id)> connect_callback,
            std::function<void(int, std::shared_ptr<char>, int len, unsigned long total_raw_len)> read_callback,
            std::function<void(std::shared_ptr<common_callback>, int tcp_id)> close_callback) {
        return connect(ip, port, false, "", connect_callback, nullptr, read_callback, close_callback);
    }

    int uv_wrapper::connect(std::string ip, int port, bool ssl_enable, std::string host,
            std::function<void(std::shared_ptr<common_callback>, int)> connect_callback,
            std::function<void(std::shared_ptr<common_callback>, int tcp_id)> ssl_connect_callback,
            std::function<void(int tcp_id, std::shared_ptr<char> data, int len, unsigned long total_raw_len)> read_callback,
            std::function<void(std::shared_ptr<common_callback>, int tcp_id)> close_callback) {
        int ret = connect(ip, port, ssl_enable, host);
        set_connected_callback(ret, connect_callback);
        set_ssl_connected_callback(ret, ssl_connect_callback);
        set_read_callback(ret, read_callback);
        set_disconnected_callback(ret, close_callback);
        return ret;
    }

    int uv_wrapper::connect_ssl(std::string ip, int port, std::string host,
            std::function<void(std::shared_ptr<common_callback>, int)> connect_callback,
            std::function<void(std::shared_ptr<common_callback>, int tcp_id)> ssl_connect_callback,
            std::function<void(int tcp_id, std::shared_ptr<char> data, int len, unsigned long total_raw_len)> read_callback,
            std::function<void(std::shared_ptr<common_callback>, int tcp_id)> close_callback) {
        return connect(ip, port, true, host, connect_callback, ssl_connect_callback, read_callback, close_callback);
    }

    int uv_wrapper::connect(std::string ip, int port, bool ssl_enable, std::string host) {
        if (loop == nullptr) {
            return -1;
        }

        static int count = 1;

        uv_connect_t* req = new uv_connect_t;

        struct sockaddr_in addr;// = new sockaddr_in;
        uv_ip4_addr(ip.c_str(), port, &addr);

        uv_tcp_t* tcp = new uv_tcp_t;
        uv_tcp_init(loop, tcp);
        uv_tcp_nodelay(tcp, 1);

        std::shared_ptr<uv_content_s> content = std::make_shared<uv_content_s>();
        content->tcp_id = count;
        content->tcp = tcp;
        content->ssl_enable = ssl_enable;
        content->remote_ip = ip;
        content->remote_port = port;

        if (ssl_callback) {
            content->ssl_impl = ssl_callback();
            if (host != "") {
                content->ssl_impl->set_host(host);
            }
        }

        tcp_id_2_content[count] = content;

        req->data = content.get();
        tcp->data = content.get();

        uv_tcp_connect(req, tcp, (struct sockaddr*)&addr, connect_event_callback);

        int ret = count;

        count ++;
        return ret;
    }

    void uv_wrapper::set_connected_callback(int tcp_id, std::function<void(std::shared_ptr<common_callback>, int)> callback) {
        if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
            auto content = tcp_id_2_content[tcp_id];
            content->connect_callback = callback;
        }
    }

    void uv_wrapper::set_ssl_connected_callback(int tcp_id, std::function<void(std::shared_ptr<common_callback>, int)> callback) {
        if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
            auto content = tcp_id_2_content[tcp_id];
            content->ssl_connect_callback = callback;
        }
    }

    void uv_wrapper::set_read_callback(int tcp_id, std::function<void(int tcp_id, std::shared_ptr<char> data, int len, unsigned long total_raw_len)> callback) {
        if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
            auto content = tcp_id_2_content[tcp_id];
            content->read_callback = callback;
        }
    }

    void uv_wrapper::set_disconnected_callback(int tcp_id, std::function<void(std::shared_ptr<common_callback>, int)> callback) {
        if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
            auto content = tcp_id_2_content[tcp_id];
            content->close_callback = callback;
        }
    }

    void uv_wrapper::clear_read_bytes(int tcp_id) {
        if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
            auto content = tcp_id_2_content[tcp_id];
            content->clear_read_bytes();
        }
    }

    void uv_wrapper::reconnect(int tcp_id) {
        //TODO TCP重连待实现

    }

    void uv_wrapper::close(int tcp_id) {
        if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
            auto content = tcp_id_2_content[tcp_id];
            tcp_id_2_content.erase(tcp_id);
            uv_close((uv_handle_t*)(content->tcp), nullptr);
            if (content->close_callback) {
                std::shared_ptr<common_callback> ccb;
                if (content->ssl_impl) {
                    if (content->ssl_impl->is_cert_invalidation()) {
                        ccb = common_callback_err_wrapper::get(E_SSL_VERIFY_CERT_FAIL);
                    } else if (content->ssl_impl->is_domain_invalidation()) {
                        ccb = common_callback_err_wrapper::get(E_SSL_VERIFY_HOST_FAIL);
                    } else {
                        ccb = common_callback::get();
                    }
                } else {
                    ccb = common_callback::get();
                }
                content->close_callback(ccb, tcp_id);
            }
        }
    }

    static void write_callback(uv_write_t* req, int status) {
        if (req != nullptr) {
            if (req->data != nullptr) {
                uv_content_s* content = (uv_content_s*)(req->data);
                if (content->write_callback != nullptr) {
                    std::shared_ptr<common_callback> ccb;
                    if (status >= 0) {
                        ccb.reset(new common_callback);
                    } else {
                        ccb.reset(new common_callback(false, status, uv_err_name(status)));
                    }
                    content->write_callback(ccb);
                }
            }
            free(req);
        }
    }

    void uv_wrapper::write(int tcp_id, char *data, int len, std::function<void(std::shared_ptr<common_callback>)> callback) {
        if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
            auto content = tcp_id_2_content[tcp_id];
            if (content->tcp != nullptr) {
                content->write_callback = callback;
                auto op = [=](char* d, int l){
                    if (d != nullptr && l > 0) {
                        uv_write_t* write = new uv_write_t;
                        write->data = content.get();
                        uv_buf_t buf = uv_buf_init(d, l);
                        uv_write(write, (uv_stream_t*)(content->tcp), &buf, 1, write_callback);
                    }
                };
                if (content->ssl_enable && content->ssl_impl) {
                    content->ssl_impl->write(data, len, [=](std::shared_ptr<common_callback> ccb, char* new_data, long new_len){
                        if (ccb->success && new_len > 0) {
                            op(new_data, new_len);
                        }
                    });
                } else {
                    op(data, len);
                }
            } else {
                close(tcp_id);
            }
        }
    }

    void uv_wrapper::write(int tcp_id, std::shared_ptr<char> data, int len, std::function<void(std::shared_ptr<common_callback>)> callback) {
        if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
            auto content = tcp_id_2_content[tcp_id];
            if (content->tcp != nullptr) {
                content->write_callback = callback;
                auto op = [=](char* d, int l){
                    if (d != nullptr && l > 0) {
                        uv_write_t* write = new uv_write_t;
                        write->data = content.get();
                        uv_buf_t buf = uv_buf_init(d, l);
                        uv_write(write, (uv_stream_t*)(content->tcp), &buf, 1, write_callback);
                    }
                };
                if (content->ssl_enable && content->ssl_impl) {
                    content->ssl_impl->write(data.get(), len, [=](std::shared_ptr<common_callback> ccb, char* new_data, long new_len){
                        if (ccb->success && new_len > 0) {
                            op(new_data, new_len);
                        }
                    });
                } else {
                    op(data.get(), len);
                }
            } else {
                close(tcp_id);
            }
        }
    }

    void uv_wrapper::write_uv(int tcp_id, char *data, int len, std::function<void(std::shared_ptr<common_callback>)> callback) {
        if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
            auto content = tcp_id_2_content[tcp_id];
            if (len > 0 && data != nullptr) {
                content->write_callback = callback;
                uv_write_t* write = new uv_write_t;
                write->data = content.get();
                uv_buf_t buf = uv_buf_init(data, len);
                uv_write(write, (uv_stream_t*)(content->tcp), &buf, 1, write_callback);
            }
        }
    }

    bool uv_wrapper::tcp_alive(int tcp_id) {
        if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
            auto content = tcp_id_2_content[tcp_id];
            if (uv_is_closing((uv_handle_t*)(content->tcp)) == 0) {
                return true;
            }
        }
        return false;
    }

    int uv_wrapper::get_fd(int tcp_id) {
        if (tcp_alive(tcp_id)) {
            if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
                auto content = tcp_id_2_content[tcp_id];
                uv_os_fd_t fd;
                uv_fileno((uv_handle_t*)(content->tcp), &fd);
                return fd;
            }
        }
        return -1;
    }

    std::shared_ptr<std::map<std::string, std::string>> uv_wrapper::get_info(int tcp_id) {
        std::shared_ptr<std::map<std::string, std::string>> ret = std::make_shared<std::map<std::string, std::string>>();
        if (tcp_id_2_content.find(tcp_id) != tcp_id_2_content.end()) {
            auto content = tcp_id_2_content[tcp_id];
            (*ret)["remote_ip"] = content->remote_ip;
            (*ret)["local_ip"] = content->local_ip;
            std::stringstream ss;
            ss << content->remote_port;
            (*ret)["remote_port"] = ss.str();
            std::stringstream sss;
            sss << content->local_port;
            (*ret)["local_port"] = sss.str();
            (*ret)["ssl"] = content->ssl_enable ? "1" : "0";
        }
        return ret;
    }

    bool uv_wrapper::is_ip4(std::string ip) {
        struct sockaddr_in addr;
        int ret = uv_ip4_addr(ip.c_str(), 80, &addr);
        return ret == 0;
    }

    bool uv_wrapper::is_ip6(std::string ip) {
        struct sockaddr_in6 addr;
        int ret = uv_ip6_addr(ip.c_str(), 80, &addr);
        return ret == 0;
    }
}





