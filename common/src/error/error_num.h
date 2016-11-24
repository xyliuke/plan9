//
// Created by liuke on 24/11/2016.
//

#ifndef COMMON_ERROR_NUM_H
#define COMMON_ERROR_NUM_H

namespace plan9
{

#define X(a, b, c)  \
    static const int a = -b;  \
    static const std::string error_string_##a = c;


#define ERROR_STRING(a) error_string_##a


    X(ERROR_PARAMETER, 10001, "parameter error")
    X(ERROR_OPERATION, 10002, "operation error")

}

#endif //COMMON_ERROR_NUM_H
