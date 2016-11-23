//
// Created by liuke on 09/11/2016.
//

#include <log/log_wrap.h>
#include <commander/cmd_factory.h>
#include <network/async_http.h>
#include <network/easy_http.h>
#include "http_init.h"

//TODO 添加上传功能；断点上传功能
//TODO 暂停下载功能；恢复下载功能
//TODO 解决下载https问题

namespace plan9 {
    void http_init::init() {
        log_wrap::io().d("register http plugin, libcurl version ", async_http::instance().version());
        /**
         * http命令的参数格式
         * args
         *     type 请求种类 get/post
         *     url 网址
         *     header 字典 header中设置值,可以为空
         *     form   字典  post请求时，表单值
         *     timeout 超时时间 默认为60s, 如果参数值为<=0,则表示不超时
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
                bool has_form = false;
                if (args.has("form")) {
                    has_form = true;
                    form.reset(new map<string, string>);
                    JSONObject form_json = args["form"];
                    form_json.enumerate([=](string key, JSONObject value, bool onlyValue){
                        form->insert(pair<string, string>(key, value.get_string()));
                    });
                }

                string id = param["aux"]["id"].get_string();
                log_wrap::net().d("http request ", id, " begin, the parameter : ", param.to_string());
                if (args.has("model") && "sync" == args["model"].get_string()) {
                    if ("post" == type || has_form) {
                        easy_http::instance().post(url, header, form, [=](int curl_code, std::string debug_trace, long http_state, char* data, size_t len){
                            log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                            if (async_http::is_ok(curl_code)) {
                                string s(data, len);
                                JSONObject ret(s);
                                cmd_factory::instance().callback(param, true, ret);
                            } else {
                                if (async_http::is_timeout(curl_code)) {
                                    cmd_factory::instance().callback(param, (int)http_state, "timeout");
                                } else {
                                    cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                                }
                            }
                        });
                    } else {
                        easy_http::instance().get(url, header, [=](int curl_code, std::string debug_trace, long http_state, char *data, size_t len){
                            if (async_http::is_ok(curl_code)) {
                                string s(data, len);
                                log_wrap::net().d("http request ", id, " end, the result : ", s);
                                JSONObject ret(s);
                                JSONObject new_ret;
                                if (ret.is_object()) {
                                    new_ret = ret;
                                } else {
                                    new_ret["data"] = ret;
                                }
                                cmd_factory::instance().callback(param, true, new_ret);
                            } else {
                                log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                                if (async_http::is_timeout(curl_code)) {
                                    cmd_factory::instance().callback(param, (int)http_state, "timeout");
                                } else {
                                    cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                                }
                            }
                        });
                    }
                } else {
                    if ("post" == type || has_form) {
                        async_http::instance().post(url, timeout, header, form, [=](int curl_code, std::string debug_trace, long http_state, char* data, size_t len){
                            log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                            if (async_http::is_ok(curl_code)) {
                                string s(data, len);
                                JSONObject ret(s);
                                cmd_factory::instance().callback(param, true, ret);
                            } else {
                                if (async_http::is_timeout(curl_code)) {
                                    cmd_factory::instance().callback(param, (int)http_state, "timeout");
                                } else {
                                    cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                                }
                            }
                        });
                    } else {
                        async_http::instance().get(url, timeout, header, [=](int curl_code, std::string debug_trace, long http_state, char *data, size_t len){
                            log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                            if (async_http::is_ok(curl_code)) {
                                string s(data, len);
                                JSONObject ret(s);
                                cmd_factory::instance().callback(param, true, ret);
                            } else {
                                if (async_http::is_timeout(curl_code)) {
                                    cmd_factory::instance().callback(param, (int)http_state, "timeout");
                                } else {
                                    cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                                }
                            }
                        });
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
         *   process true/false 是否回调进度，默认为false,不回调
         *   timeout 超时
         *   path 本地路径，包括文件名
         *   override bool 是否覆盖原有文件，不填写默认为true
         *   model asyn/sync,异步或者同步，默认为异步
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
                bool override = true;
                if (param["aux"].has("override") && !(param["aux.override"].get_bool())) {
                    override = false;
                }
                bool process = false;
                if (param["aux"].has("process")) {
                    process = param["aux.process"].get_bool();
                }
                param["aux.once"] = false;//可以重复返回下载进度
                string id = param["aux"]["id"].get_string();
                log_wrap::net().d("http request ", id, " begin, the parameter : ", param.to_string());
                if (args.has("model") && "sync" == args["model"].get_string()) {
                    shared_ptr<long> file_size(new long);
                    easy_http::instance().download(url, path, header, override, [=](int curl_code, std::string debug_trace, long http_state) mutable {
                        param["aux.once"] = true;
                        log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                        if (async_http::is_ok(curl_code)) {
                            JSONObject ret;
                            ret["file_path"] = path;
                            ret["file_size"] = (*file_size);
                            ret["state"] = "finish";
                            cmd_factory::instance().callback(param, true, ret);
                        } else {
                            if (async_http::is_timeout(curl_code)) {
                                cmd_factory::instance().callback(param, (int)http_state, "timeout");
                            } else {
                                cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                            }
                        }
                    }, [=](double time, long downloaded, long total) mutable {
                        log_wrap::net().d("http request ", id, " time ", time, " process ", downloaded, "/", total);
                        (*file_size) = total;
                        if (process) {
                            JSONObject ret;
                            ret["state"] = "process";
                            ret["time"] = time;
                            ret["download"] = downloaded;
                            ret["total"] = total;
                            cmd_factory::instance().callback(param, true, ret);
                        }
                    });
                } else {
                    shared_ptr<long> file_size(new long);
                    async_http::instance().download(url, path, timeout, header, override, [=](int curl_code, std::string debug_trace, long http_state) mutable {
                        param["aux.once"] = true;
                        log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                        if (async_http::is_ok(curl_code)) {
                            JSONObject ret;
                            ret["file_path"] = path;
                            ret["file_size"] = (*file_size);
                            ret["state"] = "finish";
                            cmd_factory::instance().callback(param, true, ret);
                        } else {
                            if (async_http::is_timeout(curl_code)) {
                                cmd_factory::instance().callback(param, (int)http_state, "timeout");
                            } else {
                                cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                            }
                        }
                    }, [=](double time, long downloaded, long total) mutable {
                        log_wrap::net().d("http request ", id, " time ", time, " process ", downloaded, "/", total);
                        (*file_size) = total;
                        if (process) {
                            JSONObject ret;
                            ret["state"] = "process";
                            ret["time"] = time;
                            ret["download"] = downloaded;
                            ret["total"] = total;
                            cmd_factory::instance().callback(param, true, ret);
                        }
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