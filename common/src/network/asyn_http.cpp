//
// Created by liuke on 8/23/16.
//

#include "asyn_http.h"
#include <boost/asio.hpp>
#include <curl/multi.h>
#include <boost/bind.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include "log/log_wrap.h"

namespace plan9
{

    static boost::asio::io_service service;
    struct socket_info {
        socket_info(boost::asio::io_service &io) : socket(io), mask(0){
        }
        boost::asio::ip::tcp::socket socket;
        int mask;
    };
    static boost::asio::deadline_timer timer(service);
    static std::map<curl_socket_t, std::shared_ptr<socket_info>> socket_map;
    struct asyn_http_object {
    public:
        CURLM *curlm;
        int still_running;
    };

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
        std::shared_ptr<std::ofstream> ofstream;

        std::stringstream debug_stream;

        std::function<void(int, std::string, long, char*, size_t)> callback;
        std::function<void()> dealloc_function;
    };
    static void set_socket(std::shared_ptr<socket_info> tcp_socket, curl_socket_t curl_socket, CURL *curl, int action, void* data);

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

    static size_t download_callback(char *data, size_t n, size_t l, void *p) {
        http_object* h = (http_object *) p;
        size_t clen = n * l;

        h->ofstream->write(data, clen);

        return clen;
    }

    static size_t progress_callback(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
        http_object *ho = (http_object *) p;

        double current_time = 0;
        curl_easy_getinfo(ho->curl, CURLINFO_TOTAL_TIME, &current_time);
        ho->time = current_time;

        if (ho->downloaded != dlnow && dltotal != 0) {
            ho->downloaded = dlnow;
            ho->download_total = dltotal;
            if (ho->download_progress_callback != nullptr) {
                ho->download_progress_callback(current_time, dlnow, dltotal);
            }
        }

        return 0;
    }

    static void check_multi_curl(void* data) {
        asyn_http_object* aho = (asyn_http_object*)data;
        char *eff_url;
        CURLMsg *msg;
        int msgs_left;
        http_object *conn;
        CURL *easy;
        CURLcode res;

        while((msg = curl_multi_info_read(aho->curlm, &msgs_left))) {
            if(msg->msg == CURLMSG_DONE) {
                easy = msg->easy_handle;
                res = msg->data.result;
                curl_easy_getinfo(easy, CURLINFO_PRIVATE, &conn);
                curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &eff_url);
                curl_multi_remove_handle(aho->curlm, easy);
                curl_easy_cleanup(easy);
                if (conn->callback) {
                    long response_code;
                    curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &response_code);
                    conn->callback(res, conn->debug_stream.str(), response_code, conn->data, conn->len);
                }
                delete conn;
            }
        }
    }

    static void event_callback(void* data, std::shared_ptr<socket_info> tcp_socket, curl_socket_t curl_socket, CURL* curl, int action, const boost::system::error_code &err) {
        asyn_http_object* aho = (asyn_http_object*)data;

        if (err) {
            log_wrap::net().e("function : ", __FUNCTION__, ", socket : ", curl_socket, ", action : ", action, " error : ", err.message());
            CURLMcode code = curl_multi_socket_action(aho->curlm, tcp_socket->socket.native_handle(), CURL_CSELECT_ERR, &aho->still_running);
        } else {
            CURLMcode code = curl_multi_socket_action(aho->curlm, tcp_socket->socket.native_handle(), action, &aho->still_running);
        }

        check_multi_curl(data);

        if (aho->still_running <= 0) {
            timer.cancel();
        } else {
            int action_continue = (tcp_socket->mask) & action;
            if (action_continue) {
                set_socket(tcp_socket, curl_socket, curl, action_continue, data);
            }
        }
    }

    static void set_socket(std::shared_ptr<socket_info> tcp_socket, curl_socket_t curl_socket, CURL *curl, int action, void* data) {


        if (action == CURL_POLL_IN) {
            //socket readable
            tcp_socket->socket.async_read_some(boost::asio::null_buffers(),
                                        boost::bind(&event_callback, data, tcp_socket, curl_socket, curl, action, _1));
        } else if (action == CURL_POLL_OUT) {
            //socket writeable
            tcp_socket->socket.async_write_some(boost::asio::null_buffers(),
                                         boost::bind(&event_callback, data, tcp_socket, curl_socket, curl, action, _1));
        } else if (action == CURL_POLL_INOUT) {
            //socket readable and writeable
            tcp_socket->socket.async_read_some(boost::asio::null_buffers(),
                                               boost::bind(&event_callback, data, tcp_socket, curl_socket, curl, action, _1));

            tcp_socket->socket.async_write_some(boost::asio::null_buffers(),
                                                boost::bind(&event_callback, data, tcp_socket, curl_socket, curl, action, _1));
        }
    }

    static int socket_callback(CURL *e, curl_socket_t s, int what, void *data, void *socket_ptr) {
        asyn_http_object* aho = (asyn_http_object*)data;
        int* action_ptr = (int*)socket_ptr;
        auto it = socket_map.find(s);
        if (it == socket_map.end()) {
            if (action_ptr) {
                log_wrap::net().e("function : ", __FUNCTION__, " socket closed already before remove CURL_POLL_REMOVE event, may be bug? socket id : ", s);
                return 0;
            } else {
                log_wrap::net().e("we don't know how to create asio::ip::tcp::socket without this fd's protocol family, please recompiled libcurl without c-ares");
                return 0;
            }
        }

        std::shared_ptr<socket_info> &tcp_socket = it->second;
        if (!action_ptr) {
            action_ptr = &(tcp_socket->mask);
            curl_multi_assign(aho->curlm, s, action_ptr);
        }

        if (what == CURL_POLL_REMOVE) {
        } else {
            //set socket
            set_socket(tcp_socket, s, e, what, data);
        }
        *action_ptr = what;
        return 0;
    }

    static void asio_timer_callback(const boost::system::error_code& error, void* data) {
        if (!error) {
            asyn_http_object* aho = (asyn_http_object*)data;
            CURLMcode code = curl_multi_socket_action(aho->curlm, CURL_SOCKET_TIMEOUT, 0, &aho->still_running);

            check_multi_curl(data);
        }
    }

    static int timer_callback(CURLM *multi, long timeout_ms, void *data) {
        timer.cancel();

        if (timeout_ms > 0) {
            timer.expires_from_now(boost::posix_time::millisec(timeout_ms));
            timer.async_wait(boost::bind(&asio_timer_callback, _1, data));
        } else {
            boost::system::error_code error;
            asio_timer_callback(error, data);
        }


        return 0;
    }

    static curl_socket_t open_socket(void *clientp, curlsocktype purpose, struct curl_sockaddr *address) {
        curl_socket_t sockfd = CURL_SOCKET_BAD;

        //创建socket
        if(purpose == CURLSOCKTYPE_IPCXN && address->family == AF_INET) {
            socket_info* tcp_socket = new socket_info(service);
            boost::system::error_code ec;
            if (address->family == AF_INET) {
                /* create a tcp socket object */
                tcp_socket->socket.open(boost::asio::ip::tcp::v4(), ec);
            } else if (address->family == AF_INET6) {
                tcp_socket->socket.open(boost::asio::ip::tcp::v6(), ec);
            }
            if(ec) {
                /* An error occurred */
                log_wrap::net().e("ERROR: Returning CURL_SOCKET_BAD to signal error. Couldn't open socket [", ec, "][", ec.message(), "]");
            } else {
                sockfd = tcp_socket->socket.native_handle();
                /* save it for monitoring */
                socket_map.insert(std::pair<curl_socket_t, std::shared_ptr<socket_info>>(sockfd, std::shared_ptr<socket_info>(tcp_socket)));
            }

        }

        return sockfd;
    }

    static int close_socket(void *clientp, curl_socket_t item) {
        auto it = socket_map.find(item);
        if(it != socket_map.end()) {
            it->second->mask = 0;
            it->second->socket.cancel();
            socket_map.erase(it);
        }
        return 0;
    }

    class asyn_http::asyn_http_impl {

    public:

        asyn_http_impl() : multi_curl(NULL) {
        }
        ~asyn_http_impl() {
        }

        void get(std::string url, long timeout_second, std::map<std::string, std::string>* header, std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback) {
            init();

            CURL *curl = create_get_easy_curl(url, timeout_second, header, callback);
            if (curl) {
                CURLMcode code = curl_multi_add_handle(multi_curl, curl);
                run();
            }
        }

        void download(std::string url, std::string path, long timeout_second, std::function<void(int curl_code, std::string debug_trace, long http_state)> callback, std::function<void(double time, long downloaded, long total)> process_callback) {
            init();

            CURL *curl = create_download_easy_curl(url, path, timeout_second, callback, process_callback);
            if (curl) {
                CURLMcode code = curl_multi_add_handle(multi_curl, curl);

                run();
            }
        }

        void post(std::string url, long timeout_second, std::map<std::string, std::string>* header, std::map<std::string, std::string>* form_params,
                  std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback) {
            init();

            CURL *curl = create_post_easy_crul(url, timeout_second, header, form_params, callback);
            if (curl) {
                CURLMcode code = curl_multi_add_handle(multi_curl, curl);

                run();
            }
        }

        CURL* create_post_easy_crul(std::string url, long timeout_second, std::map<std::string, std::string>* header, std::map<std::string, std::string>* form_params,
                  std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback) {
            http_object* ho = new http_object;

            CURL *curl = curl_easy_init();
            ho->curl = curl;
            if (!curl) {
                log_wrap::net().e("can not create easy curl");
                return NULL;
            }

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);
            curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &ho);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, ho);

            curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_second);
            curl_easy_setopt(curl, CURLOPT_PRIVATE, ho);
            curl_easy_setopt(curl, CURLOPT_OPENSOCKETFUNCTION, open_socket);
            curl_easy_setopt(curl, CURLOPT_CLOSESOCKETFUNCTION, close_socket);

            struct curl_slist* list = NULL;

            if (header != nullptr) {
                std::map<std::string, std::string>::const_iterator iterator = header->begin();
                while (iterator != header->end()) {
                    list = curl_slist_append(list, (iterator->first + ":" + iterator->second).c_str());
                    iterator ++;
                }
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
            }


            struct curl_httppost *form_post = NULL;
            struct curl_httppost *last_ptr = NULL;
            if (form_params != nullptr) {
                std::map<std::string, std::string>::const_iterator iterator = form_params->begin();
                while (iterator != form_params->end()) {
                    curl_formadd(&form_post,
                                 &last_ptr,
                                 CURLFORM_COPYNAME, iterator->first.c_str(),
                                 CURLFORM_COPYCONTENTS, iterator->second.c_str(),
                                 CURLFORM_END);
                    iterator ++;
                }
                curl_easy_setopt(curl, CURLOPT_HTTPPOST, form_post);
            }

            ho->dealloc_function = [=](){
                if (list) {
                    curl_slist_free_all(list);
                }
                if (form_post) {
                    curl_formfree(form_post);
                }
            };
            return curl;
        }

        CURL* create_download_easy_curl(std::string url, std::string path, long timeout_second,
                                        std::function<void(int curl_code, std::string debug_trace,
                                                           long http_state)> callback,
                                        std::function<void(double time, long downloaded, long total)> process_callback) {
            http_object* ho = new http_object;
            ho->download_progress_callback = process_callback;
            ho->ofstream.reset(new std::ofstream);
            ho->ofstream->open(path, std::ios::trunc);

            CURL *curl = curl_easy_init();
            if (!curl) {
                log_wrap::net().e("can not create easy curl");
                return NULL;
            }
            ho->curl = curl;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, ho);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, ho);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, debug_callback);
            curl_easy_setopt(curl, CURLOPT_DEBUGDATA, ho);

            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_second);

            curl_easy_setopt(curl, CURLOPT_PRIVATE, ho);
            curl_easy_setopt(curl, CURLOPT_OPENSOCKETFUNCTION, open_socket);
            curl_easy_setopt(curl, CURLOPT_CLOSESOCKETFUNCTION, close_socket);

            return curl;
        }

        CURL *create_get_easy_curl(std::string url, long timeout_second, std::map<std::string, std::string>* header, std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback) {
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
            curl_easy_setopt(curl, CURLOPT_OPENSOCKETFUNCTION, open_socket);
            curl_easy_setopt(curl, CURLOPT_CLOSESOCKETFUNCTION, close_socket);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, &debug_callback);
            curl_easy_setopt(curl, CURLOPT_DEBUGDATA, ho);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_second);

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

        void run() {
            if (service.stopped()) {
                service.reset();
            }

            service.run();
            curl_multi_cleanup(multi_curl);
            multi_curl = NULL;
        }

    private:
        void init() {
            if (multi_curl == NULL) {
                multi_curl = curl_multi_init();
                curl_multi_setopt(multi_curl, CURLMOPT_SOCKETFUNCTION, &socket_callback);
                curl_multi_setopt(multi_curl, CURLMOPT_SOCKETDATA, &aho);
                curl_multi_setopt(multi_curl, CURLMOPT_TIMERFUNCTION, &timer_callback);
                curl_multi_setopt(multi_curl, CURLMOPT_TIMERDATA, &aho);
                aho.curlm = multi_curl;
            }
        }





        CURLM *multi_curl;
        asyn_http_object aho;
    };

    asyn_http asyn_http::instance() {
        static asyn_http ah;
        return ah;
    }

    asyn_http::asyn_http() : impl_ (new asyn_http_impl){

    }

    void asyn_http::download(std::string url, std::string path, long timeout_second,
                             std::function<void(int curl_code, std::string debug_trace, long http_state)> callback,
                             std::function<void(double time, long downloaded, long total)> process_callback) {
        impl_->download(url, path, timeout_second, callback, process_callback);
    }

    void asyn_http::post(std::string url, long timeout_second, std::map<std::string, std::string> *header,
                         std::map<std::string, std::string> *form_params,
                         std::function<void(int curl_code, std::string debug_trace, long http_state, char *data,
                                            size_t len)> callback) {
        impl_->post(url, timeout_second, header, form_params, callback);
    }

    void asyn_http::get(std::string url, long timeout_second, std::map<std::string, std::string> *header,
                        std::function<void(int curl_code, std::string debug_trace, long http_state, char *data,
                                           size_t len)> callback) {
        impl_->get(url, timeout_second, header, callback);
    }

    void asyn_http::get(std::string url, long timeout_second,
                        std::function<void(int curl_code, std::string debug_trace, long http_state, char *data,
                                           size_t len)> callback) {
        get(url, timeout_second, nullptr, callback);
    }

    bool asyn_http::is_timeout(int curl_code) {
        return curl_code == CURLE_OPERATION_TIMEDOUT;
    }

    bool asyn_http::is_ok(int curl_code) {
        return curl_code == CURLE_OK;
    }


}
