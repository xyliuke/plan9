//
// Created by ke liu on 27/01/2018.
// Copyright (c) 2018 ke liu. All rights reserved.
//

#ifndef ANET_COMMON_CALLBACK_ERR_WRAPPER_H
#define ANET_COMMON_CALLBACK_ERR_WRAPPER_H


#import <error/err.h>
#import "common_callback.hpp"

namespace plan9
{
    class common_callback_err_wrapper {
    public:
        static std::shared_ptr<common_callback> get(err_no err) {
            auto ret = std::make_shared<common_callback>();
            set_err_no(ret, err);
            return ret;
        }

        static void set_err_no(std::shared_ptr<common_callback> ccb, err_no err) {
            if (ccb) {
                ccb->success = err == E_SUCCESS;
                ccb->error_code = err;
                ccb->reason = get_err_msg(err);
            }
        }
    };
}



#endif //ANET_COMMON_CALLBACK_ERR_WRAPPER_H
