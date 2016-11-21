//
// Created by liuke on 8/23/16.
//

#ifndef COMMON_ASYN_HTTP_H
#define COMMON_ASYN_HTTP_H


#include <memory>
#include <string>
#include <functional>
#include <map>

namespace plan9 {
    /**
     * 异步的http请求类
     * 支持常用的get/post请求和下载文件请求
     */
    //TODO 后续添加断点续传和断点下载功能,并可暂停、重传
    class async_http {
    public:
        static async_http instance();

        std::string version();

        void get(std::string url, long timeout_second, std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback);
        void get(std::string url, long timeout_second, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback);
        void download(std::string url, std::string path, long timeout_second, std::shared_ptr<std::map<std::string, std::string>> header, bool override, std::function<void(int curl_code, std::string debug_trace, long http_state)> callback,
                      std::function<void(double time, long downloaded, long total)> process_callback);

        void post(std::string url, long timeout_second, std::shared_ptr<std::map<std::string, std::string>> header, std::shared_ptr<std::map<std::string, std::string>> form_params,
                  std::function<void(int curl_code, std::string debug_trace, long http_state, char* data, size_t len)> callback);

        static bool is_timeout(int curl_code);
        static bool is_ok(int curl_code);
    private:
        async_http();
        class asyn_http_impl;
        std::shared_ptr<asyn_http_impl> impl_;
    };
}



#endif //COMMON_ASYN_HTTP_H
