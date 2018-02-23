//
//  ahttp.cpp
//  anet
//
//  Created by ke liu on 22/10/2017.
//  Copyright © 2017 ke liu. All rights reserved.
//

#include <sstream>
#include "ahttp.hpp"
#include "uvwrapper/uv_wrapper.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <set>
#include <uv.h>
#include "common/string_parser.hpp"
#include "common/tri_bool.h"
#include "algorithm/zlib_wrap.hpp"
#include "common/case_insensitive_map.h"
#include "common/char_array.h"
#include "proxy/local_proxy.h"
#include "common/common_callback_err_wrapper.h"

namespace plan9 {

    class http_info {
    public:

        http_info() : info(std::make_shared<std::map<std::string, std::string>>()) {

        }

        void set_proxy(bool used) {
            push("proxy", used ? "1" : "0");
        }

        void set_reused_tcp(bool reused) {
            push("reused_tcp", reused ? "1" : "0");
        }

        void set_fetch_time() {
            fetch = get_current_time();
            std::stringstream ss;
            ss << (fetch / 1000);
            push("fetch", ss.str());
        }

        void set_remote_ip_port(std::string ip, int port) {
            std::stringstream ss;
            ss << ip;
            ss << ":";
            ss << port;
            push("remote", ss.str());
        }

        void set_remote_ip_port(std::string ip, std::string port) {
            std::stringstream ss;
            ss << ip;
            ss << ":";
            ss << port;
            push("remote", ss.str());
        }

        void set_dns_start_time() {
            push("dns_start", get_current_time_string());
        }

        void set_dns_end_time() {
            push("dns_end", get_current_time_string());
        }

        void set_connect_start_time() {
            push("connect_start", get_current_time_string());
        }

        void set_connect_end_time() {
            push("connect_end", get_current_time_string());
        }

        void set_ssl_start_time() {
            push("ssl_start", get_current_time_string());
        }
        void set_ssl_end_time() {
            push("ssl_end", get_current_time_string());
        }

        void set_request_start_time() {
            push("request_start", get_current_time_string());
        }
        void set_request_end_time() {
            push("request_end", get_current_time_string());
        }
        void set_response_start_time() {
            push("response_start", get_current_time_string());
        }

        void set_response_end_time() {
            long end = get_current_time();
            std::stringstream ss;
            ss << (end / 1000);
            push("response_end", ss.str());
            std::stringstream sss;
            sss << ((end - fetch) / 1000);
            push("total", sss.str());
        }

        void set_request_data_size(int size) {
            std::stringstream ss;
            ss << size;
            push("request_bytes", ss.str());
        }

        void set_response_data_size(int size) {
            std::stringstream ss;
            ss << size;
            push("response_bytes", ss.str());
        }

        void set_local_ip_port(std::string ip, int port) {
            std::stringstream ss;
            ss << ip;
            ss << ":";
            ss << port;
            push("local", ss.str());
        }

        void set_local_ip_port(std::string ip, std::string port) {
            std::stringstream ss;
            ss << ip;
            ss << ":";
            ss << port;
            push("local", ss.str());
        }

        std::shared_ptr<std::map<std::string, std::string>> get_info() {
            return info;
        }

    private:
        std::shared_ptr<std::map<std::string, std::string>> info;
        std::string get_current_time_string() {
            std::stringstream ss;
            ss << (get_current_time() / 1000);
            return ss.str();
        }
        long get_current_time() {
            auto tp = std::chrono::system_clock::now();
            return tp.time_since_epoch().count();
        }
        void push(std::string key, std::string value) {
            if (info) {
                (*info)[key] = value;
            }
        }
        long fetch;
    };

    class mutex_wrap {
    public:
        mutex_wrap() {
            uv_mutex_init(&mutex);
        }

        void lock() {
            uv_mutex_lock(&mutex);
        }

        void unlock() {
            uv_mutex_unlock(&mutex);
        }

        ~mutex_wrap() {
            uv_mutex_destroy(&mutex);
        }
    private:
        uv_mutex_t mutex;
    };

    class ahttp_request::ahttp_request_impl {
    public:
        ahttp_request_impl() : header(new case_insensitive_map), method("GET"), version("1.1"), port(80), path("/"),
                            timeout(30), reused_tcp(true), used_proxy(false) {
        }

        void set_uesd_proxy(bool uesd) {
            used_proxy = uesd;
        }

        void append_header(std::string key, std::string value) {
            header->add(key, value);
        }
        void append_header(std::string key, int value) {
            std::stringstream ss;
            ss << value;
            append_header(key, ss.str());
        }
        std::string get_header(std::string key) {
            bool find = false;
            std::string ret = header->get("host", &find);
            if (find) {
                return ret;
            }
            return "";
        }
        void append_header(std::shared_ptr<std::map<std::string, std::string>> headers) {
            if (headers) {
                std::map<std::string, std::string>::const_iterator it = headers->begin();
                while (it != headers->end()) {
                    append_header(it->first, it->second);
                    it ++;
                }
            }
        }

        void set_method(std::string method) {
            this->method = method;
        }
        void set_http_version(std::string protocol) {
            this->version = protocol;
        }

        void set_url(std::string url) {
            this->url = string_parser::trim(url);
            size_t index_1 = url.find_first_of("://", 0);
            if (index_1 == std::string::npos) {
                return;
            }
            protocol = url.substr(0, index_1);

            size_t index_2 = url.find_first_of("/", index_1 + 3);
            if (index_2 == std::string::npos) {
                domain = url.substr(index_1 + 3, url.size() - index_1 - 2);
                path = "/";
            } else {
                domain = url.substr(index_1 + 3, index_2 - index_1 - 3);
                path = url.substr(index_2, url.size() - index_2);
            }
            size_t index_3 = domain.find_first_of(":", 0);
            if (index_3 != std::string::npos) {
                std::stringstream ss(domain.substr(index_3 + 1, domain.size() - index_3 - 1));
                ss >> port;
                domain = domain.substr(0, index_3);
            } else {
                std::string p_lower = string_parser::to_lower(protocol);
                if (p_lower == "https") {
                    port = 443;
                } else if (p_lower == "http") {
                    port = 80;
                }
            }
            if (!is_ip_format(domain)) {
//                std::stringstream ss;
//                ss << domain;
//                ss << ":";
//                ss << port;
                append_header("Host", domain);
            }
        }


        void append_body_data(std::shared_ptr<std::map<std::string, std::string>> data) {
            if (data && data->size() > 0) {
                if (boundary == "") {
                    boundary = get_boundary_string();
                    header->add("Content-Type", "multipart/form-data;boundary=" + boundary);
                }
                if (!this->data) {
                    this->data.reset(new std::map<std::string, std::string>);
                }

                std::map<std::string, std::string>::const_iterator it = data->begin();
                while (it != data->end()) {
                    (*this->data)[it->first] = it->second;
                    it ++;
                }
            }
        }

        void append_body_data_from_file(std::string key, std::string file) {
            if (!data_from_file) {
                data_from_file.reset(new std::map<std::string, std::string>);
            }

            if (boundary == "") {
                boundary = get_boundary_string();
                header->add("Content-Type", "multipart/form-data;boundary=" + boundary);
            }
            (*data_from_file)[key] = file;
        }

