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

    /**
     * 根据网址url得到所有ip地址
     */
    std::shared_ptr<std::set<std::string>> getipbyname(std::string url);

private:
    class tcp_client_impl;
    std::shared_ptr<tcp_client_impl> impl_;
};


#endif //COMMON_TCP_CLIENT_H
