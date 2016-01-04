//
// Created by liuke on 16/1/2.
//

#include "tcp.h"
#include <log/log_wrap.h>
#include <boost/asio.hpp>

namespace plan9
{
    static char netwrok_begin = '^';
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
                bool succ;
                bool pong;
                std::string msg = unwrap_data(bytes, &succ, &pong);
                if (succ) {
                    log_wrap::net().i("recv data : ", msg);
                    if (pong) {

                    } else {
                        if (read_handle != nullptr) {
                            read_handle(msg);
                        }
                    }
                } else {

                }

                socket_->async_read_some(boost::asio::buffer(read_tmp_buf), std::bind(&tcp_impl::on_read, this, std::placeholders::_1, std::placeholders::_2));

            } else if (error_code.value() == boost::asio::error::eof){
                //服务器断开
                log_wrap::net().e("recv fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->isconnect = false;
                if (connect_handle != nullptr) {
                    connect_handle(false);
                }
            } else {
                log_wrap::net().e("recv fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->isconnect = false;
                if (connect_handle != nullptr) {
                    connect_handle(false);
                }
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
                    log_wrap::net().i("write data : ", w);
                    write_handle(w);
                }
            } else if (error_code.value() == boost::asio::error::eof){
                //服务器断开
                log_wrap::net().e("write fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->isconnect = false;
                if (connect_handle != nullptr) {
                    connect_handle(false);
                }
            } else {
                log_wrap::net().e("write fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->isconnect = false;
                if (connect_handle != nullptr) {
                    connect_handle(false);
                }
            }
        }

        void on_connect(const boost::system::error_code &error_code) {
            if (error_code.value() == 0) {
                log_wrap::net().i("connected ip : ", ip, " port : ", port);
                this->isconnect = true;
                socket_->async_read_some(boost::asio::buffer(read_tmp_buf), std::bind(&tcp_impl::on_read, this, std::placeholders::_1, std::placeholders::_2));
                if (connect_handle != nullptr) {
                    connect_handle(true);
                }
            } else {
                log_wrap::net().e("disconnected ip : ", ip, " port : ", port, " reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->isconnect = false;
                if (connect_handle != nullptr) {
                    connect_handle(false);
                }
            }

        }

        void on_ping(const boost::system::error_code &error_code) {
            if (error_code.value() == boost::asio::error::operation_aborted) {
                //timer被取消
                return;
            }

            if (!isconnect) {
                log_wrap::net().e("can not write data to server, becasue have not connect the server");
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
                log_wrap::net().e("disconnected to server");
                if (connect_handle != nullptr) {
                    connect_handle(true);
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
            io_service->run();
        }

        void reconnect() {
            if (isconnect) return;
            if (this->port < 0) {
                log_wrap::net().e("can not reconnect server, becasue have not legal ip and port");
                return;
            }
            using namespace boost::asio;
            ip::tcp::endpoint ep(ip::address::from_string(this->ip), this->port);
            socket_.reset(new ip::tcp::socket(*io_service));
            socket_->async_connect(ep, std::bind(&tcp_impl::on_connect, this, std::placeholders::_1));
        }

        void write(std::string msg) {
            if (!isconnect) {
                log_wrap::net().e("can not write data to server, becasue have not connect the server");
                return;
            }
            int len = wrap_data(msg);
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
        int wrap_data(std::string msg) {
            write_buf[0] = netwrok_begin;
            write_buf[1] = 0x0001;
            int len = (int)msg.size();
            write_buf[2] = (char)(len & 0xFF000000);
            write_buf[3] = (char)(len & 0x00FF0000);
            write_buf[4] = (char)(len & 0x0000FF00);
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

        std::string unwrap_data(size_t size, bool* success, bool* pong) {
            if (read_size == 0) {
                memcpy(read_buf, read_tmp_buf, size);
                read_size = size;
            } else {
                memcpy(read_buf + read_size, read_tmp_buf, size);
                read_size += size;
            }
            int p_len = get_len_from_header(read_buf);
            if (read_size - 6 < p_len) {
                //还有数据没有读完
                *success = false;
            } else {
                *success = true;

                char type = get_type_from_header(read_buf);
                *pong = false;
                if (type == 0x0001) {
                    char rr[BUF_SIZE];
                    std::copy(read_buf + 6, read_buf + 6 + p_len, rr);
                    rr[p_len] = '\0';
                    std::string ret(rr);
                    read_size = 0;
                    return ret;
                } else if (type == 0x0003){
                    //pong包
                    *pong = true;
                    char rr[BUF_SIZE];
                    std::copy(read_buf + 6, read_buf + 6 + p_len, rr);
                    rr[p_len] = '\0';
                    std::string ret(rr);
                    read_size = 0;
                    return ret;
                } else {
                    char rr[BUF_SIZE];
                    std::copy(read_buf + 6, read_buf + 6 + p_len, rr);
                    rr[p_len] = '\0';
                    std::string ret(rr);
                    read_size = 0;
                    return ret;
                }
            }
            return "";
        }

        char get_type_from_header(char* buf) {
            return buf[1];
        }
        int get_len_from_header(char* buf) {
            int a1 = buf[2];
            int a2 = buf[3];
            int a3 = buf[4];
            int a4 = buf[5];
            return (a1 << 24) + (a2 << 16) + (a3 << 8) + a4;
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

    private:
        std::function<void(bool)> connect_handle;
        std::function<void(std::string)> read_handle;
        std::function<void(std::string)> write_handle;
        std::shared_ptr<boost::asio::io_service> io_service;

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
        char read_tmp_buf[BUF_SIZE];//读的临时缓冲区
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
        impl->write(msg);
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