//
// Created by liuke on 15/10/24.
//

#ifndef COMMON_TCP_CLIENT_H
#define COMMON_TCP_CLIENT_H


#include <iosfwd>
#include <string>
#include <set>

class tcp_client {

public:
    tcp_client();
    virtual ~tcp_client();

    bool connect(std::string addr, int port);
    void close();
    void write(std::string msg);
    inline void set_recv_function(std::function<void(char* data, int len)> function) {
        recv_ = function;
    }

    /**
     * 根据网址url得到所有ip地址
     */
    std::shared_ptr<std::set<std::string>> getipbyname(std::string url);

private:
    class tcp_client_impl;
    std::shared_ptr<tcp_client_impl> impl_;
    std::function<void(char* data, int len)> recv_;
};


#endif //COMMON_TCP_CLIENT_H
