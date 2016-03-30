//
// Created by liuke on 16/1/13.
//

#include "tcp_wrap_default.h"
#include "protocol.h"
#include <network/tcp.h>
#include <thread/timer.h>
#include <log/log_wrap.h>
#include <json/json_wrap.h>
#include <thread/timer_wrap.h>
#include <thread/thread_wrap.h>
#include <thread/thread_define.h>
#include <algorithm/compress.h>

namespace plan9
{
    class tcp_wrap_default::tcp_wrap_default_impl {

    public:
        tcp_wrap_default_impl() : tcp_(new tcp),
                                  connected(false),
                                  timer_(new plan9::timer),
                                  ping_timer_(new plan9::timer),
                                  check_network_timer_(new plan9::timer),
                                  next_connect_time(1000),
                                  ping_interval(30 * 1000),
                                  buf_size(0),
                                  network_disconnect_interval(ping_interval * 3),
                                  compress(false),
                                  encrypt(false)  {
            tcp_->set_connect_handler([=](bool connect) {
                thread_wrap::post_background([=](){
                    if (connect) {
                        do_ping();
                        next_connect_time = 1000;
                        timer_->cancel();
                    } else {
                        timer_->start([=]() {
                            tcp_->reconnect();
                        }, next_connect_time);

                        if (next_connect_time > 60000) {
                            next_connect_time = 1000;
                        } else {
                            next_connect_time *= 2;
                        }
                    }
                    send_connect_handler(connect);
                });
            });

            tcp_->set_read_handler([=](char data[], int len){
                thread_wrap::post_background([=](){
                    do_ping();

                    std::copy(data, data + len, buf);
                    buf_size += len;

                    std::tuple<bool, int, char, char, char, int, char*> item = protocol::get_protocol(buf, buf_size);
                    if (std::get<0>(item)) {
                        //解析成功
                        if (protocol::is_pong_type(item)) {
                            //pong
                            log_wrap::net().d("recv pong from server, id : ", std::get<1>(item),
                                              " version : ", (int)std::get<2>(item),
                                              " server type : ", (int)std::get<3>(item),
                                              " data type : ", (int)std::get<4>(item),
                                              " data len : ", std::get<5>(item));
                        } else if (protocol::is_normal_json_type(item)) {
                            char* c = std::get<6>(item);
                            int l = std::get<5>(item);
                            std::string str(c, l);
                            log_wrap::net().d("recv json from server, id : ", std::get<1>(item),
                                              " version : ", (int)std::get<2>(item),
                                              " server type : ", (int)std::get<3>(item),
                                              " data type : ", (int)std::get<4>(item),
                                              " data len : ", std::get<5>(item),
                                              " data json : ", str);
                            bool error;
                            Json::Value data = json_wrap::parse(str, &error);
                            if (!error) {
                                if (data.isMember("aux") && data["aux"].isMember("id")) {
                                    std::string id = data["aux"]["id"].asString();
                                    if (send_map.find(id) != send_map.end()) {
                                        auto callback = send_map[id];
                                        callback(data);
                                        send_map.erase(id);
                                    } else {
                                        send_read_handler(str);
                                    }
                                } else {
                                    log_wrap::net().e("recv data is illegal json format : ", str);
                                }
                            } else {
                                log_wrap::net().e("recv data is illegal json format : ", str);
                                send_read_handler(str);
                            }
                        } else if (protocol::is_normal_string_type(item)) {
                            std::string str(std::get<6>(item), std::get<5>(item));
                            log_wrap::net().d("recv string from server, id : ", std::get<1>(item),
                                              " version : ", (int)std::get<2>(item),
                                              " server type : ", (int)std::get<3>(item),
                                              " data type : ", (int)std::get<4>(item),
                                              " data len : ", std::get<5>(item),
                                              " data string : ", str);
                            send_read_handler(str);
                        } else if (protocol::is_compress_json_type(item)) {
                            log_wrap::net().d("recv compress data : ",
                                              plan9::util::instance().char_to_string(data, len));
                        } else if (protocol::is_encrypt_json_type(item)) {
                            log_wrap::net().d("recv encrypt data : ",
                                              plan9::util::instance().char_to_string(data, len));
                        } else if (protocol::is_encrypt_compress_json_type(item)) {
                            log_wrap::net().d("recv encrypt and compress data : ",
                                              plan9::util::instance().char_to_string(data, len));
                        } else {
                            log_wrap::net().e("recv data : ", plan9::util::instance().char_to_string(data, len));
                        }
                        removeFinishProtocol();
                    }

                });
            });

            tcp_->set_write_handler([=](char data[], int len) {
                thread_wrap::post_background([=]() {
                    log_wrap::net().i("send data : ", util::instance().char_to_string(data, len));
                    do_ping();
                });
            });
        }

        void connect(std::string ip, int port) {
            tcp_->connect(ip, port);
        }


        void send(int id, char type, Json::Value msg, std::function<void(Json::Value)> callback, int timeout) {
            if (msg.isMember("aux") && msg["aux"].isMember("id")) {
                std::string sid = msg["aux"]["id"].asString();
                send_map[sid] = callback;

                if (compress && encrypt) {
                    write_compress_encrypt(id, type, msg);
                } else if (compress) {
                    write_compress(id, type, msg);
                } else {
                    write(id, type, msg);
                }

#ifdef THREAD_ENABLE
//                if (timeout > 0) {
//                    timer_wrap::instance().create_timer(timeout, [=]() mutable {
//                        send_map.erase(sid);
//                        msg["result"]["success"] = false;
//                        msg["result"]["reason"] = "timeout";
//                        callback(msg);
//                    });
//                }
#endif
            } else {
                log_wrap::net().e("send data is illegal json format : ", json_wrap::to_string(msg));
            }
        }

