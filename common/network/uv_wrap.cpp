//
// Created by liuke on 15/11/11.
//

#include <stdlib.h>
#include <assert.h>
#include "uv_wrap.h"
#include "uv.h"
#include "../log/log_wrap.h"


namespace plan9
{

    static void on_connect(uv_connect_t* req, int status);
    static void write_cb(uv_write_t *req, int status);

    class uv_wrap::uv_wrap_impl
    {
    public:

        uv_wrap_impl() {
            loop = uv_default_loop();
            client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
            client->data = this;
            uv_tcp_init(loop, client);
            connect_t = (uv_connect_t*)malloc(sizeof(uv_connect_t));
            connect_t->data = this;
            memset(buf, 0 , 10000);
            len = 0;
            write = (uv_write_t*)malloc(sizeof(uv_write_t));
//            uv_run(loop, UV_RUN_DEFAULT);
        }

        ~uv_wrap_impl() {
            free(client);
            free(connect_t);
            free(buf);
        }

        void connect(std::string ip, int port) {
            this->ip = ip;
            this->port = port;
            uv_ip4_addr(ip.c_str(), port, &dest);
            uv_tcp_connect(connect_t, client, (sockaddr*)&dest, on_connect);
            log_wrap::logI("connecting...");
        }
        void close() {

        }
        void re_connect() {

        }
        void send(const char* data, size_t len) {
            write_buf = uv_buf_init(buf, len);
            memcpy(write_buf.base, data, len);
            write->bufs = &write_buf;

            uv_write(write, (uv_stream_t*)client, &write_buf, 1, NULL);
            log_wrap::logI("send: %s", data);
        }
        void deal_read(const uv_buf_t* buf, int nread) {
            log_wrap::logI("recv: %s", buf->base);
            memcpy(read_buf + len, buf->base, nread);
            len += nread;
        }

        void deal_connect() {
            log_wrap::logI("connected ");
        }

        void deal_disconnect() {
            log_wrap::logI("connect fail");
        }

        uv_loop_t* getloop() {
            return loop;
        }

        uv_tcp_t* gettcp() {
            return client;
        }

    private:
        uv_loop_t* loop;
        uv_tcp_t* client;
        uv_connect_t* connect_t;
        struct sockaddr_in dest;
        uv_write_t* write;
        uv_buf_t write_buf;
        std::string ip;
        int port;

        char buf[10000];
        char read_buf[10000];
        int len;
    };

    static void write_cb(uv_write_t *req, int status) {
        if (status == 0) {
        }
    }

    static void read_cb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
        if (nread == UV_EOF) {
            uv_close((uv_handle_t*) client, NULL);
        } else if (nread > 0) {
            uv_wrap::uv_wrap_impl* impl = (uv_wrap::uv_wrap_impl*)(client->data);
            impl->deal_read(buf, nread);
        }
        if (nread == 0) free(buf->base);
    }

    static void alloc_cb(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
        *buf = uv_buf_init((char*) malloc(size), size);
        assert(buf->base != NULL);
    }

    static void on_connect(uv_connect_t* req, int status) {
        uv_wrap::uv_wrap_impl* impl = (uv_wrap::uv_wrap_impl*)(req->data);
        if (status == 0) {
            uv_read_start((uv_stream_t *) (impl->gettcp()), alloc_cb, read_cb);
            impl->deal_connect();
        } else {
            impl->deal_disconnect();
        }
    }


    uv_wrap::uv_wrap() : impl_(new uv_wrap_impl){

    }

    uv_wrap::~uv_wrap() {

    }

    void uv_wrap::connect(std::string ip, int port) {
        impl_->connect(ip, port);
    }

    void uv_wrap::send(const char *data, size_t len) {
        impl_->send(data, len);
    }

    void uv_wrap::deal_read(std::function<void(std::string)> function) {
        read_func = function;
    }

    uv_loop_t* uv_wrap::getloop() {
        return impl_->getloop();
    }

}