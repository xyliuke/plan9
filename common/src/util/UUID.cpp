//
// Created by liuke on 15/12/12.
//

#include <string>
#include "UUID.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>
#include <util/time.h>

namespace plan9
{
    static char ALP[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string UUID::uuid() {
        using namespace boost::uuids;
        random_generator rgen;
        boost::uuids::uuid uuid_str = rgen();
        return to_string(uuid_str);
    }

    std::string UUID::id() {
        static char i = 0;
        std::stringstream ss;
        ss << "ID";
        ss << time::microseconds();
        ss << ALP[i];
        if (i > 24) {
            i = 0;
        } else {
            i++;
        }
        return ss.str();
    }
}