//
// Created by liuke on 18/07/2017.
//

#include <curl/curl.h>
#include <uv.h>
#include <cstdlib>
#include <init/common.h>
#include <log/log_wrap.h>
#include <iostream>
#include "async_uv_http.h"

namespace plan9
{

    uv_loop_t *loop;
    CURLM *curl_handle;
    uv_timer_t timeout;

    static long verify_peer = 0L;//1L;
    static long verify_host = 0L;//2L;

    struct http_object {
    public:
        http_object() {
            cap = 0;
            len = 0;
            header_len = 0;
            data = NULL;
            downloaded = 0;
            download_total = 0;
        }

        ~http_object() {
            if (dealloc_function) {
                dealloc_function();
            }
            dealloc_function = nullptr;
        }

    public:
        CURL *curl;
        char *data;
        size_t len;
        size_t cap;
        double time;
        char header[1024];
        size_t header_len;

        std::function<void(double, long, long)> download_progress_callback;
        curl_off_t downloaded;
        curl_off_t download_total;
        curl_off_t uploaded;
        curl_off_t upload_total;
        bool is_download;
        std::shared_ptr<std::ofstream> ofstream;

        std::stringstream debug_stream;

        std::function<void(int, std::string, long, char*, size_t)> callback;
        std::function<void()> dealloc_function;
    };

    typedef struct curl_context_s {
        uv_poll_t poll_handle;
        curl_socket_t sockfd;
    } curl_context_t;

    static curl_context_t* create_curl_context(curl_socket_t sockfd)
    {
        curl_context_t *context;

        context = (curl_context_t *) malloc(sizeof *context);

        context->sockfd = sockfd;

        uv_poll_init_socket(loop, &context->poll_handle, sockfd);
        context->poll_handle.data = context;

        return context;
    }

    static void curl_close_cb(uv_handle_t *handle)
    {
        curl_context_t *context = (curl_context_t *) handle->data;
        free(context);
    }

    static void destroy_curl_context(curl_context_t *context)
    {
        uv_close((uv_handle_t *) &context->poll_handle, curl_close_cb);
    }

    static size_t write_callback(char *data, size_t n, size_t l, void *p) {
        http_object *h = (http_object *) p;
        size_t clen = n * l;
        if (h->cap == 0) {
            h->cap = clen * 2;
            h->data = (char *) malloc(h->cap);
        } else if ((h->cap - h->len) < clen) {
            size_t cap_tmp = h->cap * 2;

            while ((cap_tmp - h->len) < clen) {
                cap_tmp *= 2;
            }
            h->cap = cap_tmp;
            h->data = (char *) realloc(h->data, cap_tmp);
        }

        std::copy(data, data + clen, h->data + h->len);
        h->len += clen;
        return clen;
    }

