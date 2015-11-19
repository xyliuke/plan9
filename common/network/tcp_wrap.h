//
// Created by liuke on 15/11/8.
//

#ifndef COMMON_TCP_WRAP_H
#define COMMON_TCP_WRAP_H

#include <iosfwd>
#include <string>
#include "tcp_client.h"

namespace plan9 {

    class tcp_wrap {
    public:
        tcp_wrap();

        void connect(std::string url);

        void connect(std::string ip, int port);

        void send(std::string msg);

    private:
        std::shared_ptr <tcp_client> tcp_;
    };

}
#endif //COMMON_TCP_WRAP_H
