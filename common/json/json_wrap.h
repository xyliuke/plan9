//
// Created by liuke on 15/11/5.
//

#ifndef COMMON_JSON_WRAP_H
#define COMMON_JSON_WRAP_H

#include <json/json.h>

namespace plan9
{
    class json_wrap : public Json::Value {

    public:
        json_wrap();
        json_wrap(std::string json_str);
        virtual ~json_wrap();

        std::string toString();
    private:
        static Json::Reader reader_;
        static Json::FastWriter writer_;
    };
}




#endif //COMMON_JSON_WRAP_H
