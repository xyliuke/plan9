//
// Created by liuke on 16/1/2.
//

#include "tcp.h"
#include <log/log_wrap.h>
#include <boost/asio.hpp>
#include <thread>
#include <thread/thread_define.h>

namespace plan9
{
    static const char netwrok_begin = '^';
    static const int BUF_SIZE = 10240;

    class tcp::tcp_impl {

    public:

        tcp_impl() : io_service(new boost::asio::io_service),
                     isconnect(false),
                     port(-1),
                     enable_ping(false),
                     ping_delay_second(20),
                     check_disconnect_interval(30) {

        }

        void on_read(const boost::system::error_code &error_code, std::size_t bytes) {
            if (error_code.value() == 0) {
                //成功
                has_connect();
                deal_with_data(bytes);
                socket_->async_read_some(boost::asio::buffer(read_tmp_buf), std::bind(&tcp_impl::on_read, this, std::placeholders::_1, std::placeholders::_2));

            } else if (error_code.value() == boost::asio::error::eof){
                //服务器断开
                log_wrap::net().e("recv fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->isconnect = false;
                sendConnectState(false);
            } else {
                log_wrap::net().e("recv fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->isconnect = false;
                sendConnectState(false);
            }
        }

        void on_write(const boost::system::error_code &error_code, std::size_t bytes) {
            if (error_code.value() == 0) {
                //成功
                do_ping();
                if (write_handle != nullptr) {
                    char rr[BUF_SIZE];
                    int len = get_len_from_header(write_buf);
                    std::copy(write_buf + 6, write_buf + len + 6, rr);
                    rr[len] = '\0';
                    std::string w(rr);
                    write_handle(w);
                }
            } else if (error_code.value() == boost::asio::error::eof){
                //服务器断开
                log_wrap::net().e("send fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->isconnect = false;
                sendConnectState(false);
            } else {
                log_wrap::net().e("send fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->isconnect = false;
                sendConnectState(false);
            }
        }

        void on_connect(const boost::system::error_code &error_code) {
            if (error_code.value() == 0) {
                log_wrap::net().i("connected ip : ", ip, " port : ", port);
                this->isconnect = true;
                socket_->async_read_some(boost::asio::buffer(read_tmp_buf), std::bind(&tcp_impl::on_read, this, std::placeholders::_1, std::placeholders::_2));
                sendConnectState(true);
            } else {
                log_wrap::net().e("disconnected ip : ", ip, " port : ", port, " reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->isconnect = false;
                sendConnectState(false);
            }

        }

        void on_ping(const boost::system::error_code &error_code) {
            if (error_code.value() == boost::asio::error::operation_aborted) {
                //timer被取消
                return;
            }

            if (!isconnect) {
                return;
            }
            int len = wrap_ping_data();
            socket_->async_write_some(boost::asio::buffer(write_buf, (size_t)len), std::bind(&tcp_impl::on_write, this, std::placeholders::_1, std::placeholders::_2));
        }

        void on_disconnect(const boost::system::error_code& error_code) {
            if (error_code.value() == boost::asio::error::operation_aborted) {
                //timer被取消
            } else {
                //长时间没有与服务器通信
                if (isconnect) {
                    isconnect = false;
                    log_wrap::net().e("disconnected to server");
                    if (connect_handle != nullptr) {
                        connect_handle(false);
                    }
                }
            }
        }

        void connect(std::string ip_str, int port) {
            using namespace boost::asio;
            this->ip = ip_str;
            this->port = port;

            ip::tcp::endpoint ep(ip::address::from_string(ip_str), port);
            socket_.reset(new ip::tcp::socket(*io_service));
            socket_->async_connect(ep, std::bind(&tcp_impl::on_connect, this, std::placeholders::_1));
            work.reset(new io_service::work(*io_service));
#ifdef THREAD_ENABLE
            thread_.reset(new std::thread(std::bind(&tcp_impl::run, this)));
#else
            run();
#endif
        }

        void run() {
            io_service->run();
        }

        void reconnect() {
            if (isconnect) return;
            if (this->port < 0) {
                log_wrap::net().e("can not reconnect server, because have not legal ip and port");
                return;
            }
            log_wrap::net().e("reconnect to server");
            using namespace boost::asio;
            ip::tcp::endpoint ep(ip::address::from_string(this->ip), this->port);
            socket_.reset(new ip::tcp::socket(*io_service));
            socket_->async_connect(ep, std::bind(&tcp_impl::on_connect, this, std::placeholders::_1));
        }

        void write(network_server_type type, std::string msg) {
            if (!isconnect) {
                log_wrap::net().e("can not write data to server, because have not connect the server");
                return;
            }

            int len = wrap_string_data(static_cast<char>(type), msg);
            socket_->async_write_some(boost::asio::buffer(write_buf, (size_t)len), std::bind(&tcp_impl::on_write, this, std::placeholders::_1, std::placeholders::_2));
        }

        void enablePing(bool enable) {
            if (enable != enable_ping) {
                enable_ping = enable;
                do_ping();
            }
        }


        void close() {
            io_service->stop();
        }

        void set_connect_handler(std::function<void(bool)> function) {
            connect_handle = function;
        }

        void set_read_handler(std::function<void(std::string msg)> function) {
            read_handle = function;
        }

        void set_write_handler(std::function<void(std::string msg)> function) {
            write_handle = function;
        }

    private:

        int wrap_string_data(char type, std::string msg) {
            write_buf[0] = netwrok_begin;
            write_buf[1] = (char)(type | 0x01);
            int len = (int)msg.size();
            write_buf[2] = (char)((len & 0xFF000000) >> 24);
            write_buf[3] = (char)((len & 0xFF000000) >> 16);
            write_buf[4] = (char)((len & 0xFF000000) >> 8);
            write_buf[5] = (char)(len & 0x000000FF);
            std::copy(msg.begin(), msg.end(), write_buf + 6);
            return len + 6;
        }

        int wrap_ping_data() {
            write_buf[0] = netwrok_begin;
            write_buf[1] = 0x0002;
            std::string msg = "ping";
            int len = (int)msg.size();
            write_buf[2] = (char)(len & 0xFF000000);
            write_buf[3] = (char)(len & 0x00FF0000);
            write_buf[4] = (char)(len & 0x0000FF00);
            write_buf[5] = (char)(len & 0x000000FF);
            std::copy(msg.begin(), msg.end(), write_buf + 6);
            return len + 6;
        }

        char get_type_from_header(char* buf) {
            return buf[1];
        }
        int get_len_from_header(char* buf) {
            int a1 = (unsigned char)buf[2];
            int a2 = (unsigned char)buf[3];
            int a3 = (unsigned char)buf[4];
            int a4 = (unsigned char)buf[5];
            return (a1 << 24) + (a2 << 16) + (a3 << 8) + a4;
        }

        bool isStringType(char type) {
            return type == 0x01;
        }

        bool isPingType(char type) {
            return type == 0x02;
        }

        bool isPongType(char type) {
            return type == 0x03;
        }



        void do_ping() {
            if (enable_ping) {
                if (ping_timer == nullptr) {
                    ping_timer.reset(new boost::asio::deadline_timer(*(this->io_service)));
                    ping_timer->expires_from_now(boost::posix_time::seconds(ping_delay_second));
                    ping_timer->async_wait(std::bind(&tcp_impl::on_ping, this, std::placeholders::_1));
                } else {
                    ping_timer->cancel();
                    ping_timer->expires_from_now(boost::posix_time::seconds(ping_delay_second));
                    ping_timer->async_wait(std::bind(&tcp_impl::on_ping, this, std::placeholders::_1));
                }

                if (check_disconnect_timer == nullptr) {
                    check_disconnect_timer.reset(new boost::asio::deadline_timer(*(this->io_service)));
                    check_disconnect_timer->expires_from_now(boost::posix_time::seconds(check_disconnect_interval));
                    check_disconnect_timer->async_wait(std::bind(&tcp_impl::on_disconnect, this, std::placeholders::_1));
                }

            } else {
                if (ping_timer != nullptr) {
                    ping_timer->cancel();
                    ping_timer.reset();
                }

                if (check_disconnect_timer != nullptr) {
                    check_disconnect_timer->cancel();
                    check_disconnect_timer.reset();
                }
            }
        }

        void has_connect() {
            if (enable_ping) {
                if (check_disconnect_timer != nullptr) {
                    check_disconnect_timer->cancel();
                    check_disconnect_timer->expires_from_now(boost::posix_time::seconds(check_disconnect_interval));
                    check_disconnect_timer->async_wait(std::bind(&tcp_impl::on_disconnect, this, std::placeholders::_1));
                }
            }
        }

        //处理接收到的数据
        void deal_with_data (size_t len) {
            log_wrap::net().i("recv data size : ", len);
            opPacket(len);
            if (read_buf[0] == '^') {
                opMsg();
            } else {
                log_wrap::net().e("the data is illegal protocol");
            }

        }

        //将数据复制到缓冲区
        void opPacket(size_t len) {
            memcpy(read_buf + read_size, read_tmp_buf, len);
            read_size += len;
        }

        void opMsg() {
            while (true) {
                int len = get_len_from_header(read_buf);
                if (read_buf[0] == '^' && (len + 6) <= read_size ) {
                    char type = get_type_from_header(read_buf);
                    if (isStringType(type)) {
                        std::string msg = getStringData();
                        log_wrap::net().i("recv data : ", msg);
                        sendReadState(msg);
                    } else if (isPongType(type)) {
                        log_wrap::net().i("recv data : pong");
                    } else if(isPingType(type)) {
                        log_wrap::net().i("recv data : ping");
                    } else {
                        log_wrap::net().i("recv other protocol data type : ", type);
                    }
                    clearFirstCompleteMsg();
                } else {
                    break;
                }
            }
        }

        void clearFirstCompleteMsg() {
            int len = get_len_from_header(read_buf);
            std::copy(read_buf + 6 + len, read_buf + read_size, read_buf);
            read_size -= (len + 6);
        }

        std::string getStringData() {
            int len = get_len_from_header(read_buf);
            char buf[BUF_SIZE];
            std::copy(read_buf + 6, read_buf + 6 + len, buf);
            buf[len] = '\0';
            std::string ret(buf);
            return ret;
        }

        void sendConnectState(bool connect) {
            if (connect_handle != nullptr) {
                connect_handle(connect);
            }
        }

        void sendWriteState(std::string msg) {
            if (write_handle != nullptr) {
                write_handle(msg);
            }
        }

        void sendReadState(std::string msg) {
            if (read_handle != nullptr) {
                read_handle(msg);
            }
        }


    private:
        std::function<void(bool)> connect_handle;
        std::function<void(std::string)> read_handle;
        std::function<void(std::string)> write_handle;
        std::shared_ptr<boost::asio::io_service> io_service;
        std::shared_ptr<std::thread> thread_;
        std::shared_ptr<boost::asio::io_service::work> work;

        std::string ip;
        int port;
        bool isconnect;
        bool enable_ping;
        int ping_delay_second;
        int check_disconnect_interval;//断线检测间隔

        std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
        std::shared_ptr<boost::asio::deadline_timer> ping_timer;//定期发送ping包
        std::shared_ptr<boost::asio::deadline_timer> check_disconnect_timer;//这个是和ping_timer同时使用,enable_ping为false,则这个不起作用

        char read_buf[BUF_SIZE];//读缓存
        size_t read_size = 0;//上次放在读缓存区的字节数
        char read_tmp_buf[2048];//读的临时缓冲区
        char write_buf[BUF_SIZE];//写缓存
    };


    tcp::tcp() : impl(new tcp_impl) {

    }

    void tcp::connect(std::string ip, int port) {
        impl->connect(ip, port);
    }

    void tcp::set_connect_handler(std::function<void(bool)> function) {
        impl->set_connect_handler(function);
    }

    void tcp::write(std::string msg) {
        impl->write(network_server_type::SERVER_CONNECT, msg);
    }

    void tcp::write(network_server_type type, std::string msg) {
        impl->write(type, msg);
    }

    void tcp::set_read_handler(std::function<void(std::string msg)> function) {
        impl->set_read_handler(function);
    }

    void tcp::set_write_handler(std::function<void(std::string msg)> function) {
        impl->set_write_handler(function);
    }

    void tcp::reconnect() {
        impl->reconnect();
    }

    void tcp::close() {
        impl->close();
    }

    void tcp::enable_ping() {
        impl->enablePing(true);
    }
}