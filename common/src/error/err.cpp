//
// Created by ke liu on 27/01/2018.
// Copyright (c) 2018 ke liu. All rights reserved.
//

#include "err.h"


namespace plan9
{
    static std::string err_msg[NUM_ERR] = {
    "success",
#define X(a, b) b,
            ERR_TABLE
#undef X
    };

    std::string get_err_msg(err_no e) {
        return err_msg[e];
    }
}