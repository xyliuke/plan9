//
// Created by liuke on 16/6/3.
//

#ifndef COMMON_HTTP_H
#define COMMON_HTTP_H

#include <memory>
#include <string>
#include <functional>

namespace plan9 {

    class http {
    public:
        http();
        //get数据
        void get(std::string url, std::function<void(char*, size_t)> callback);
        //下载数据到文件,并有进度通知
        bool get(std::string url, std::string file, std::function<void(long, long)> process);

        void get_string(std::string url, std::function<void(std::string)> callback);

    private:
        class http_impl;
        std::shared_ptr<http_impl> impl_;
    };

}

#endif //COMMON_HTTP_H
