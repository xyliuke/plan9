//
// Created by liuke on 16/1/2.
//

#include "tcp_wrap.h"
#include <log/log_wrap.h>
#include <boost/asio.hpp>



namespace plan9
{

    static char netwrok_begin = '^';
    static const int BUF_SIZE = 10240;

    class tcp_wrap::tcp_wrap_impl {

    public:

        tcp_wrap_impl() : io_service(new boost::asio::io_service) {

        }

        void on_read(const boost::system::error_code &error_code, std::size_t bytes) {
            if (error_code.value() == 0) {
                //成功
                bool succ;
                std::string msg = unwrap_data(bytes, &succ);
                if (succ) {
                    log_wrap::net().e("recv data : ", msg);
                    if (read_handle != nullptr) {
                        read_handle(msg);
                    }
                } else {

                }

                socket_->async_read_some(boost::asio::buffer(read_tmp_buf), std::bind(&tcp_wrap_impl::on_read, this, std::placeholders::_1, std::placeholders::_2));

            } else if (error_code.value() == boost::asio::error::eof){
                //服务器断开
                log_wrap::net().e("recv fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
            } else {
                log_wrap::net().e("recv fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
            }
        }

        void on_write(const boost::system::error_code &error_code, std::size_t bytes) {
            if (error_code.value() == 0) {
                //成功
                log_wrap::net().e("write data : ");
            } else if (error_code.value() == boost::asio::error::eof){
                //服务器断开
                log_wrap::net().e("write fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
            } else {
                log_wrap::net().e("write fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
            }
        }

        void connect_handler(const boost::system::error_code &error_code) {
            if (error_code.value() == 0) {
                log_wrap::net().i("connected ip : ", ip, " port : ", port);
                socket_->async_read_some(boost::asio::buffer(read_tmp_buf), std::bind(&tcp_wrap_impl::on_read, this, std::placeholders::_1, std::placeholders::_2));
                if (connect_handle != nullptr) {
                    connect_handle(true);
                }
            } else {
                log_wrap::net().e("disconnected ip : ", ip, " port : ", port, " reason : ", error_code.message(), "; error_code : ", error_code.value());
                if (connect_handle != nullptr) {
                    connect_handle(false);
                }
            }

        }

        void connect(std::string ip_str, int port) {
            using namespace boost::asio;
            this->ip = ip_str;
            this->port = port;
            ip::tcp::endpoint ep(ip::address::from_string(ip_str), port);
            socket_.reset(new ip::tcp::socket(*io_service));
            socket_->async_connect(ep, std::bind(&tcp_wrap_impl::connect_handler, this, std::placeholders::_1));
            io_service->run();
        }

        void write(std::string msg) {
            int len = wrap_data(msg);
            socket_->async_write_some(boost::asio::buffer(write_buf, (size_t)len), std::bind(&tcp_wrap_impl::on_write, this, std::placeholders::_1, std::placeholders::_2));
        }

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

        std::string unwrap_data(size_t size, bool* success) {
            if (read_size == 0) {
                memcpy(read_buf, read_tmp_buf, size);
                read_size = size;
            } else {
                memcpy(read_buf + read_size, read_tmp_buf, size);
                read_size += size;
            }
            int p_len = get_len_from_header();
            if (read_size - 6 < p_len) {
                //还有数据没有读完
                *success = false;
            } else {
                *success = true;
                char rr[10240];
                std::copy(read_buf + 6, read_buf + 6 + p_len, rr);
                rr[p_len] = '\0';
                std::string ret(rr);
                read_size = 0;
                return ret;
            }
            return "";
        }

        char get_type_from_header() {
            return read_buf[1];
        }
        int get_len_from_header() {
            int a1 = read_buf[2];
            int a2 = read_buf[3];
            int a3 = read_buf[4];
            int a4 = read_buf[5];
            return (a1 << 24) + (a2 << 16) + (a3 << 8) + a4;
        }

        void set_connect_handler(std::function<void(bool)> function) {
            connect_handle = function;
        }

        void close() {
            io_service->stop();
        }

        void set_read_handler(std::function<void(std::string msg)> function) {
            read_handle = function;
        }

    private:
        std::function<void(bool)> connect_handle;
        std::function<void(std::string)> read_handle;
        std::shared_ptr<boost::asio::io_service> io_service;
        std::string ip;
        int port;
        std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
        char read_buf[BUF_SIZE];//读缓存
        size_t read_size = 0;//上次放在读缓存区的字节数
        char read_tmp_buf[BUF_SIZE];//读的临时缓冲区
        char write_buf[BUF_SIZE];//写缓存
    };


    tcp_wrap tcp_wrap::instance() {
        static tcp_wrap tcp;
        return tcp;
    }

    tcp_wrap::tcp_wrap() : impl(new tcp_wrap_impl) {

    }

    void tcp_wrap::connect(std::string ip, int port) {
        impl->connect(ip, port);
    }

    void tcp_wrap::set_connect_handler(std::function<void(bool)> function) {
        impl->set_connect_handler(function);
    }

    void tcp_wrap::write(std::string msg) {
        impl->write(msg);
    }

    void tcp_wrap::set_read_handler(std::function<void(std::string msg)> function) {
        impl->set_read_handler(function);
    }

}