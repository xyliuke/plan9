//
// Created by guazi on 8/4/16.
//

#include "easy_http.h"
#include "curl/curl.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <sys/stat.h>

namespace plan9 {

    static float download_interval = 1;//下载文件通知时间间隔，默认为1秒
    static float upload_interval = 1;//下载文件通知时间间隔，默认为1秒

    class http_object_easy {

    public:
        http_object_easy() {
            cap = 0;
            len = 0;
            header_len = 0;
            data = NULL;
            downloaded = 0;
            download_total = 0;
            uploaded = 0;
            upload_total = 0;
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
        bool is_download;//true为download;false为upload
        std::shared_ptr<std::ofstream> ofstream;

        std::stringstream debug_stream;
    };

    static size_t write_callback(char *data, size_t n, size_t l, void *p) {
        http_object_easy *h = (http_object_easy *) p;
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
        http_object_easy *h = (http_object_easy *) p;
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
        auto ho = (http_object_easy*)userp;
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
        http_object_easy* h = (http_object_easy *) p;
        size_t clen = n * l;

        h->ofstream->write(data, clen);

        return clen;
    }

    static size_t progress_callback(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
        http_object_easy *ho = (http_object_easy *) p;
        if (ho->download_progress_callback != nullptr) {
            double current_time = 0;
            curl_easy_getinfo(ho->curl, CURLINFO_TOTAL_TIME, &current_time);

            if (ho->is_download) {
                if (ho->downloaded != dlnow && dltotal != 0 && (((int)current_time - (int)ho->time) >= download_interval || dltotal == dlnow)) {
                    ho->time = current_time;
                    ho->downloaded = dlnow;
                    ho->download_total = dltotal;
                    ho->download_progress_callback(current_time, dlnow, dltotal);
                }
            } else {
                if (ho->uploaded != ulnow && ultotal != 0 && (((int)current_time - (int)ho->time) >= download_interval || ultotal == ulnow)) {
                    ho->time = current_time;
                    ho->uploaded = ulnow;
                    ho->upload_total = ultotal;
                    ho->download_progress_callback(current_time, ulnow, ultotal);
                }
            }
        }
        return 0;
    }


    class easy_http::easy_http_impl {
    public:
        easy_http_impl() {
            curl_global_init(CURL_GLOBAL_ALL);
        }

        ~easy_http_impl() {
            curl_global_cleanup();
        }

        void get(std::string url, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(int curl_code, std::string debug_trace, long http_state, char *, size_t)> callback) {
            http_object_easy ho;
            CURL *curl = curl_easy_init();
            ho.curl = curl;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ho);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

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

            CURLcode ret = curl_easy_perform(curl);
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (callback != nullptr) {
                callback(ret, ho.debug_stream.str(), response_code, ho.data, ho.len);
            }

            curl_easy_cleanup(curl);
            if (list != NULL) {
                curl_slist_free_all(list);
            }
        }