        void append_body_data(std::string key, std::string value) {
            if (!this->data) {
                this->data.reset(new std::map<std::string, std::string>);
            }
            if (boundary == "") {
                boundary = get_boundary_string();
                header->add("Content-Type", "multipart/form-data;boundary=" + boundary);
            }
            (*data)[key] = value;
        }

        void set_reused_tcp(bool reused) {
            reused_tcp = reused;
        }

        bool is_reused_tcp() {
            return reused_tcp;
        }

        void set_timeout(int seconds) {
            timeout = seconds;
        }

        int get_timeout() {
            return timeout;
        }

        std::string get_header(std::string key, bool* find) {
            return header->get(key, find);
        }

        void set_keep_alive(bool keep_alive) {
            append_header("Connection", "Keep-Alive");
        }
        bool is_keep_alive() {
            return true;
        }

        bool is_use_ssl() {
            return string_parser::to_lower(protocol) == "https";
        }

        bool is_ip_format_host() {
            return is_ip_format(domain);
        }

        std::shared_ptr<char_array> get_http_method_string () {
            std::shared_ptr<char_array> array = std::make_shared<char_array>(20);
            array->append(method);
            array->append(" ");
            if (used_proxy) {
                array->append(protocol);
                array->append("://");
                array->append(domain);
            }
            array->append(path);
            array->append(" ");
            array->append("HTTP/");
            array->append(version);
            return array;
        }

        std::shared_ptr<char_array> get_http_header_string() {
            std::shared_ptr<char_array> ss = std::make_shared<char_array>(200);
            if (header != nullptr) {
                header->const_iteration([=](std::string key, std::string value) -> bool {
                    ss->append(key);
                    ss->append(":");
                    ss->append(value);
                    ss->append("\r\n");
                    return true;
                });
            }
            return ss;
        }

        std::shared_ptr<char_array> get_http_body_string(){
            std::shared_ptr<char_array> ret;
            if (data && data->size() > 0) {
                ret.reset(new char_array(1024));
                ret->append("--");
                ret->append(boundary);
                std::map<std::string, std::string>::const_iterator it = data->begin();
                while (it != data->end()) {
                    ret->append("\r\nContent-Disposition: form-data;name=\"");
                    ret->append(it->first);
                    ret->append("\"\r\n\r\n");
                    ret->append(it->second);
                    ret->append("\r\n");
                    ret->append("--");
                    ret->append(boundary);
                    it ++;
                }
                ret->append("--");
            }
            return ret;
        }

        std::string get_domain() {
            if (is_ip_format_host()) {
                return get_header("host");
            }
            return domain;
        }

        int get_port() {
            return port;
        }

        std::shared_ptr<char_array> get_http_string() {
            std::shared_ptr<char_array> method = get_http_method_string();
            std::shared_ptr<char_array> body = get_http_body_string();

            if (body && body->get_len() > 0) {
                append_header("Content-Length", body->get_len());
            }
            std::shared_ptr<char_array> header = get_http_header_string();
            method->append("\r\n");
            if (header) {
                method->append(header.get());
                method->append("\r\n");
            }
            if (body) {
                method->append(body.get());
                method->append("\r\n");
            }
            return method;
        }


        void get_http_data(std::function<void(std::shared_ptr<char> data, int len, int sent, int total)> callback) {
            if (callback) {
                std::shared_ptr<char_array> http = get_http_string();
                callback(http->get_data(), http->get_len(), 0, http->get_len());

                //TODO上传文件
                get_http_data_from_file(http->get_len(), callback);
            }
        }

        void get_http_data_from_file(long send, std::function<void(std::shared_ptr<char> data, long len, long sent, long total)> callback) {
            if (callback && data_from_file && data_from_file->size() > 0) {
                using namespace std;

                long total = 0;
                std::map<std::string, std::string>::const_iterator it = data_from_file->begin();
                //计算文件大小
                while (it != data_from_file->end()) {
                    string file = it->second;
                    ifstream ifs(file, ios::binary | ios::in);
                    total += ifs.tellg();
                }

                std::function<void(std::shared_ptr<char>, long, long)> func = bind(callback, placeholders::_1, placeholders::_2, placeholders::_3, total);

                it = data_from_file->begin();
                long send_bytes = send;
                while (it != data_from_file->end()) {

                    auto ss = std::make_shared<char_array>();
                    *ss << boundary;
                    *ss << "\r\nContent-Disposition: form-data;name=\"";
                    *ss << it->first;
                    *ss << "\"\r\n";

                    func(ss->get_data(), ss->get_len(), send_bytes);
                    send_bytes += ss->get_len();

                    string file = it->second;
                    ifstream ifs(file, ios::binary | ios::in);
                    if (ifs.is_open()) {
                        while (!ifs.eof()) {
                            auto buf = std::make_shared<char_array>(1024);
                            ifs.read(buf->get_data_ptr(), buf->get_cap());
                            func(buf->get_data(), ifs.gcount(), send_bytes);
                            send_bytes += ifs.gcount();
                        }
                    }

                    auto sss = std::make_shared<char_array>();
                    *sss << "\r\n";
                    *sss << boundary;
                    func(sss->get_data(), sss->get_len(), send_bytes);
                    send_bytes += sss->get_len();

                    it ++;
                }
            }
        }

        static std::string get_boundary_string() {
            auto tp = std::chrono::system_clock::now();
            std::stringstream ss;
            ss << "--";
            ss << "Boundary+";
            ss << tp.time_since_epoch().count();
            return ss.str();
        }
    private:
        std::shared_ptr<case_insensitive_map> header;
        std::shared_ptr<std::map<std::string, std::string>> data;
        std::shared_ptr<std::map<std::string, std::string>> data_from_file;
        std::string method;
        std::string version;
        std::string url;
        std::string protocol;
        std::string domain;
        std::string path;
        int port;
        std::string boundary;
        int timeout;
        bool reused_tcp;
        bool used_proxy;
    };

    const std::string ahttp_request::METHOD_GET = "GET";
    const std::string ahttp_request::METHOD_POST = "POST";
    const std::string ahttp_request::METHOD_HEAD = "HEAD";
    const std::string ahttp_request::METHOD_OPTIONS = "OPTIONS";//暂不支持
    const std::string ahttp_request::METHOD_PUT = "PUT";//暂不支持
    const std::string ahttp_request::METHOD_DELETE = "DELETE";//暂不支持
    const std::string ahttp_request::METHOD_TRACE = "TRACE";//暂不支持
    const std::string ahttp_request::METHOD_CONNECT = "CONNECT";//暂不支持

    ahttp_request::ahttp_request() : impl(new ahttp_request_impl) {

    }

    void ahttp_request::set_uesd_proxy(bool uesd) {
        impl->set_uesd_proxy(uesd);
    }

    void ahttp_request::append_header(std::string key, std::string value) {
        impl->append_header(key, value);
    }

    std::string ahttp_request::get_header(std::string key) {
        return impl->get_header(key);
    }

    void ahttp_request::append_header(std::string key, int value) {
        impl->append_header(key, value);
    }

    void ahttp_request::append_header(std::shared_ptr<std::map<std::string, std::string>> headers) {
        impl->append_header(headers);
    }

    void ahttp_request::set_method(std::string method) {
        impl->set_method(method);
    }

    void ahttp_request::set_http_version(std::string version) {
        impl->set_http_version(version);
    }

    void ahttp_request::set_url(std::string url) {
        impl->set_url(url);
    }

