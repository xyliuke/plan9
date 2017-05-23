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
#include <random>

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
        static char j = 0;
        std::stringstream ss;
        ss << "ID-";

#ifdef IOS
        ss << "I-";
#elif ANDROID
        ss << "A-";
#elif WIN32
        ss << "W-";
#elif MACOSX
        ss << "M-";
#elif LINUX
        ss << "L-";
#else
        ss << "O-";
#endif

        ss << time::microseconds();
        ss << "-";

        int r = UUID::random();
        if (r < 10) {
            ss << "000";
        } else if (r < 100) {
            ss << "00";
        } else if (r < 1000) {
            ss << "0";
        }
        ss << r;
        ss << "-";

        ss << ALP[i];
        ss << ALP[j];


        if (j > 24) {
            j = 0;
            if (i > 24) {
                i = 0;
            } else {
                i ++;
            }
        } else {
            j ++;
        }
        return ss.str();
    }

    int UUID::random() {
        return random(9999);
    }

    int UUID::random(int max) {
        std::random_device device;
        std::default_random_engine e(device()) ;
        std::uniform_int_distribution<int> u(0, max);
        return u(e);
    }
}