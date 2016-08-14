//
// Created by liuke on 16/6/3.
//

#include "http.h"
#include <ostream>
#include <iostream>
#include <boost/filesystem.hpp>

#ifdef __i386__
#include <include-i386/curl/curl.h>
#elif __x86_64__
#include <include-x86_64/curl/curl.h>
#include <fstream>

#elif __arm__
#include <include-armv7/curl/curl.h>
#elif __arm64__
#include <curl/include-arm64/curl/curl.h>
#endif

namespace plan9
{

    class write_buf {
    public:
        void* buf;
        size_t cap;
        size_t len;
    };

    class progress {
    public:
        double lastruntime;
        CURL *curl;
        std::function<void(long, long)> callback;
        curl_off_t downloaded;
        curl_off_t download_total;
        curl_off_t uploaded;
        curl_off_t upload_total;
        bool is_download;//是download模式为true,否则为upload模式
    };

    static size_t write_data_callback(void *buffer, size_t size, size_t nmemb, void *userp) {
        write_buf * writeBuf = (write_buf *)userp;
        size_t s = size * nmemb;
        size_t cap = writeBuf->cap;
        while (s > (cap - writeBuf->len)) {
            cap *= 2;
        }
        writeBuf->cap = cap;
        writeBuf->buf = realloc(writeBuf->buf, cap);
        memcpy((char*)writeBuf->buf + writeBuf->len, buffer, s);
        writeBuf->len += s;
        return s;
    }

    static size_t write_data_to_file_callback(void *buffer, size_t size, size_t nmemb, void *userp) {
        size_t s = size * nmemb;
        std::ofstream* ofstream = (std::ofstream*)userp;
        ofstream->write((char*)buffer, s);
        return s;
    }

    static int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
        progress *myp = (progress*)clientp;
        CURL *curl = myp->curl;
        double curtime = 0;

        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &curtime);
        myp->lastruntime = curtime;
        if (myp->callback != nullptr) {
            if (myp->is_download && myp->downloaded != dlnow) {
                myp->downloaded = dlnow;
                myp->download_total = dltotal;
                myp->callback(dlnow, dltotal);
            } else if (!myp->is_download && myp->uploaded != ulnow) {
                myp->uploaded = ulnow;
                myp->upload_total = ultotal;
                myp->callback(ulnow, ultotal);
            }
        }

        if((dlnow != 0 && dlnow == dltotal) || (ulnow != 0 && ulnow == ultotal)) {
            //不再执行
            return 1;
        }
        return 0;
    }


    class http::http_impl
    {
    public:

        http_impl() {
            curl_global_init(CURL_GLOBAL_ALL);
        }

        ~http_impl() {
            curl_global_cleanup();
        }

        void get(std::string url, std::function<void(char *, size_t)> callback) {

            write_buf writeBuf;
            writeBuf.cap = 1024;
            writeBuf.buf = malloc(writeBuf.cap);
            writeBuf.len = 0;

            CURL* curl_ = curl_easy_init();
            curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_data_callback);
            curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &writeBuf);
            curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1);
            curl_easy_setopt(curl_, CURLOPT_FORBID_REUSE, 1);

            CURLcode ret = curl_easy_perform(curl_);
            if (ret == CURLE_OK) {
                callback((char*)(writeBuf.buf), writeBuf.len);
            }
            curl_easy_cleanup(curl_);
        }

        bool get(std::string url, std::string file, std::function<void(long, long)> process) {

            namespace bs = boost::filesystem;
            bs::path p(file);
            if (!bs::exists(p.parent_path())) {
                bs::create_directories(p.parent_path());
            } else if (bs::is_regular_file(p.parent_path())){
                return false;
            }

            std::ofstream ofstream;
            ofstream.open(file, std::ios::trunc);

            CURL* curl_ = curl_easy_init();
            curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_data_to_file_callback);
            curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &ofstream);
            curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1);
            curl_easy_setopt(curl_, CURLOPT_FORBID_REUSE, 1);

            curl_easy_setopt(curl_, CURLOPT_XFERINFOFUNCTION, progress_callback);
            progress prog;
            prog.is_download = true;
            prog.curl = curl_;
            prog.lastruntime = 0;
            prog.callback = process;
            curl_easy_setopt(curl_, CURLOPT_XFERINFODATA, &prog);
            curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 0);

            CURLcode ret = curl_easy_perform(curl_);
            if (ret == CURLE_OK) {

            }
            ofstream.close();

            curl_easy_cleanup(curl_);
            return true;
        }

        void get_header(std::string url) {
            CURL* curl_ = curl_easy_init();
            curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl_, CURLOPT_HEADER, 1);    //只需要header头
            curl_easy_setopt(curl_, CURLOPT_NOBODY, 1);//不得到内容
            CURLcode ret = curl_easy_perform(curl_);
            double size = 0;
            if (ret == CURLE_OK) {
                curl_easy_getinfo(curl_, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);
            }
            curl_easy_cleanup(curl_);
        }

    private:
    };



    http::http() : impl_(new http_impl) {

    }

    void http::get(std::string url, std::function<void(char *, size_t)> callback) {
        impl_->get(url, callback);
    }

    bool http::get(std::string url, std::string file, std::function<void(long, long)> process) {
        return impl_->get(url, file, process);
    }

    void http::get_string(std::string url, std::function<void(std::string)> callback) {
        get(url, [=](char* data, size_t len){
            std::string ret(data, len);
            delete(data);
            callback(ret);
        });
    }

}