    void ahttp_request::append_body_data(std::shared_ptr<std::map<std::string, std::string>> data) {
        impl->append_body_data(data);
    }

    void ahttp_request::append_body_data(std::string key, std::string value) {
        impl->append_body_data(key, value);
    }

    void ahttp_request::append_body_data_from_file(std::string key, std::string file) {
        impl->append_body_data_from_file(key, file);
    }

    void ahttp_request::set_reused_tcp(bool reused) {
        impl->set_reused_tcp(reused);
    }

    bool ahttp_request::is_reused_tcp() {
        return impl->is_reused_tcp();
    }

    void ahttp_request::set_timeout(int seconds) {
        impl->set_timeout(seconds);
    }

    int ahttp_request::get_timeout() {
        return impl->get_timeout();
    }

    void ahttp_request::set_keep_alive(bool keep_alive) {
        impl->set_keep_alive(keep_alive);
    }

    bool ahttp_request::is_keep_alive() {
        return impl->is_keep_alive();
    }

    bool ahttp_request::is_use_ssl() {
        return impl->is_use_ssl();
    }

    bool ahttp_request::is_ip_format_host() {
        return impl->is_ip_format_host();
    }

    bool ahttp_request::is_ip_format(std::string str) {
        return uv_wrapper::is_ip4(str) || uv_wrapper::is_ip6(str);
    }

    std::string ahttp_request::get_http_method_string() {
        return impl->get_http_method_string()->to_string();
    }

    std::string ahttp_request::get_http_header_string() {
        return impl->get_http_header_string()->to_string();
    }

    std::string ahttp_request::get_http_string() {
        return impl->get_http_string()->to_string();
    }

    void ahttp_request::get_http_data(std::function<void(std::shared_ptr<char> data, int len, int sent, int total)> callback) {
        return impl->get_http_data(callback);
    }

    std::string ahttp_request::to_string() {
        return impl->get_http_string()->to_string();
    }
    std::string ahttp_request::get_domain() {
        return impl->get_domain();
    }

    int ahttp_request::get_port() {
        return impl->get_port();
    }


    class ahttp_response::ahttp_response_impl {
    public:

        ahttp_response_impl() : header_buf((char*)malloc(65536)), header_buf_size(65536), header_len(0), block_num(0), status(-1),
                                data_buf(nullptr), data_buf_size(0), data_len(0), content_length(0), http_header_end_position(0),
                                http_status_end_position(0), headers(new case_insensitive_map),
                                transfer_encoding_chunked(tri_undefined), total_len(0) {
        }

        ~ahttp_response_impl() {
            if (header_buf != nullptr) {
                free(header_buf);
                header_buf = nullptr;
            }
            if (data_buf != nullptr) {
                free(data_buf);
                data_buf = nullptr;
            }
            if (ofstream) {
                ofstream->close();
            }
        }

        bool fill_header_buf(std::shared_ptr<char> data, int len, int* new_data_begin) {
            *new_data_begin = 0;
            if (http_header_end_position > 0) {
                return true;
            }
            int pos = find_header_end_position(data, len);
            http_header_end_position = pos + header_len;
            int real_len = 0;
            if (pos < 0) {
                real_len = len;
            } else {
                real_len = pos;
                *new_data_begin = pos + 1;
            }
            if (header_buf_size - this->header_len > real_len) {
                //有足够空间
            } else {
                header_buf = (char*)realloc(header_buf, real_len * 2 + header_buf_size);
                header_buf_size += real_len * 2;
            }
            memcpy(header_buf + this->header_len, data.get(), real_len);
            this->header_len += real_len;
            return pos >= 0;
        }

        int find_header_end_position(std::shared_ptr<char> data, int len) {
            for (int i = 0; i < len; ++i) {
                char c = *(data.get() + i);
                if (c == '\r' && i < (len - 3) && '\n' == *(data.get() + i + 1) && '\r' == *(data.get() + i + 2) && '\n' == *(data.get() + i + 3)) {
                    return i + 3;
                }
            }
            return -1;
        }

        bool fill_body_buf(char* data, int len) {
            if (data_buf == nullptr) {
                data_buf = (char*) malloc(len << 2);
                data_buf_size = (len << 2);
            }
            if (http_header_end_position > 0) {
                if (data_buf_size - data_len < len) {
                    //空间不够
                    data_buf = (char*) realloc(data_buf, data_len + (len << 2));
                }
                memcpy(data_buf + data_len, data, len);
                data_len += len;
                bool over = is_over(data_buf, data_len);
                if (over) {
                    parse_http_body();
                }
                return over;
            }
            return true;
        }

        bool fill_body_to_file(char* data, int len) {
            if (!ofstream) {
                ofstream.reset(new std::ofstream);
                ofstream->open(file, std::ios::app | std::ios::binary);
            }
            ofstream->write(data, len);
            data_len += len;
            bool over = is_over(data, len);
            if (over) {
                ofstream->close();
            }
            return over;
        }

        bool append_response_data(std::shared_ptr<char> data, int len) {
            total_len += len;
            block_num ++;
            int new_pos = 0;
            if (fill_header_buf(data, len, &new_pos)) {
                parse();
                if (new_pos < len) {
                    if (file != "") {
                        //保存到文件
                        return fill_body_to_file(data.get() + new_pos, len - new_pos);
                    } else {
                        return fill_body_buf(data.get() + new_pos, len - new_pos);
                    }
                }
            }
            return false;
        }

        void set_response_file(std::string file) {
            this->file = file;
        }

        bool is_over(char* data, int len) {
            if (get_content_length() > 0 && data_len >= get_content_length()) {
                return true;
            }
            if (len >= 6 && data[len - 5] == '0' && data[len - 4] == '\r' && data[len - 3] == '\n' && data[len - 2] == '\r' && data[len - 1] == '\n') {
                return true;
            }
            return false;
        }

        void parse() {
            parse_http_status();
            parse_http_header();
        }

        void parse_http_status() {
            if (http_version == "" && status < 0) {
                std::string header = find_response_status_string();
                std::shared_ptr<std::vector<std::string>> sp = string_parser::split(header, " ");
                if (sp->size() == 3) {
                    http_version = sp->at(0);
                    std::stringstream ss(sp->at(1));
                    ss >> status;
                }
            }
        }

        std::string find_response_status_string() {
            for (int i = 5; i < header_buf_size; i += 2) {
                char c = header_buf[i];
                if (c == '\r' && header_buf[i + 1] == '\n') {
                    http_status_end_position = i + 1;
                    return std::string(header_buf, 0, i);
                } else if (c == '\n' && header_buf[i - 1] == '\r') {
                    http_status_end_position = i;
                    return std::string(header_buf, 0, i - 1);
                }
            }
            return "";
        }

        void parse_http_header() {
            if (http_status_end_position > 0) {
                int pre_header_item_end_pos = http_status_end_position;
                for (int i = http_status_end_position + 1; i < header_len; i ++) {
                    char c = header_buf[i];
                    if ('\r' == c && '\n' == header_buf[i + 1]){
                        int key_begin, key_end, value_begin, value_end;
                        if (parse_key_value(pre_header_item_end_pos + 1, i - 1, &key_begin, &key_end, &value_begin, &value_end)) {
                            std::string k = string_parser::trim(std::string(header_buf, key_begin, key_end - key_begin + 1));
                            std::string v = string_parser::trim(std::string(header_buf, value_begin, value_end - value_begin + 1));
                            headers->add(k, v);
                        }

                        if ('\r' == header_buf[i + 2] && '\n' == header_buf[i + 3]) {
                            http_header_end_position = i + 3;
                            break;
                        } else {
                            pre_header_item_end_pos = i + 1;
                        }
                    }
                }
            }
        }

