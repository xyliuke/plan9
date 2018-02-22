//
// Created by ke liu on 28/11/2017.
// Copyright (c) 2017 ke liu. All rights reserved.
//

#include <cstdlib>
#include <cstring>
#include "char_array.h"

namespace plan9
{
    char_array::char_array() : len(0), cap(100) {
    }
    char_array::char_array(int size) :len(0), cap(size){
    }
    char_array::~char_array() {
        len = 0;
        cap = 0;
    }
    void char_array::append(char *data, int len) {
        re_data(len);
        memcpy(this->data.get() + this->len, data, len);
        this->len += len;
    }
    void char_array::append(std::string data) {
        append((char*)data.c_str(), data.length());
    }

    void char_array::append(char_array *data) {
        append(data->get_data_ptr(), data->get_len());
    }

    void char_array::operator<<(std::string data) {
        append(data);
    }

    void char_array::insert(char *data, int len, int pos) {
        re_data(len);
        memmove(this->data.get() + pos + len, this->data.get() + pos, this->len - pos);
        memcpy(this->data.get() + pos, data, len);
        this->len += len;
    }
    void char_array::erase(int pos, int len) {
        memcpy(this->data.get() + pos, this->data.get() + pos + len, this->len - pos - len);
        this->len -= len;
    }
    char* char_array::get_data_ptr() {
        return data.get();
    }
    std::shared_ptr<char> char_array::get_data() {
        return data;
    }
    int char_array::get_len() {
        return len;
    }
    int char_array::get_cap() {
        return cap;
    }

    std::string char_array::to_string() {
        return std::string(data.get(), len);
    }

    void char_array::re_data(int len) {
        if (data == nullptr) {
            data.reset(new char[cap]{});
        }
        if (cap - this->len < len) {
            cap += (len << 1);
            std::shared_ptr<char> n(new char[cap]{});
            memcpy(n.get(), data.get(), this->len);
            data = n;
        }
    }

}