        void set_connect_handler(std::function<void(bool)> function) {
            connect_handler = function;
        }


        void set_read_handler(std::function<void(std::string)> function) {
            read_handler = function;
        }

        void send_connect_handler(bool connect) {
            if (connected == connect) return;
            connected = connect;
            if (connect_handler != nullptr) {
                connect_handler(connect);
            }
        }

        void send_read_handler(std::string msg) {
            if (read_handler != nullptr) {
                read_handler(msg);
            }
        }

        void set_compress(bool compress) {
            this->compress = compress;
        }

        void set_encrypt(bool encrypt) {
            this->encrypt = encrypt;
        }


    private:

        void write(int id, char type, Json::Value json) {
            std::string str = json_wrap::to_string(json);
            if (str.length() > 0xFFF0) {
                log_wrap::net().e("send data from tcp_wrap_default error, the data length is over 65536 byte : ", str);
            } else {
                log_wrap::net().d("send data from tcp_wrap_default : ", str);
                const char* data = str.c_str();
                std::tuple<char*, int> p = protocol::create_protocol(id, protocol::get_protocol_version(), type, protocol::NORMAL_JSON_DATA_TYPE, str.length(), data);
                tcp_->write(std::get<0>(p), std::get<1>(p));
            }
        }

        void write_compress(int id, char type, Json::Value json) {
            std::string str = json_wrap::to_string(json);
            if (str.length() > 0xFFF0) {
                log_wrap::net().e("send data from tcp_wrap_default error, the data length is over 65536 byte : ", str);
            } else {
                log_wrap::net().d("send compress data from tcp_wrap_default : ", str);
                const char* data = str.c_str();
                int buf_len = compress_wrap::maybe_compressed_size(str.length());
                char buf[buf_len];
                compress_wrap::compress(data, str.length(), buf, &buf_len);
                std::tuple<char*, int> p = protocol::create_protocol(id, protocol::get_protocol_version(), type, protocol::COMPRESS_JSON_DATA_TYPE, buf_len, buf);
                char* protocol = std::get<0>(p);
                tcp_->write(protocol, std::get<1>(p));
                delete(protocol);
            }
        }

        void write_compress_encrypt(int id, char type, Json::Value json) {
            std::string str = json_wrap::to_string(json);
            if (str.length() > 0xFFF0) {
                log_wrap::net().e("send data from tcp_wrap_default error, the data length is over 65536 byte : ", str);
            } else {
                log_wrap::net().d("send encrypt and compress data from tcp_wrap_default : ", str);
//                const char* data = str.c_str();
//                char buf[compress_wrap::maybe_compressed_size(str.length())];
//                int buf_len = 0;
//                compress_wrap::compress(data, str.length(), buf, &buf_len);
//                std::tuple<char*, int> p = protocol::create_protocol(id, protocol::get_protocol_version(), type, protocol::ENCRYPT_COMPRESS_JSON_DATA_TYPE, buf_len, buf);
//                tcp_->write(std::get<0>(p), std::get<1>(p));
            }
        }


        void do_ping() {
            ping_timer_->cancel();
            ping_timer_->start([=]() {
                thread_wrap::post_background([=]() {
                    std::tuple<char *, int> p = protocol::create_ping_protocol(0, 1);
                    int len = std::get<1>(p);
                    if (len > 0) {
                        log_wrap::net().d("write ping to server");
                        tcp_->write(std::get<0>(p), len);
                    }
                });
            }, ping_interval);

            check_network_timer_->cancel();
            check_network_timer_->start([=]() {
                thread_wrap::post_background([=]() {
                    timer_->start([=]() {
                        tcp_->reconnect();
                    }, next_connect_time);

                    if (next_connect_time > 60000) {
                        next_connect_time = 1000;
                    } else {
                        next_connect_time *= 2;
                    }
                    send_connect_handler(false);
                });
            }, network_disconnect_interval);
        }

        void removeFinishProtocol() {
            buf_size = protocol::remove_first_protocol(buf, buf_size);
        }

    private:
        std::shared_ptr<tcp> tcp_;
        std::function<void(bool)> connect_handler;
        std::function<void(std::string)> read_handler;
        bool connected;
        std::shared_ptr<timer> timer_;
        std::shared_ptr<timer> ping_timer_;
        std::shared_ptr<timer> check_network_timer_;

        long ping_interval;
        long network_disconnect_interval;//多长时间没有数据表示断网

        long next_connect_time;
        std::map<std::string, std::function<void(Json::Value)>> send_map;

        char buf[0xFFFF];
        int buf_size = 0;

        bool compress;
        bool encrypt;
    };



    tcp_wrap_default::tcp_wrap_default() : impl(new tcp_wrap_default_impl) {
    }

    tcp_wrap_default tcp_wrap_default::instance() {
        static tcp_wrap_default tcp;
        return tcp;
    }

    void tcp_wrap_default::connect(std::string ip, int port) {
        impl->connect(ip, port);
    }

    void tcp_wrap_default::set_connect_handler(std::function<void(bool)> function) {
        impl->set_connect_handler(function);
    }

    void tcp_wrap_default::set_read_handler(std::function<void(std::string)> function) {
        impl->set_read_handler(function);
    }

    void tcp_wrap_default::send(int id, char type, Json::Value msg, std::function<void(Json::Value)> callback,
                                int timeout) {
        impl->send(id, type, msg, callback, timeout);
    }


    void tcp_wrap_default::set_compress(bool compress) {
        impl->set_compress(compress);
    }

    void tcp_wrap_default::set_encrypt(bool encrypt) {
        impl->set_encrypt(encrypt);
    }
}