        void parse_http_body() {
            if (!ofstream) {
                //没有写文件
                if (is_transfer_encoding_chunked()) {
                    int data_real_index = 0;
                    int data_real_len = 0;
                    for (int i = 0; i < data_len; ++i) {
                        char c = data_buf[i];
                        if (c == '\r' && data_buf[i + 1] == '\n') {
                            data_real_index = i + 2;
                            break;
                        }
                    }
                    data_real_len = string_parser::dex_to_dec(data_buf, data_real_index - 2);
                    std::string zip = get_header_string("Content-Encoding");
                    if ("gzip" == zip) {
                        unsigned long len = 0;
                        char* new_data = zlib_wrap::ungzip(data_buf + data_real_index, data_real_len, &len);
                        if (len > 0) {
                            free(data_buf);
                            data_buf = new_data;
                            data_len = (int)len;
                            data_buf_size = (int)len;
                        }
                    } else {
                        char* new_data = (char*) malloc(data_real_len);
                        memcpy(new_data, data_buf + data_real_index, data_real_len);
                        free(data_buf);
                        data_buf = new_data;
                        data_len = data_real_len;
                    }
                }
            }
        }

        bool parse_key_value(int begin, int end, int*key_begin, int* key_end, int* value_begin, int* value_end) {
            *key_begin = begin;
            *value_end = end;
            for (int i = begin; i <= end; ++i) {
                if (':' == header_buf[i]) {
                    *key_end = i - 1;
                    *value_begin = i + 1;
                    return true;
                }
            }
            return false;
        }

        long get_len() {
            return total_len;
        }

        long get_content_length() {
            if (content_length <= 0) {
                content_length = get_header<long>("Content-Length");
            }
            return content_length;
        }

        bool is_transfer_encoding_chunked() {
            if (transfer_encoding_chunked == tri_undefined) {
                std::string v = get_header_string("Transfer-Encoding");
                if (string_parser::to_lower(v) == "chunked") {
                    transfer_encoding_chunked = tri_true;
                } else {
                    transfer_encoding_chunked = tri_false;
                }
                return transfer_encoding_chunked == tri_true;
            } else {
                return transfer_encoding_chunked == tri_true;
            }
        }

        int get_status() {
            return status;
        }

        long get_body_len() {
            return total_len - header_len;
        }

        long get_header_len() {
            return header_len;
        }

        template <typename T>
        T get_header(std::string key) {
            std::string ret = string_parser::trim(get_header_string(key));
            std::stringstream ss(ret);
            T t;
            ss >> t;
            return t;
        }

        std::string get_header_string(std::string key) {
            bool find;
            std::string ret = headers->get(key, &find);
            if (find) {
                return ret;
            }
            return "";
        }

        std::shared_ptr<std::map<std::string, std::string>> get_headers() {
            return headers->get();
        };
        std::string get_body_string() {
            return std::string(data_buf, data_len);
        }

        std::string to_string() {
            if (ofstream) {
                return std::string(header_buf, header_len);
            } else {
                std::stringstream ss;
                ss.write(header_buf, header_len);
                ss.write(data_buf, data_len);
                return ss.str();
            }
        }

    private:
        long total_len;
        char* header_buf;
        int header_buf_size;
        int header_len;
        char* data_buf;
        int data_buf_size;
        int data_len;
        int block_num;
        std::string http_version;
        int status;
        int http_status_end_position;
        int http_header_end_position;
        std::shared_ptr<case_insensitive_map> headers;
        std::string file;
        std::shared_ptr<std::ofstream> ofstream;
        long content_length;
        tri_bool transfer_encoding_chunked;
    };

    ahttp_response::ahttp_response() : impl(new ahttp_response_impl) {

    }

    template <typename T>
    T ahttp_response::get_header(std::string key) {
        return impl->get_header<T>(key);
    }

    std::string ahttp_response::get_header(std::string key) {
        return impl->get_header_string(key);
    }

    std::shared_ptr<std::map<std::string, std::string>> ahttp_response::get_headers() {
        return impl->get_headers();
    }

    bool ahttp_response::append_response_data(std::shared_ptr<char> data, int len) {
        return impl->append_response_data(data, len);
    }

    void ahttp_response::set_response_data_file(std::string file) {
        impl->set_response_file(file);
    }

    int ahttp_response::get_response_code() {
        return impl->get_status();
    }

    long ahttp_response::get_response_data_length() {
        return impl->get_body_len();
    }

    long ahttp_response::get_response_header_length() {
        return impl->get_header_len();
    }

    long ahttp_response::get_response_length() {
        return impl->get_len();
    }

    long ahttp_response::get_content_length() {
        return impl->get_content_length();
    }

    std::string ahttp_response::to_string() {
        return impl->to_string();
    }

    std::string ahttp_response::get_body_string() {
        return impl->get_body_string();
    }

    class ahttp::ahttp_impl {
    private:
        class http_content {
        private:
            std::map<int, std::shared_ptr<std::vector<ahttp_impl*>>> tcp_http_map;
            std::set<ahttp_impl*> unconnected_list;
        public:

            ahttp_impl* get_next(int tcp_id) {
                auto list = get_http_list(tcp_id, true);
                if (list->size() > 0) {
                    return list->at(0);
                }
                if (unconnected_list.size() > 0) {
                    auto http = *(unconnected_list.begin());
                    unconnected_list.erase(unconnected_list.begin());
                    list->push_back(http);
                    return http;
                }

                auto it = get_begin();
                while (it != get_end()) {
                    if (it->first != tcp_id) {
                        auto http_list = it->second;
                        if (http_list->size() > 1) {
                            auto itt = http_list->begin();
                            itt ++;//不取第一个，第一个正在请求中
                            while (itt != http_list->end()) {
                                auto http = *itt;
                                if (http->is_long_time_not_exec()) {
                                    http_list->erase(itt);
                                    list->push_back(http);
                                    return http;
                                }
                            }
                        }
                    }
                    it ++;
                }
                return nullptr;
            }

            int get_connected_num() {
                return (int)tcp_http_map.size();
            }

            int get_all_http_num() {
                return (int)(tcp_http_map.size() + unconnected_list.size());
            }

            std::map<int, std::shared_ptr<std::vector<ahttp_impl*>>>::iterator get_begin() {
                return tcp_http_map.begin();
            }

            std::map<int, std::shared_ptr<std::vector<ahttp_impl*>>>::iterator get_end() {
                return tcp_http_map.end();
            }

            //返回true时继续迭代，false时不再迭代
            void enumerate(std::function<bool(int, std::shared_ptr<std::vector<ahttp_impl*>>)> callback) {
                if (callback) {
                    std::map<int, std::shared_ptr<std::vector<ahttp_impl*>>>::iterator it = tcp_http_map.begin();
                    while (it != tcp_http_map.end()) {
                        bool ret = callback(it->first, it->second);
                        if (!ret) {
                            return;
                        }
                        it ++;
                    }
                }
            }



            void add_unconnected_list(ahttp_impl* http) {
                unconnected_list.insert(http);
            }

