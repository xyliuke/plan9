//
// Created by liuke on 15/11/11.
//

#ifndef COMMON_UV_WRAP_H
#define COMMON_UV_WRAP_H


#include <iosfwd>
#include <string>
#include <functional>
#include <uv.h>


namespace plan9 {


    class uv_wrap {

    public:
        uv_wrap();

        virtual ~uv_wrap();

        void connect(std::string ip, int port);

        void send(const char *data, size_t len);

        void deal_read(std::function<void(std::string)> function);

        uv_loop_t* getloop();


        class uv_wrap_impl;
    private:
        std::shared_ptr<uv_wrap_impl> impl_;
        std::function<void(std::string)> read_func;
    };

}


#endif //COMMON_UV_WRAP_H
