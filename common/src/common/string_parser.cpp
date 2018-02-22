//
//  string_parser.cpp
//  anet
//
//  Created by ke liu on 22/10/2017.
//  Copyright © 2017 ke liu. All rights reserved.
//

#include <functional>
#include "string_parser.hpp"


namespace plan9 {


    static std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    static std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    std::string string_parser::trim(std::string string) {
        return ltrim(rtrim(string));
    }

    std::string string_parser::to_lower(std::string string) {
        char ret[string.size()];
        transform(string.begin(), string.end(), ret, tolower);
        std::string s(ret, string.size());
        return s;
    }

    std::string string_parser::to_upper(std::string string) {
        char ret[string.size()];
        transform(string.begin(), string.end(), ret, toupper);
        std::string s(ret, string.size());
        return s;
    }

    std::shared_ptr<std::vector<std::string>> string_parser::split(std::string string, std::string split) {
        std::shared_ptr<std::vector<std::string>> ret = std::make_shared<std::vector<std::string>>();
        std::string::size_type pos1, pos2;
        pos2 = string.find(split);
        pos1 = 0;
        while(std::string::npos != pos2) {
            std::string item = string.substr(pos1, pos2-pos1);
            ret->push_back(item);

            pos1 = pos2 + split.size();
            pos2 = string.find(split, pos1);
        }
        if(pos1 != string.length())
            ret->push_back(string.substr(pos1));

        return ret;
    }

    int string_parser::dex_to_dec(char *dex, long len) {
        int ret = 0;
        for (int i = 0; i < len; ++ i) {
            char c = dex[i];
            ret += dex_to_dec(c);
            if (i != len - 1) {
                ret = ret << 4;
            }
        }
        return ret;
    }
    int string_parser::dex_to_dec(char c) {
        if (c >= '0' && c <= '9') {
            return (c - 48);
        } else if (c >= 'A' && c <= 'F'){
            return c - 'A' + 10;
        } else if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        }
        return 0;
    }
}