            void add_connected_list(int tcp_id, ahttp_impl* http) {
                auto list = get_http_list(tcp_id, true);
                if (list) {
                    list->push_back(http);
                }
                unconnected_list.erase(http);
            }

            //删除tcp_id下的http数据
            void remove_http(ahttp_impl* http) {
                if (http != nullptr) {
                    std::map<int, std::shared_ptr<std::vector<ahttp_impl*>>>::iterator itt = tcp_http_map.begin();
                    while (itt != tcp_http_map.end()) {
                        auto list = itt->second;
                        std::vector<ahttp_impl*>::iterator it = list->begin();
                        while (it != list->end()) {
                            if (*it == http) {
                                list->erase(it);
                                return;
                            }
                            it ++;
                        }
                        itt ++;
                    }
                }
            }

            std::shared_ptr<std::vector<ahttp_impl*>> get_http_list(int tcp_id, bool or_create) {
                if (tcp_http_map.find(tcp_id) != tcp_http_map.end()) {
                    return tcp_http_map[tcp_id];
                }
                if (or_create) {
                    std::shared_ptr<std::vector<ahttp_impl*>> ret = std::make_shared<std::vector<ahttp_impl*>>();
                    tcp_http_map[tcp_id] = ret;
                    return ret;
                }
                std::shared_ptr<std::vector<ahttp_impl*>> ret;
                return ret;
            }

            std::shared_ptr<std::vector<ahttp_impl*>> remove_http_list(int tcp_id) {
                std::shared_ptr<std::vector<ahttp_impl*>> ret;
                if (tcp_http_map.find(tcp_id) != tcp_http_map.end()) {
                    ret = tcp_http_map[tcp_id];
                    tcp_http_map.erase(tcp_id);
                }
                return ret;
            }

            bool contains_tcp_ip(int tcp_id) {
                return tcp_http_map.find(tcp_id) != tcp_http_map.end();
            }
        };
        static std::shared_ptr<http_content> get_content(std::string url, bool or_create) {
            if (url_2_http.find(url) != url_2_http.end()) {
                return url_2_http[url];
            }
            if (or_create) {
                std::shared_ptr<http_content> ret = std::make_shared<http_content>();
                url_2_http[url] = ret;
                return ret;
            }
            std::shared_ptr<http_content> ret;
            return ret;
        }
        static std::shared_ptr<std::vector<ahttp_impl*>> get_http_list(int tcp_id) {
            std::shared_ptr<std::vector<ahttp_impl*>> ret;
            std::map<std::string, std::shared_ptr<http_content>>::iterator it = url_2_http.begin();
            while (it != url_2_http.end()) {
                auto content = it->second;
                auto http_list = content->get_http_list(tcp_id, false);
                if (http_list) {
                    ret = http_list;
                    break;
                }
                it ++;
            }
            return ret;
        }

        static std::shared_ptr<http_content> get_content(int tcp_id) {
            std::shared_ptr<http_content> ret;
            std::map<std::string, std::shared_ptr<http_content>>::iterator it = url_2_http.begin();
            while (it != url_2_http.end()) {
                auto content = it->second;
                auto http_list = content->get_http_list(tcp_id, false);
                if (http_list) {
                    ret = content;
                    break;
                }
                it ++;
            }
            return ret;
        }

        static std::shared_ptr<std::vector<ahttp_impl*>> remove_http_list(int tcp_id) {
            std::shared_ptr<std::vector<ahttp_impl*>> ret;
            std::map<std::string, std::shared_ptr<http_content>>::iterator it = url_2_http.begin();
            while (it != url_2_http.end()) {
                auto content = it->second;
                if (content->contains_tcp_ip(tcp_id)) {
                    auto http_list = content->remove_http_list(tcp_id);
                    if (http_list) {
                        ret = http_list;
                        break;
                    }
                }
                it ++;
            }
            return ret;
        }

    public:
        ahttp_impl() : timer_id(-1), read_begin(false), low_priority(false),
                       dns_resolve_callback(nullptr),
                        validate_domain(false), validate_cert(false), info(std::make_shared<http_info>()) {
        }

        ~ahttp_impl() {
            cancel();
        }

        static void exec_reused_connect(int tcp_id) {
            auto content = get_content(tcp_id);
            auto http = content->get_next(tcp_id);
            if (http != nullptr) {
                http->set_local_info(tcp_id);
                http->info->set_reused_tcp(true);
                http->info->set_dns_start_time();
                http->info->set_dns_end_time();
                http->info->set_connect_start_time();
                http->info->set_connect_end_time();
                std::shared_ptr<common_callback> ccb = std::make_shared<common_callback>();
                http->send_dns_event(ccb);

                std::shared_ptr<common_callback> ccb1 = std::make_shared<common_callback>();
                http->send_connected_event(ccb1);

                if (http->request->is_use_ssl()) {
                    http->info->set_ssl_start_time();
                    http->info->set_ssl_end_time();
                    http->send_ssl_connected_event(common_callback::get());
                }
                http->info->set_request_start_time();
                http->request->get_http_data([=](std::shared_ptr<char> data, int len, int sent, int total){
                    uv_wrapper::write(tcp_id, data, len, [=](std::shared_ptr<common_callback> write_callback){
                        http->send_send_event(write_callback, sent + len, total);
                    });
                });
            }
        }

        static std::string get_unique_domain(std::string domain, int port) {
            std::stringstream ss;
            ss << domain;
            ss << ":";
            ss << port;
            return ss.str();
        }

        static void exec_new_connect(ahttp::ahttp_impl* http, std::string domain, std::string ip, int port) {
            http->info->set_connect_start_time();
            auto connected_callback = [=](std::shared_ptr<common_callback> ccb, int tcp_id) {
                if (ccb->success) {
                    http->info->set_request_start_time();
                    http->request->get_http_data([=](std::shared_ptr<char> data, int len, int sent, int total){
                        uv_wrapper::write(tcp_id, data, len, [=](std::shared_ptr<common_callback> write_callback){
                            http->send_send_event(write_callback, sent + len, total);
                        });
                    });
                } else {
                    uv_wrapper::close(tcp_id);
                }
            };
            uv_wrapper::connect(ip, port, http->request->is_use_ssl(), domain, [=](std::shared_ptr<common_callback> ccb, int tcp_id){
                //tcp connected
                mutex.lock();
                std::string uni_domain = http->get_uni_domain();
                std::shared_ptr<http_content> content = get_content(uni_domain, false);
                if (content) {
                    content->add_connected_list(tcp_id, http);
                    
                    http->info->set_connect_end_time();
                    http->set_local_info(tcp_id);
                    http->send_connected_event(ccb);
                    if (http->request->is_use_ssl()) {

                    } else {
                        connected_callback(ccb, tcp_id);
                    }
                    auto ssl = uv_wrapper::get_ssl_impl_by_tcp_id(tcp_id);
                    if (ssl) {
                        ssl->validate_domain(http->validate_domain);
                        ssl->validate_cert(http->validate_cert);
                        http->info->set_ssl_start_time();
                    }
                }
                mutex.unlock();
            }, [=](std::shared_ptr<common_callback> ccb, int tcp_id) {
                //ssl connected
                http->info->set_ssl_end_time();
                http->send_ssl_connected_event(ccb);
                if (http->request->is_use_ssl()) {
                    connected_callback(ccb, tcp_id);
                }
            }, [=](int tcp_id, std::shared_ptr<char>data, int len) {
                auto http_list = get_http_list(tcp_id);
                if (http_list) {
                    if (http_list->size() > 0) {
                        auto h = http_list->at(0);
                        h->send_read_begin_event(common_callback::get());
                        if (h->append(data, len)) {
                            mutex.lock();
                            http_list->erase(http_list->begin());
                            uv_wrapper::cancel_timer(h->timer_id);
                            mutex.unlock();
                            h->send_http_callback(common_callback::get());
                            exec_reused_connect(tcp_id);
                        }
                    }
                }
            }, [=](std::shared_ptr<common_callback> disconnect_ccb, int tcp_id) {
                mutex.lock();
                auto http_list = remove_http_list(tcp_id);
                mutex.unlock();

                if (http_list) {
                    std::vector<ahttp_impl*>::iterator it = http_list->begin();
                    while (it != http_list->end()) {
                        auto http = *it;
                        http->send_disconnected_event(disconnect_ccb);
                        it ++;
                    }
                }
            });
        }

