//
// Created by guazi on 8/4/16.
//

#ifndef HTTP_HTTP_H
#define HTTP_HTTP_H

#include <functional>
#include <string>
#include <map>

namespace plan9 {


    class easy_http {
    public:
        static easy_http instance();

        /**
         * 简单的get请求, 只适合于最简单的请求,不能用于下载大文件等操作
         * @param url 请求的url
         * @param callback 请求的结果回调
         */
        void get(std::string url, std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback);

        void get(std::string url, std::map<std::string, std::string>* header, std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback);

        /**
         * 下载文件到本地
         * @param url 文件的url
         * @param path 本地文件路径
         * @param callback 下载进度回调
         */
        void download(std::string url, std::string path, std::function<void(int curl_code, std::string debug_trace, long http_state)> callback,
                      std::function<void(double time, long downloaded, long total)> process_callback);

        void post(std::string url, std::map<std::string, std::string>* header, std::map<std::string, std::string>* form_params,
                  std::function<void(int curl_code, std::string debug_trace, long http_state, char* data, size_t len)> callback);

        void upload(std::string url, std::string path, std::map<std::string, std::string>* header, std::map<std::string, std::string>* form_params,
                    std::function<void(int curl_code, std::string debug_trace, long http_state)> callback, std::function<void(double time, long uploaded, long total)> progress_callback);

    private:
        easy_http();

        class easy_http_impl;

        std::shared_ptr<easy_http_impl> impl_;
    };

}


#endif //HTTP_HTTP_H