    static void add_download(const char *url, int num)
    {
        char filename[50];
        FILE *file;
        CURL *handle;

        snprintf(filename, 50, "%d.download", num);

        file = fopen(filename, "wb");
        if(!file) {
            fprintf(stderr, "Error opening %s\n", filename);
            return;
        }

        handle = curl_easy_init();
//        curl_easy_setopt(handle, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
//        curl_easy_setopt(handle, CURLOPT_PRIVATE, file);
        curl_easy_setopt(handle, CURLOPT_URL, url);
        curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, 30 * 1000L);
        curl_multi_add_handle(curl_handle, handle);
        fprintf(stderr, "Added download %s -> %s\n", url, filename);
    }

    static int debug_callback(CURL *handle, curl_infotype type, char *data, size_t size, void *userp) {
        auto ho = (http_object*)userp;
        switch (type) {
            case CURLINFO_TEXT:
                ho->debug_stream << std::string(data, size);
                break;
            case CURLINFO_HEADER_OUT:
                ho->debug_stream << std::string(data, size);
                break;
            case CURLINFO_DATA_OUT:
                break;
            case CURLINFO_SSL_DATA_OUT:
                break;
            case CURLINFO_HEADER_IN:
                ho->debug_stream << std::string(data, size);
                break;
            case CURLINFO_DATA_IN:
                break;
            case CURLINFO_SSL_DATA_IN:
                break;
            case CURLINFO_END:
                ho->debug_stream << std::string(data, size);
                break;
            default:
                ho->debug_stream << std::string(data, size);
                break;
        }
        return 0;
    }

    static void check_multi_info(void)
    {
        char *done_url;
        CURLMsg *message;
        int pending;
        CURL *easy_handle;

        while((message = curl_multi_info_read(curl_handle, &pending))) {
            if (message->msg == CURLMSG_DONE) {
                easy_handle = message->easy_handle;

                CURLcode res = message->data.result;
                curl_easy_getinfo(easy_handle, CURLINFO_EFFECTIVE_URL, &done_url);
                http_object* http = nullptr;
                curl_easy_getinfo(easy_handle, CURLINFO_PRIVATE, &http);
                printf("%s DONE\n", done_url);

                curl_multi_remove_handle(curl_handle, easy_handle);
                curl_easy_cleanup(easy_handle);
                //释放
                if (http) {
                    if (http->callback) {
                        long response_code;
                        curl_easy_getinfo(easy_handle, CURLINFO_RESPONSE_CODE, &response_code);
                        http->callback(res, http->debug_stream.str(), response_code, http->data, http->len);
                    }
                }
            }
        }
    }

    static void curl_perform(uv_poll_t *req, int status, int events)
    {
        int running_handles;
        int flags = 0;
        curl_context_t *context;

        if(events & UV_READABLE)
            flags |= CURL_CSELECT_IN;
        if(events & UV_WRITABLE)
            flags |= CURL_CSELECT_OUT;

        context = (curl_context_t *) req->data;

        curl_multi_socket_action(curl_handle, context->sockfd, flags,
                                 &running_handles);

        check_multi_info();
    }

    static void on_timeout(uv_timer_t *req)
    {
        int running_handles;
        curl_multi_socket_action(curl_handle, CURL_SOCKET_TIMEOUT, 0,
                                 &running_handles);
        check_multi_info();
    }

    static int start_timeout(CURLM *multi, long timeout_ms, void *userp)
    {
        if(timeout_ms < 0) {
            uv_timer_stop(&timeout);
        }
        else {
            if(timeout_ms == 0)
                timeout_ms = 1; /* 0 means directly call socket_action, but we'll do it
                         in a bit */
            uv_timer_start(&timeout, on_timeout, timeout_ms, 0);
        }
        return 0;
    }

    static int handle_socket(CURL *easy, curl_socket_t s, int action, void *userp,
                             void *socketp)
    {
        curl_context_t *curl_context;
        int events = 0;

        switch(action) {
            case CURL_POLL_IN:
            case CURL_POLL_OUT:
            case CURL_POLL_INOUT:
                curl_context = socketp ?
                               (curl_context_t *) socketp : create_curl_context(s);

                curl_multi_assign(curl_handle, s, (void *) curl_context);

                if(action != CURL_POLL_IN)
                    events |= UV_WRITABLE;
                if(action != CURL_POLL_OUT)
                    events |= UV_READABLE;

                uv_poll_start(&curl_context->poll_handle, events, curl_perform);
                break;
            case CURL_POLL_REMOVE:
                if(socketp) {
                    uv_poll_stop(&((curl_context_t*)socketp)->poll_handle);
                    destroy_curl_context((curl_context_t*) socketp);
                    curl_multi_assign(curl_handle, s, NULL);
                }
                break;
            default:
                abort();
        }

        return 0;
    }



    class async_uv_http::async_uv_http_impl {
    public:
        void get(std::string url, long timeout_second, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback) {
//            loop = uv_default_loop();
//            if(curl_global_init(CURL_GLOBAL_ALL)) {
//                fprintf(stderr, "Could not init curl\n");
//                return;
//            }
//
//            uv_timer_init(loop, &timeout);
//
//            curl_handle = curl_multi_init();
//            curl_multi_setopt(curl_handle, CURLMOPT_SOCKETFUNCTION, &handle_socket);
//            curl_multi_setopt(curl_handle, CURLMOPT_TIMERFUNCTION, &start_timeout);
//
//            add_download(url.c_str(), 1);
//
//            uv_run(loop, UV_RUN_DEFAULT);
//            curl_multi_cleanup(curl_handle);


            init();

            CURL *curl = create_get_easy_curl(url, timeout_second, header, callback);

            curl_handle = curl_multi_init();
            curl_multi_setopt(curl_handle, CURLMOPT_SOCKETFUNCTION, &handle_socket);
            curl_multi_setopt(curl_handle, CURLMOPT_TIMERFUNCTION, &start_timeout);
            curl_multi_add_handle(curl_handle, curl);

            run();
            curl_multi_cleanup(curl_handle);
        }


    private:

        void init() {
            if (!loop) {
                loop = uv_default_loop();
                uv_timer_init(loop, &timeout);
            }
        }

        void run() {
//            if (!uv_loop_alive(loop)) {
                uv_run(loop, UV_RUN_DEFAULT);
//            }
        }

        CURL *create_get_easy_curl(std::string url, long timeout_second, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback) {
            CURL *curl = curl_easy_init();
            if (!curl) {
                log_wrap::net().e("can not create easy curl");
                return NULL;
            }

            http_object* ho = new http_object;
            ho->callback = callback;
            ho->curl = curl;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, ho);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_PRIVATE, ho);
//            curl_easy_setopt(curl, CURLOPT_OPENSOCKETFUNCTION, open_socket);
//            curl_easy_setopt(curl, CURLOPT_CLOSESOCKETFUNCTION, close_socket);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

            curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, &debug_callback);
            curl_easy_setopt(curl, CURLOPT_DEBUGDATA, ho);
            if (timeout_second <= 0) {
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0);
            } else {
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_second);
            }

            struct curl_slist* list = NULL;
            if (header != nullptr) {
                std::map<std::string, std::string>::const_iterator iterator = header->begin();
                while (iterator != header->end()) {
                    list = curl_slist_append(list, (iterator->first + ":" + iterator->second).c_str());
                    iterator ++;
                }

                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
            }
            ho->dealloc_function = [=](){
                if (list) {
                    curl_slist_free_all(list);
                }
            };
            return curl;
        }

    private:
//        CURLM *curl_handle;
//        uv_timer_t timeout;
    };


    async_uv_http async_uv_http::instance() {
        static async_uv_http inst;
        return inst;
    }

    async_uv_http::async_uv_http() : impl_(new async_uv_http_impl){

    }

    void async_uv_http::get(std::string url, long timeout_second, std::shared_ptr<std::map<std::string, std::string>> header,
                            std::function<void(int curl_code, std::string debug_trace, long http_state, char *data,
                                               size_t len)> callback) {
        impl_->get(url, timeout_second, header, callback);
    }
}
