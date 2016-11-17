//
// Created by liuke on 09/11/2016.
//

#include <log/log_wrap.h>
#include <commander/cmd_factory.h>
#include <network/asyn_http.h>
#include <network/easy_http.h>
#include "http_init.h"

namespace plan9 {
    void http_init::init() {
        log_wrap::io().d("register http plugin");
        /**
         * http命令的参数格式
         * args
         *     type 请求种类 get/post
         *     url 网址
         *     header 字典 header中设置值,可以为空
         *     timeout 超时时间 默认为60s
         *     model asyn/sync,异步或者同步，默认为异步
         */
        cmd_factory::instance().register_cmd("http", [=](JSONObject param) {
            using namespace std;
            if (!param.is_undefined() && !param.is_null()) {
                JSONObject args = param["args"];
                std::string type = args["type"].get_string();
                string url = args["url"].get_string();
                int timeout = 60;
                if (args.has("timeout")) {
                    timeout = args["timeout"].get_int();
                }
                shared_ptr<map<string, string>> header;
                if (args.has("header")) {
                    header.reset(new map<string, string>);
                    JSONObject header_json = args["header"];
                    header_json.enumerate([=](string key, JSONObject value, bool onlyValue){
                        header->insert(pair<string, string>(key, value.get_string()));
                    });
                }
                shared_ptr<map<string, string>> form;
                if (args.has("form")) {
                    form.reset(new map<string, string>);
                    JSONObject form_json = args["form"];
                    form_json.enumerate([=](string key, JSONObject value, bool onlyValue){
                        form->insert(pair<string, string>(key, value.get_string()));
                    });
                }

                string id = param["aux"]["id"].get_string();
                log_wrap::net().d("http request ", id, " begin, the parameter : ", param.to_string());
                if (args.has("model") && "sync" == args["model"].get_string()) {
                    if ("get" == type) {
                        easy_http::instance().get(url, header, [=](int curl_code, std::string debug_trace, long http_state, char *data, size_t len){
                            log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                            if (asyn_http::instance().is_ok(curl_code)) {
                                string s(data, len);
                                JSONObject ret(s);
                                cmd_factory::instance().callback(param, true, ret);
                            } else {
                                if (asyn_http::instance().is_timeout(curl_code)) {
                                    cmd_factory::instance().callback(param, (int)http_state, "timeout");
                                } else {
                                    cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                                }
                            }
                        });
                    } else if ("post" == type) {
                        easy_http::instance().post(url, header, form, [=](int curl_code, std::string debug_trace, long http_state, char* data, size_t len){
                            log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                            if (asyn_http::instance().is_ok(curl_code)) {
                                string s(data, len);
                                JSONObject ret(s);
                                cmd_factory::instance().callback(param, true, ret);
                            } else {
                                if (asyn_http::instance().is_timeout(curl_code)) {
                                    cmd_factory::instance().callback(param, (int)http_state, "timeout");
                                } else {
                                    cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                                }
                            }
                        });
                    } else {
                        log_wrap::net().w("http not support the request, the param : ", param.to_string());
                        cmd_factory::instance().callback(param, -1, "http only support get or post");
                    }

                } else {
                    if ("get" == type) {
                        asyn_http::instance().get(url, timeout, header, [=](int curl_code, std::string debug_trace, long http_state, char *data, size_t len){
                            log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                            if (asyn_http::instance().is_ok(curl_code)) {
                                string s(data, len);
                                JSONObject ret(s);
                                cmd_factory::instance().callback(param, true, ret);
                            } else {
                                if (asyn_http::instance().is_timeout(curl_code)) {
                                    cmd_factory::instance().callback(param, (int)http_state, "timeout");
                                } else {
                                    cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                                }
                            }
                        });
                    } else if ("post" == type) {
                        asyn_http::instance().post(url, timeout, header, form, [=](int curl_code, std::string debug_trace, long http_state, char* data, size_t len){
                            log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                            if (asyn_http::instance().is_ok(curl_code)) {
                                string s(data, len);
                                JSONObject ret(s);
                                cmd_factory::instance().callback(param, true, ret);
                            } else {
                                if (asyn_http::instance().is_timeout(curl_code)) {
                                    cmd_factory::instance().callback(param, (int)http_state, "timeout");
                                } else {
                                    cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                                }
                            }
                        });
                    } else {
                        log_wrap::net().w("http not support the request, the param : ", param.to_string());
                        cmd_factory::instance().callback(param, -1, "http only support get or post");
                    }
                }
            } else {
                log_wrap::net().e("http request parameter error, the param : ", param.to_string());
                cmd_factory::instance().callback(param, -1, "parameter error");
            }
        });


        /**
         *  下载文件操作
         *   url 文件路径
         *   header 字典 header中设置值,可以为空
         *   timeout 超时
         *   path 本地路径，包括文件名
         *     model asyn/sync,异步或者同步，默认为异步
         */
        cmd_factory::instance().register_cmd("download", [=](JSONObject param){
            using namespace std;
            if (!param.is_undefined() && !param.is_null()) {
                JSONObject args = param["args"];
                string url = args["url"].get_string();
                int timeout = 60;
                if (args.has("timeout")) {
                    timeout = args["timeout"].get_int();
                }
                shared_ptr<map<string, string>> header;
                if (args.has("header")) {
                    header.reset(new map<string, string>);
                    JSONObject header_json = args["header"];
                    header_json.enumerate([=](string key, JSONObject value, bool onlyValue){
                        header->insert(pair<string, string>(key, value.get_string()));
                    });
                }
                string path;
                if (args.has("path")) {
                    path = args["path"].get_string();
                }
                string id = param["aux"]["id"].get_string();
                log_wrap::net().d("http request ", id, " begin, the parameter : ", param.to_string());
                if (args.has("model") && "sync" == args["model"].get_string()) {
                    shared_ptr<long> file_size(new long);
                    easy_http::instance().download(url, path, header,[=](int curl_code, std::string debug_trace, long http_state) mutable {
                        log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                        if (asyn_http::instance().is_ok(curl_code)) {
                            JSONObject ret;
                            ret["file_path"] = path;
                            ret["file_size"] = (*file_size);
                            ret["state"] = "finish";
                            cmd_factory::instance().callback(param, true, ret);
                        } else {
                            if (asyn_http::instance().is_timeout(curl_code)) {
                                cmd_factory::instance().callback(param, (int)http_state, "timeout");
                            } else {
                                cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                            }
                        }
                    }, [=](double time, long downloaded, long total) mutable {
                        log_wrap::net().d("http request ", id, " time ", time, " process ", downloaded, "/", total);
                        (*file_size) = total;
                        JSONObject ret;
                        ret["state"] = "process";
                        ret["time"] = time;
                        ret["download"] = downloaded;
                        ret["total"] = total;
                        cmd_factory::instance().callback_multi(param, true, 0, "", ret);
                    });
                } else {
                    shared_ptr<long> file_size(new long);
                    asyn_http::instance().download(url, path, timeout, header, [=](int curl_code, std::string debug_trace, long http_state) mutable {
                        log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                        if (asyn_http::instance().is_ok(curl_code)) {
                            JSONObject ret;
                            ret["file_path"] = path;
                            ret["file_size"] = (*file_size);
                            ret["state"] = "finish";
                            cmd_factory::instance().callback(param, true, ret);
                        } else {
                            if (asyn_http::instance().is_timeout(curl_code)) {
                                cmd_factory::instance().callback(param, (int)http_state, "timeout");
                            } else {
                                cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                            }
                        }
                    }, [=](double time, long downloaded, long total) mutable {
                        log_wrap::net().d("http request ", id, " time ", time, " process ", downloaded, "/", total);
                        (*file_size) = total;
                        JSONObject ret;
                        ret["state"] = "process";
                        ret["time"] = time;
                        ret["download"] = downloaded;
                        ret["total"] = total;
                        cmd_factory::instance().callback_multi(param, true, 0, "", ret);
                    });
                }
            } else {
                log_wrap::net().e("http request parameter error, the param : ", param.to_string());
                cmd_factory::instance().callback(param, -1, "parameter error");
            }
        });

        /**
         * 上传文件操作
         */
        cmd_factory::instance().register_cmd("upload", [=](JSONObject param){

        });
    }
}