        static void exec_direct(ahttp::ahttp_impl* http) {
            if (http && http->request != nullptr) {
                http->info->set_proxy(false);
                std::string uni_domain = http->get_uni_domain();
                std::shared_ptr<http_content> content = get_content(uni_domain, true);

                int tcp_id = -1;
                std::shared_ptr<std::vector<ahttp_impl*>> http_list;
                int close_tcp = -1;
                std::map<int, std::shared_ptr<std::vector<ahttp_impl*>>>::iterator it = content->get_begin();
                while (it != content->get_end()) {
                    auto list = it->second;
                    if (list->size() == 0) {
                        //空闲tcp
                        tcp_id = it->first;
                        if (uv_wrapper::tcp_alive(tcp_id)) {
                            http_list = list;
                            break;
                        } else {
                            close_tcp = tcp_id;
                        }
                    } else {
                        if (tcp_id < 0 || http_list->size() > list->size()) {
                            http_list = list;
                            tcp_id = it->first;
                        }
                    }
                    it ++;
                }
                if (close_tcp > 0) {
                    auto close_http_list = content->remove_http_list(close_tcp);
                    if (close_http_list) {
                        std::vector<ahttp_impl*>::iterator it_1 = close_http_list->begin();
                        while (it_1 != close_http_list->end()) {
                            auto impl = *it_1;
                            std::shared_ptr<common_callback> ccb = std::make_shared<common_callback>();
                            impl->send_disconnected_event(ccb);
                            it_1 ++;
                        }
                    }
                }
                if (http_list && http_list->size() == 0) {
                    content->add_connected_list(tcp_id, http);
                    exec_reused_connect(tcp_id);
                    return;
                }

                //没有空闲的tcp链路或者没有tcp链路
                if (content->get_all_http_num() < max_connection_num) {
                    //一个域名连接的tcp小于最大连接数
                    if (http->low_priority && content->get_all_http_num() > 0) {
                        content->add_unconnected_list(http);
                    } else {
                        //创建新的链路
                        content->add_unconnected_list(http);
                        http->info->set_reused_tcp(false);
                        if (http->request->is_ip_format(http->request->get_domain())) {
                            http->info->set_dns_start_time();
                            http->info->set_dns_end_time();
                            http->send_dns_event(common_callback::get());
                            exec_new_connect(http, http->request->get_header("host"), http->request->get_domain(), http->request->get_port());
                        } else {
                            //TODO 如果解析成多个ip后，处理第一个ip连接失败的情况
                            http->info->set_dns_start_time();
                            http->get_dns_resolve()(http->request->get_domain(), http->request->get_port(), [=](std::shared_ptr<common_callback> ccb, std::shared_ptr<std::vector<std::string>> ips){
                                http->info->set_dns_end_time();
                                http->send_dns_event(ccb);
                                if (ccb->success) {
                                    if (ips->size() > 0) {
                                        std::string ip = (*ips)[0];
                                        exec_new_connect(http, http->request->get_domain(), ip, http->request->get_port());
                                    }
                                } else {
                                    common_callback_err_wrapper::set_err_no(ccb, E_DNS_RESOLVE);
                                    http->send_http_callback(ccb);
                                }
                            });
                        }
                    }
                } else {
                    //复用tcp链路
                    content->add_unconnected_list(http);
                }
            }
        }
        static void exec_proxy(ahttp::ahttp_impl* http) {
            if (http && http->request) {
                http->request->set_uesd_proxy(true);
                http->info->set_proxy(true);

                std::string uni_domain = http->get_uni_domain();
                auto content = get_content(uni_domain, true);

                auto send_op = [=](int tcp_id) {
                    http->info->set_request_start_time();
                    http->request->get_http_data([=](std::shared_ptr<char> data, int len, int sent, int total){
                        uv_wrapper::write(tcp_id, data, len, [=](std::shared_ptr<common_callback> write_callback){
                            http->send_send_event(write_callback, sent + len, total);
                        });
                    });
                };

                if (content->get_connected_num() > 0) {
                    //存在代理服务器链路
                    int tcp_id = content->get_begin()->first;
                    auto list = content->get_begin()->second;
                    if (uv_wrapper::tcp_alive(tcp_id)) {
                        list->push_back(http);
                        if (list->size() == 0) {
                            //复用tcp
                            send_op(tcp_id);
                        }
                    }
                }

                auto connect_op = [=](std::string ip, int port) {
                    http->info->set_remote_ip_port(ip, port);
                    http->info->set_reused_tcp(false);
                    http->info->set_connect_start_time();
                    uv_wrapper::connect(ip, port, [=](std::shared_ptr<common_callback> ccb, int tcp_id) {
                        http->info->set_connect_end_time();
                        http->set_local_info(tcp_id);
                        if (http->request->is_use_ssl()) {
                            http->info->set_ssl_start_time();
                            http->info->set_ssl_end_time();
                        }

                        content->add_connected_list(tcp_id, http);
                        send_op(tcp_id);
                    }, [=](int tcp_id, std::shared_ptr<char> data, int len) {
                        mutex.lock();
                        auto http_list = get_http_list(tcp_id);
                        mutex.unlock();
                        if (http_list) {
                            if (http_list->size() > 0) {
                                auto h = http_list->at(0);
                                h->send_read_begin_event(common_callback::get());
                                if (h->append(data, len)) {
                                    mutex.lock();
                                    http_list->erase(http_list->begin());
                                    uv_wrapper::cancel_timer(h->timer_id);
                                    mutex.unlock();
                                    h->send_http_callback(common_callback::get());
                                    exec_reused_connect(tcp_id);

                                }
                            }
                        }
                    }, [=](std::shared_ptr<common_callback> ccb, int tcp_id){
                        remove_http_list(tcp_id);
                    });
                };

                if (uv_wrapper::is_ip4(proxy_host) || uv_wrapper::is_ip6(proxy_host)) {
                    connect_op(proxy_host, proxy_port);
                } else {
                    http->info->set_dns_start_time();
                    uv_wrapper::resolve(proxy_host, proxy_port, [=](std::shared_ptr<common_callback> ccb, std::shared_ptr<std::vector<std::string>> ips) {
                        http->info->set_dns_end_time();
                        if (ccb->success && ips->size() > 0) {
                            std::string ip = ips->at(0);
                            connect_op(ip, proxy_port);
                        }
                    });
                }

            }
        }
        static void exec(ahttp::ahttp_impl* http) {
            auto exec_op = [=]() {
                if (proxy_port > 0 && proxy_host.length() > 0) {
                    //使用了代理
                    exec_proxy(http);
                } else {
                    exec_direct(http);
                };
            };
            if (auto_use_proxy) {
                local_proxy::get_proxy([=](std::shared_ptr<std::map<std::string, std::string>> proxy){
                    if (proxy->find("HTTPPort") != proxy->end() && proxy->find("HTTPProxy") != proxy->end()) {
                        std::string port = proxy->at("HTTPPort");
                        proxy_host = proxy->at("HTTPProxy");
                        proxy_port = atoi(port.c_str());
                    } else {
                        proxy_host = "";
                        proxy_port = -1;
                    }
                    exec_op();
                });
            } else {
                exec_op();
            }

        }
        void exec2(std::shared_ptr<ahttp_request> model, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
            info->set_fetch_time();
            request = model;
            this->callback = callback;
            if (model->get_timeout() > 0) {
                timer_id = uv_wrapper::post_timer([=](){
                    mutex.lock();
                    std::string uni_domain = get_uni_domain();
                    if (url_2_http.find(uni_domain) != url_2_http.end()) {
                        auto content = url_2_http[uni_domain];
                        content->remove_http(this);
                    }
                    mutex.unlock();
                    info->set_response_data_size(0);
                    info->set_response_start_time();
                    info->set_response_end_time();
                    send_http_callback(common_callback_err_wrapper::get(E_TIME_OUT));
                }, model->get_timeout() * 1000, 0);
            }
            //设置调用初始化时间
            auto tp = std::chrono::system_clock::now();
            time = tp.time_since_epoch().count();

            ahttp_impl::exec(this);
        }

