//
// Created by liuke on 09/11/2016.
//

#include <log/log_wrap.h>
#include <commander/cmd_factory.h>
#include <network/async_http.h>
#include <network/easy_http.h>
#include <error/error_num.h>
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
                            string s(data, len);
                            log_wrap::net().d("http request ", id, " end, the result : ", debug_trace, "\n", s);
                            if (async_http::is_ok(curl_code) && http_state == 200) {
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
                            if (async_http::is_ok(curl_code) && http_state == 200) {
                                string s(data, len);
                                log_wrap::net().d("http request ", id, " end, the result : ", debug_trace, "\n", s);
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
                            if (async_http::is_ok(curl_code) && http_state == 200) {
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
                            if (async_http::is_ok(curl_code) && http_state == 200) {
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
                        if (async_http::is_ok(curl_code) && http_state == 200) {
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
                    shared_ptr<double> download_time(new double);
                    async_http::instance().download(url, path, timeout, header, override, [=](int curl_code, std::string debug_trace, long http_state) mutable {
                        param["aux.once"] = true;
                        log_wrap::net().d("http request ", id, " end, the result : ", debug_trace);
                        if (async_http::is_ok(curl_code) && http_state == 200) {
                            JSONObject ret;
                            ret["file_path"] = path;
                            ret["file_size"] = (*file_size);
                            ret["time"] = *download_time;
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
                        *download_time = time;
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
         *   url 文件路径
         *   header 字典 header中设置值,可以为空
         *   form 字典 form表单的值
         *   file_key   上传文件的key值，
         *   process true/false 是否回调进度，默认为false,不回调
         *   timeout 超时
         *   path 本地文件完整路径
         *   model asyn/sync,异步或者同步，默认为异步
         *
         */
         //TODO 异步的upload
        JSONObject sp_json;
        sp_json.put("url", "http://www......", "upload url, required");
        sp_json.put("path", "./a.txt or /a/b/c.txt", "full file path, required");
        sp_json.put("file_key", "file", "file form key, default is file");
        sp_json.put("header", "key value", "http header, optional");
        sp_json.put("form", "key value", "http post form, optional");
        sp_json.put("process", "true/false", "callback progress, default false");
        sp_json.put("timeout", "seconds", "default 60s");
        sp_json.put("model", "asyn/sync", "default asyn");
        const std::string upload_json_string = sp_json.to_format_string();
        cmd_factory::instance().register_cmd("upload", [=](JSONObject param){
            if (param.has("args")) {
                JSONObject args = param["args"];
                if (args.has("url") && args.has("path")) {
                    using namespace std;
                    string url = args["url"].get_string();
                    string path = args["path"].get_string();

                    string file_key = "file";
                    if (args.has("file_key")) {
                        file_key = args["file_key"].get_string();
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
                    bool process = false;
                    if (args.has("process")) {
                        process = args["process"].get_bool();
                    }
                    int timeout = 60;
                    if (args.has("timeout")) {
                        timeout = args["timeout"].get_int();
                    }
                    string model = "asyn";
                    if (args.has("model")) {
                        model = args["model"].get_string();
                    }

                    log_wrap::net().d("http upload begin, param : ", param.to_string());

                    shared_ptr<long> upload_total(new long);
                    shared_ptr<double> upload_time(new double);
                    if ("asyn" == model) {
                        async_http::instance().upload(url, path, timeout, file_key, header, form, [param, upload_time, upload_total](int curl_code, std::string debug_trace, long http_state, char* data, size_t data_len)mutable{
                            param["aux.once"] = true;
                            string d = string(data, data_len);
                            log_wrap::net().d("http request ", param["aux.id"].get_string(), " end, the result : ", debug_trace, "\n", d);
                            JSONObject d_json(d);
                            if (async_http::is_ok(curl_code) && http_state == 200) {
                                JSONObject ret;
                                ret["upload_time"] = *upload_time;
                                ret["upload_size"] = *upload_total;
                                ret["data"] = d_json;
                                ret["state"] = "finish";
                                cmd_factory::instance().callback(param, true, ret);
                            } else {
                                if (async_http::is_timeout(curl_code)) {
                                    cmd_factory::instance().callback(param, (int)http_state, "timeout");
                                } else {
                                    cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                                }
                            }
                        }, [upload_total, upload_time, process, param, path](double time, long uploaded, long total)mutable{
                            (*upload_total) = total;
                            (*upload_time) = time;
                            log_wrap::net().d(path, " upload process ", time, "s : ", uploaded, "/", total);
                            if (process) {
                                JSONObject ret;
                                ret["state"] = "process";
                                ret["time"] = time;
                                ret["upload"] = uploaded;
                                ret["total"] = total;
                                param["aux.once"] = false;
                                cmd_factory::instance().callback(param, true, ret);
                            }
                        });

                    } else if ("sync"){
                        easy_http::instance().upload(url, path, file_key, header, form, [param, upload_time, upload_total](int curl_code, std::string debug_trace, long http_state, char* data, size_t data_len)mutable{
                            param["aux.once"] = true;
                            string d = string(data, data_len);
                            log_wrap::net().d("http request ", param["aux.id"].get_string(), " end, the result : ", debug_trace, "\n", d);
                            JSONObject d_json(d);
                            if (async_http::is_ok(curl_code) && http_state == 200) {
                                JSONObject ret;
                                ret["upload_time"] = *upload_time;
                                ret["upload_size"] = *upload_total;
                                ret["data"] = d_json;
                                ret["state"] = "finish";
                                cmd_factory::instance().callback(param, true, ret);
                            } else {
                                if (async_http::is_timeout(curl_code)) {
                                    cmd_factory::instance().callback(param, (int)http_state, "timeout");
                                } else {
                                    cmd_factory::instance().callback(param, (int)http_state, debug_trace);
                                }
                            }
                        }, [upload_total, upload_time, process, param, path](double time, long uploaded, long total)mutable{
                            (*upload_total) = total;
                            (*upload_time) = time;
                            log_wrap::net().d(path, " upload process ", time, "s : ", uploaded, "/", total);
                            if (process) {
                                JSONObject ret;
                                ret["state"] = "process";
                                ret["time"] = time;
                                ret["upload"] = uploaded;
                                ret["total"] = total;
                                param["aux.once"] = false;
                                cmd_factory::instance().callback(param, true, ret);
                            }
                        });
                    } else {
                        log_wrap::net().e("http request parameter error, the param : ", param.to_string());
                        cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), " , refer to ", upload_json_string));
                    }

                } else {
                    cmd_factory::instance().callback(param, ERROR_PARAMETER, util::instance().cat(ERROR_STRING(ERROR_PARAMETER), " , refer to ", upload_json_string));
                }
            } else {
                log_wrap::net().e("http request parameter error, the param : ", param.to_string());
                cmd_factory::instance().callback(param, ERROR_PARAMETER, ERROR_STRING(ERROR_PARAMETER));
            }
        });
    }
}