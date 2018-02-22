//
// Created by ke liu on 28/11/2017.
// Copyright (c) 2017 ke liu. All rights reserved.
//

#ifndef ANET_CHAR_ARRAY_H
#define ANET_CHAR_ARRAY_H

#include <string>

namespace plan9 {

    class char_array {
    public:
        char_array();
        char_array(int size);
        ~char_array();
        void append(char *data, int len);
        void append(std::string data);
        void append(char_array* data);
        void insert(char* data, int len, int pos);
        void erase(int pos, int len);
        int get_len();
        int get_cap();
        char* get_data_ptr();
        std::shared_ptr<char> get_data();
        std::string to_string();
        void operator << (std::string data);
    private:
        std::shared_ptr<char> data;
        int len;
        int cap;
        void re_data(int len);
    };

}


#endif //ANET_CHAR_ARRAY_H