        void cancel() {
            mutex.lock();

            std::string uni_domain = get_uni_domain();
            if (url_2_http.find(uni_domain) != url_2_http.end()) {
                auto content = url_2_http[uni_domain];
                content->remove_http(this);
            }
            mutex.unlock();
        }

        static void set_max_connection(int max) {
            if (max > 0 && max < 16) {
                max_connection_num = max;
            }
        }

        static void set_proxy(std::string host, int port) {
            if (proxy_host != host || port != proxy_port) {
                if (proxy_port > 0 && proxy_host.length() > 0) {
                    url_2_http.erase(get_unique_domain(proxy_host, proxy_port));
                }
                proxy_host = host;
                proxy_port = port;
            }
        }

        static void set_auto_proxy(bool auto_use) {
            if (auto_use_proxy && !auto_use) {
                set_proxy("", -1);
            }
            auto_use_proxy = auto_use;
        }

        void set_high_priority() {
            low_priority = false;
        }

        void set_low_priority() {
            low_priority = true;
        }

        void is_validate_domain(bool validate) {
            validate_domain = validate;
        }

        void is_validate_cert(bool validate) {
            validate_cert = validate;
        }

        void set_dns_resolve(std::function<void(std::string url, int port, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>)> callback) {
            this->dns_resolve_callback = callback;
        }

