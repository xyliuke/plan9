//
// Created by guazi on 8/4/16.
//

#ifndef HTTP_HTTP_H
#define HTTP_HTTP_H

#include <functional>
#include <string>
#include <map>
#include <memory>

namespace plan9 {


    class http {
    public:
        static http instance();

        /**
         * 简单的get请求, 只适合于最简单的请求,不能用于下载大文件等操作
         * @param url 请求的url
         * @param callback 请求的结果回调
         */
        void get(std::string url, std::function<void(std::string header, char *data, size_t len)> callback);

        void get(std::string url, std::map<std::string, std::string>* header, std::function<void(std::string header, char *data, size_t len)> callback);

        /**
         * 下载文件到本地
         * @param url 文件的url
         * @param path 本地文件路径
         * @param callback 下载进度回调
         */
        void download(std::string url, std::string path, std::function<void(long downloaded, long total)> callback);

        void post(std::string url, std::map<std::string, std::string>* header, std::map<std::string, std::string>* params, std::function<void(std::string header, char* data, size_t len)> callback);

    private:
        http();

        class http_impl;

        std::shared_ptr<http_impl> impl_;
    };

}


#endif //HTTP_HTTP_H
