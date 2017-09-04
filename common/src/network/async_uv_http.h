//
// Created by liuke on 18/07/2017.
//

#ifndef COMMON_ASYNC_UV_HTTP_H
#define COMMON_ASYNC_UV_HTTP_H

#include <string>
#include <functional>

namespace plan9
{
    class async_uv_http {
    public:
        static async_uv_http instance();

        void get(std::string url, long timeout_second, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback);

    private:
        async_uv_http();
        class async_uv_http_impl;
        std::shared_ptr<async_uv_http_impl> impl_;
    };

}

#endif //COMMON_ASYNC_UV_HTTP_H