        void set_dns_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback) {
            dns_callback = callback;
        }
        void set_connected_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback) {
            connect_callback = callback;
        }
        void set_ssl_connected_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback) {
            ssl_connect_callback = callback;
        }
        void set_read_event_callback(std::function<void(std::shared_ptr<common_callback>, long size)> callback) {
            read_callback = callback;
        }
        void set_send_event_callback(std::function<void(std::shared_ptr<common_callback>, long sent, long total)> callback) {
            send_callback = callback;
        }
        void set_read_begin_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback) {
            read_begin_callback = callback;
        }
        void set_read_end_event_callback(std::function<void(std::shared_ptr<common_callback>, long bytes)> callback) {
            read_end_callback = callback;
        }
        void set_disconnected_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback) {
            disconnect_callback = callback;
        }

        void get(std::string url, int timeout, std::shared_ptr<std::map<std::string, std::string>>header, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
            auto request = std::make_shared<ahttp_request>();
            request->set_method(ahttp_request::METHOD_GET);
            request->set_url(url);
            request->append_header(header);
            request->set_timeout(timeout);
            exec2(request, callback);
        }

        void post(std::string url, int timeout, std::shared_ptr<std::map<std::string, std::string>>header, std::shared_ptr<std::map<std::string, std::string>> data, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
            auto request = std::make_shared<ahttp_request>();
            request->set_method(ahttp_request::METHOD_POST);
            request->set_url(url);
            request->append_header(header);
            request->append_body_data(data);
            request->set_timeout(timeout);
            exec2(request, callback);
        }


        void download(std::string url, std::string file, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(long current, long total)> process_callback, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
            auto request = std::make_shared<ahttp_request>();
            request->set_method(ahttp_request::METHOD_GET);
            request->set_url(url);
            request->append_header(header);
            request->set_timeout(0);
            this->response = std::make_shared<ahttp_response>();
            this->response->set_response_data_file(file);
            if (process_callback) {
                this->set_read_event_callback([=](std::shared_ptr<common_callback> ccb, int size){
                    long total = response->get_content_length();
                    process_callback(response->get_response_data_length(), total);
                });
            }
            exec2(request, callback);
        }

        void upload(std::string url, std::string file, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(long current, long total)> process_callback, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
            auto request = std::make_shared<ahttp_request>();
            request->set_method(ahttp_request::METHOD_POST);
            request->set_url(url);
            request->append_header(header);
            request->set_timeout(0);
            request->append_body_data_from_file("file", file);
            this->response = std::make_shared<ahttp_response>();
            if (process_callback) {
                this->set_read_event_callback([=](std::shared_ptr<common_callback> ccb, int size){
                    long total = response->get_content_length();
                    process_callback(response->get_response_data_length(), total);
                });
            }
            exec2(request, callback);
        }

        std::shared_ptr<std::map<std::string, std::string>> get_debug_info() {
            return info->get_info();
        };

    private:
        void send_dns_event(std::shared_ptr<common_callback> callback) {
            if (dns_callback) {
                dns_callback(callback);
            }
        }
        void send_connected_event(std::shared_ptr<common_callback> callback) {
            if (connect_callback) {
                connect_callback(callback);
            }
        }
        void send_ssl_connected_event(std::shared_ptr<common_callback> callback) {
            if (ssl_connect_callback) {
                ssl_connect_callback(callback);
            }
        }
        void send_send_event(std::shared_ptr<common_callback> callback, long bytes, long total) {
            if (bytes >= total) {
                info->set_request_end_time();
                info->set_request_data_size((int)total);
            }
            if (send_callback) {
                send_callback(callback, bytes, total);
            }
        }
        void send_read_event(std::shared_ptr<common_callback> callback, int bytes) {
            if (read_callback) {
                read_callback(callback, bytes);
            }
        }
        void send_read_begin_event(std::shared_ptr<common_callback> callback) {
            if (!read_begin) {
                info->set_response_start_time();
                if (read_begin_callback) {
                    read_begin_callback(callback);
                }
                read_begin = true;
            }
        }
        void send_read_end_event(std::shared_ptr<common_callback> callback, long bytes) {
            info->set_response_end_time();
            if (read_end_callback) {
                read_end_callback(callback, bytes);
            }
        }
        void send_disconnected_event(std::shared_ptr<common_callback> callback) {
            if (disconnect_callback) {
                disconnect_callback(callback);
            }
        }

        void send_http_callback(std::shared_ptr<common_callback> ccb) {
            if (callback) {
                if (!response) {
                    response.reset(new ahttp_response);
                }
                callback(ccb, request, response);
            }
        }

        bool append(std::shared_ptr<char> data, int len) {
            mutex.lock();
            if (!response) {
                response.reset(new ahttp_response);
            }
            bool isEnd = response->append_response_data(data, len);
            send_read_event(common_callback::get(), len);
            if (isEnd) {
                info->set_response_data_size((int)(response->get_response_length()));
                send_read_end_event(nullptr, response->get_response_length());
            }
            mutex.unlock();
            return isEnd;
        }

        std::string get_uni_domain() {
            if (proxy_port > 0 && proxy_host.length() > 0) {
                return get_unique_domain(proxy_host, proxy_port);
            } else {
                return get_unique_domain(request->get_domain(), request->get_port());
            }

        }

        void set_local_info(int tcp_id) {
            auto tcp_info = uv_wrapper::get_info(tcp_id);
            info->set_local_ip_port((*tcp_info)["local_ip"], (*tcp_info)["local_port"]);
            info->set_remote_ip_port((*tcp_info)["remote_ip"], (*tcp_info)["remote_port"]);
        }

        std::function<void(std::string url, int port, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>)> get_dns_resolve() {
            if (dns_resolve_callback) {
                return dns_resolve_callback;
            }
            static std::function<void(std::string url, int port, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>)> default_dns_resolve = nullptr;
            if (!default_dns_resolve) {
                default_dns_resolve = std::bind(&uv_wrapper::resolve, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            }
            return default_dns_resolve;
        }

        bool is_long_time_not_exec() {
            auto tp = std::chrono::system_clock::now();
            long t = tp.time_since_epoch().count();
            return t - time > 1000000;
        }
        
        static std::map<std::string, std::shared_ptr<http_content>> url_2_http;

        static mutex_wrap mutex;
        std::shared_ptr<ahttp_response> response;
        std::shared_ptr<ahttp_request> request;
        std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback;
        std::function<void(std::shared_ptr<common_callback>)> dns_callback;
        std::function<void(std::shared_ptr<common_callback>)> connect_callback;
        std::function<void(std::shared_ptr<common_callback>)> ssl_connect_callback;
        std::function<void(std::shared_ptr<common_callback>, long, long)> send_callback;
        std::function<void(std::shared_ptr<common_callback>, long)> read_callback;
        std::function<void(std::shared_ptr<common_callback>)> read_begin_callback;
        std::function<void(std::shared_ptr<common_callback>, long)> read_end_callback;
        std::function<void(std::shared_ptr<common_callback>)> disconnect_callback;
        std::function<void(std::string url, int port, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>)> dns_resolve_callback;
        int timer_id;
        bool read_begin;
        bool validate_domain;
        bool validate_cert;
        bool low_priority;
        long time;
        std::shared_ptr<http_info> info;
        static int max_connection_num;
        static std::string proxy_host;
        static int proxy_port;
        static bool auto_use_proxy;
        typedef enum http_event_ {
            DNS,
            CONNECT,
            REQUEST,
            RESPONSE,
        } http_event;
    };

    std::map<std::string, std::shared_ptr<ahttp::ahttp_impl::http_content>> ahttp::ahttp_impl::url_2_http;
    int ahttp::ahttp_impl::proxy_port = -1;
    std::string ahttp::ahttp_impl::proxy_host = "";
    bool ahttp::ahttp_impl::auto_use_proxy = false;

    int ahttp::ahttp_impl::max_connection_num = 4;
    mutex_wrap ahttp::ahttp_impl::mutex;

    ahttp::ahttp() : impl(new ahttp_impl) {

    }

    ahttp::~ahttp() {
        cancel();
    }

    void ahttp::set_auto_proxy(bool auto_use) {
        ahttp_impl::set_auto_proxy(auto_use);
    }

    void ahttp::set_proxy(std::string host, int port) {
        ahttp_impl::set_proxy(host, port);
    }

    void ahttp::set_max_connection(int max) {
        ahttp_impl::set_max_connection(max);
    }

    void ahttp::set_low_priority() {
        impl->set_low_priority();
    }

    void ahttp::set_high_priority() {
        impl->set_high_priority();
    }

    void ahttp::is_validate_domain(bool validate) {
        impl->is_validate_domain(validate);
    }

    void ahttp::is_validate_cert(bool validate) {
        impl->is_validate_cert(validate);
    }

    void ahttp::cancel() {
        impl->cancel();
    }

    void ahttp::exec(std::shared_ptr<ahttp_request> model, std::function<void(std::shared_ptr<common_callback>ccb, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
        impl->exec2(model, callback);
    }

    void ahttp::set_dns_resolve(std::function<void(std::string url, int port, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<std::vector<std::string>>)>)> callback) {
        impl->set_dns_resolve(callback);
    }

    void ahttp::set_dns_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback) {
        impl->set_dns_event_callback(callback);
    }
    void ahttp::set_connected_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback) {
        impl->set_connected_event_callback(callback);
    }

    void ahttp::set_ssl_connected_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback) {
        impl->set_ssl_connected_event_callback(callback);
    }

    void ahttp::set_read_event_callback(std::function<void(std::shared_ptr<common_callback>, long size)> callback) {
        impl->set_read_event_callback(callback);
    }

    void ahttp::set_send_event_callback(std::function<void(std::shared_ptr<common_callback>, long, long)> callback) {
        impl->set_send_event_callback(callback);
    }

    void ahttp::set_read_begin_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback) {
        impl->set_read_begin_event_callback(callback);
    }

    void ahttp::set_read_end_event_callback(std::function<void(std::shared_ptr<common_callback>, long)> callback) {
        impl->set_read_end_event_callback(callback);
    }

//    void ahttp::set_disconnected_event_callback(std::function<void(std::shared_ptr<common_callback>)> callback) {
//        impl->set_disconnected_event_callback(callback);
//    }

    void ahttp::get(std::string url, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
        impl->get(url, ahttp_request::get_default_timeout(), header, callback);
    }

    void ahttp::get(std::string url, int timeout, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
        impl->get(url, timeout, header, callback);
    }

    void ahttp::post(std::string url, std::shared_ptr<std::map<std::string, std::string>> header, std::shared_ptr<std::map<std::string, std::string>> data, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
        impl->post(url, ahttp_request::get_default_timeout(), header, data, callback);
    }

    void ahttp::post(std::string url, int timeout, std::shared_ptr<std::map<std::string, std::string>> header, std::shared_ptr<std::map<std::string, std::string>> data, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
        impl->post(url, timeout, header, data, callback);
    }

    void ahttp::download(std::string url, std::string file, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(long current, long total)> process_callback, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
        impl->download(url, file, header, process_callback, callback);
    }

    void ahttp::upload(std::string url, std::string file, std::shared_ptr<std::map<std::string, std::string>> header, std::function<void(long current, long total)> process_callback, std::function<void(std::shared_ptr<common_callback>, std::shared_ptr<ahttp_request>, std::shared_ptr<ahttp_response>)> callback) {
        impl->upload(url, file, header, process_callback, callback);
    }

    std::shared_ptr<std::map<std::string, std::string>> ahttp::get_network_info() {
        return impl->get_debug_info();
    }

}
