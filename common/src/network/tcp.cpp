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
                     is_connect(false),
                     port(-1),
                     enable_ping(false),
                     ping_delay_second(20),
                     check_disconnect_interval(30) {

        }

        void on_read(const boost::system::error_code &error_code, std::size_t bytes) {
            if (error_code.value() == 0) {
                //成功
                read_size = 0;
                opPacket(bytes);
                log_wrap::net().d("recv data : ", util::instance().char_to_string(read_buf, bytes));
                sendReadState(read_buf, bytes);
                socket_->async_read_some(boost::asio::buffer(read_tmp_buf), std::bind(&tcp_impl::on_read, this, std::placeholders::_1, std::placeholders::_2));

            } else if (error_code.value() == boost::asio::error::eof){
                //服务器断开
                log_wrap::net().e("recv fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->is_connect = false;
                sendConnectState(false);
            } else {
                log_wrap::net().e("recv fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->is_connect = false;
                sendConnectState(false);
            }
        }

        void on_write(const boost::system::error_code &error_code, std::size_t bytes) {
            if (error_code.value() == 0) {
                //成功
                sendWriteState(write_buf, bytes);
            } else if (error_code.value() == boost::asio::error::eof){
                //服务器断开
                log_wrap::net().e("send fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->is_connect = false;
                sendConnectState(false);
            } else {
                log_wrap::net().e("send fail, reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->is_connect = false;
                sendConnectState(false);
            }
        }

        void on_connect(const boost::system::error_code &error_code) {
            if (error_code.value() == 0) {
                log_wrap::net().i("connected ip : ", ip, " port : ", port);
                this->is_connect = true;
                socket_->async_read_some(boost::asio::buffer(read_tmp_buf), std::bind(&tcp_impl::on_read, this, std::placeholders::_1, std::placeholders::_2));
                sendConnectState(true);
            } else {
                log_wrap::net().e("disconnected ip : ", ip, " port : ", port, " reason : ", error_code.message(), "; error_code : ", error_code.value());
                this->is_connect = false;
                sendConnectState(false);
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
            if (is_connect) return;
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

        void write(const char data[], int len) {
            if (!is_connect) {
                log_wrap::net().e("can not write data to server, because have not connect the server");
                return;
            }

            wrap_byte_data(data, len);
            socket_->async_write_some(boost::asio::buffer(write_buf, (size_t)len), std::bind(&tcp_impl::on_write, this, std::placeholders::_1, std::placeholders::_2));
        }


        void close() {
            io_service->stop();
        }

        void set_connect_handler(std::function<void(bool)> function) {
            connect_handle = function;
        }

        void set_read_handler(std::function<void(char data[], int len)> function) {
            read_handle = function;
        }

        void set_write_handler(std::function<void(char data[], int len)> function) {
            write_handle = function;
        }

    private:
        int wrap_byte_data(const char data[], int len) {
            std::copy(data, data + len, write_buf);
            return len;
        }

        //将数据复制到缓冲区
        void opPacket(size_t len) {
            memcpy(read_buf + read_size, read_tmp_buf, len);
            read_size += len;
        }

        void sendConnectState(bool connect) {
            if (connect_handle != nullptr) {
                connect_handle(connect);
            }
        }

        void sendWriteState(char data[], int len) {
            if (write_handle != nullptr) {
                write_handle(data, len);
            }
        }

        void sendReadState(char data[], int len) {
            if (read_handle != nullptr) {
                read_handle(data, len);
            }
        }


    private:
        std::function<void(bool)> connect_handle;
        std::function<void(char data[], int len)> read_handle;
        std::function<void(char data[], int len)> write_handle;
        std::shared_ptr<boost::asio::io_service> io_service;
        std::shared_ptr<std::thread> thread_;
        std::shared_ptr<boost::asio::io_service::work> work;

        std::string ip;
        int port;
        bool is_connect;
        bool enable_ping;
        int ping_delay_second;
        int check_disconnect_interval;//断线检测间隔

        std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
//        std::shared_ptr<boost::asio::deadline_timer> ping_timer;//定期发送ping包
//        std::shared_ptr<boost::asio::deadline_timer> check_disconnect_timer;//这个是和ping_timer同时使用,enable_ping为false,则这个不起作用

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

//    void tcp::write(std::string msg) {
//        impl->write(network_server_type::SERVER_CONNECT, msg);
//    }

//    void tcp::write(network_server_type type, std::string msg) {
//        impl->write(type, msg);
//    }

    void tcp::write(const char *data, int len) {
        impl->write(data, len);
    }

    void tcp::set_read_handler(std::function<void(char data[], int len)> function) {
        impl->set_read_handler(function);
    }

    void tcp::set_write_handler(std::function<void(char data[], int len)> function) {
        impl->set_write_handler(function);
    }

    void tcp::reconnect() {
        impl->reconnect();
    }

    void tcp::close() {
        impl->close();
    }
}