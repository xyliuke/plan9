//
// Created by ke liu on 23/02/2018.
//

#include <log/log_wrap.h>
#include <ssl/ssl_shake.h>
#include "ahttp_wrapper.h"
#include "ahttp1.h"

namespace plan9
{

    class ahttp_wrapper::ahttp_wrapper_impl {
    public:
        ahttp_wrapper_impl() : http_map(std::make_shared<std::map<int, std::shared_ptr<ahttp1>>>()) {

        }
        int get(std::string url, int timeout, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(std::shared_ptr<common_callback>, int http_code, std::string data)> callback) {
            using namespace std;
            int id = 0;
            shared_ptr<ahttp1> http = create_http(&id);
            http->set_debug_mode(true);
            http->get(url, timeout, header, [=](std::shared_ptr<common_callback> ccb, std::shared_ptr<ahttp_request> request, std::shared_ptr<ahttp_response> response) {
                log_wrap::net().d("http get callback");
                if (callback) {
                    callback(ccb, response->get_response_code(), response->get_body_string());
                }
                remove_http(id);
            });
            return id;
        }

    private:
        std::shared_ptr<std::map<int, std::shared_ptr<ahttp1>>> http_map;

        std::shared_ptr<ahttp1> create_http(int* id) {
            static int count = 0;
            std::shared_ptr<ahttp1> http = std::make_shared<ahttp1>();
            (*http_map)[count] = http;
            count ++;
            return http;
        }

        void remove_http(int id) {
            http_map->erase(id);
        }
    };

    ahttp_wrapper ahttp_wrapper::instance() {
        static ahttp_wrapper h;
        return h;
    }

    ahttp_wrapper::ahttp_wrapper() : impl(std::make_shared<ahttp_wrapper_impl>()) {
        plan9::uv_wrapper::set_ssl_impl([=] () -> std::shared_ptr<plan9::ssl_interface> {
            std::shared_ptr<plan9::ssl_interface> ret = std::make_shared<plan9::ssl_shake>();
            return ret;
        });
    }

    int ahttp_wrapper::get(std::string url, int timeout, std::shared_ptr<std::map<std::string, std::string>> header,
                            std::function<void(std::shared_ptr<common_callback>, int http_code,
                                               std::string data)> callback) {
        return impl->get(url, timeout, header, callback);
    }
}