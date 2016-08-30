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
    class asyn_http {
    public:
        static asyn_http instance();

        void get(std::string url, long timeout_second, std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback);
        void get(std::string url, long timeout_second, std::map<std::string, std::string>* header, std::function<void(int curl_code, std::string debug_trace, long http_state, char *data, size_t len)> callback);
        void download(std::string url, std::string path, long timeout_second, std::function<void(int curl_code, std::string debug_trace, long http_state)> callback,
                      std::function<void(double time, long downloaded, long total)> process_callback);

        void post(std::string url, long timeout_second, std::map<std::string, std::string>* header, std::map<std::string, std::string>* form_params,
                  std::function<void(int curl_code, std::string debug_trace, long http_state, char* data, size_t len)> callback);
    private:
        asyn_http();
        class asyn_http_impl;
        std::shared_ptr<asyn_http_impl> impl_;
    };
}



#endif //COMMON_ASYN_HTTP_H
