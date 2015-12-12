//
// Created by liuke on 15/12/12.
//

#include <string>
#include "UUID.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <sstream>

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
        using namespace std::chrono;
        static char i = 0;
        system_clock::time_point today = system_clock::now();
        microseconds m = today.time_since_epoch();
        long long int count = m.count();
        std::stringstream ss;
        ss << "ID";
        ss << count;
        ss << ALP[i];
        if (i > 24) {
            i = 0;
        } else {
            i++;
        }
        return ss.str();
    }
}