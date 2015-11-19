//
// Created by liuke on 15/10/24.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include "tcp_client.h"

class tcp_client::tcp_client_impl
{
public:
    bool connect(std::string addr, int port);
    void close();
    void write(std::string msg);
    void read();
    std::shared_ptr<std::set<std::string>> getipbyname(std::string url);


private:
    int sockfd;
    std::string addr_;
    int port_;
};



bool tcp_client::tcp_client_impl::connect(std::string addr, int port)
{
    addr_ = addr;
    port_ = port;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sockaddrIn;
    bzero(&sockaddrIn, sizeof(sockaddrIn));
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_port = htons(port);
    int ret = inet_pton(AF_INET, addr.c_str(), &sockaddrIn.sin_addr);
    if (ret != 1)
    {
        //error
    }
    ret = ::connect(sockfd, (struct sockaddr*)&sockaddrIn, sizeof(sockaddrIn));
    return ret == 1;
}

void tcp_client::tcp_client_impl::close()
{
    ::close(sockfd);
}

void tcp_client::tcp_client_impl::write(std::string msg)
{
    ::write(sockfd, msg.c_str(), strlen(msg.c_str()));
}

void tcp_client::tcp_client_impl::read()
{
    char buf[10000];
    bzero(buf, 10000);
    ssize_t size = ::read(sockfd, buf, 10000);
    std::cout << buf << std::endl;
}
std::shared_ptr<std::set<std::string>> tcp_client::tcp_client_impl::getipbyname(std::string url)
{
    std::shared_ptr<std::set<std::string>> ret(new std::set<std::string>);
    struct hostent* host =  gethostbyname(url.c_str());
    struct sockaddr_in sockaddr;
    char ip[15];
    for (int i = 0; ; ++ i) {
        if (host->h_addr_list[i] != NULL) {
            const char* cip = inet_ntop(AF_INET, &sockaddr.sin_addr, ip, 15);
            ret->insert(std::string(cip));
        } else {
            break;
        }
    }
    return ret;
}





tcp_client::tcp_client() : impl_(new tcp_client_impl)
{
}

tcp_client::~tcp_client()
{
    close();
}


bool tcp_client::connect(std::string addr, int port)
{
    return impl_->connect(addr, port);
}

void tcp_client::close()
{
    impl_->close();
}

void tcp_client::write(std::string msg)
{
    impl_->write(msg);
}

std::shared_ptr<std::set<std::string>> tcp_client::getipbyname(std::string url)
{
    return impl_->getipbyname(url);
}