//
// Created by guazi on 8/4/16.
//

#include "http.h"
#include "curl/curl.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

namespace plan9 {
    class http_object {
    public:
        CURL *curl;
        char *data;
        size_t len;
        size_t cap;
        double time;
        char header[1024];
        size_t header_len;

        std::function<void(long, long)> download_progress_callback;
        curl_off_t downloaded;
        curl_off_t download_total;
        std::shared_ptr<std::ofstream> ofstream;

        std::stringstream debug_stream;
    };

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

    static size_t header_callback(char *data, size_t n, size_t l, void *p) {
        http_object *h = (http_object *) p;
        size_t clen = n * l;
        std::copy(data, data + clen, h->header + h->header_len);
        h->header_len += clen;
        return clen;
    }

    static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
    {
        if(size*nmemb < 1)
            return 0;
        return 0;
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
                break;
            default:
                break;
        }
        return 0;
    }

    static size_t download_callback(char *data, size_t n, size_t l, void *p) {
        http_object* h = (http_object *) p;
        size_t clen = n * l;

        h->ofstream->write(data, clen);

        return clen;
    }

    static size_t progress_callback(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
        http_object *ho = (http_object *) p;

        double curtime = 0;
        curl_easy_getinfo(ho->curl, CURLINFO_TOTAL_TIME, &curtime);
        ho->time = curtime;

        if (ho->downloaded != dlnow && dltotal != 0) {
            ho->downloaded = dlnow;
            ho->download_total = dltotal;
            ho->download_progress_callback(dlnow, dltotal);
        }

        return (dltotal != 0 && dltotal == dlnow) ? 1 : 0;
    }


    class http::http_impl {
    public:
        http_impl() {
            curl_global_init(CURL_GLOBAL_ALL);
//        curlm = curl_multi_init();
//        curl_multi_setopt(curlm, CURLMOPT_MAXCONNECTS, 2L);
        }

        ~http_impl() {
//        curl_multi_cleanup(curlm);
            curl_global_cleanup();
        }

        void get(std::string url, std::map<std::string, std::string>* header, std::function<void(std::string header, char *, size_t)> callback) {
            http_object ho;
            CURL *curl = curl_easy_init();
            ho.curl = curl;
            ho.cap = 0;
            ho.len = 0;
            ho.header_len = 0;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ho);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &ho);

            curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, &debug_callback);
            curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &ho);

            struct curl_slist* list = NULL;
            if (header != nullptr) {
                std::map<std::string, std::string>::const_iterator iterator = header->begin();
                while (iterator != header->end()) {
                    list = curl_slist_append(list, (iterator->first + ":" + iterator->second).c_str());
                    iterator ++;
                }

                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
            }

            if (curl_easy_perform(curl) == CURLE_OK) {
                if (callback != nullptr) {
                    callback(std::string(ho.header, ho.header_len), ho.data, ho.len);
                }
            } else {
                if (callback != nullptr) {
                    callback(std::string(ho.header, ho.header_len), (char *) 0, -1);
                }
            }

            curl_easy_cleanup(curl);
            if (list != NULL) {
                curl_slist_free_all(list);
            }

            std::cout << ho.debug_stream.str() << std::endl;
        }

        void download(std::string url, std::string path, std::function<void(long downloaded, long total)> callback) {
            http_object ho;
            ho.len = 0;
            ho.cap = 0;
            ho.download_progress_callback = callback;
            ho.downloaded = 0;
            ho.download_total = 0;
            ho.ofstream.reset(new std::ofstream);
            ho.ofstream->open(path, std::ios::trunc);

            CURL *curl = curl_easy_init();
            ho.curl = curl;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ho);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &ho);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

            curl_easy_perform(curl);

            curl_easy_cleanup(curl);
            ho.ofstream->close();
        }



        void post(std::string url, std::map<std::string, std::string>* header, std::map<std::string, std::string>* params,
                        std::function<void(std::string header, char *data, size_t len)> callback) {
            CURL *curl = curl_easy_init();
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            struct curl_slist* list = NULL;
            if (header != nullptr) {
                std::map<std::string, std::string>::const_iterator iterator = header->begin();
                while (iterator != header->end()) {
                    list = curl_slist_append(list, (iterator->first + ":" + iterator->second).c_str());
                    iterator ++;
                }
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
            }

            if (params != nullptr) {
                struct curl_httppost *formpost=NULL;
                struct curl_httppost *lastptr=NULL;

                std::map<std::string, std::string>::const_iterator iterator = params->begin();
                while (iterator != params->end()) {
                    curl_formadd(&formpost,
                                 &lastptr,
                                 CURLFORM_COPYNAME, iterator->first.c_str(),
                                 CURLFORM_COPYCONTENTS, iterator->second.c_str(),
                                 CURLFORM_END);
                    iterator ++;
                }
                curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
            }
            CURLcode ret = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            if (list != NULL) {
                curl_slist_free_all(list);
            }
        }

        void resolve(std::string url) {
//            curl_slist* host = curl_slist_append(NULL, "example.com:80:127.0.0.1");
            CURL *curl = curl_easy_init();
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//            curl_easy_setopt(curl, CURLOPT_RESOLVE, host);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, &read_callback);
            curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, &debug_callback);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
//            curl_easy_setopt(curl, CURLOPT_SOCKOPTFUNCTION, &sockopt_callback);
//            curl_easy_setopt(curl, CURLOPT_OPENSOCKETFUNCTION, &opensocket_callback);
            CURLcode ret = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
        }

    private:
        CURLM *curlm;
    };

    http::http() : impl_(new http_impl) {

    }

    http http::instance() {
        static http h;
        return h;
    }

    void http::get(std::string url, std::function<void(std::string header, char *, size_t)> callback) {
        impl_->get(url, nullptr, callback);
    }

    void http::download(std::string url, std::string path, std::function<void(long downloaded, long total)> callback) {
        impl_->download(url, path, callback);
    }

    void http::get(std::string url, std::map<std::string, std::string> *header,
                   std::function<void(std::string header, char *data, size_t len)> callback) {
        impl_->get(url, header, callback);
    }

    void http::post(std::string url, std::map<std::string, std::string> *header, std::map<std::string, std::string> *params,
               std::function<void(std::string header, char *data, size_t len)> callback) {
        impl_->post(url, header, params, callback);
    }
}