        void download(std::string url, std::string path, std::shared_ptr<std::map<std::string, std::string>> header, bool override, std::function<void(int curl_code, std::string debug_trace, long http_state)> callback, std::function<void(double time, long downloaded, long total)> process_callback) {
            http_object_easy ho;
            ho.is_download = true;
            ho.download_progress_callback = process_callback;
            ho.ofstream.reset(new std::ofstream);

            CURL *curl = curl_easy_init();
            ho.curl = curl;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ho);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &ho);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, &debug_callback);
            curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &ho);

            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            if (override) {
                ho.ofstream->open(path, std::ios::trunc);
            } else {
                ho.ofstream->open(path, std::ios::app);
                boost::filesystem::path p(path);
                if (boost::filesystem::exists(p) && boost::filesystem::is_regular_file(p)) {
                    boost::uintmax_t size = boost::filesystem::file_size(p);
                    if (size > 0) {
                        std::stringstream ss;
                        ss << size;
                        ss << "-";
                        curl_easy_setopt(curl, CURLOPT_RANGE, ss.str().c_str());
                    }
                }
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

            CURLcode ret = curl_easy_perform(curl);
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            if (callback != nullptr) {
                callback(ret, ho.debug_stream.str(), response_code);
            }

            curl_easy_cleanup(curl);
            ho.ofstream->close();
        }



        void post(std::string url, std::shared_ptr<std::map<std::string, std::string>> header, std::shared_ptr<std::map<std::string, std::string>> form_params,
                        std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback) {
            http_object_easy ho;

            CURL *curl = curl_easy_init();
            ho.curl = curl;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, &debug_callback);
            curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &ho);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ho);

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

            CURLcode ret = curl_easy_perform(curl);
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (callback != nullptr) {
                callback(ret, ho.debug_stream.str(), response_code, ho.data, ho.len);
            }
            curl_easy_cleanup(curl);
            if (list != NULL) {
                curl_slist_free_all(list);
            }
        }

        void upload(std::string url, std::string path, std::string file_key, std::shared_ptr<std::map<std::string, std::string>> header,
                               std::shared_ptr<std::map<std::string, std::string>> form_params,
                               std::function<void(int curl_code, std::string debug_trace, long http_state, char* data, size_t data_len)> callback,
                               std::function<void(double time, long uploaded, long total)> process_callback) {

            http_object_easy ho;
            ho.is_download = false;
            ho.download_progress_callback = process_callback;

            CURL *curl = curl_easy_init();
            ho.curl = curl;

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, &debug_callback);
            curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &ho);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ho);
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &ho);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

            struct curl_slist *list = NULL;

            if (header != nullptr) {
                std::map<std::string, std::string>::const_iterator iterator = header->begin();
                while (iterator != header->end()) {
                    list = curl_slist_append(list, (iterator->first + ":" + iterator->second).c_str());
                    iterator++;
                }
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
            }

            struct curl_httppost *form_post = NULL;
            struct curl_httppost *last_ptr = NULL;
            curl_formadd(&form_post,
                         &last_ptr,
                         CURLFORM_COPYNAME, "reqformat",
                         CURLFORM_COPYCONTENTS, "plain",
                         CURLFORM_END);
            curl_formadd(&form_post, &last_ptr, CURLFORM_COPYNAME, file_key.c_str(), CURLFORM_FILE, path.c_str(), CURLFORM_END);
            if (form_params != nullptr) {
                std::map<std::string, std::string>::const_iterator iterator = form_params->begin();
                while (iterator != form_params->end()) {
                    curl_formadd(&form_post,
                                 &last_ptr,
                                 CURLFORM_COPYNAME, iterator->first.c_str(),
                                 CURLFORM_COPYCONTENTS, iterator->second.c_str(),
                                 CURLFORM_END);
                    iterator++;
                }
            }
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, form_post);

            CURLcode ret = curl_easy_perform(curl);
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (callback != nullptr) {
                callback(ret, ho.debug_stream.str(), response_code, ho.data, ho.len);

            }
            curl_easy_cleanup(curl);
            if (list != NULL) {
                curl_slist_free_all(list);
            }
        }
    };

    easy_http::easy_http() : impl_(new easy_http_impl) {

    }

    easy_http easy_http::instance() {
        static easy_http h;
        return h;
    }

    void easy_http::get(std::string url, std::function<void(int curl_code, std::string debug_trace, long http_state, char *, size_t)> callback) {
        impl_->get(url, nullptr, callback);
    }

    void easy_http::download(std::string url, std::string path, std::shared_ptr<std::map<std::string, std::string>> header, bool override, std::function<void(int curl_code, std::string debug_trace, long http_state)> callback,
                             std::function<void(double time, long downloaded, long total)> process_callback) {
        impl_->download(url, path, header, override, callback, process_callback);
    }

    void easy_http::get(std::string url, std::shared_ptr<std::map<std::string, std::string>> header,
                   std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback) {
        impl_->get(url, header, callback);
    }

    void easy_http::post(std::string url, std::shared_ptr<std::map<std::string, std::string>> header,
                         std::shared_ptr<std::map<std::string, std::string>> form_params,
                         std::function<void(int curl_code, std::string debug_trace, long http_state, char *data,
                                            size_t len)> callback) {
        impl_->post(url, header, form_params, callback);
    }

    void easy_http::upload(std::string url, std::string path, std::string file_key, std::shared_ptr<std::map<std::string, std::string>> header,
                           std::shared_ptr<std::map<std::string, std::string>> form_params,
                           std::function<void(int curl_code, std::string debug_trace, long http_state, char* data, size_t data_len)> callback,
                           std::function<void(double time, long uploaded, long total)> progress_callback) {
        impl_->upload(url, path, file_key, header, form_params, callback, progress_callback);
    }

    void easy_http::set_download_notify_interval(float second) {
        download_interval = second;
    }
}
